#pragma once

#include <chrono>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include <gecom/GL.hpp>

#include "Collider.hpp"
#include "Assets.hpp"
#include "Material.hpp"
#include "ComponentSystem.hpp"


namespace pxljm {


	//
	// Physics Update component
	//
	class PhysicsUpdatable: public virtual EntityComponent {
	public:
		virtual void registerWith(Scene &) override;
		virtual void deregisterWith(Scene &) override;

		virtual void physicsUpdate() = 0;
	};



	//
	// Physics component
	//
	class Physical : public virtual EntityComponent {
	public:
		virtual void addToDynamicsWorld(btDynamicsWorld *) = 0;
		virtual void removeFromDynamicsWorld() = 0;
	};


	// Rigid Body component
	//
	class RigidBody: public virtual Physical, private btMotionState  {
	private:

		collider_ptr m_collider = nullptr;
		btDynamicsWorld * m_world = nullptr;
		std::unique_ptr<btRigidBody> m_rigidBody;

		double m_mass = 1.0;
		bool m_kinematic = false;

	public:
		RigidBody();
		RigidBody(collider_ptr, double = 1.0);

		virtual void start() override;
		virtual void registerWith(Scene &) override;
		virtual void deregisterWith(Scene &) override;

		virtual void addToDynamicsWorld(btDynamicsWorld *) override;
		virtual void removeFromDynamicsWorld() override;

		virtual void onEnable(bool) override;

		btRigidBody * getRigidBody();


		void setKinematic(bool);
		bool isKinematic();

		// Properties
		void setCollider(collider_ptr);
		collider_ptr getCollider();

		// Anisotropic Friction
		void setAnisotropicFriction(i3d::vec3d);
		i3d::vec3d getAnisotropicFriction();

		// Friction
		void setFriction(double);
		double getFriction();

		// Rolling Friction
		void setRollingFriction(double);
		double getRollingFriction();

		// Bounceness
		void setRestitution(double);
		double getRestitution();

		// Damping (drag)
		void setDamping(double, double);
		double getLinearDamping();
		double getAngularDamping();

		// Manual scale for movement
		void setLinearFactor(i3d::vec3d);
		void setAngularFactor(i3d::vec3d);
		i3d::vec3d getLinearFactor();
		i3d::vec3d getAngularFactor();

		// Mass, Kinematic / Dynamic
		void setMass(double);
		double getMass();

		//TODO
		//center of mass
		//shape offset

		// Current state
		void setLinearVelocity(i3d::vec3d);
		void setAngularVelocity(i3d::vec3d);
		// void setLocalLinearVelocity(i3d::vec3d);
		// void setLocalAngularVelocity(i3d::vec3d);

		i3d::vec3d getLinearVelocity();
		i3d::vec3d getAngularVelocity();
		// i3d::vec3d getVelocityAtPoint(i3d::vec3d);
		// i3d::vec3d getLocalLinearVelocity();
		// i3d::vec3d getLocalAngularVelocity();
		// i3d::vec3d getLocalVelocityAtPoint(i3d::vec3d);


		// Dynamic Rigid Bodies
		void applyForce(i3d::vec3d, i3d::vec3d);
		void applyImpulse(i3d::vec3d);
		void applyImpulse(i3d::vec3d, i3d::vec3d);

		void applyTorque(i3d::vec3d);
		void applyTorqueImpulse(i3d::vec3d);

		void clearForces();

		// Bullet active state
		void wakeUp();
		bool isAwake();



		// Bullet Physics related methods btMotionState
		virtual void getWorldTransform (btTransform &) const override;
		virtual void setWorldTransform (const btTransform &) override;

	};


	//
	// Physics Trigger component
	//
	class Trigger : public virtual Physical, public virtual PhysicsUpdatable {
	private:
		collider_ptr m_collider = nullptr;
		btDynamicsWorld * m_world = nullptr;
		std::unique_ptr<btGhostObject> m_ghostObject;

	public:
		Trigger(collider_ptr);

		virtual void start() override;
		virtual void registerWith(Scene &) override;
		virtual void deregisterWith(Scene &) override;

		virtual void onEnable(bool) override;

		virtual void addToDynamicsWorld(btDynamicsWorld *) override;
		virtual void removeFromDynamicsWorld() override;

		virtual void physicsUpdate() override;

