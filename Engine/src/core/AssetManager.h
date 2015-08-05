#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include <vector>

#include "Renderer.h"
#include "Model.h"

class AssetID;

class AssetManager
{
public:
	AssetManager(Renderer* pRender);
	~AssetManager();

	void Initialise();

	void LoadModel(const char* idName, const char* path);

	unsigned int LoadShaderFile(unsigned int type, const char* path);
	void LoadShader(const char* idName, const char* vert, const char* frag);
	void LoadShader(const char* idName, const char* vert, const char* geom, const char* frag);

	void LoadTexture(const char* idName, const char* path);
	void LoadTextureRGBA(const char* idName, const char* path);

	Model* GetModel(const char* idName);
	unsigned int GetShader(const char* idName);
	unsigned int GetTexture(const char* idName);
	bool DoesModelExist(const char* idName);

private:
	std::vector<Model*> m_modelList;
	std::vector<AssetID*> m_shaderList;
	std::vector<AssetID*> m_textureList;

	Renderer* m_pRender;
};

class AssetID
{
public:

	AssetID(const char* idName, unsigned int asset)
	{
		m_idName = idName;
		m_asset = asset;
	}

	~AssetID(){}

	const char* GetID();
	unsigned int GetAsset();
	void DeleteShader() { glDeleteShader(m_asset); };
	void DeleteTexture() { glDeleteTextures(1, &m_asset); };


private:
	const char* m_idName;
	unsigned int m_asset;
};

#endif