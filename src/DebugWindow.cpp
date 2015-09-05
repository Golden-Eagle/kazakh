#include "DebugWindow.hpp"

#include <vector>

namespace pxljm {


	DebugWindowDrawable::~DebugWindowDrawable() {
		DebugWindowManager::deregisterDebugWindowDrawable(this);
	}


	void DebugWindowDrawable::drawWindow(bool *opened) {
		// hacky temp size thing
		if (!ImGui::Begin(debugWindowTitle().c_str(), opened, ImGuiWindowFlags_AlwaysAutoResize)) {
			// Early out if the window is collapsed, as an optimization.
			ImGui::End();
			return;
		}
		debugDraw();
		ImGui::End();
	}



	// void DebugEventDispatcher::dispatchWindowRefreshEvent(const window_refresh_event &);
	// void DebugEventDispatcher::dispatchWindowCloseEvent(const window_close_event &);
	// void DebugEventDispatcher::dispatchWindowPosEvent(const window_pos_event &);
	// void DebugEventDispatcher::dispatchWindowSizeEvent(const window_size_event &);
	// void DebugEventDispatcher::dispatchFramebufferSizeEvent(const framebuffer_size_event &);
	// void DebugEventDispatcher::dispatchWindowFocusEvent(const window_focus_event &);
	// void DebugEventDispatcher::dispatchWindowIconEvent(const window_icon_event &);

	void DebugEventDispatcher::dispatchMouseEvent(const gecom::mouse_event &e) {
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2(float(e.pos.x), float(e.pos.y));
	}


	void DebugEventDispatcher::dispatchMouseButtonEvent(const gecom::mouse_button_event &e) {
		ImGuiIO& io = ImGui::GetIO();
		if (e.button >= 0 && e.button < 3)
			io.MouseDown[e.button] = (e.action == GLFW_PRESS);
	}

	void DebugEventDispatcher::dispatchMouseScrollEvent(const gecom::mouse_scroll_event &e) {
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheel += float(e.offset.h); // Use fractional mouse wheel, 1.0 unit 5 lines.
	}

	void DebugEventDispatcher::dispatchKeyEvent(const gecom::key_event &e) {
		ImGuiIO& io = ImGui::GetIO();
		if (e.action == GLFW_PRESS)
			io.KeysDown[e.key] = true;
		if (e.action == GLFW_RELEASE)
			io.KeysDown[e.key] = false;

		//(void)mods; // Modifiers are not reliable across systems
		io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
		io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
		io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
	}

	void DebugEventDispatcher::dispatchCharEvent(const gecom::char_event &e) {
		ImGuiIO& io = ImGui::GetIO();
		if (e.codepoint > 0 && e.codepoint < 0x10000)
			io.AddInputCharacter((unsigned short)e.codepoint);
	}







	std::unordered_set<DebugWindowDrawable *> DebugWindowManager::g_windows;
	//double       DebugWindowManager::g_Time = 0;
	//bool         DebugWindowManager::g_MousePressed = { false, false, false };
	//float        DebugWindowManager::g_MouseWheel = 0.0f;
	GLuint       DebugWindowManager::g_fontTexture = 0;
	int          DebugWindowManager::g_shaderHandle = 0, DebugWindowManager::g_vertHandle = 0, DebugWindowManager::g_fragHandle = 0;
	int          DebugWindowManager::g_AttribLocationTex = 0, DebugWindowManager::g_AttribLocationProjMtx = 0;
	int          DebugWindowManager::g_AttribLocationPosition = 0, DebugWindowManager::g_AttribLocationUV = 0, DebugWindowManager::g_AttribLocationColor = 0;
	unsigned int DebugWindowManager::g_VboHandle = 0, DebugWindowManager::g_VaoHandle = 0, DebugWindowManager::g_ElementsHandle = 0;


	DebugWindowManager::DebugWindowManager() {
		ImGuiIO& io = ImGui::GetIO();

		// Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
		io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
		io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
		io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
		io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
		io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
		io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
		io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
		io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
		io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
		io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
		io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
		io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
		io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
		io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
		io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
		io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

		io.RenderDrawListsFn = renderDrawLists;
		// io.SetClipboardTextFn = ImGui_ImplGlfwGL3_SetClipboardText;
		// io.GetClipboardTextFn = ImGui_ImplGlfwGL3_GetClipboardText;
		
		//TODO setup WED
	}

