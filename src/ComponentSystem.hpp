#pragma once

#include <memory>

#include <gecom/Uncopyable.hpp>

#include "imgui.h"

#include "Pxljm.hpp"
#include "DebugWindow.hpp"


namespace pxljm {

	class Entity;
	using entity_ptr = std::shared_ptr<Entity>;

	class Scene;

	class EntityComponent {
	private:
		virtual void start() { }
		virtual void registerWith(Scene &) = 0;
		virtual void deregisterWith(Scene &) = 0;

		virtual void debugDraw() { }; //TODO make pure virtual

		std::weak_ptr<Entity> m_entity;
		friend class Entity;

	public:
		virtual ~EntityComponent() { }

		bool hasEntity();
		entity_ptr entity() const;
	};

	class ComponentSystem {
	public:
		virtual ~ComponentSystem() { }
	};
}