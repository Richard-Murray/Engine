#ifndef PHYSICS_COMPONENT_H
#define PHYSICS_COMPONENT_H

#include "Component.h"
#include "../Entity.h"

enum class PHYSICSOBJECT { PLANE, SPHERE, AABB, BOX, NUMSHAPES, NONE};

class PhysicsComponent : public Component
{
public:
	PhysicsComponent() = delete; //Component(nullptr) {}
	PhysicsComponent(Entity* pEntity) : Component(pEntity) {}
	~PhysicsComponent();

	void Update(float deltaTime);

	void Initialise();
	void SetStatic(bool boolStatic);
	void SetPlane(glm::vec3 normal, float distance);
	void SetSphere(float mass, float radius);	
	void SetAABB(float mass, glm::vec3 boxGeometry);
	void SetBox(float mass, glm::vec3 boxGeometry, glm::vec3 rotation);	

	PHYSICSOBJECT GetObjectType();

	void ApplyForce(glm::vec3 force);

	void SetPosition(glm::vec3 position);
	void SetVelocity(glm::vec3 velocity);

	glm::vec3 GetPosition(){ return m_position; };
	glm::vec3 GetVelocity(){ return m_velocity; };
	float GetMass(){ return m_mass; };
	glm::vec3 GetNormal(){ return m_normal; };
	glm::vec3 GetBoxGeometry(){ return m_boxGeometry; };
	float GetRadius(){ return m_radius; };
	float GetDistance(){ return m_distance; };
	bool GetStatic(){ return m_static; };

private:
	glm::mat4 m_physicsTransform;

	PHYSICSOBJECT m_objectType;
	glm::vec3 m_position;
	glm::vec3 m_velocity;
	glm::vec3 m_rotation;
	glm::vec3 m_normal; //Plane
	glm::vec3 m_boxGeometry;

	float m_mass;
	float m_drag;
	float m_radius;
	float m_distance; //Plane

	bool m_static;

	//glm::vec3 m_boxGeometry;
	//float m_radius;
};

class SpringJoint
{
public:
	SpringJoint(PhysicsComponent* connection1, PhysicsComponent* connection2, float springCoefficient, float damping)
	{
		_connections[0] = connection1;
		_connections[1] = connection2;
		_springCoefficient = springCoefficient;
		_damping = damping;
		_restLength = glm::length(_connections[0]->GetPosition() - _connections[1]->GetPosition());
	}

	void virtual Update(glm::vec3 gravity, float timeStep)
	{
		float displacement = glm::length(_connections[0]->GetPosition() - _connections[1]->GetPosition()) - _restLength;
		float force = -_springCoefficient * displacement;

		_connections[1]->ApplyForce(-glm::vec3(_connections[0]->GetPosition() - _connections[1]->GetPosition()) * force);

	}

private:
	//void virtual debug();
	PhysicsComponent* _connections[2];
	float _damping;
	float _restLength;
	float _springCoefficient;

};

#endif