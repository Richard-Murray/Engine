#include "PhysicsManager.h"
#include "EntityManager.h"

PhysicsManager::PhysicsManager()
{

}

PhysicsManager::~PhysicsManager()
{

}

void PhysicsManager::Initialise(EntityManager* pEntityManager)
{
	m_pEntityManager = pEntityManager;
	m_pPhysicsComponentList = &m_pEntityManager->GetPhysicsComponentList();
}

void PhysicsManager::Update(float deltaTime)
{
	CheckForCollision();
}

//function pointer array for doing our collisions
typedef bool(*fn)(PhysicsComponent*, PhysicsComponent*);

//function pointer array for doing our collisions
static fn collisionfunctionArray[] =
{
	PhysicsManager::Plane2Plane,	PhysicsManager::Plane2Sphere,	PhysicsManager::Plane2AABB,		PhysicsManager::Plane2Box,
	PhysicsManager::Sphere2Plane,	PhysicsManager::Sphere2Sphere,	PhysicsManager::Sphere2AABB,	PhysicsManager::Sphere2Box,
	PhysicsManager::AABB2Plane,		PhysicsManager::AABB2Sphere,	PhysicsManager::AABB2AABB,		PhysicsManager::AABB2Box,
	PhysicsManager::Box2Plane,		PhysicsManager::Box2Sphere, PhysicsManager::Box2AABB,			PhysicsManager::Box2Box
};

void PhysicsManager::CheckForCollision()
{
	int actorCount = m_pPhysicsComponentList->size();
	//need to check for collisions against all objects except this one.
	for (int outer = 0; outer < actorCount - 1; outer++)
	{
		for (int inner = outer + 1; inner < actorCount; inner++)
		{
			PhysicsComponent* object1 = (*m_pPhysicsComponentList)[outer];
			PhysicsComponent* object2 = (*m_pPhysicsComponentList)[inner];
			int _shapeID1 = (int)object1->GetObjectType();
			int _shapeID2 = (int)object2->GetObjectType();
			//using function pointers
			int functionIndex = (_shapeID1 * (int)PHYSICSOBJECT::NUMSHAPES) + _shapeID2;
			fn collisionFunctionPtr = collisionfunctionArray[functionIndex];
			if (collisionFunctionPtr != NULL)
			{
				collisionFunctionPtr(object1, object2);
			}
		}
	}
}

bool PhysicsManager::Plane2Plane(PhysicsComponent* object1, PhysicsComponent* object2)
{
	return true;
}
bool PhysicsManager::Plane2Sphere(PhysicsComponent* object1, PhysicsComponent* object2)
{
	glm::vec3 collisionNormal = object1->GetNormal();
	float sphereDistanceToPlane = glm::dot(object2->GetPosition(), object1->GetNormal()) - object1->GetDistance();
	if (sphereDistanceToPlane < 0)
	{
		collisionNormal *= -1;
		sphereDistanceToPlane *= -1;
	}
	float intersection = object2->GetRadius() - sphereDistanceToPlane;
	if (intersection > 0)
	{
		glm::vec3 planeNormal = object1->GetNormal(); //isn't this redundant?
		if (sphereDistanceToPlane < 0)
		{
			planeNormal *= -1;
		}
		glm::vec3 forceVector = -1 * object2->GetMass() * planeNormal * (glm::dot(planeNormal, object2->GetVelocity()));
		object2->ApplyForce(2 * forceVector);
		object2->SetPosition(object2->GetPosition() + collisionNormal * intersection * 0.5f);
		//object1->SetVelocity(glm::vec3(0)); //debug stuff
		return true;
	}
	else
	{
		return false;
	}
}
bool PhysicsManager::Plane2AABB(PhysicsComponent* object1, PhysicsComponent* object2)
{
	return true;
}
bool PhysicsManager::Plane2Box(PhysicsComponent* object1, PhysicsComponent* object2)
{
	return true;
}

