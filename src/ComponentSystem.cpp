
#include "ComponentSystem.hpp"
#include "Scene.hpp"


using namespace pxljm;


void EntityComponent::debugDraw() {
	ImGui::Text("> --- Unknown Component --- ");
}


bool EntityComponent::hasEntity() { return bool(m_entity.lock()); }


entity_ptr EntityComponent::entity() const { return m_entity.lock(); }