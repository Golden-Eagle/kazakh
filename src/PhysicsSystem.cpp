#include "PhysicsSystem.hpp"
#include "Scene.hpp"


using namespace std;
using namespace pxljm;
using namespace gecom;
using namespace i3d;


//
// Physics Update component
//
void PhysicsUpdatable::registerWith(Scene &s) { s.physicsSystem().registerPhysicsUpdatable(this); }

void PhysicsUpdatable::deregisterWith(Scene &s) { s.physicsSystem().deregisterPhysicsUpdatable(this); }






//
// Physics component
//
// Rigid Body component
//
void RigidBody::registerWith(Scene &s) { s.physicsSystem().registerRigidBody(this); }
void RigidBody::deregisterWith(Scene &s) { s.physicsSystem().deregisterRigidBody(this); }


RigidBody::RigidBody() {
	cout << "Fuck you for trying to construct something like this!" <<  endl;
	abort(); // LOL wut u doin?!?!
}

RigidBody::RigidBody(collider_ptr c, double m) : m_collider(c), m_mass(m) {  }


void RigidBody::start() {
	regenerateRigidBody();
}


void RigidBody::addToDynamicsWorld(btDynamicsWorld * world) {
	removeFromDynamicsWorld();
	m_world = world;
	if (m_enabled)
		m_world->addRigidBody(m_rigidBody.get());
}


void RigidBody::removeFromDynamicsWorld() {
	if (m_world) {
		if (m_enabled)
			m_world->removeRigidBody(m_rigidBody.get());
		m_world = nullptr;
	}
}


void RigidBody::setCollider(collider_ptr col) {
	m_collider = col;

	btCollisionShape *shape = m_collider->getCollisionShape();
	btVector3 inertia(0, 0, 0);
	shape->calculateLocalInertia(m_mass, inertia);

	m_rigidBody->setMassProps(m_mass, inertia);
	m_rigidBody->updateInertiaTensor();
}


collider_ptr RigidBody::getCollider() {
	return m_collider;
}


btRigidBody * RigidBody::getRigidBody() {
	return m_rigidBody.get();
}


void RigidBody::setEnable(bool e) {
	m_enabled = e;
	if (m_world) {
		if (e) m_world->addRigidBody(m_rigidBody.get());
		else m_world->removeRigidBody(m_rigidBody.get());
	}
}
bool RigidBody::isEnabled() { return m_enabled; }


// Anisotropic Friction
void RigidBody::setAnisotropicFriction(i3d::vec3d f) { m_rigidBody->setAnisotropicFriction(i3d2bt(f)); }
i3d::vec3d RigidBody::getAnisotropicFriction() { return bt2i3d(m_rigidBody->getAnisotropicFriction()); }

// Friction
void RigidBody::setFriction(double f) { m_rigidBody->setFriction(f); }
double RigidBody::getFriction() { return m_rigidBody->getFriction(); }

// Rolling Friction
void RigidBody::setRollingFriction(double f) { m_rigidBody->setRollingFriction(f); }
double RigidBody::getRollingFriction() { return m_rigidBody->getRollingFriction(); }

// Bounceness
void RigidBody::setRestitution(double f) { m_rigidBody->setRestitution(f); }
double RigidBody::getRestitution() { return m_rigidBody->getRestitution(); }

// Damping (drag)
void RigidBody::setDamping(double lin, double ang) { m_rigidBody->setDamping(lin, ang); }
double RigidBody::getLinearDamping() { return m_rigidBody->getLinearDamping(); }
double RigidBody::getAngularDamping() { return m_rigidBody->getAngularDamping(); }

// Manual scale for movement
void RigidBody::setLinearFactor(i3d::vec3d f) { m_rigidBody->setLinearFactor(i3d2bt(f)); }
void RigidBody::setAngularFactor(i3d::vec3d f) { m_rigidBody->setAngularFactor(i3d2bt(f)); }
i3d::vec3d RigidBody::getLinearFactor() { return bt2i3d(m_rigidBody->getLinearFactor()); }
i3d::vec3d RigidBody::getAngularFactor() { return bt2i3d(m_rigidBody->getAngularFactor()); }

// Mass, Kinematic / Dynamic
void RigidBody::setMass(double m) {
	m_mass = m;
	btCollisionShape *shape = m_collider->getCollisionShape();
	btVector3 inertia(0, 0, 0);
	shape->calculateLocalInertia(m_mass, inertia);
	m_rigidBody->setMassProps(m_mass, inertia);
	m_rigidBody->updateInertiaTensor();
}

