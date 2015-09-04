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
	public:
		virtual std::string debugWindowTitle() = 0;

		virtual ~DebugWindowDrawable();
	private:
		void drawWindow(bool *);
		friend DebugWindowManager;
	};


	class DebugWindowManager {
	private:
		static std::unordered_set<DebugWindowDrawable* > g_windows;

		static int          g_shaderHandle, g_vertHandle, g_fragHandle;
		static double       g_Time;
		static int          g_AttribLocationTex, g_AttribLocationProjMtx;
		static GLuint g_fontTexture;
		static int          g_AttribLocationPosition, g_AttribLocationUV, g_AttribLocationColor;
		static unsigned int g_VboHandle, g_VaoHandle, g_ElementsHandle;

		std::shared_ptr<gecom::WindowEventProxy> m_wep = std::make_shared<gecom::WindowEventProxy>();

		gecom::subscription_ptr m_mousebutton_sub;
		gecom::subscription_ptr m_scroll_sub;
		gecom::subscription_ptr m_key_sub;
		gecom::subscription_ptr m_char_sub;

	public:
		DebugWindowManager();

		static void renderDrawLists(ImDrawData *);
		static void registerDebugWindowDrawable(DebugWindowDrawable *);
		static void deregisterDebugWindowDrawable(DebugWindowDrawable *);

		void createFontsTexture();
		bool createDeviceObjects();


		void draw(int w, int h, int fw, int fh);
	};
}