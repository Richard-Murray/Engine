#include "PointLightComponent.h"

void PointLightComponent::Initialise()
{
	SetComponentType("PointLight");
	SetDiffuse(glm::vec3(0, 0, 0));
	SetRadius(0);
	Show();
}

void PointLightComponent::Draw(BaseCamera* camera, unsigned int VAOCube)
{
	glm::vec4 viewSpacePosition = camera->GetView() * glm::vec4(m_parent->GetWorldTransform()[3].xyz, 1);

	unsigned int lightPositionUniform = glGetUniformLocation(m_shaderProgram, "lightPosition");
	unsigned int lightPositionViewUniform = glGetUniformLocation(m_shaderProgram, "lightPositionView");
	unsigned int lightRadiusUniform = glGetUniformLocation(m_shaderProgram, "lightRadius");
	unsigned int lightDiffuseUniform = glGetUniformLocation(m_shaderProgram, "lightDiffuse");

	glUniform3fv(lightPositionUniform, 1, &m_parent->GetWorldTransform()[3].xyz[0]);
	glUniform3fv(lightPositionViewUniform, 1, &viewSpacePosition[0]);
	glUniform1f(lightRadiusUniform, m_radius);
	glUniform3fv(lightDiffuseUniform, 1, &m_diffuse[0]);

	glBindVertexArray(VAOCube);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
}

void PointLightComponent::AttachShader(const char* idName)
{
	m_shaderProgram = m_shaderProgram = GetAssetManager()->GetShader(idName);
}

void PointLightComponent::SetDiffuse(glm::vec3 diffuse)
{
	m_diffuse = diffuse;
}

void PointLightComponent::SetRadius(float radius)
{
	m_radius = radius;
}

void PointLightComponent::Hide()
{
	m_hidden = true;
}

void PointLightComponent::Show()
{
	m_hidden = false;
}