bool PhysicsManager::Sphere2Plane(PhysicsComponent* object1, PhysicsComponent* object2)
{
	glm::vec3 collisionNormal = object2->GetNormal();
	float sphereDistanceToPlane = glm::dot(object1->GetPosition(), object2->GetNormal()) - object2->GetDistance();
	if (sphereDistanceToPlane < 0)
	{
		collisionNormal *= -1;
		sphereDistanceToPlane *= -1;
	}
	float intersection = object1->GetRadius() - sphereDistanceToPlane;
	if (intersection > 0)
	{
		glm::vec3 planeNormal = object2->GetNormal(); //isn't this redundant?
		if (sphereDistanceToPlane < 0)
		{
			planeNormal *= -1;
		}
		glm::vec3 forceVector = -1 * object1->GetMass() * planeNormal * (glm::dot(planeNormal, object1->GetVelocity()));
		object1->ApplyForce(2 * forceVector);
		object1->SetPosition(object1->GetPosition() + collisionNormal * intersection * 0.5f);
		//object1->SetVelocity(glm::vec3(0)); //debug stuff
		return true;
	}
	else
	{
		return false;
	}
}
bool PhysicsManager::Sphere2Sphere(PhysicsComponent* object1, PhysicsComponent* object2)
{
	if (glm::distance(object2->GetPosition(), object1->GetPosition()) < object1->GetRadius() + object2->GetRadius())
	{
		glm::vec3 delta = object2->GetPosition() - object1->GetPosition();
		float distance = glm::length(delta);
		float intersection = object1->GetRadius() + object2->GetRadius() - distance;
		if (intersection > 0)
		{
			glm::vec3 collisionNormal = glm::normalize(delta);
			glm::vec3 relativeVelocity = object1->GetVelocity() - object2->GetVelocity();
			glm::vec3 collisionVector = collisionNormal *(glm::dot(relativeVelocity, collisionNormal));
			glm::vec3 forceVector = collisionVector * 1.0f / (1 / object1->GetMass() + 1 / object2->GetMass());
			//use newtons third law to apply colision forces to colliding bodies.
			//object1->applyForceToActor(object2, 2 * forceVector);
			object2->ApplyForce(forceVector * 2);
			object1->ApplyForce(-forceVector * 2);
			//move our spheres out of collision
			glm::vec3 seperationVector = collisionNormal * intersection *.5f;

			if (!object1->GetStatic() && !object2->GetStatic())
			{
				object1->SetPosition(object1->GetPosition() - seperationVector);// -= seperationVector; //uh oh
				object2->SetPosition(object2->GetPosition() + seperationVector);
			}
			if (!object1->GetStatic() && object2->GetStatic())
			{
				object1->SetPosition(object1->GetPosition() - seperationVector);
			}
			if (object1->GetStatic() && !object2->GetStatic())
			{
				object2->SetPosition(object2->GetPosition() + seperationVector);
			}
			//if (!object1->GetStatic())
			//{
			//	object1->SetPosition(object1->GetPosition() - seperationVector);// -= seperationVector; //uh oh
			//}
			//if (!object2->GetStatic())
			//{
			//	object2->SetPosition(object2->GetPosition() + seperationVector);
			//}
			//object2->SetPosition() += seperationVector;
			return true;
		}
		return true;
	}
	else
	{
		return false;
	}
}
bool PhysicsManager::Sphere2AABB(PhysicsComponent* object1, PhysicsComponent* object2)
{
	float obj2minX = object2->GetPosition().x - object2->GetBoxGeometry().x / 2;
	float obj2minY = object2->GetPosition().y - object2->GetBoxGeometry().y / 2;
	float obj2minZ = object2->GetPosition().z - object2->GetBoxGeometry().z / 2;
					   							  
	float obj2maxX = object2->GetPosition().x + object2->GetBoxGeometry().x / 2;
	float obj2maxY = object2->GetPosition().y + object2->GetBoxGeometry().y / 2;
	float obj2maxZ = object2->GetPosition().z + object2->GetBoxGeometry().z / 2;

	float squaredDistance = 0;

	if (object1->GetPosition().x < obj2minX)
	{
		float diff = object1->GetPosition().x - obj2minX;
		squaredDistance += diff * diff;
	}
	else if (object1->GetPosition().x > obj2maxX)
	{
		float diff = object1->GetPosition().x - obj2maxX;
		squaredDistance += diff * diff;
	}

	if (object1->GetPosition().y < obj2minY) 
	{
		float diff = object1->GetPosition().y - obj2minY;
		squaredDistance += diff * diff;
	}

	else if (object1->GetPosition().y > obj2maxY) 
	{
		float diff = object1->GetPosition().y - obj2maxY;
		squaredDistance += diff * diff;
	}

	if (object1->GetPosition().z < obj2minZ) 
	{
		float diff = object1->GetPosition().z - obj2minZ;
		squaredDistance += diff * diff;
	}

	else if (object1->GetPosition().z > obj2maxZ) 
	{
		float diff = object1->GetPosition().z - obj2maxZ;
		squaredDistance += diff * diff;
	}

	if (squaredDistance <= object1->GetRadius())
	{
		//object1->ApplyForce(glm::vec3(0, -1, 0)); //debug
		return true;
	}
	else
	{
		return false;
	}
}
bool PhysicsManager::Sphere2Box(PhysicsComponent* object1, PhysicsComponent* object2)
{
	return true;
}