double RigidBody::getMass() {
	return m_mass;
}

//TODO
//center of mass
//shape offset

// Current state
void RigidBody::setLinearVelocity(i3d::vec3d v) { m_rigidBody->setLinearVelocity(i3d2bt(v)); }
void RigidBody::setAngularVelocity(i3d::vec3d v) { m_rigidBody->setAngularVelocity(i3d2bt(v)); }
// void RigidBody::setLocalLinearVelocity(i3d::vec3d v) { m_rigidBody->setLinearVelocity(i3d2bt(vec3(entity()->root()->matrix() * vec4(v, 0))); }
// void RigidBody::setLocalAngularVelocity(i3d::vec3d v) { m_rigidBody->setAngularVelocity(i3d2bt(vec3(entity()->root()->matrix() * vec4(v, 0))); }

i3d::vec3d RigidBody::getLinearVelocity() { return bt2i3d(m_rigidBody->getLinearVelocity()); }
i3d::vec3d RigidBody::getAngularVelocity() { return bt2i3d(m_rigidBody->getAngularVelocity()); }
// i3d::vec3d RigidBody::getVelocityAtPoint(i3d::vec3d) { m_rigidBody. }
// i3d::vec3d RigidBody::getLocalLinearVelocity() { m_rigidBody.getLinearVelocity() }
// i3d::vec3d RigidBody::getLocalAngularVelocity() { m_rigidBody.getAngularVelocity() }
// i3d::vec3d RigidBody::getLocalVelocityAtPoint(i3d::vec3d) { m_rigidBody. }


// Dynamic Rigid Bodies
void RigidBody::applyForce(i3d::vec3d v, i3d::vec3d p) { m_rigidBody->applyForce(i3d2bt(v), i3d2bt(p)); }
void RigidBody::applyImpulse(i3d::vec3d v) { m_rigidBody->applyCentralImpulse(i3d2bt(v)); }
void RigidBody::applyImpulse(i3d::vec3d v, i3d::vec3d p) { m_rigidBody->applyImpulse(i3d2bt(v), i3d2bt(p)); }

void RigidBody::applyTorque(i3d::vec3d v) { m_rigidBody->applyTorque(i3d2bt(v)); }
void RigidBody::applyTorqueImpulse(i3d::vec3d v) { m_rigidBody->applyTorqueImpulse(i3d2bt(v)); }

void RigidBody::clearForces() { m_rigidBody->clearForces(); }

// Bullet active state
void RigidBody::wakeUp() { m_rigidBody->activate(); }
bool RigidBody::isAwake() { return m_rigidBody->isActive(); }















void RigidBody::getWorldTransform (btTransform &centerOfMassWorldTrans) const {
	vec3d pos = entity()->root()->getPosition();
	quatd rot = entity()->root()->getRotation();
	centerOfMassWorldTrans.setOrigin(i3d2bt(pos));
	centerOfMassWorldTrans.setRotation(i3d2bt(rot));
}


void RigidBody::setWorldTransform (const btTransform &centerOfMassWorldTrans) {
	btVector3 pos = centerOfMassWorldTrans.getOrigin();
	btQuaternion rot = centerOfMassWorldTrans.getRotation();
	entity()->root()->setPosition(bt2i3d(pos));
	entity()->root()->setRotation(bt2i3d(rot));
}


void RigidBody::regenerateRigidBody() {
	// Create rigid body
	btCollisionShape *shape = m_collider->getCollisionShape();
	btVector3 inertia(0, 0, 0);
	shape->calculateLocalInertia(m_mass, inertia);
	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(m_mass, this, shape, inertia);
	m_rigidBody = make_unique<btRigidBody>(rigidBodyCI);

	// Set Attributes
	// static cast to physics component pointer be cause the type needs to be
	// exactly what we reinterpret_cast it as, in the collision handler
	m_rigidBody->setUserPointer(static_cast<Physical *>(this)); // ben figured this out.. ask him why
}




//
// Collision Callback component
//
void CollisionCallback::registerWith(Scene &s) { s.physicsSystem().registerCollisionCallback(this); }


void CollisionCallback::deregisterWith(Scene &s) { s.physicsSystem().deregisterCollisionCallback(this); }


void CollisionCallback::onCollisionEnter(Physical *) { }