	DebugWindowManager::~DebugWindowManager(){
		if (g_VaoHandle) glDeleteVertexArrays(1, &g_VaoHandle);
		if (g_VboHandle) glDeleteBuffers(1, &g_VboHandle);
		if (g_ElementsHandle) glDeleteBuffers(1, &g_ElementsHandle);
		g_VaoHandle = g_VboHandle = g_ElementsHandle = 0;

		glDetachShader(g_shaderHandle, g_vertHandle);
		glDeleteShader(g_vertHandle);
		g_vertHandle = 0;

		glDetachShader(g_shaderHandle, g_fragHandle);
		glDeleteShader(g_fragHandle);
		g_fragHandle = 0;

		glDeleteProgram(g_shaderHandle);
		g_shaderHandle = 0;

		if (g_fontTexture) {
			glDeleteTextures(1, &g_fontTexture);
			ImGui::GetIO().Fonts->TexID = 0;
			g_fontTexture = 0;
		}
		ImGui::Shutdown();
	}


	void DebugWindowManager::renderDrawLists(ImDrawData* draw_data) {
		// Backup GL state
		GLint last_program, last_texture, last_array_buffer, last_element_array_buffer, last_vertex_array;
		glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
		glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
		glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
		glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

		// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_SCISSOR_TEST);
		glActiveTexture(GL_TEXTURE0);

		// Handle cases of screen coordinates != from framebuffer coordinates (e.g. retina displays)
		ImGuiIO& io = ImGui::GetIO();
		float fb_height = io.DisplaySize.y * io.DisplayFramebufferScale.y;
		draw_data->ScaleClipRects(io.DisplayFramebufferScale);

		// Setup orthographic projection matrix
		const float ortho_projection[4][4] =
		{
			{ 2.0f/io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
			{ 0.0f,                  2.0f/-io.DisplaySize.y, 0.0f, 0.0f },
			{ 0.0f,                  0.0f,                  -1.0f, 0.0f },
			{-1.0f,                  1.0f,                   0.0f, 1.0f },
		};
		glUseProgram(g_shaderHandle);
		glUniform1i(g_AttribLocationTex, 0);
		glUniformMatrix4fv(g_AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
		glBindVertexArray(g_VaoHandle);

		for (int n = 0; n < draw_data->CmdListsCount; n++)
		{
			const ImDrawList* cmd_list = draw_data->CmdLists[n];
			const ImDrawIdx* idx_buffer_offset = 0;

			glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
			glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.size() * sizeof(ImDrawVert), (GLvoid*)&cmd_list->VtxBuffer.front(), GL_STREAM_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ElementsHandle);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.size() * sizeof(ImDrawIdx), (GLvoid*)&cmd_list->IdxBuffer.front(), GL_STREAM_DRAW);

			for (const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin(); pcmd != cmd_list->CmdBuffer.end(); pcmd++)
			{
				if (pcmd->UserCallback)
				{
					pcmd->UserCallback(cmd_list, pcmd);
				}
				else
				{
					glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
					glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
					glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, GL_UNSIGNED_SHORT, idx_buffer_offset);
				}
				idx_buffer_offset += pcmd->ElemCount;
			}
		}

		// Restore modified GL state
		glUseProgram(last_program);
		glBindTexture(GL_TEXTURE_2D, last_texture);
		glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
		glBindVertexArray(last_vertex_array);
		glDisable(GL_SCISSOR_TEST);