bool PhysicsManager::AABB2Plane(PhysicsComponent* object1, PhysicsComponent* object2)
{
	float obj1minX = object1->GetPosition().x - object1->GetBoxGeometry().x / 2;
	float obj1minY = object1->GetPosition().y - object1->GetBoxGeometry().y / 2;
	float obj1minZ = object1->GetPosition().z - object1->GetBoxGeometry().z / 2;

	float obj1maxX = object1->GetPosition().x + object1->GetBoxGeometry().x / 2;
	float obj1maxY = object1->GetPosition().y + object1->GetBoxGeometry().y / 2;
	float obj1maxZ = object1->GetPosition().z + object1->GetBoxGeometry().z / 2;

	std::vector<glm::vec3> points;

	points.push_back(glm::vec3(obj1minX, obj1minY, obj1minZ));
	points.push_back(glm::vec3(obj1minX, obj1maxY, obj1minZ));
	points.push_back(glm::vec3(obj1maxX, obj1maxY, obj1minZ));
	points.push_back(glm::vec3(obj1maxX, obj1minY, obj1minZ));

	points.push_back(glm::vec3(obj1minX, obj1minY, obj1maxZ));
	points.push_back(glm::vec3(obj1minX, obj1maxY, obj1maxZ));
	points.push_back(glm::vec3(obj1maxX, obj1maxY, obj1maxZ));
	points.push_back(glm::vec3(obj1maxX, obj1minY, obj1maxZ));

	/*glm::vec3 point1 = glm::vec3(obj1minX, obj1minY, obj1minZ);
	glm::vec3 point2 = glm::vec3(obj1minX, obj1maxY, obj1minZ);
	glm::vec3 point3 = glm::vec3(obj1maxX, obj1maxY, obj1minZ);
	glm::vec3 point4 = glm::vec3(obj1maxX, obj1minY, obj1minZ);

	glm::vec3 point5 = glm::vec3(obj1minX, obj1minY, obj1maxZ);
	glm::vec3 point6 = glm::vec3(obj1minX, obj1maxY, obj1maxZ);
	glm::vec3 point7 = glm::vec3(obj1maxX, obj1maxY, obj1maxZ);
	glm::vec3 point8 = glm::vec3(obj1maxX, obj1minY, obj1maxZ);*/

	for (auto iter = points.begin(); iter != points.end(); iter++)
	{
		if (glm::dot((*iter), object2->GetNormal()) < object2->GetDistance())
		{
			object1->SetVelocity(glm::vec3(0));
			return true;
		}
	}
	return false;

	/*glm::vec3 vec1, vec2;

	if (object2->GetNormal().x >= 0)
	{
		vec1.x = obj1minX;
		vec2.x = obj1maxX;
	}
	else
	{
		vec1.x = obj1maxX;
		vec2.x = obj1minX;
	}
	if (object2->GetNormal().y >= 0)
	{
		vec1.y = obj1minY;
		vec2.y = obj1maxY;
	}
	else
	{
		vec1.y = obj1maxY;
		vec2.y = obj1minY;
	}
	if (object2->GetNormal().z >= 0)
	{
		vec1.z = obj1minZ;
		vec2.z = obj1maxZ;
	}
	else
	{
		vec1.z = obj1maxZ;
		vec2.z = obj1minZ;
	}
	float posSide = (object2->GetNormal().x * vec2.x) + (object2->GetNormal().y * vec2.y) + (object2->GetNormal().z * vec2.z) + object2->GetDistance();
	if (posSide > 0)
	{
		//box is completely on positive side of plane
		//return false;
	}
	float negSide = (object2->GetNormal().x * vec1.x) + (object2->GetNormal().y * vec1.y) + (object2->GetNormal().z * vec1.z) + object2->GetDistance();
	if (negSide < 0)
	{
		//box is completely on negative side of plane
		return false;
	}*/

	
	return true;
}
bool PhysicsManager::AABB2Sphere(PhysicsComponent* object1, PhysicsComponent* object2)
{
	float obj1minX = object1->GetPosition().x - object1->GetBoxGeometry().x / 2;
	float obj1minY = object1->GetPosition().y - object1->GetBoxGeometry().y / 2;
	float obj1minZ = object1->GetPosition().z - object1->GetBoxGeometry().z / 2;

	float obj1maxX = object1->GetPosition().x + object1->GetBoxGeometry().x / 2;
	float obj1maxY = object1->GetPosition().y + object1->GetBoxGeometry().y / 2;
	float obj1maxZ = object1->GetPosition().z + object1->GetBoxGeometry().z / 2;
	
	float squaredDistance = 0;

	if (object2->GetPosition().x < obj1minX)
	{
		float diff = object2->GetPosition().x - obj1minX;
		squaredDistance += diff * diff;
	}
	else if (object2->GetPosition().x > obj1maxX)
	{
		float diff = object2->GetPosition().x - obj1maxX;
		squaredDistance += diff * diff;
	}

	if (object2->GetPosition().y < obj1minY) {
		float diff = object2->GetPosition().y - obj1minY;
		squaredDistance += diff * diff;
	}

	else if (object2->GetPosition().y > obj1maxY) {
		float diff = object2->GetPosition().y - obj1maxY;
		squaredDistance += diff * diff;
	}
    
	if (object2->GetPosition().z < obj1minZ) {
		float diff = object2->GetPosition().z - obj1minZ;
		squaredDistance += diff * diff;
	}

	else if (object2->GetPosition().z > obj1maxZ) {
		float diff = object2->GetPosition().z - obj1maxZ;
		squaredDistance += diff * diff;
	}

	if (squaredDistance <= object2->GetRadius())
	{
		object2->ApplyForce(glm::vec3(0, -1, 0)); //debug
		return true;
	}
	else
	{
		return false;
	}
}
bool PhysicsManager::AABB2AABB(PhysicsComponent* object1, PhysicsComponent* object2)
{
	float obj1minX = object1->GetPosition().x - object1->GetBoxGeometry().x / 2;
	float obj1minY = object1->GetPosition().y - object1->GetBoxGeometry().y / 2;
	float obj1minZ = object1->GetPosition().z - object1->GetBoxGeometry().z / 2;

	float obj1maxX = object1->GetPosition().x + object1->GetBoxGeometry().x / 2;
	float obj1maxY = object1->GetPosition().y + object1->GetBoxGeometry().y / 2;
	float obj1maxZ = object1->GetPosition().z + object1->GetBoxGeometry().z / 2;

	float obj2minX = object2->GetPosition().x - object2->GetBoxGeometry().x / 2;
	float obj2minY = object2->GetPosition().y - object2->GetBoxGeometry().y / 2;
	float obj2minZ = object2->GetPosition().z - object2->GetBoxGeometry().z / 2;

	float obj2maxX = object2->GetPosition().x + object2->GetBoxGeometry().x / 2;
	float obj2maxY = object2->GetPosition().y + object2->GetBoxGeometry().y / 2;
	float obj2maxZ = object2->GetPosition().z + object2->GetBoxGeometry().z / 2;

	if (obj1minX < obj2maxX
		&&	obj1maxX > obj2minX
		&&	obj1minY < obj2maxY
		&&	obj1maxY > obj2minY
		&&	obj1minZ < obj2maxZ
		&&	obj1maxZ > obj2minZ)
	{		
		object2->ApplyForce(glm::vec3(0, -1, 0)); //debug
		return true;
	}
	else
	{
		return false;
	}
}
bool PhysicsManager::AABB2Box(PhysicsComponent* object1, PhysicsComponent* object2)
{
	return true;
}

bool PhysicsManager::Box2Plane(PhysicsComponent* object1, PhysicsComponent* object2)
{
	return true;
}
bool PhysicsManager::Box2Sphere(PhysicsComponent* object1, PhysicsComponent* object2)
{
	return true;
}
bool PhysicsManager::Box2AABB(PhysicsComponent* object1, PhysicsComponent* object2)
{
	return true;
}
bool PhysicsManager::Box2Box(PhysicsComponent* object1, PhysicsComponent* object2)
{
	return true;
}