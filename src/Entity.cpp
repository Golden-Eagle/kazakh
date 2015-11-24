#include "Entity.hpp"
#include "Scene.hpp"

using namespace std;
using namespace pxljm;
using namespace i3d;

//
// Entity
//
Entity::Entity(std::string name, vec3d pos, quatd rot) : m_name(name) {
	m_root = EntityTransform(pos, rot);
	m_components.push_back(&m_root);
}


Entity::Entity(std::string name, quatd rot) : m_name(name) {
	m_root = EntityTransform(rot);
	m_components.push_back(&m_root);
}


Entity::~Entity() {
	// Remove all compoenents from scene
	// if (m_scene)
	// 	for (EntityComponent *c : m_components)
	// 		c->deregisterWith(*m_scene);
}


void Entity::debugDraw() {
	ImGui::Text("Entity WOOP");

	for (int i = 0; i < m_components.size(); ++i) {
		ImGui::PushID(i);
		m_components[i]->debugDraw();
		ImGui::PopID();
	}
}


string Entity::debugWindowTitle() {
	return "Entity :: " + m_name;
}


string Entity::getName() {
	return m_name;
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


void Entity::addComponent(std::unique_ptr<EntityComponent> ec) {
	EntityComponent *ecp = ec.get();

	m_dynamicComponents.push_back(std::move(ec));
	m_components.push_back(ecp);

	ecp->m_entity = shared_from_this();

	ecp->start();

	if (m_scene)
		ecp->registerWith(*m_scene);
}


void Entity::removeComponent(EntityComponent *c) {
	auto it = find(m_components.begin(), m_components.end(), c);
	if (it == m_components.end()) {
		if (m_scene)
			(*it)->deregisterWith(*m_scene);
		m_components.erase(it);
	}
}


EntityTransform * Entity::root() {
	return &m_root;
}

const std::vector<EntityComponent *> & Entity::getAllComponents() const {
	return m_components;
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


void EntityTransform::debugDraw() {
	if (ImGui::TreeNode("Entity Transform")) {

		static float pos_shown[3] = { 0.0f, 0.0f, 0.0f };
		vec3d pos = getPosition();


		ImGui::PushItemWidth(80);

		ImGui::Text("Position");
		ImGui::SameLine();

		if (ImGui::InputFloat("X", &pos_shown[0])) pos.x() = pos_shown[0];
		else pos_shown[0] = pos.x();
		ImGui::SameLine();

		if (ImGui::InputFloat("Y", &pos_shown[1])) pos.y() = pos_shown[1];
		else pos_shown[1] = pos.y();
		ImGui::SameLine();

		if (ImGui::InputFloat("Z", &pos_shown[2])) pos.z() = pos_shown[2];
		else pos_shown[2] = pos.z();
		setPosition(pos);

		ImGui::PopItemWidth();



		static float rot_placeholder[3] = { 0.0f, 0.0f, 0.0f };
		ImGui::InputFloat3("Rotation", rot_placeholder);

		
		ImGui::TreePop();
	}
}


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

