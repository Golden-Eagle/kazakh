
#include <chrono>

#include "Scene.hpp"

using namespace std;
using namespace pxljm;
using namespace gecom;
using namespace i3d;
using namespace std::chrono_literals;

Scene::Scene() {}


Scene::~Scene() { }


void Scene::update() {

	// Input Update
	//
	m_updateSystem.inputUpdate();

	// Physics
	//
	m_physicsSystem.tick();

	// Sound
	//
	// m_soundSystem.update();

	// Update
	//
	m_updateSystem.update(m_physicsSystem.lastTick(), 10ms, 5ms);

	// Animation
	// Later Josh... later...
}



void Scene::add( entity_ptr e){
	e->registerWith(*this);
	m_entities.push_back(e);
}


void Scene::debugDraw() {
	ImGui::Text("SCENE HELLO WORLD");
	ImGui::CollapsingHeader("Entities");
	for (int i = 0; i < m_entities.size(); i++) {
		ImGui::PushID(i);
		if (ImGui::TreeNode(m_entities[i]->getName().c_str())) {
			m_entities[i]->debugDraw();
			ImGui::TreePop();
		}
		ImGui::PopID();
	}
}


string Scene::debugWindowTitle() {
	return "Scene";
}


CameraSystem & Scene::cameraSystem() { return m_cameraSystem; }


DrawableSystem & Scene::drawableSystem() { return m_drawableSystem; }


PhysicsSystem & Scene::physicsSystem() { return m_physicsSystem; }


UpdateSystem & Scene::updateSystem() { return m_updateSystem; }


LightSystem & Scene::lightSystem() { return m_lightSystem; }


// SoundSystem & Scene::soundSystem() { return m_soundSystem; }


