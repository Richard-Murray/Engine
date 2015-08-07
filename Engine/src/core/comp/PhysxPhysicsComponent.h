#ifndef PHYSX_PHYSICS_COMPONENT_H
#define PHYSX_PHYSICS_COMPONENT_H

#include "Component.h"
#include "../Entity.h"

#include <PxPhysicsAPI.h>
#include <PxScene.h>
#include <pvd/PxVisualDebugger.h>

using namespace physx;

class PhysxPhysicsComponent : public Component
{
public:
	PhysxPhysicsComponent() = delete; //Component(nullptr) {}
	PhysxPhysicsComponent(Entity* pEntity) : Component(pEntity) {}
	~PhysxPhysicsComponent();

	void Update(float deltaTime);

	void Initialise();
	void SetName(const char* name, bool dynamic);
	void SetPhysicsScene(PxPhysics* physics, PxScene* physicsScene);
	void SetPhysicsObjectType(const char* type);
	void SetBox(glm::vec3 boxGeometry, float density, bool dynamic);
	void SetSphere(float radius, float density, bool dynamic);
	void SetCapsule(float radius, float halfHeight, float density, bool dynamic);
	void AttachRigidBodyConvex(FBXFile* m_FBX, float density, PxMaterial* g_PhysicsMaterial, PxCooking* physicsCooker);
	//void SetCapsule()

	//void SetSphere(float radius);
	//void SetRadius(float radius);

private:
	PxPhysics* m_physics;
	PxScene* m_physicsScene;
	PxMaterial* m_material;

	PxRigidDynamic* m_dynamicActor;
	PxRigidStatic* m_staticActor;
	
	float m_radius;
	const char* m_physicsObjectType;
	bool m_dynamic;
};

#endif