void CollisionCallback::onCollision(Physical *) { }


void CollisionCallback::onCollisionExit(Physical *) { }





PhysicsDebugDrawer::PhysicsDebugDrawer() {
	// setup the buffer!

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo_pos);
	glGenBuffers(1, &m_vbo_col);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_pos);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_col);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Cleanup
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	m_shader = assets::getShader("physics_debug");

}


void PhysicsDebugDrawer::draw(mat4d view, mat4d proj) {
	if (m_position.size() > 0) {
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_pos);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_position.size(), &m_position[0], GL_STREAM_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_col);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_color.size(), &m_color[0], GL_STREAM_DRAW);

		m_shader->bind();
		glUniformMatrix4fv(m_shader->uniformLocation("uModelViewMatrix"), 1, true, i3d::mat4f(view));
		glUniformMatrix4fv(m_shader->uniformLocation("uProjectionMatrix"), 1, true, i3d::mat4f(proj));

		glDrawArrays(GL_LINES, 0, m_position.size());

		glBindVertexArray(0);
		glFinish();
	}

	m_position.clear();
	m_color.clear();
}


void PhysicsDebugDrawer::drawLine (const btVector3 &from, const btVector3 &to, const btVector3 &color) {
	m_position.push_back(from.getX());
	m_position.push_back(from.getY());
	m_position.push_back(from.getZ());

	m_color.push_back(color.getX());
	m_color.push_back(color.getY());
	m_color.push_back(color.getZ());

	m_position.push_back(to.getX());
	m_position.push_back(to.getY());
	m_position.push_back(to.getZ());

	m_color.push_back(color.getX());
	m_color.push_back(color.getY());
	m_color.push_back(color.getZ());
}


void PhysicsDebugDrawer::drawLine (const btVector3 &from, const btVector3 &to, const btVector3 &fromColor, const btVector3 &toColor) {
	m_position.push_back(from.getX());
	m_position.push_back(from.getY());
	m_position.push_back(from.getZ());

	m_color.push_back(fromColor.getX());
	m_color.push_back(fromColor.getY());
	m_color.push_back(fromColor.getZ());

	m_position.push_back(to.getX());
	m_position.push_back(to.getY());
	m_position.push_back(to.getZ());

	m_color.push_back(toColor.getX());
	m_color.push_back(toColor.getY());
	m_color.push_back(toColor.getZ());
}


void PhysicsDebugDrawer::drawContactPoint (const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color) { }


void PhysicsDebugDrawer::reportErrorWarning (const char *warningString) {
	cout << "BULLLET HAS SOME WIERD ERRORS" << warningString << endl;
}


void PhysicsDebugDrawer::draw3dText (const btVector3 &location, const char *textString) { }


void PhysicsDebugDrawer::setDebugMode (int debugMode) { m_mode = debugMode; }


int PhysicsDebugDrawer::getDebugMode () const {	return m_mode; }








// 
// Physical System
//
static void physicsSystemTickCallback(btDynamicsWorld *world, btScalar timeStep) {
	PhysicsSystem *w = reinterpret_cast<PhysicsSystem *>(world->getWorldUserInfo());
	w->processPhysicsCallback(timeStep);
}


PhysicsSystem::PhysicsSystem() {

	broadphase = new btDbvtBroadphase();
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	solver = new btSequentialImpulseConstraintSolver();

	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

	// Callback
	dynamicsWorld->setInternalTickCallback(physicsSystemTickCallback, static_cast<void *>(this));

	// TODO make it realitive?
	// If not configurable
	// HACK
	dynamicsWorld->setGravity(btVector3(0, -9.81, 0));



	// Debug HACK
	m_debugDrawer.setDebugMode(btIDebugDraw::DBG_DrawWireframe);
    dynamicsWorld->setDebugDrawer(&m_debugDrawer);




	// SUPER HACK
	// Hard limit for the ground to be at y=0
	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
	btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
	dynamicsWorld->addRigidBody(groundRigidBody);

}


PhysicsSystem::~PhysicsSystem() {
	delete dynamicsWorld;
	delete solver;
	delete dispatcher;
	delete collisionConfiguration;
	delete broadphase;
}


void PhysicsSystem::registerPhysicsUpdatable(PhysicsUpdatable * c) {
	m_physicsUpdatables.insert(c);
}


void PhysicsSystem::deregisterPhysicsUpdatable(PhysicsUpdatable *c) {
	m_physicsUpdatables.erase(c);
}


