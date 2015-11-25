#pragma once

#include <iostream>
#include <queue>
#include <memory>
#include <string>
#include <vector>

#include <gecom/Window.hpp>
#include <gecom/Initial3D.hpp>

// Entity Stuff
//
#include "ComponentSystem.hpp"

#include "Entity.hpp"
#include "CameraSystem.hpp"
#include "DrawSystem.hpp"
#include "LightSystem.hpp"
#include "PhysicsSystem.hpp"
#include "UpdateSystem.hpp"
#include "SoundSystem.hpp"

#include "DebugWindow.hpp"

namespace pxljm {

	class Scene;
	using scene_ptr = std::shared_ptr<Scene>;

	//Colection of entities
	class Scene : public std::enable_shared_from_this<Scene> {
	private:
		std::vector<entity_ptr> m_entities;

		UpdateSystem   m_updateSystem;
		DrawableSystem m_drawableSystem;
		PhysicsSystem  m_physicsSystem;
		LightSystem    m_lightSystem;
		// SoundSystem	   m_soundSystem;
		CameraSystem   m_cameraSystem;

	public:
		Scene();
		~Scene();

		void update();
		void add( entity_ptr e);

		CameraSystem & cameraSystem();
		DrawableSystem & drawableSystem();
		PhysicsSystem & physicsSystem();
		UpdateSystem & updateSystem();
		LightSystem & lightSystem();
		// SoundSystem & soundSystem();
	};
}