		glDisable(GL_BLEND);
	}


	void DebugWindowManager::registerDebugWindowDrawable(DebugWindowDrawable* c) {
		g_windows.insert(c);
	}


	void DebugWindowManager::deregisterDebugWindowDrawable(DebugWindowDrawable* c) {
		g_windows.erase(c);
	}


	void DebugWindowManager::createFontsTexture() {
		ImGuiIO& io = ImGui::GetIO();

		unsigned char* pixels;
		int width, height;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be compatible with user's existing shader.

		glGenTextures(1, &g_fontTexture);
		glBindTexture(GL_TEXTURE_2D, g_fontTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

		// Store our identifier
		io.Fonts->TexID = (void *)(intptr_t)g_fontTexture;

		// Cleanup (don't clear the input data if you want to append new fonts later)
		io.Fonts->ClearInputData();
		io.Fonts->ClearTexData();
	}


	bool DebugWindowManager::createDeviceObjects() {
		// Backup GL state
		GLint last_texture, last_array_buffer, last_vertex_array;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
		glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
		glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

		const GLchar *vertex_shader =
			"#version 330\n"
			"uniform mat4 ProjMtx;\n"
			"in vec2 Position;\n"
			"in vec2 UV;\n"
			"in vec4 Color;\n"
			"out vec2 Frag_UV;\n"
			"out vec4 Frag_Color;\n"
			"void main()\n"
			"{\n"
			"   Frag_UV = UV;\n"
			"   Frag_Color = Color;\n"
			"   gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
			"}\n";

		const GLchar* fragment_shader =
			"#version 330\n"
			"uniform sampler2D Texture;\n"
			"in vec2 Frag_UV;\n"
			"in vec4 Frag_Color;\n"
			"out vec4 Out_Color;\n"
			"void main()\n"
			"{\n"
			"   Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
			"}\n";

		g_shaderHandle = glCreateProgram();
		g_vertHandle = glCreateShader(GL_VERTEX_SHADER);
		g_fragHandle = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(g_vertHandle, 1, &vertex_shader, 0);
		glShaderSource(g_fragHandle, 1, &fragment_shader, 0);
		glCompileShader(g_vertHandle);
		glCompileShader(g_fragHandle);
		glAttachShader(g_shaderHandle, g_vertHandle);
		glAttachShader(g_shaderHandle, g_fragHandle);
		glLinkProgram(g_shaderHandle);

		g_AttribLocationTex = glGetUniformLocation(g_shaderHandle, "Texture");
		g_AttribLocationProjMtx = glGetUniformLocation(g_shaderHandle, "ProjMtx");
		g_AttribLocationPosition = glGetAttribLocation(g_shaderHandle, "Position");
		g_AttribLocationUV = glGetAttribLocation(g_shaderHandle, "UV");
		g_AttribLocationColor = glGetAttribLocation(g_shaderHandle, "Color");

		glGenBuffers(1, &g_VboHandle);
		glGenBuffers(1, &g_ElementsHandle);

		glGenVertexArrays(1, &g_VaoHandle);
		glBindVertexArray(g_VaoHandle);
		glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
		glEnableVertexAttribArray(g_AttribLocationPosition);
		glEnableVertexAttribArray(g_AttribLocationUV);
		glEnableVertexAttribArray(g_AttribLocationColor);

	#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
		glVertexAttribPointer(g_AttribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, pos));
		glVertexAttribPointer(g_AttribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, uv));
		glVertexAttribPointer(g_AttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, col));
	#undef OFFSETOF

		createFontsTexture();

		// Restore modified GL state
		// glBindTexture(GL_TEXTURE_2D, last_texture);
		// glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
		// glBindVertexArray(last_vertex_array);

		return true;
	}


	void DebugWindowManager::draw(int w, int h, int fw, int fh) {
		if(!g_fontTexture)
			createDeviceObjects();

		// Setup display size (every frame to accommodate for window resizing)
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2((float)w, (float)h);
		io.DisplayFramebufferScale = ImVec2((float)fw / w, (float)fh / h);

		//// Setup time step
		//double current_time =  glfwGetTime();
		//io.DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f/60.0f);
		//g_Time = current_time;

		ImGui::NewFrame();

		for (auto it = g_windows.begin(); it != g_windows.end();) {
			bool openWindow = true;
			(*it)->drawWindow(&openWindow);
			if (!openWindow) {
				it = g_windows.erase(it);
			} else {
				it++;
			}
		}

		ImGui::Render();
	}

	std::shared_ptr<gecom::WindowEventDispatcher> DebugWindowManager::getEventDispatcher() {
		return m_debug_wed;
	}
}