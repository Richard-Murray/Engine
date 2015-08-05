#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include "Entity.h"

#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "comp/RenderableComponent.h"
#include "comp/PointLightComponent.h"
#include "comp/PhysxPhysicsComponent.h"
#include "comp/PhysicsComponent.h"

//TODO: make directional light components

class EntityManager
{
public:
	EntityManager();
	~EntityManager();

	void Update(float deltaTime);
	void Draw(BaseCamera* camera);

	void CreateEntity(const char* idName);
	Entity* GetEntity(const char* idName);
	Entity* GetNewEntity();

	std::vector<RenderableComponent*>& GetRenderableComponentList();
	std::vector<PointLightComponent*>& GetPointLightComponentList();
	std::vector<PhysxPhysicsComponent*>& GetPhysxPhysicsComponentList();
	std::vector<PhysicsComponent*>& GetPhysicsComponentList();

	void AttachRenderable(Entity* pEntity, const char* model, const char* shader, const char* texture);
	void AttachPointLight(Entity* pEntity, const char* shader, glm::vec3 diffuse, float radius);
	void AttachPhysxPhysics(Entity* pEntity, /*const char* type, */PxPhysics* physics, PxScene* physicsScene);
	void AttachPhysics(Entity* pEntity);

	void DeleteEntities();

private:
	std::vector<Entity*> m_entityList;
	std::vector<RenderableComponent*> m_renderableComponentList;
	std::vector<PointLightComponent*> m_pointLightComponentList;
	std::vector<PhysxPhysicsComponent*> m_physxPhysicsComponentList;
	std::vector<PhysicsComponent*> m_physicsComponentList;
	
};


#endif