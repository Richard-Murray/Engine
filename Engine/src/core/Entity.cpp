#include "Entity.h"

Entity::Entity(const char* idName)
{
	m_worldTransform = glm::mat4(1);
	m_idName = idName;
}

Entity::~Entity()
{
	for (int i = 0; i < m_componentList.size(); i++)
	{
		delete m_componentList[i];
	}
	m_componentList.clear();
	//need to find their pointers in entitymanager vector and delete them if not shutdown
}

void Entity::Initialise(AssetManager* assetManager)
{
	m_pAssetManager = assetManager;/*
	m_diffuseTexture = m_pAssetManager->GetTexture("Default");
	m_attachedDiffuse = true;*/
}

//void Entity::AttachModel(const char* idName)
//{
//	m_model = m_pAssetManager->GetModel(idName);
//	m_attachedModel = true;
//}
//
//void Entity::AttachShader(const char* idName)
//{
//	m_shaderProgram = m_pAssetManager->GetShader(idName);
//	m_attachedShader = true;
//}
//
//void Entity::AttachTexture(const char* idName)
//{
//	m_diffuseTexture = m_pAssetManager->GetTexture(idName);
//	m_attachedDiffuse = true;
//}

void Entity::Update(float deltaTime)
{

}

//void Entity::Draw(BaseCamera* camera)
//{
//	if (m_attachedModel == true && m_attachedShader == true && !m_hidden)
//	{
//		glUseProgram(m_shaderProgram);
//
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, m_diffuseTexture);
//
//		unsigned int projectionViewUniform = glGetUniformLocation(m_shaderProgram, "ProjectionView");
//		unsigned int viewUniform = glGetUniformLocation(m_shaderProgram, "View");
//		unsigned int worldTransformUniform = glGetUniformLocation(m_shaderProgram, "WorldTransform");
//		unsigned int diffuseUniform = glGetUniformLocation(m_shaderProgram, "Diffuse");
//
//		glUniformMatrix4fv(viewUniform, 1, GL_FALSE, &(camera->GetView()[0][0]));
//		glUniformMatrix4fv(projectionViewUniform, 1, GL_FALSE, &(camera->GetProjectionView()[0][0]));
//		glUniformMatrix4fv(worldTransformUniform, 1, GL_FALSE, &(m_worldTransform[0][0]));
//		glUniform1i(diffuseUniform, 0);
//
//		m_model->DrawModel();
//	}
//}

void Entity::SetWorldTransform(glm::mat4 transform)
{
	m_worldTransform = transform;
}

void Entity::SetWorldTranslation(glm::vec3 translate)
{
	m_worldTransform[3].xyz = translate;
}

glm::mat4 Entity::GetWorldTransform()
{
	return m_worldTransform;
}

AssetManager* Entity::GetAssetManager()
{
	return m_pAssetManager;
}

//void Entity::TestMoveTransform(glm::mat4 transform)
//{
//	m_worldTransform = m_worldTransform * transform;
//}

const char* Entity::GetID()
{
	return m_idName;
}

//void Entity::Hide()
//{
//	m_hidden = true;
//}
//
//void Entity::Show()
//{
//	m_hidden = false;
//}

std::vector<Component*>& Entity::GetComponentList()
{
	return m_componentList;
}

Component* Entity::GetComponentOfType(const char* type)
{
	for (auto iter = m_componentList.begin(); iter != m_componentList.end(); iter++)
	{
		if (strcmp((*iter)->GetComponentType(), type) == 0)
		{
			return (*iter);
		}
	}

	return 0;
}