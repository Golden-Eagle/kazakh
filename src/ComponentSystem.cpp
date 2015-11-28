
#include "ComponentSystem.hpp"
#include "Scene.hpp"


using namespace pxljm;

void EntityComponent::setEnabled(bool e) {
	if (e != m_enabled) {
		m_enabled = e;
		onEnable(e);
	}
}

bool EntityComponent::isEnabled() const { return m_enabled; }

bool EntityComponent::hasEntity() { return bool(m_entity.lock()); }

entity_ptr EntityComponent::entity() const { return m_entity.lock(); }