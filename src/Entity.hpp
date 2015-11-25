#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <typeinfo>
#include <typeindex>
#include <utility>
#include <unordered_set>
#include <vector>

#include <gecom/Uncopyable.hpp>
#include <gecom/Initial3D.hpp>

#include "ComponentSystem.hpp"

namespace pxljm {

	//
	// Transform component
	//
	class TransformComponent : public virtual EntityComponent {
	public:
		virtual void registerWith(Scene &) override;
		virtual void deregisterWith(Scene &) override;

		virtual i3d::mat4d matrix() const = 0;

		virtual i3d::vec3d getPosition() const = 0;
		virtual i3d::quatd getRotation() const = 0;

		virtual void setPosition(i3d::vec3d) = 0;
		virtual void setRotation(i3d::quatd) = 0;
	};

	//
	// Entity Transform component
	//
	class EntityTransform : public virtual TransformComponent {
	private:
		i3d::vec3d position;
		i3d::quatd rotation;

		EntityTransform *m_parent = nullptr;
		std::unordered_set<EntityTransform *> m_children;

	public:
		EntityTransform(i3d::vec3d = i3d::vec3d(), i3d::quatd = i3d::quatd());
		EntityTransform(i3d::quatd);

		virtual i3d::mat4d matrix() const;
		virtual i3d::mat4d localMatrix() const;

		virtual i3d::vec3d getPosition() const;
		virtual i3d::quatd getRotation() const;

		virtual i3d::vec3d getLocalPosition() const;
		virtual i3d::quatd getLocalRotation() const;

		virtual void setPosition(i3d::vec3d);
		virtual void setRotation(i3d::quatd);

		virtual void setLocalPosition(i3d::vec3d);
		virtual void setLocalRotation(i3d::quatd);

		void addChild(EntityTransform *);
		void removeChild(EntityTransform *);

		bool hasParent();
		EntityTransform * getParent();
	};

	//
	// Entity
	//
	class Entity : gecom::Uncopyable, public std::enable_shared_from_this<Entity> {
	public:
		template <typename ComponentT, typename MessageT>
		using message_handler_memfn_t = void (ComponentT::*)(Entity *, Entity *, MessageT *);

	private:

		// inner helper struct
		struct message_handler {
			void (*proxy)(Entity *, Entity *, Message *, const message_handler *); // dispatch proxy
			void *c; // component
			alignas(void *) unsigned char h[16]; // member function pointer
		};


		// fields
		Scene *m_scene = nullptr;
		EntityTransform m_root;
		std::vector<EntityComponent *> m_components;
		std::vector<std::unique_ptr<EntityComponent>> m_dynamicComponents;
		std::unordered_map<std::type_index, std::vector<message_handler>> m_handlers;

		std::string m_name;



		// helper methods
		template <typename ComponentT, typename MessageT>
		static void dynamicDispatchProxy(Entity *receiver, Entity *sender, Message *m, const message_handler *mh) {
			auto hh = reinterpret_cast<const message_handler_memfn_t<ComponentT, MessageT> &>(mh->h);
			auto cc = reinterpret_cast<ComponentT *>(mh->c);
			auto mm = static_cast<MessageT *>(m);
			(cc->*hh)(receiver, sender, mm);
		}

		template <typename ComponentT, typename MessageT, message_handler_memfn_t<ComponentT, MessageT> HandlerF>
		static void staticDispatchProxy(Entity *receiver, Entity *sender, Message *m, const message_handler *mh) {
			auto cc = reinterpret_cast<ComponentT *>(mh->c);
			auto mm = static_cast<MessageT *>(m);
			(cc->*HandlerF)(receiver, sender, mm);
		}


	public:
		Entity(std::string, i3d::vec3d = i3d::vec3d(), i3d::quatd = i3d::quatd());
		Entity(std::string, i3d::quatd);
		virtual ~Entity();

		std::string getName();
		EntityTransform * root();

		void registerWith(Scene &);
		void deregister();


		// Components
		//
		void addComponent(std::unique_ptr<EntityComponent>);

		template<typename T, typename... Args>
		T * emplaceComponent(Args&&... args)  {
			std::unique_ptr<T> ec = std::make_unique<T>(std::forward<Args>(args)...);
			addComponent(std::move(ec));
			return ec.get();
		}

		void removeComponent(EntityComponent *);

		const std::vector<EntityComponent *> & getAllComponents() const;

		template<typename T>
		T * getComponent() const {
			for (EntityComponent * c : m_components)
				if (auto i = dynamic_cast<T*>(c))
					return i;
			return nullptr;
		}
		
		template<typename T>
		std::vector<T *> getComponents() const {
			std::vector<T *> componentList;
			for (EntityComponent * c : m_components)
				if (auto i = dynamic_cast<T*>(c))
					componentList.push_back(i);
			return componentList;
		}



		// Registration methods
		//
		template <typename ComponentT, typename MessageT>
		void registerMessageHandler(ComponentT *c, message_handler_memfn_t<ComponentT, MessageT> h) {
			message_handler mh;
			mh.proxy = dynamicDispatchProxy<ComponentT, MessageT>;
			reinterpret_cast<ComponentT *&>(mh.c) = c;
			static_assert(sizeof(h) <= sizeof(mh.h), "member function pointer too big");
			reinterpret_cast<message_handler_memfn_t<ComponentT, MessageT> &>(mh.h) = h;
			m_handlers[typeid(MessageT)].push_back(mh);
		}

		template <typename ComponentT, typename MessageT, message_handler_memfn_t<ComponentT, MessageT> HandlerF>
		void registerMessageHandler(ComponentT *c) {
			message_handler mh;
			mh.proxy = staticDispatchProxy<ComponentT, MessageT, HandlerF>;
			reinterpret_cast<ComponentT *&>(mh.c) = c;
			reinterpret_cast<message_handler_memfn_t<ComponentT, MessageT> &>(mh.h) = nullptr;
			m_handlers[typeid(MessageT)].push_back(mh);
		}

		void sendMessage(Entity *, Message *);
	};
}