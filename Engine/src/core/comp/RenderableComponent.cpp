#include "RenderableComponent.h"

void RenderableComponent::Initialise()
{
	SetComponentType("Renderable");
	m_diffuseTexture = GetAssetManager()->GetTexture("Default");
	m_attachedDiffuse = true;
	m_renderableTransform = glm::mat4(1);
}

void RenderableComponent::AttachModel(const char* idName)
{
	m_model = GetAssetManager()->GetModel(idName);
	m_attachedModel = true;
}

void RenderableComponent::AttachShader(const char* idName)
{
	m_shaderProgram = GetAssetManager()->GetShader(idName);
	m_attachedShader = true;
}

void RenderableComponent::AttachTexture(const char* idName)
{
	m_diffuseTexture = GetAssetManager()->GetTexture(idName);
	m_attachedDiffuse = true;

	if (m_diffuseTexture == 0)
	{
		m_diffuseTexture = GetAssetManager()->GetTexture("Default");
	}
}

void RenderableComponent::Draw(BaseCamera* camera)
{
	if (m_attachedModel == true && m_attachedShader == true && !m_hidden)
	{
		glUseProgram(m_shaderProgram);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_diffuseTexture);

		unsigned int projectionViewUniform = glGetUniformLocation(m_shaderProgram, "ProjectionView");
		unsigned int viewUniform = glGetUniformLocation(m_shaderProgram, "View");
		unsigned int worldTransformUniform = glGetUniformLocation(m_shaderProgram, "WorldTransform");
		unsigned int diffuseUniform = glGetUniformLocation(m_shaderProgram, "Diffuse");

		glUniformMatrix4fv(viewUniform, 1, GL_FALSE, &(camera->GetView()[0][0]));
		glUniformMatrix4fv(projectionViewUniform, 1, GL_FALSE, &(camera->GetProjectionView()[0][0]));
		glUniformMatrix4fv(worldTransformUniform, 1, GL_FALSE, &((m_parent->GetWorldTransform() * m_renderableTransform)[0][0]/*m_worldTransform[0][0])*/));
		glUniform1i(diffuseUniform, 0);

		m_model->DrawModel();
	}
}

void RenderableComponent::Hide()
{
	m_hidden = true;
}

void RenderableComponent::Show()
{
	m_hidden = false;
}

void RenderableComponent::SetRenderableTransform(glm::mat4 transform)
{
	m_renderableTransform = transform;
}