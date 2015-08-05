#ifndef PHYSICS_MANAGER_H
#define PHYSICS_MANAGER_H

#include "comp\PhysicsComponent.h"

#include <vector>

class EntityManager;

class PhysicsManager
{
public:
	PhysicsManager();
	~PhysicsManager();

	void Initialise(EntityManager* pEntityManager);

	void Update(float deltaTime);

	void CheckForCollision();

	static bool Plane2Plane		(PhysicsComponent* object1, PhysicsComponent* object2); //UNUSED
	static bool Plane2Sphere	(PhysicsComponent* object1, PhysicsComponent* object2); //IMMEDIATE
	static bool Plane2AABB		(PhysicsComponent* object1, PhysicsComponent* object2); //IMMEDIATE
	static bool Plane2Box		(PhysicsComponent* object1, PhysicsComponent* object2); //TODO

	static bool Sphere2Plane	(PhysicsComponent* object1, PhysicsComponent* object2);
	static bool Sphere2Sphere	(PhysicsComponent* object1, PhysicsComponent* object2);
	static bool Sphere2AABB		(PhysicsComponent* object1, PhysicsComponent* object2);
	static bool Sphere2Box		(PhysicsComponent* object1, PhysicsComponent* object2); //TODO

	static bool AABB2Plane		(PhysicsComponent* object1, PhysicsComponent* object2); //IMMEDIATE
	static bool AABB2Sphere		(PhysicsComponent* object1, PhysicsComponent* object2);
	static bool AABB2AABB		(PhysicsComponent* object1, PhysicsComponent* object2);
	static bool AABB2Box		(PhysicsComponent* object1, PhysicsComponent* object2); //TODO

	static bool Box2Plane		(PhysicsComponent* object1, PhysicsComponent* object2); //TODO
	static bool Box2Sphere		(PhysicsComponent* object1, PhysicsComponent* object2); //TODO
	static bool Box2AABB		(PhysicsComponent* object1, PhysicsComponent* object2); //TODO
	static bool Box2Box			(PhysicsComponent* object1, PhysicsComponent* object2); //TODO


private:
	EntityManager* m_pEntityManager;
	std::vector<PhysicsComponent*>* m_pPhysicsComponentList;

};

#endif