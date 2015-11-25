
#include "ComponentSystem.hpp"
#include "Scene.hpp"


using namespace pxljm;


bool EntityComponent::hasEntity() { return bool(m_entity.lock()); }

entity_ptr EntityComponent::entity() const { return m_entity.lock(); }