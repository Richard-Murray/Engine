#ifndef RENDERABLE_COMPONENT_H
#define RENDERABLE_COMPONENT_H

#include "Component.h"
#include "../Entity.h"

class RenderableComponent : public Component
{
public:
	RenderableComponent() = delete; //Component(nullptr) {}
	RenderableComponent(Entity* pEntity) : Component(pEntity) {}

	void Draw(BaseCamera* camera);

	void Initialise(/*AssetManager* pAssetManager*/);

	void AttachModel(const char* idName);
	void AttachShader(const char* idName);
	void AttachTexture(const char* idName);

	void Hide();
	void Show();

	void SetRenderableTransform(glm::mat4 transform);

private:

	Model* m_model;
	
	glm::mat4 m_renderableTransform;

	unsigned int m_shaderProgram;
	unsigned int m_diffuseTexture;

	bool m_attachedModel = false;
	bool m_attachedShader = false;
	bool m_attachedDiffuse = false;

	bool m_hidden = false;
};

#endif