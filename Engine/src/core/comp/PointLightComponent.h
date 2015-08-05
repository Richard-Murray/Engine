#ifndef POINT_LIGHT_COMPONENT_H
#define POINT_LIGHT_COMPONENT_H

#include "Component.h"
#include "../Entity.h"

class PointLightComponent : public Component
{
public:
	PointLightComponent() = delete; //Component(nullptr) {}
	PointLightComponent(Entity* pEntity) : Component(pEntity) {}

	void Draw(BaseCamera* camera, unsigned int VAOCube);

	void Initialise();
	void AttachShader(const char* idName);
	void SetDiffuse(glm::vec3 diffuse);
	void SetRadius(float radius);

	void Hide();
	void Show();

private:
	glm::vec3 m_diffuse;

	float m_radius;

	unsigned int m_shaderProgram;

	bool m_hidden = false;
};

#endif