void PhysicsSystem::registerRigidBody(RigidBody * c) {
	m_rigidbodies.insert(c);
	c->addToDynamicsWorld(dynamicsWorld);
}


void PhysicsSystem::deregisterRigidBody(RigidBody *c) {
	c->removeFromDynamicsWorld();
	m_rigidbodies.erase(c);
}


void PhysicsSystem::registerCollisionCallback(CollisionCallback *c) {
	m_collisionCallbacks[c->entity().get()].insert(c);
}


void PhysicsSystem::deregisterCollisionCallback(CollisionCallback *c) {
	unordered_set<CollisionCallback *> &cset = m_collisionCallbacks[c->entity().get()];
	cset.erase(c);
	if (cset.empty()) {
		m_collisionCallbacks.erase(c->entity().get());
	}
}


void PhysicsSystem::tick() {
	dynamicsWorld->stepSimulation(1 / 60.f, 10);
}


void PhysicsSystem::debugDraw(i3d::mat4d view, i3d::mat4d proj) {
	dynamicsWorld->debugDrawWorld();
	m_debugDrawer.draw(view, proj);
}


void PhysicsSystem::processPhysicsCallback(btScalar timeStep) {

	// Process collisions
	//
	swap(m_currentFrame, m_lastFrame);
	m_currentFrame.clear();

	int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; ++i) {
		btPersistentManifold* contactManifold =  dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		const btCollisionObject *obA = contactManifold->getBody0();
		const btCollisionObject *obB = contactManifold->getBody1();

		auto collisionPair = make_pair(obA, obB);

		Physical * physicsA = reinterpret_cast<Physical *>(obA->getUserPointer());
		Physical * physicsB = reinterpret_cast<Physical *>(obB->getUserPointer());

		Entity * entityA = physicsA ? physicsA->entity().get() : nullptr;
		Entity * entityB = physicsB ? physicsB->entity().get() : nullptr;

		int numContacts = contactManifold->getNumContacts();
		for (int j = 0; j<numContacts; ++j) {
			btManifoldPoint& pt = contactManifold->getContactPoint(j);

			// Actual contact
			if (pt.getDistance() < 0.f) {
				m_currentFrame.insert(collisionPair);
				auto it  = m_lastFrame.find(collisionPair);
				if(it != m_lastFrame.end()) {
					//existing collision
					
					if (m_collisionCallbacks.find(entityA) != m_collisionCallbacks.end())
						for (CollisionCallback *callback : m_collisionCallbacks.at(entityA))
							callback->onCollision(physicsB);

					if (m_collisionCallbacks.find(entityB) != m_collisionCallbacks.end())
						for (CollisionCallback *callback : m_collisionCallbacks.at(entityB))
							callback->onCollision(physicsA);

					m_lastFrame.erase(it);
				} else {
					//new collision
					
					if (m_collisionCallbacks.find(entityA) != m_collisionCallbacks.end())
						for (CollisionCallback *callback : m_collisionCallbacks.at(entityA))
							callback->onCollisionEnter(physicsB);

					if (m_collisionCallbacks.find(entityB) != m_collisionCallbacks.end())
						for (CollisionCallback *callback : m_collisionCallbacks.at(entityB))
							callback->onCollisionEnter(physicsA);

				}

				break; //break out of btManifoldPoint iteration
			}
		}
	}

	for (auto collisionExitPair : m_lastFrame) {
		Physical * physicsA = reinterpret_cast<Physical *>(collisionExitPair.first->getUserPointer());
		Physical * physicsB = reinterpret_cast<Physical *>(collisionExitPair.second->getUserPointer());

		Entity * entityA = physicsA ? physicsA->entity().get() : nullptr;
		Entity * entityB = physicsB ? physicsB->entity().get() : nullptr;


		if (m_collisionCallbacks.find(entityA) != m_collisionCallbacks.end())
			for (CollisionCallback *callback : m_collisionCallbacks.at(entityA))
				callback->onCollisionExit(physicsB);

		if (m_collisionCallbacks.find(entityB) != m_collisionCallbacks.end())
			for (CollisionCallback *callback : m_collisionCallbacks.at(entityB))
				callback->onCollisionExit(physicsA);
	}



	// Process physics updates
	//
	for (PhysicsUpdatable *pc : m_physicsUpdatables)
		pc->physicsUpdate();
}