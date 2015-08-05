#include "PhysicsComponent.h"

PhysicsComponent::~PhysicsComponent()
{

}

void PhysicsComponent::Initialise()
{
	SetComponentType("Physics");
	m_physicsTransform = m_parent->GetWorldTransform();
	m_position = m_parent->GetWorldTransform()[3].xyz;
	m_velocity = glm::vec3(0);
	m_rotation = glm::vec3(0, 1, 0);
	m_drag = 0.99f;
}

void PhysicsComponent::SetStatic(bool boolStatic)
{
	m_static = boolStatic;
}

void PhysicsComponent::Update(float deltaTime)
{
	if (!m_static)
	{
		m_velocity += glm::vec3(0, -10, 0) * deltaTime;
		m_velocity *= m_drag;

		m_position += m_velocity * deltaTime;
	}
	else
	{
		m_velocity = glm::vec3(0);
	}

	m_physicsTransform[3].xyz = m_position;
	//m_physicsTransform = glm::rotate(m_physicsTransform, );
	m_parent->SetWorldTransform(m_physicsTransform);

	//glm::mat4 rotMatrix = <calculate rotation matrix based on angular velocity>



	//calc physicstransform
	//m_parent->SetWorldTransform(m_physicsTransform);
	//m_parent->SetWorldTranslation(m_parent->GetWorldTransform()[3].xyz + glm::vec3(0, -0.1f, 0));
}

void PhysicsComponent::SetSphere(float mass, float radius)
{
	m_objectType = PHYSICSOBJECT::SPHERE;
	m_mass = mass;
	m_radius = radius;
	m_rotation = glm::vec3(0, 1, 0);
}

void PhysicsComponent::SetPlane(glm::vec3 normal, float distance)
{
	m_objectType = PHYSICSOBJECT::PLANE;
	m_normal = normal;
	m_distance = distance;
}

void PhysicsComponent::SetAABB(float mass, glm::vec3 boxGeometry)
{
	m_objectType = PHYSICSOBJECT::AABB;
	m_mass = mass;
	m_boxGeometry = boxGeometry;
}

void PhysicsComponent::SetBox(float mass, glm::vec3 boxGeometry, glm::vec3 rotation)
{
	m_objectType = PHYSICSOBJECT::BOX;
	m_mass = mass;
	m_boxGeometry = boxGeometry;
	m_rotation = rotation;
}

PHYSICSOBJECT PhysicsComponent::GetObjectType()
{
	return m_objectType;
}

void PhysicsComponent::ApplyForce(glm::vec3 force)
{
	m_velocity += force / m_mass;
}

void PhysicsComponent::SetPosition(glm::vec3 position)
{
	m_position = position;
}

void PhysicsComponent::SetVelocity(glm::vec3 velocity)
{
	m_velocity = velocity;
}