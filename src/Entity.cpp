#include "Entity.hpp"
#include "Scene.hpp"

using namespace std;
using namespace pxljm;
using namespace i3d;

//
// Entity
//
Entity::Entity(string name, vec3d pos, quatd rot) : m_name(name) {
	m_root = EntityTransform(pos, rot);
	m_components.push_back(&m_root);
}


Entity::Entity(string name, quatd rot) : m_name(name) {
	m_root = EntityTransform(rot);
	m_components.push_back(&m_root);
}


Entity::~Entity() {
	// Remove all compoenents from scene
	// if (m_scene)
	// 	for (EntityComponent *c : m_components)
	// 		c->deregisterWith(*m_scene);
}


string Entity::getName() {
	return m_name;
}


Scene * Entity::getScene() {
	return m_scene;
}


EntityTransform * Entity::root() {
	return &m_root;
}


void Entity::registerWith(Scene &s) {
	m_scene = &s;

	for (EntityComponent *c : m_components)
		c->registerWith(s);
}


void Entity::deregister() {
	for (EntityComponent *c : m_components)
		c->deregisterWith(*m_scene);
	m_scene = nullptr;
}


void Entity::addComponent(unique_ptr<EntityComponent> ec) {
	EntityComponent *ecp = ec.get();

	m_dynamicComponents.push_back(std::move(ec));
	m_components.push_back(ecp);

	ecp->m_entity = shared_from_this();
	ecp->onAttachToEntity();

	ecp->start();

	if (m_scene)
		ecp->registerWith(*m_scene);
}


void Entity::removeComponent(EntityComponent *c) {
	auto it = find(m_components.begin(), m_components.end(), c);
	if (it == m_components.end()) {
		(*it)->onDetachFromEntity();
		if (m_scene)
			(*it)->deregisterWith(*m_scene);
		m_components.erase(it);
	}
}


const vector<EntityComponent *> & Entity::getAllComponents() const {
	return m_components;
}


void Entity::sendMessage(Message &m) {
	auto it = m_handlers.find(typeid(m));
	if (it == m_handlers.end()) return;
	for (message_handler &mh : it->second) {
		mh.proxy(this, m, &mh);
	}
}

void Entity::sendMessage(Message &&m) {
	sendMessage(m);
}


//
// Transform component
//
void TransformComponent::registerWith(Scene &) { }


void TransformComponent::deregisterWith(Scene &) { }



//
// Entity Transform component
//
EntityTransform::EntityTransform(vec3d pos, quatd rot) : position(pos), rotation(rot) { }


EntityTransform::EntityTransform(quatd rot) : position(vec3d()), rotation(rot) { }


mat4d EntityTransform::matrix() const {
	return (m_parent) ? m_parent->matrix() * mat4d::translate(position) * mat4d::rotate(rotation)
		: mat4d::translate(position) * mat4d::rotate(rotation);
}


mat4d EntityTransform::localMatrix() const {
	return mat4d::translate(position) * mat4d::rotate(rotation);
}


vec3d EntityTransform::getPosition() const {
	return (m_parent) ? vec3d(m_parent->matrix() * vec4d(position)) : position;
}


quatd EntityTransform::getRotation() const {
	return (m_parent) ? m_parent->getRotation() * rotation : rotation;
}


vec3d EntityTransform::getLocalPosition() const {
	return position;
}


quatd EntityTransform::getLocalRotation() const {
	return rotation;
}


void EntityTransform::setPosition(vec3d pos) {
	if (m_parent) {
		position = !m_parent->matrix() * pos;
	} else {
		position = pos;
	}
}


void EntityTransform::setRotation(quatd rot) {
	if (m_parent) {
		rotation = !m_parent->getRotation() * rot;
	}
	else {
		rotation = rot;
	}
}

void EntityTransform::setLocalPosition(vec3d pos) {
	position = pos;
}

void EntityTransform::setLocalRotation(quatd rot) {
	rotation = rot;
}


void EntityTransform::addChild(EntityTransform *c) {
	m_children.insert(c);
	if (c->m_parent) {
		c->m_parent->removeChild(c);
	}
	c->m_parent = this;
}


void EntityTransform::removeChild(EntityTransform *c) {
	m_children.erase(c);
	c->m_parent = nullptr;
}


bool EntityTransform::hasParent() {
	return m_parent;
}

EntityTransform * EntityTransform::getParent() {
	return m_parent;
}

