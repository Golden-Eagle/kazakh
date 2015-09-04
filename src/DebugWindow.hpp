#pragma once

#include <unordered_set>
#include "imgui.h"
#include <gecom/Window.hpp>


namespace pxljm {
	class DebugWindowManager;

	class DebugDrawable {
	public:
		virtual void debugDraw() = 0;

		virtual ~DebugDrawable() { }
	};

	class DebugWindowDrawable : public DebugDrawable {
		friend DebugWindowManager;
	public:
		virtual std::string debugWindowTitle() = 0;

		virtual ~DebugWindowDrawable();
	private:
		void drawWindow(bool *);
	};



	class DebugEventDispatcher : public gecom::WindowEventDispatcher {
	public:
		DebugEventDispatcher() { }

		// virtual void dispatchWindowRefreshEvent(const window_refresh_event &);
		// virtual void dispatchWindowCloseEvent(const window_close_event &);
		// virtual void dispatchWindowPosEvent(const window_pos_event &);
		// virtual void dispatchWindowSizeEvent(const window_size_event &);
		// virtual void dispatchFramebufferSizeEvent(const framebuffer_size_event &);
		// virtual void dispatchWindowFocusEvent(const window_focus_event &);
		// virtual void dispatchWindowIconEvent(const window_icon_event &);
		virtual void dispatchMouseEvent(const gecom::mouse_event &);
		virtual void dispatchMouseButtonEvent(const gecom::mouse_button_event &);
		virtual void dispatchMouseScrollEvent(const gecom::mouse_scroll_event &);
		virtual void dispatchKeyEvent(const gecom::key_event &);
		virtual void dispatchCharEvent(const gecom::char_event &);
	};



	class DebugWindowManager {
		friend DebugEventDispatcher;
	private:
		static std::unordered_set<DebugWindowDrawable* > g_windows;

		//static double       g_Time;
		//static bool         g_MousePressed[3];
		//static float        g_MouseWheel;
		//static ImVec2       g_MousePosition;
		static GLuint       g_fontTexture;
		static int          g_shaderHandle, g_vertHandle, g_fragHandle;
		static int          g_AttribLocationTex, g_AttribLocationProjMtx;
		static int          g_AttribLocationPosition, g_AttribLocationUV, g_AttribLocationColor;
		static unsigned int g_VboHandle, g_VaoHandle, g_ElementsHandle;

		std::shared_ptr<DebugEventDispatcher> m_debug_wed = std::make_shared<DebugEventDispatcher>();

	public:
		DebugWindowManager();
		~DebugWindowManager();

		static void renderDrawLists(ImDrawData *);
		static void registerDebugWindowDrawable(DebugWindowDrawable *);
		static void deregisterDebugWindowDrawable(DebugWindowDrawable *);

		void createFontsTexture();
		bool createDeviceObjects();

		void draw(int w, int h, int fw, int fh);
		std::shared_ptr<gecom::WindowEventDispatcher> getEventDispatcher();
	};
}