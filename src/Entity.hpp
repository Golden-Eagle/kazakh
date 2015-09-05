#pragma once

#include <algorithm>
#include <memory>
#include <vector>
#include <unordered_set>

#include <gecom/Uncopyable.hpp>
#include <gecom/Initial3D.hpp>

#include "ComponentSystem.hpp"

namespace pxljm {

	//
	// Tranform component
	//
	class TransformComponent : public virtual EntityComponent {
	public:
		virtual void registerWith(Scene &) override;
		virtual void deregisterWith(Scene &) override;

		virtual i3d::mat4d matrix() = 0;

		virtual i3d::vec3d getPosition() const = 0;
		virtual i3d::quatd getRotation() const = 0;

		virtual void setPosition(i3d::vec3d) = 0;
		virtual void setRotation(i3d::quatd) = 0;
	};

	//
	// Entity Tranform component
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

		void debugDraw() override;

		virtual i3d::mat4d matrix();
		virtual i3d::mat4d localMatrix();

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
	class Entity : gecom::Uncopyable, public std::enable_shared_from_this<Entity>, public DebugWindowDrawable {
	private:
		Scene *m_scene = nullptr;
		EntityTransform m_root;
		std::vector<std::unique_ptr<EntityComponent>> m_dynamicComponents;

		std::vector<EntityComponent *> m_components;

		std::string m_name;

	public:
		Entity(std::string, i3d::vec3d = i3d::vec3d(), i3d::quatd = i3d::quatd());
		Entity(std::string, i3d::quatd);
		virtual ~Entity();

		void debugDraw() override;
		std::string debugWindowTitle() override;

		std::string getName();

		void registerWith(Scene &);
		void deregister();


		void addComponent(std::unique_ptr<EntityComponent>);
		template<typename T, typename... Args>
		T * emplaceComponent(Args&&... args)  {
			std::unique_ptr<T> ec = std::make_unique<T>(std::forward<Args>(args)...);
			addComponent(std::move(ec));
			return ec.get();
		}
		void removeComponent(EntityComponent *);


		EntityTransform * root();
		const std::vector<EntityComponent *> & getAllComponents() const;

		//TODO need to make this depth first search?
		template<typename T>
		T * getComponent() const {
			for (EntityComponent * c : m_components)
				if (auto i = dynamic_cast<T*>(c))
					return i;
			return nullptr;
		}

		//TODO need to make this depth first search?
		template<typename T>
		std::vector<T *> getComponents() const {
			std::vector<T *> componentList;
			for (EntityComponent * c : m_components)
				if (auto i = dynamic_cast<T*>(c))
					componentList.push_back(i);
			return componentList;
		}
	};
}