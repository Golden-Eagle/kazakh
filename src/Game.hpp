
#include <vector>
#include <stdexcept>
#include <thread>

#include "Assets.hpp"
#include "Camera.hpp"
#include "Scene.hpp"
#include "GLOW.hpp"
#include "Renderer.hpp"
#include "Initial3D.hpp"
#include "Window.hpp"
#include "SimpleShader.hpp"
#include "ComponentTest.hpp"
#include "LevelLoader.hpp"
#include "State.hpp"

// Inclusions for compoenets

#include "Mesh.hpp"
#include "Collider.hpp"

namespace pxljm {

	class Game {
	private:
		StateManager m_stateManager;
	public:
		Game() {
			m_win = gecom::createWindow().size(1024, 768).hint(GLFW_SAMPLES, 16).title("Pxljm 2015").visible(true);
			m_win->makeContextCurrent();

			m_win->onKeyPress.subscribe([&](const gecom::key_event &e) {
				if (e.key == GLFW_KEY_TAB) {
					}

				return false;
			}).forever();

			assets::init("./AssetConfig.json");

		}

		~Game();

		template <typename FirstStateT>
		void init() {
			m_stateManager.init<FirstStateT>(this);
		}

		gecom::Window* window() const {
			return m_win;
		}


		void run() {
			double lastFPSTime = glfwGetTime();
			int fps = 0;

			while (!m_win->shouldClose()) {
				glfwPollEvents();

				double now = glfwGetTime();

				m_stateManager.update();
				m_stateManager.draw();

				m_win->swapBuffers();

				if (now - lastFPSTime > 1) {
					char fpsString[200];
					sprintf(
						fpsString, "Pxljm 2015 [%d FPS @%dx%d]",
						fps, m_win->width(), m_win->height());
					m_win->title(fpsString);
					fps = 0;
					lastFPSTime = now;
				}
				fps++;
			}

			delete m_win;

			glfwTerminate();
		}
	private:
		gecom::Window *m_win;
		scene_ptr m_scene;
		Renderer *m_renderer;
	};
}