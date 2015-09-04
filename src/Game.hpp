
#ifndef PXLJM_GAME_HPP
#define PXLJM_GAME_HPP

#include <vector>
#include <stdexcept>
#include <thread>

#include <gecom/Window.hpp>
#include <gecom/Initial3D.hpp>

#include "Pxljm.hpp"

#include "Assets.hpp"
#include "Scene.hpp"
#include "Renderer.hpp"
#include "SimpleShader.hpp"
#include "ComponentTest.hpp"
#include "State.hpp"

// Inclusions for compoenets

#include "Mesh.hpp"
#include "Collider.hpp"

namespace pxljm {

	class Game {
	private:
		StateManager m_stateManager;
		DebugWindowManager m_debugManager;
		gecom::subscription_ptr m_debugManager_sub;
	public:
		Game() {
			m_win = gecom::createWindow().size(1024, 768).hint(GLFW_SAMPLES, 16).title("Kazakh 2015").visible(true).contextVersion(4, 1);
			m_win->makeCurrent();


			m_win->onKeyPress.subscribe([&](const gecom::key_event &e) {
				if (e.key == GLFW_KEY_TAB) {
					}

				return false;
			})->forever();

			m_debugManager_sub = m_win->subscribeEventDispatcher(m_debugManager.getEventDispatcher());

			assets::init("./AssetConfig.json");

		}

		~Game() { }

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

				//debug draw here
				m_debugManager.draw(m_win->width(), m_win->height(), m_win->width(), m_win->height());

				m_win->swapBuffers();

				if (now - lastFPSTime > 1) {
					char fpsString[200];
					sprintf(
						fpsString, "Kazakh 2015 [%d FPS @%dx%d]",
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

#endif
