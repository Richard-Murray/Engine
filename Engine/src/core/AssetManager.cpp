#include "AssetManager.h"

AssetManager::AssetManager(Renderer* pRender)
{
	m_pRender = pRender;
}

AssetManager::~AssetManager()
{
	for (auto iter = m_modelList.begin(); iter != m_modelList.end(); iter++)
	{
		m_pRender->CleanupOpenGLBuffers((*iter)->GetModel());
		delete (*iter);
	}
	for (auto iter = m_shaderList.begin(); iter != m_shaderList.end(); iter++)
	{
		(*iter)->DeleteShader();
		delete (*iter);
	}
	for (auto iter = m_textureList.begin(); iter != m_textureList.end(); iter++)
	{
		(*iter)->DeleteTexture();
		delete (*iter);
	}
}

void AssetManager::Initialise()
{
	LoadTextureRGBA("Default", "data/textures/default.png");
}

void AssetManager::LoadModel(const char* idName, const char* path)
{
	if (!DoesModelExist(idName))
	{
		m_modelList.push_back(new Model());
		m_modelList.back()->LoadModel(idName, path, m_pRender);
	}
}

Model* AssetManager::GetModel(const char* idName)
{
	if (DoesModelExist(idName))
	{
		for (auto iter = m_modelList.begin(); iter != m_modelList.end(); iter++)
		{
			if (strcmp((*iter)->GetID(), idName) == 0)
			{
				return (*iter);
			}
		}
	}
	else
	{
		printf("Error: Failed to find model!\n");
		printf("%s\n", idName);
		return nullptr;
	}
}

unsigned int AssetManager::GetShader(const char* idName)
{
	for (auto iter = m_shaderList.begin(); iter != m_shaderList.end(); iter++)
	{
		if (strcmp((*iter)->GetID(), idName) == 0)
		{
			return (*iter)->GetAsset();
		}
	}

	printf("Error: Failed to find shader!\n");
	printf("%s\n", idName);

	return 0;
}

unsigned int AssetManager::GetTexture(const char* idName)
{
	for (auto iter = m_textureList.begin(); iter != m_textureList.end(); iter++)
	{
		if (strcmp((*iter)->GetID(), idName) == 0)
		{
			return (*iter)->GetAsset();
		}
	}

	printf("Error: Failed to find texture!\n");
	printf("%s\n", idName);

	return 0;
}

bool AssetManager::DoesModelExist(const char* idName)
{
	for (auto iter = m_modelList.begin(); iter != m_modelList.end(); iter++)
	{
		if (strcmp((*iter)->GetID(), idName) == 0)
		{
			return true;
		}
	}


	return false;
}

unsigned int AssetManager::LoadShaderFile(unsigned int type, const char* path)
{
	FILE* file = fopen(path, "rb");
	if (file == nullptr)
		return 0;

	//read shader source
	fseek(file, 0, SEEK_END);
	unsigned int length = ftell(file);
	fseek(file, 0, SEEK_SET);
	char* source = new char[length + 1];
	memset(source, 0, length + 1);
	fread(source, sizeof(char), length, file);
	fclose(file);

	unsigned int shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, 0);
	glCompileShader(shader);
	delete[] source;

	return shader;
}

void AssetManager::LoadShader(const char* idName, const char* vert, const char* frag)
{
	unsigned int vs = LoadShaderFile(GL_VERTEX_SHADER, vert);
	unsigned int fs = LoadShaderFile(GL_FRAGMENT_SHADER, frag);

	int success = GL_FALSE;

	unsigned int shader = glCreateProgram();
	glAttachShader(shader, vs);
	glAttachShader(shader, fs);
	glLinkProgram(shader);

	glGetProgramiv(shader, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetProgramInfoLog(shader, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}

	glDeleteShader(fs);
	glDeleteShader(vs);

	m_shaderList.push_back(new AssetID(idName, shader));
}

void AssetManager::LoadShader(const char* idName, const char* vert, const char* geom, const char* frag)
{
	unsigned int vs = LoadShaderFile(GL_VERTEX_SHADER, vert);
	unsigned int gs = LoadShaderFile(GL_GEOMETRY_SHADER, geom);
	unsigned int fs = LoadShaderFile(GL_FRAGMENT_SHADER, frag);


	int success = GL_FALSE;
	//
	//unsigned int shader;

	unsigned int shader = glCreateProgram();
	glAttachShader(shader, vs);
	glAttachShader(shader, fs);
	glAttachShader(shader, gs);
	glLinkProgram(shader);

	glGetProgramiv(shader, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetProgramInfoLog(shader, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}

	glDeleteShader(fs);
	glDeleteShader(vs);
	glDeleteShader(gs);

	m_shaderList.push_back(new AssetID(idName, shader));
}

void AssetManager::LoadTexture(const char* idName, const char* path)
{
	int imageWidth = 0, imageHeight = 0, imageFormat = 0;

	unsigned char* data = stbi_load(path, &imageWidth, &imageHeight, &imageFormat, STBI_default);

	if (data == nullptr)
	{
		printf("Error: Failed to load texture!\n");
		printf("%s\n", idName);
	}

	unsigned int texture;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

	m_textureList.push_back(new AssetID(idName, texture));
}

void AssetManager::LoadTextureRGBA(const char* idName, const char* path)
{
	int imageWidth = 0, imageHeight = 0, imageFormat = 0;

	unsigned char* data = stbi_load(path, &imageWidth, &imageHeight, &imageFormat, STBI_default);

	unsigned int texture;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

	m_textureList.push_back(new AssetID(idName, texture));
}

const char* AssetID::GetID()
{
	return m_idName;
}

unsigned int AssetID::GetAsset()
{
	return m_asset;
}