
#include <gecom/Initial3D.hpp>
#include <gecom/Log.hpp>

#include "SimpleShader.hpp"

#include "Renderer.hpp"

using namespace std;
using namespace initial3d;
using namespace gecom;

namespace {

	static const std::string shader_fullscreen_source = R"delim(
// vertex shader
#ifdef _VERTEX_

flat out int instanceID;

void main() {
	instanceID = gl_InstanceID;
}

#endif

// geometry shader
#ifdef _GEOMETRY_

layout(points) in;
layout(triangle_strip, max_vertices = 3) out;

flat in int instanceID[];

out vec2 fullscreen_tex_coord;
flat out int fullscreen_layer;

void main() {
	// output a single triangle that covers the whole screen
	// if instanced, set layer to instance id
	
	gl_Position = vec4(3.0, 1.0, 0.0, 1.0);
	gl_Layer = instanceID[0];
	fullscreen_layer = instanceID[0];
	fullscreen_tex_coord = vec2(2.0, 1.0);
	EmitVertex();
	
	gl_Position = vec4(-1.0, 1.0, 0.0, 1.0);
	gl_Layer = instanceID[0];
	fullscreen_layer = instanceID[0];
	fullscreen_tex_coord = vec2(0.0, 1.0);
	EmitVertex();
	
	gl_Position = vec4(-1.0, -3.0, 0.0, 1.0);
	gl_Layer = instanceID[0];
	fullscreen_layer = instanceID[0];
	fullscreen_tex_coord = vec2(0.0, -1.0);
	EmitVertex();
	
	EndPrimitive();
	
}

#endif

// fragment shader
#ifdef _FRAGMENT_

in vec2 fullscreen_tex_coord;
flat in int fullscreen_layer;

// main() should be implemented by includer

#endif

)delim";

	static const std::string shader_deferred0_source = shader_fullscreen_source + R"delim(
uniform sampler2D sampler_depth;
uniform sampler2D sampler_color;
uniform sampler2D sampler_normal;

#ifdef _FRAGMENT_

out vec4 frag_color;

void main() {
	frag_color = texture(sampler_color, fullscreen_tex_coord).rgba;
}

#endif

)delim";

}

namespace pxljm {

	void Renderer::initFBO(size2i sz) {
		if (sz == m_fbsize) return;
		
		glActiveTexture(GL_TEXTURE0);

		if (!m_fbo_scene) {
			// make fbo and textures
			glGenFramebuffers(1, &m_fbo_scene);
			glGenTextures(1, &m_tex_scene_depth);
			glGenTextures(1, &m_tex_scene_color);
			glGenTextures(1, &m_tex_scene_normal);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo_scene);

			// depth texture
			glBindTexture(GL_TEXTURE_2D, m_tex_scene_depth);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, sz.w, sz.h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_tex_scene_depth, 0);

			// color + metallicity texture
			glBindTexture(GL_TEXTURE_2D, m_tex_scene_color);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, sz.w, sz.h, 0, GL_RGBA, GL_FLOAT, nullptr);
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex_scene_color, 0);

			// normal + roughness + specularity texture
			glBindTexture(GL_TEXTURE_2D, m_tex_scene_normal);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, sz.w, sz.h, 0, GL_RGBA, GL_FLOAT, nullptr);
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_tex_scene_normal, 0);

			GLenum bufs_scene[] { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
			glDrawBuffers(2, bufs_scene);
		}

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo_scene);

		glBindTexture(GL_TEXTURE_2D, m_tex_scene_depth);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, sz.w, sz.h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

		glBindTexture(GL_TEXTURE_2D, m_tex_scene_color);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, sz.w, sz.h, 0, GL_RGBA, GL_FLOAT, nullptr);

		glBindTexture(GL_TEXTURE_2D, m_tex_scene_normal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, sz.w, sz.h, 0, GL_RGBA, GL_FLOAT, nullptr);

		m_fbsize = sz;
	}

	void Renderer::renderScene(Scene &s) {
		glClearColor(0.9f, .9f, 0.9f, 1.f); // default background color

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		auto size = m_win->framebufferSize();

		if (size == size2i(0, 0)) return;

		initFBO(size);

		s.cameraSystem().update(size.w, size.h);
		glViewport(0, 0, size.w, size.h);

		// draw material properties to scene buffer
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo_scene);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		double zfar = s.cameraSystem().getPrimaryCamera()->getZfar();
		i3d::mat4d view = s.cameraSystem().getPrimaryCamera()->getViewMatrix();
		i3d::mat4d proj = s.cameraSystem().getPrimaryCamera()->getProjectionMatrix();
		std::priority_queue<DrawCall *> drawList = s.drawableSystem().getDrawQueue(view);

		for (; !drawList.empty(); drawList.pop()) {
			auto d = drawList.top();
			// Bind shader program
			// Bind material properties
			// Bind Geometry
			// Then render

			material_ptr m = d->material();
			m->shader->bind();
			m->bind(proj, zfar);
			d->draw();
		}

		static GLuint prog_deferred0 = 0;
		if (!prog_deferred0) {
			prog_deferred0 = gecom::makeShaderProgram("330 core", { GL_VERTEX_SHADER, GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER }, shader_deferred0_source);
		}

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glUseProgram(prog_deferred0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_tex_scene_depth);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_tex_scene_color);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_tex_scene_normal);

		glUniform1i(glGetUniformLocation(prog_deferred0, "sampler_depth"), 0);
		glUniform1i(glGetUniformLocation(prog_deferred0, "sampler_color"), 1);
		glUniform1i(glGetUniformLocation(prog_deferred0, "sampler_normal"), 2);

		draw_dummy();

		glUseProgram(0);

		s.physicsSystem().debugDraw(s);

		// s.uiRenderSystem().render(s, m_win->width(), m_win->height(), size.w, size.h);
	}
}
