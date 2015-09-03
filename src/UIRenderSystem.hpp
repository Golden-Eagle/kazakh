#include "ComponentSystem.hpp"
#include "imgui.h"
#include <gecom/Window.hpp>

namespace pxljm {
	class UIRenderComponent : public EntityComponent {
	public:
		virtual void draw() =0;
		void registerWith(Scene&) { }
		void deregisterWith(Scene&) { }
	};

	class TestUIComponent : public UIRenderComponent {
		float joyAx[16];

		void draw() override {
			// set but not used?!?!?
			// ImVec4 clear_color = ImColor(114, 144, 154);
			// static float f = 0.0f;

			// ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5);
			// ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.8, 0, 0, 0.5));
			ImGui::Begin("Test Window");
			ImGui::SetWindowSize(ImVec2(200, 400), ImGuiSetCond_Always);

			int count;
			const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);

			ImGui::Text(glfwGetJoystickName(GLFW_JOYSTICK_1));

			for(int i = 0; i < count; i++) {
				joyAx[i] = axes[i];
				char joyname[30];
				sprintf(joyname, "Axis %d", i);
            	ImGui::SliderFloat(joyname, &joyAx[i], -1.0f, 1.0f);
			}

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            ImGui::End();
            // ImGui::PopStyleColor();
		};
	};

	class UIRenderSystem : public ComponentSystem {
	private:
		static std::vector<UIRenderComponent* > g_components;
		static int          g_shaderHandle, g_vertHandle, g_fragHandle;
		static double       g_Time;
		static int          g_AttribLocationTex, g_AttribLocationProjMtx;
		static GLuint g_fontTexture;
		static int          g_AttribLocationPosition, g_AttribLocationUV, g_AttribLocationColor;
		static unsigned int g_VboHandle, g_VaoHandle, g_ElementsHandle;

	public:
		UIRenderSystem() {
			ImGuiIO& io = ImGui::GetIO();
			io.RenderDrawListsFn = RenderDrawLists;
		}

		static void registerUiComponent(UIRenderComponent* c) {
			g_components.push_back(c);
		}

		void createFontsTexture();
		bool createDeviceObjects();

		static void RenderDrawLists(ImDrawData* draw_data);

		void render(Scene&,int w, int h, int fw, int fh);
	};
}