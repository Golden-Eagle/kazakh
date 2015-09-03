#ifndef PXLJM_PLAYSTATE_HPP
#define PXLJM_PLAYSTATE_HPP

// fuck these spaces

#include <chrono>

#include "Pxljm.hpp"
#include "Game.hpp"
// #include "Joystick.hpp"
#include <gecom/Window.hpp>

namespace pxljm {

	class FPSController : public InputUpdatable, public Updatable {
	private:
		i3d::vec3d m_movement;
		gecom::subscription_ptr m_mb_sub;
		gecom::point2d m_lastmouse;

		double horzRot = 0;
		double vertRot = 0;

	public:
		double speed = 5.0; // meters per second
		double rotateSpeed = i3d::math::pi() / 300; // radians per pixel

		FPSController() { }
		~FPSController() { }

		virtual void registerWith(Scene &s) override {
			InputUpdatable::registerWith(s);
			Updatable::registerWith(s);

			m_mb_sub = s.updateSystem().eventProxy()->onMouseMove.subscribe([this](const gecom::mouse_event &e) {
				if (e.window->getButton(GLFW_MOUSE_BUTTON_LEFT)) {
					horzRot += (m_lastmouse.x - e.pos.x) * rotateSpeed;
					vertRot += (m_lastmouse.y - e.pos.y) * rotateSpeed;
					i3d::quatd rotx = i3d::quatd::axisangle(i3d::vec3d::i(), vertRot);
					i3d::quatd roty = i3d::quatd::axisangle(i3d::vec3d::j(), horzRot);
					entity()->root()->setLocalRotation(roty * rotx);
				}
				m_lastmouse = e.pos;
				return false;
			});

		}

		virtual void deregisterWith(Scene &s) override {
			InputUpdatable::deregisterWith(s);
			Updatable::deregisterWith(s);
		}

		virtual void inputUpdate(gecom::WindowEventProxy &win) override {
			i3d::vec3d movement;

			if (win.getKey(GLFW_KEY_W) || win.getKey(GLFW_KEY_UP))
				movement -= i3d::vec3d::k();
			if (win.getKey(GLFW_KEY_S) || win.getKey(GLFW_KEY_DOWN))
				movement += i3d::vec3d::k();

			if (win.getKey(GLFW_KEY_A) || win.getKey(GLFW_KEY_LEFT))
				movement -= i3d::vec3d::i();
			if (win.getKey(GLFW_KEY_D) || win.getKey(GLFW_KEY_RIGHT))
				movement += i3d::vec3d::i();

			if (win.getKey(GLFW_KEY_LEFT_SHIFT) || win.getKey(GLFW_KEY_RIGHT_SHIFT))
				movement -= i3d::vec3d::j();
			if (win.getKey(GLFW_KEY_SPACE))
				movement += i3d::vec3d::j();

			m_movement = movement;
		}

		virtual void update(clock_t::time_point now, clock_t::time_point prev) override {
			double dt = timeDelta(now, prev);

			if (m_movement.mag() > 0.1) {
				m_movement = ~m_movement;
				m_movement *= speed * dt;
				i3d::vec3d deltapos = entity()->root()->getLocalRotation() * m_movement;
				entity()->root()->setLocalPosition(entity()->root()->getLocalPosition() + deltapos);
			}
		}
	};





	class PlayState : public State < std::string > {
		Renderer m_renderer;
		std::shared_ptr<Scene> m_scene;
		std::shared_ptr<Entity> m_player;
		std::shared_ptr<Entity> m_ground;
		std::shared_ptr<Entity> m_camera;
		PerspectiveCamera *m_cameraComponent;

		Game* m_game;
		gecom::subscription_ptr m_window_scene_sub;


	public:
		PlayState(Game* game) : m_renderer(game->window()), m_game(game) {
			m_scene = std::make_shared<Scene>();


			//manually subscribe windw event proxy to window
			m_window_scene_sub = game->window()->subscribeEventDispatcher(m_scene->updateSystem().eventProxy());


			// The ASTEROID!!!!!!!!!
			m_player = std::make_shared<Entity>(i3d::vec3d(0, 2, 0));
			// m_player->emplaceComponent<MeshDrawable>(
			// 	assets::getMesh("cube"),
			// 	assets::getMaterial("basic"));
			m_player->emplaceComponent<RigidBody>(std::make_shared<SphereCollider>(1));
			m_player->emplaceComponent<MeshDrawable>(
				assets::getMesh("asteroid"),
				assets::getMaterial("asteroid"));
			m_scene->add(m_player);



			// Ground plane
			m_ground = std::make_shared<Entity>(i3d::vec3d(0, 0, 0));
			m_ground->emplaceComponent<RigidBody>(std::make_shared<BoxCollider>(i3d2bt(i3d::vec3d(100, 0.1, 100))), 0);
			m_ground->emplaceComponent<MeshDrawable>(
				assets::getMesh("big_plane"),
				assets::getMaterial("basic"));
			m_scene->add(m_ground);



			// FPS controller
			m_camera = std::make_shared<Entity>(i3d::vec3d(0, 1, 5));
			m_camera->emplaceComponent<FPSController>();
			m_cameraComponent = m_camera->emplaceComponent<PerspectiveCamera>();
			m_scene->add(m_camera);
			m_scene->cameraSystem().setCamera(m_cameraComponent);


		}

		virtual action_ptr updateForeground() override {
			// Game loop
			m_scene->update();
			return nullAction();
		}

		virtual void drawForeground() override {
			m_renderer.renderScene(*m_scene);
		}
	};
}

#endif