		// Properties
		void setCollider(collider_ptr);
		collider_ptr getCollider();
	};



	//
	// Physics Trigger Callback component
	//
	class CollisionEnterMessage : public Message {
		using Message::Message;
	};

	class CollisionMessage : public Message {
		using Message::Message;
	};

	class CollisionExitMessage : public Message {
		using Message::Message;
	};

	class CollisionCallback : public virtual EntityComponent {
	public:
		virtual void registerWith(Scene &) override;
		virtual void deregisterWith(Scene &) override;
		virtual void onAttachToEntity() override;
		virtual void onDetachFromEntity() override;
		virtual void onCollisionEnter(Entity *, CollisionEnterMessage &) { }
		virtual void onCollision(Entity *, CollisionMessage &) { }
		virtual void onCollisionExit(Entity *, CollisionExitMessage &) { }
	};



	//
	// Physics Trigger Callback component
	//
	class TriggerEnterMessage : public Message {
		using Message::Message;
	};

	class TriggerMessage : public Message {
		using Message::Message;
	};

	class TriggerExitMessage : public Message {
		using Message::Message;
	};

	class TriggerCallback : public virtual EntityComponent {
	public:
		virtual void registerWith(Scene &) override;
		virtual void deregisterWith(Scene &) override;
		virtual void onAttachToEntity() override;
		virtual void onDetachFromEntity() override;

		virtual void onTriggerEnter(Entity *, TriggerEnterMessage &) { }
		virtual void onTrigger(Entity *, TriggerMessage &) { }
		virtual void onTriggerExit(Entity *, TriggerExitMessage &) { }
	};







	//
	// Physics System Debug Drawer
	//
	class PhysicsDebugDrawer : public btIDebugDraw {
	private:
		GLuint m_vao = 0;
		GLuint m_vbo_pos = 0;
		GLuint m_vbo_col = 0;

		std::vector<float> m_position;
		std::vector<float> m_color;

		shader_ptr m_shader;

		int m_mode = 0;

	public:
		PhysicsDebugDrawer();
		~PhysicsDebugDrawer() { };

		virtual void draw(i3d::mat4d, i3d::mat4d, double);

		virtual void drawLine (const btVector3 &, const btVector3 &, const btVector3 &);
		virtual void drawLine (const btVector3 &, const btVector3 &, const btVector3 &, const btVector3 &);
		virtual void drawContactPoint (const btVector3 &, const btVector3 &, btScalar, int, const btVector3 &);
		virtual void reportErrorWarning (const char *);
		virtual void draw3dText (const btVector3 &, const char *);
		virtual void setDebugMode (int);
		virtual int getDebugMode () const;
	};




	// 
	// Component System for Physcial Components
	// 
	class PhysicsSystem : public ComponentSystem {
	public:
		using clock_t = std::chrono::steady_clock;

	private:
		PhysicsDebugDrawer m_debugDrawer;

		clock_t::time_point m_lastTick;

		// Physics collections
		std::unordered_set<PhysicsUpdatable *> m_physicsUpdatables;
		std::unordered_set<RigidBody *> m_rigidbodies;
		std::unordered_set<Trigger *> m_triggers;

		// internal collision sets
		bool m_collisionsA_isCurrent = true;
		std::unordered_set<std::pair<const btCollisionObject *, const btCollisionObject *>> m_currentFrame;
		std::unordered_set<std::pair<const btCollisionObject *, const btCollisionObject *>> m_lastFrame;

		// physics internals
		btBroadphaseInterface* broadphase;
		btDefaultCollisionConfiguration* collisionConfiguration;
		btCollisionDispatcher* dispatcher;
		btSequentialImpulseConstraintSolver* solver;

		btDiscreteDynamicsWorld* dynamicsWorld;


	public:
		PhysicsSystem();
		virtual ~PhysicsSystem();

		void registerPhysicsUpdatable(PhysicsUpdatable *);
		void deregisterPhysicsUpdatable(PhysicsUpdatable *);

		void registerRigidBody(RigidBody *);
		void deregisterRigidBody(RigidBody *);

		void registerTrigger(Trigger *);
		void deregisterTrigger(Trigger *);

		void resetClock();
		void tick();

		clock_t::time_point lastTick();

		void debugDraw(Scene &);

		void processPhysicsCallback(btScalar);
	};
}