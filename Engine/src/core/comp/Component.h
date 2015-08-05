#ifndef COMPONENT_H
#define COMPONENT_H

//#include "../Entity.h"
#include "../Camera.h"
#include "../AssetManager.h"

class Entity;

class Component
{
public:
	Component(Entity* parent);
	virtual ~Component();

	virtual void Update(float deltaTime);
	virtual void Draw(BaseCamera* camera);
	/*void AddParentEntity(Entity* parent);*/
	const char* GetID();
	AssetManager* GetAssetManager();
	void SetComponentType(const char* componentType);
	const char* GetComponentType();

protected:
	Entity* m_parent;
	const char* m_componentType;
};

#endif