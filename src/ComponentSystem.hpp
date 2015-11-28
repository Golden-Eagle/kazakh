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

		bool m_enabled = true;

		std::weak_ptr<Entity> m_entity;
		friend class Entity;

		virtual void onAttachToEntity() { }
		virtual void onDetachFromEntity() { }

		virtual void onEnable(bool) { }

	public:
		virtual ~EntityComponent() { }

		void setEnabled(bool);
		bool isEnabled() const;

		bool hasEntity();
		entity_ptr entity() const;
	};

	class ComponentSystem {
	public:
		virtual ~ComponentSystem() { }
	};

	class Message {
	public:
		Entity *sender = nullptr;

		Message() { }
		Message(Entity *sender_) : sender(sender_) { }

		virtual ~Message() { }
	};
}
