#ifndef RENDERER_H
#define RENDERER_H

#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <stb_image.h>
#include "FBXFile.h"

#include "Camera.h"
#include "RenderTarget.h"


//#include "PhysxRagdoll.h"
class Ragdoll;
class ParticleFluidEmitter;

class Entity;

class AssetManager;
class EntityManager;
class CheckersManager;

struct OpenGLInfo
{
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;
	unsigned int m_index_count;
};

class Renderer
{
public:
	Renderer(FlyCamera* camera, GLFWwindow* window);
	~Renderer();

	void Update(float deltaTime);
	void Draw();
	void DrawScene(BaseCamera* camera);
	void DrawLighting(BaseCamera* camera);

	void Load();

	unsigned int LoadShader(unsigned int type, const char* path);
	void CreateShader(unsigned int &shader, const char* vert, const char* frag);
	void CreateShader(unsigned int &shader, const char* vert, const char* geom, const char* frag);

	void LoadTexture(unsigned int &texture, const char* path);
	void LoadTextureRGBA(unsigned int &texture, const char* path);

	void CreateOpenGLBuffers(FBXFile* fbx);
	void CleanupOpenGLBuffers(FBXFile* fbx);

	void AddEntityManager(EntityManager* entityManager);
	void AddAssetManager(AssetManager* assetManager);
	void AddCheckersManager(CheckersManager* checkersManager);
	void AddRagdollRendering(Ragdoll* pRagdoll); //all temporary
	void AddFluidRendering(ParticleFluidEmitter* pFluidEmitter);

	void DrawGeometryPass(BaseCamera* camera);
	void DrawLightPass(BaseCamera* camera);
	void DrawDirectionalLight(BaseCamera* camera, const glm::vec3& direction, const glm::vec3& diffuse);
	void DrawPointLight(BaseCamera* camera, const glm::vec3& position, float radius, const glm::vec3& diffuse);
	void DrawCompositePass(BaseCamera* camera);
	void DrawPostProcessingPass(BaseCamera* camera);

private:
	AssetManager* m_pAssetManager;
	EntityManager* m_pEntityManager;

	CheckersManager* m_pCheckersManager;

private:

	//RENDER TARGETS
	RenderTarget* m_pGeometryPassRenderTarget;
	RenderTarget* m_pLightPassRenderTarget;
	RenderTarget* m_pPostProcessRenderTarget;

	//DEFAULTS
	unsigned int m_defaultTexture;

	//SHADERS
	unsigned int m_programGeometryBufferID;
	unsigned int m_programDirectionalLightID;
	unsigned int m_programPointLightID;
	unsigned int m_programCompositeID;

	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;

	FBXFile* m_FBX;
	
	FlyCamera* m_camera;
	GLFWwindow* m_window;

	std::vector<OpenGLInfo> m_gl_info;

private:
	//Test content

	//Texture plane
	void GenerateTexturePlane();
	unsigned int m_VAOtest;
	unsigned int m_texture, m_normalMap;

	//Shadow maps
	glm::vec3 m_lightDir;
	glm::mat4 m_lightMatrix;

	void CreateCube();
	unsigned int m_VAOCube;

	//Postprocess
	//RenderTarget* m_pPostProcessRenderTarget;
	void CreateFullScreenQuad();
	unsigned int m_VAOfullScreenQuad;

	//Ragdoll pointer for rendering
	Ragdoll* m_pRagdoll;

	//same with fluids
	ParticleFluidEmitter* m_pFluidEmitter;
};



#endif