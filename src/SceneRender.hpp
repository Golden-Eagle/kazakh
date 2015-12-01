#pragma once

#include <iostream>
#include <memory>
#include <queue>
#include <vector>

#include "Render.hpp"
#include "DrawSystem.hpp"

#include <gecom/Window.hpp>
#include <gecom/Initial3D.hpp>


namespace pxljm {

	// Forward decleration of Camera
	class Camera;

	class Renderer {
	public:
		// If you want to create a renderer to be used by SceneRenderStrategy
		// you must declare a static method "std::unique_ptr<RenderTask> getRenderTask(Camera *)"
		virtual ~Renderer() { }
	};

	class RenderStrategy {
	public:
		virtual ~RenderStrategy() { }
		virtual std::unique_ptr<RenderTask> getRenderTask(Camera *) = 0;
	};


	template <typename RendererT>
	class SceneRenderStrategy : public RenderStrategy {
	public:
		SceneRenderStrategy() { }
		virtual ~SceneRenderStrategy() { }
		// Requires RendererT to have a static method "getRenderTask"
		// that matches this method signiture exactly.
		virtual std::unique_ptr<RenderTask> getRenderTask(Camera *c) {
			return RendererT::getRenderTask(c);
		}
	};


	template <typename RendererT>
	class DefaultSceneRenderTask : public RenderTask {
	public:
		using renderfac_t = RendererT & (*)();// TODO better name?
	private:
		renderfac_t m_renderfac = nullptr;
		
		// TODO real data

	public:
		// Simple data
		i3d::mat4d viewMatrix;
		i3d::mat4d projectionMatrix;
		float zfar;

		gecom::size2i size;

		std::priority_queue<DrawCall *> drawQueue;

		DefaultSceneRenderTask() { }

		void rendererFactory(renderfac_t factory) { m_renderfac = factory; }
		
		virtual void render() override {
			RendererT &r = m_renderfac();
			r.render(*this);
		}
		
		// TODO methods for adding data 'n' shit yo
	};


	class DefaultSceneRenderer : public Renderer {
	private:
		gecom::size2i m_fbsize { 0, 0 };
		GLuint m_fbo_scene = 0;
		GLuint m_tex_scene_depth = 0;
		GLuint m_tex_scene_color = 0;
		GLuint m_tex_scene_normal = 0;

		void initFBO(gecom::size2i);

	public:
		using task_t = DefaultSceneRenderTask<DefaultSceneRenderer>;

		static DefaultSceneRenderer & instance();
		static std::unique_ptr<task_t> getRenderTask(Camera *);

		DefaultSceneRenderer() { }

		virtual void render(task_t &); // doesn't override
	};
}