#include "EntityManager.h"

EntityManager::EntityManager()
{

}

EntityManager::~EntityManager()
{
	//delete entities, entities delete components
	for (int i = 0; i < m_entityList.size(); i++)
	{
		delete m_entityList[i];
	}
	m_entityList.clear();
	//then clear every component list
	m_renderableComponentList.clear();
	m_pointLightComponentList.clear();
	m_physxPhysicsComponentList.clear();
	m_physicsComponentList.clear();
}

void EntityManager::Update(float deltaTime)
{
	for (auto iter = m_entityList.begin(); iter != m_entityList.end(); iter++)
	{
		(*iter)->Update(deltaTime);
	}

	for (auto iter = GetPhysxPhysicsComponentList().begin(); iter != GetPhysxPhysicsComponentList().end(); iter++)
	{
		(*iter)->Update(deltaTime);
	}

	for (auto iter = GetPhysicsComponentList().begin(); iter != GetPhysicsComponentList().end(); iter++)
	{
		(*iter)->Update(deltaTime);
	}
}

//void EntityManager::Draw(BaseCamera* camera)
//{
//	for (auto iter = m_entityList.begin(); iter != m_entityList.end(); iter++)
//	{
//		//(*iter)->Draw(camera);
//	}
//}

void EntityManager::CreateEntity(const char* idName)
{
	m_entityList.push_back(new Entity(idName));
}

Entity* EntityManager::GetEntity(const char* idName)
{
	for (auto iter = m_entityList.begin(); iter != m_entityList.end(); iter++)
	{
		if ((*iter)->GetID() == idName)
			return (*iter);
	}

	return nullptr;
}

Entity* EntityManager::GetNewEntity()
{
	return m_entityList.back();
}

std::vector<RenderableComponent*>& EntityManager::GetRenderableComponentList()
{
	return m_renderableComponentList;
}

std::vector<PointLightComponent*>& EntityManager::GetPointLightComponentList()
{
	return m_pointLightComponentList;
}

std::vector<PhysxPhysicsComponent*>& EntityManager::GetPhysxPhysicsComponentList()
{
	return m_physxPhysicsComponentList;
}

std::vector<PhysicsComponent*>& EntityManager::GetPhysicsComponentList()
{
	return m_physicsComponentList;
}

void EntityManager::AttachRenderable(Entity* pEntity, const char* model, const char* shader, const char* texture)
{
	m_renderableComponentList.push_back(new RenderableComponent(pEntity));
	m_renderableComponentList.back()->Initialise();
	m_renderableComponentList.back()->AttachModel(model);
	m_renderableComponentList.back()->AttachShader(shader);
	m_renderableComponentList.back()->AttachTexture(texture);
	pEntity->GetComponentList().push_back(m_renderableComponentList.back());
}

void EntityManager::AttachPointLight(Entity* pEntity, const char* shader, glm::vec3 diffuse, float radius)
{
	m_pointLightComponentList.push_back(new PointLightComponent(pEntity));
	m_pointLightComponentList.back()->Initialise();
	m_pointLightComponentList.back()->AttachShader(shader);
	m_pointLightComponentList.back()->SetDiffuse(diffuse);
	m_pointLightComponentList.back()->SetRadius(radius);
	pEntity->GetComponentList().push_back(m_pointLightComponentList.back());

}

void EntityManager::AttachPhysxPhysics(Entity* pEntity, /*const char* type, */PxPhysics* physics, PxScene* physicsScene)
{
	m_physxPhysicsComponentList.push_back(new PhysxPhysicsComponent(pEntity));
	m_physxPhysicsComponentList.back()->Initialise();
	m_physxPhysicsComponentList.back()->SetPhysicsScene(physics, physicsScene);
	//m_physxPhysicsComponentList.back()->SetBox(glm::vec3(0.5f, 0.5f, 0.5f), 10);
	pEntity->GetComponentList().push_back(m_physxPhysicsComponentList.back());
}

void EntityManager::AttachPhysics(Entity* pEntity)
{
	m_physicsComponentList.push_back(new PhysicsComponent(pEntity));
	m_physicsComponentList.back()->Initialise();
	pEntity->GetComponentList().push_back(m_physicsComponentList.back());
}

void EntityManager::DeleteEntities()
{
	//delete entities, entities delete components
	for (int i = 0; i < m_entityList.size(); i++)
	{
		delete m_entityList[i];
	}
	m_entityList.clear();
	//then clear every component list
	m_renderableComponentList.clear();
	m_pointLightComponentList.clear();
	m_physxPhysicsComponentList.clear();
	m_physicsComponentList.clear();
}