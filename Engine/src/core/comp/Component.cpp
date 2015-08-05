#include "Component.h"
#include "../Entity.h"

Component::Component(Entity* parent)
{
	m_parent = parent;
}

Component::~Component()
{

}

void Component::Update(float deltaTime)
{

}

void Component::Draw(BaseCamera* camera)
{

}

//void Component::AddParentEntity(Entity* parent) 
//{ 
//	m_parent = parent; 
//}

const char* Component::GetID() 
{ 
	return m_parent->GetID(); 
}

AssetManager* Component::GetAssetManager() 
{ 
	return m_parent->GetAssetManager(); 
}

void Component::SetComponentType(const char* component)
{
	m_componentType = component;
}

const char* Component::GetComponentType()
{
	return m_componentType;
}