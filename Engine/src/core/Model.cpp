#include "Model.h"

Model::Model()
{
	m_idName = nullptr;
	m_FBX = nullptr;
	m_diffuse = 0;
}

Model::~Model()
{
	delete m_FBX;
}

void Model::LoadModel(const char* name, const char* path, Renderer* render)
{
	if (m_FBX == nullptr)
	{
		m_FBX = new FBXFile();
		m_FBX->load(path);
		m_FBX->initialiseOpenGLTextures();
		render->CreateOpenGLBuffers(m_FBX);
		m_idName = name;
	}
	else
	{
		printf("Error: Model already exists!\n");
		printf("%s\n", name);
	}
}

FBXFile* Model::GetModel()
{
	if (m_FBX != nullptr)
	{
		return m_FBX;
	}
	else
	{
		return 0;
	}
}

const char* Model::GetID()
{
	return m_idName;
}

void Model::DrawModel()
{
	if (this != nullptr)
	{
		for (unsigned int i = 0; i < m_FBX->getMeshCount(); ++i)
		{
			FBXMeshNode* mesh = m_FBX->getMeshByIndex(i);

			unsigned int* glData = (unsigned int*)mesh->m_userData;

			//glActiveTexture(GL_TEXTURE1);
			//glBindTexture(GL_TEXTURE_2D, mesh->m_material->textures[FBXMaterial::DiffuseTexture]->handle);

			glBindVertexArray(glData[0]);
			glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
		}
	}
}
