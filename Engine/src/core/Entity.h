#ifndef ENTITY_H
#define ENTITY_H

#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Model.h"
#include "AssetManager.h"

#include "comp\Component.h"

class Entity
{
public:
	Entity(const char* idName);
	~Entity();

	void Initialise(AssetManager* pAssetManager);

	/*void AttachModel(const char* idName);
	void AttachShader(const char* idName);
	void AttachTexture(const char* idName);*/
	virtual void Update(float deltaTime);
	/*virtual void Draw(BaseCamera* camera);*/

	void SetWorldTransform(glm::mat4 transform);
	void SetWorldTranslation(glm::vec3 translate);
	glm::mat4 GetWorldTransform();
	AssetManager* GetAssetManager(); //helper for components
	/*void TestMoveTransform(glm::mat4 transform);*/

	const char* GetID();
	std::vector<Component*>& GetComponentList();
	Component* GetComponentOfType(const char* type);

	/*void Hide();
	void Show();*/

	//Component* m_pRenderableComponent; //test

private:
	glm::mat4 m_worldTransform;

	const char* m_idName;

	AssetManager* m_pAssetManager;

	std::vector<Component*> m_componentList;
	
	/*Model* m_model;
	unsigned int m_shaderProgram;
	unsigned int m_diffuseTexture;

	bool m_attachedModel = false;
	bool m_attachedShader = false;
	bool m_attachedDiffuse = false;

	bool m_hidden = false;*/
};

#endif