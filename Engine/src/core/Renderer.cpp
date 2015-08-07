#include "Renderer.h"
#include "AssetManager.h"
#include "Entity.h"
#include "EntityManager.h"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

#include "comp/RenderableComponent.h"
#include "PhysxRagdoll.h"
#include "ParticleFluidEmitter.h"

Renderer::Renderer(FlyCamera* camera, GLFWwindow* window)
{
	m_camera = camera;
	m_window = window;
}

Renderer::~Renderer()
{
	delete m_pGeometryPassRenderTarget;
	delete m_pLightPassRenderTarget;
	delete m_pPostProcessRenderTarget;
}

void Renderer::Update(float deltaTime)
{
	/*float s = glm::cos((float)glfwGetTime()) * 0.5f + 0.5f;

	mat4 moveTransform = mat4(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		-0.01f, 0, 0, 1);*/
}

void Renderer::Draw()
{
	////Deferred rendering drawing
	m_pGeometryPassRenderTarget->SetAsActiveRenderTarget();
	//m_pGeometryPassRenderTarget->ClearAsActiveRenderTarget();//
	// G-Pass: render out the albedo, position and normal
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawGeometryPass(m_camera);

	glDisable(GL_DEPTH_TEST);

	m_pLightPassRenderTarget->SetAsActiveRenderTarget();
	glClear(GL_COLOR_BUFFER_BIT);
	DrawLightPass(m_camera);

	m_pLightPassRenderTarget->ClearAsActiveRenderTarget();
	glClear(GL_COLOR_BUFFER_BIT);
	DrawCompositePass(m_camera);

	glEnable(GL_DEPTH_TEST);
}

void Renderer::DrawGeometryPass(BaseCamera* camera)
{
	for (auto iter = m_pEntityManager->GetRenderableComponentList().begin(); iter != m_pEntityManager->GetRenderableComponentList().end(); iter++)
	{
		(*iter)->Draw(camera);
	}

	PxU32 nLinks = m_pRagdoll->GetArticulation()->getNbLinks();
	PxArticulationLink** links = new PxArticulationLink*[nLinks];
	m_pRagdoll->GetArticulation()->getLinks(links, nLinks);	while (nLinks--)
	{
		PxArticulationLink* link = links[nLinks];
		PxU32 nShapes = link->getNbShapes();
		PxShape** shapes = new PxShape*[nShapes];
		link->getShapes(shapes, nShapes);
		while (nShapes--)
		{
			PxTransform ragTrans = link->getGlobalPose();

			PxVec3 vX = ragTrans.q.getBasisVector0();
			PxVec3 vY = ragTrans.q.getBasisVector1();
			PxVec3 vZ = ragTrans.q.getBasisVector2();

			glm::mat4 matrixTransform;
			matrixTransform[0].xyz = glm::vec3(vX.x, vX.y, vX.z);
			matrixTransform[1].xyz = glm::vec3(vY.x, vY.y, vY.z);
			matrixTransform[2].xyz = glm::vec3(vZ.x, vZ.y, vZ.z);
			matrixTransform[3].xyz = glm::vec3(ragTrans.p.x, ragTrans.p.y, ragTrans.p.z);

			m_pEntityManager->GetEntity("ragdollrenderer")->SetWorldTransform(matrixTransform);
			static_cast<RenderableComponent*>(m_pEntityManager->GetEntity("ragdollrenderer")->GetComponentOfType("Renderable"))->Draw(camera);
		}
	}
	delete[] links;

	m_pFluidEmitter->renderParticles(camera, m_pEntityManager->GetEntity("ragdollrenderer"));


	//m_pEntityManager->GetRenderableComponentList().back()->Draw(camera);
	//m_pEntityManager->Draw(camera);
}

void Renderer::DrawLightPass(BaseCamera* camera)
{
	// Light Pass: render lights as geometry, sampling position and
	// normals disable depth testing and enable additive blending

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	//Directional Lights
	glUseProgram(m_programDirectionalLightID);

	unsigned int positionTextureUniform = glGetUniformLocation(m_programDirectionalLightID, "positionTexture");
	unsigned int normalTextureUniform = glGetUniformLocation(m_programDirectionalLightID, "normalTexture");

	glUniform1i(positionTextureUniform, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_pGeometryPassRenderTarget->GetRenderTexture(1));

	glUniform1i(normalTextureUniform, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_pGeometryPassRenderTarget->GetRenderTexture(2));

	//DRAW DIRECTIONAL LIGHTS HERE
	// draw lights as fullscreen quads
	DrawDirectionalLight(camera, glm::vec3(-1), glm::vec3(0.5));

	//Point Lights
	glUseProgram(m_programPointLightID);

	positionTextureUniform = glGetUniformLocation(m_programPointLightID, "positionTexture");
	normalTextureUniform = glGetUniformLocation(m_programPointLightID, "normalTexture");
	unsigned int projectionViewUniform = glGetUniformLocation(m_programPointLightID, "ProjectionView");

	glUniform1i(positionTextureUniform, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_pGeometryPassRenderTarget->GetRenderTexture(1));

	glUniform1i(normalTextureUniform, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_pGeometryPassRenderTarget->GetRenderTexture(2));

	glUniformMatrix4fv(projectionViewUniform, 1, GL_FALSE, &(camera->GetProjectionView()[0][0]));

	glCullFace(GL_FRONT);

	//DRAW POINT LIGHTS HERE

	//DrawPointLight(camera, vec3(sinf(glfwGetTime()) * 5, 3, cosf(glfwGetTime()) * 5), 5, vec3(1, 0, 0));
	//DrawPointLight(camera, vec3(0, 1, 4.4), 10, vec3(0, 0, 1));
	//DrawPointLight(camera, vec3(8.8, 1, 4.4), 10, vec3(1, 0, 0));

	for (auto iter = m_pEntityManager->GetPointLightComponentList().begin(); iter != m_pEntityManager->GetPointLightComponentList().end(); iter++)
	{
		(*iter)->Draw(camera, m_VAOCube);
	}

	glCullFace(GL_BACK);

	glDisable(GL_BLEND);
}

void Renderer::DrawCompositePass(BaseCamera* camera)
{
	// Composite Pass: render a quad and combine albedo and light

	glUseProgram(m_programCompositeID);

	unsigned int albedoTextureUniform = glGetUniformLocation(m_programCompositeID, "albedoTexture");
	unsigned int lightTextureUniform = glGetUniformLocation(m_programCompositeID, "lightTexture");

	glUniform1i(albedoTextureUniform, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_pGeometryPassRenderTarget->GetRenderTexture(0));

	glUniform1i(lightTextureUniform, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_pLightPassRenderTarget->GetRenderTexture(0));

	glBindVertexArray(m_VAOfullScreenQuad);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::DrawDirectionalLight(BaseCamera* camera, const glm::vec3& direction, const glm::vec3& diffuse)
{
	glm::vec4 viewSpaceLight = camera->GetView() * glm::vec4(glm::normalize(direction), 0);

	unsigned int lightDirectionUniform = glGetUniformLocation(m_programDirectionalLightID, "lightDirection");
	unsigned int lightDiffuseUniform = glGetUniformLocation(m_programDirectionalLightID, "lightDiffuse");

	glUniform3fv(lightDirectionUniform, 1, &viewSpaceLight[0]);
	glUniform3fv(lightDiffuseUniform, 1, &diffuse[0]);

	glBindVertexArray(m_VAOfullScreenQuad);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::DrawPointLight(BaseCamera* camera, const glm::vec3& position, float radius, const glm::vec3& diffuse)
{
	glm::vec4 viewSpacePosition = camera->GetView() * glm::vec4(position, 1);

	unsigned int lightPositionUniform = glGetUniformLocation(m_programPointLightID, "lightPosition");
	unsigned int lightPositionViewUniform = glGetUniformLocation(m_programPointLightID, "lightPositionView");
	unsigned int lightRadiusUniform = glGetUniformLocation(m_programPointLightID, "lightRadius");
	unsigned int lightDiffuseUniform = glGetUniformLocation(m_programPointLightID, "lightDiffuse");

	glUniform3fv(lightPositionUniform, 1, &position[0]);
	glUniform3fv(lightPositionViewUniform, 1, &viewSpacePosition[0]);
	glUniform1f(lightRadiusUniform, radius);
	glUniform3fv(lightDiffuseUniform, 1, &diffuse[0]);

	glBindVertexArray(m_VAOCube);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

}

void Renderer::Load()
{
	m_programGeometryBufferID = m_pAssetManager->GetShader("GeometryPass");
	m_programDirectionalLightID = m_pAssetManager->GetShader("DirectionalLight");
	m_programPointLightID = m_pAssetManager->GetShader("PointLight");
	m_programCompositeID = m_pAssetManager->GetShader("CompositePass");

	m_pGeometryPassRenderTarget = new RenderTarget();
	m_pGeometryPassRenderTarget->SetSize(1280, 720);
	m_pGeometryPassRenderTarget->Initialise();
	m_pGeometryPassRenderTarget->AttachColourBuffer(0, GL_RGB8); //albedo
	m_pGeometryPassRenderTarget->AttachColourBuffer(1, GL_RGB32F); //position
	m_pGeometryPassRenderTarget->AttachColourBuffer(2, GL_RGB32F); //normal
	m_pGeometryPassRenderTarget->AttachDepthBuffer();
	m_pGeometryPassRenderTarget->SetDrawBuffers();

	m_pLightPassRenderTarget = new RenderTarget();
	m_pLightPassRenderTarget->SetSize(1280, 720);
	m_pLightPassRenderTarget->Initialise();
	m_pLightPassRenderTarget->AttachColourBuffer(0, GL_RGB8);
	m_pLightPassRenderTarget->SetDrawBuffers();

	m_pPostProcessRenderTarget = new RenderTarget();
	m_pPostProcessRenderTarget->SetSize(1280, 720);
	m_pPostProcessRenderTarget->Initialise();
	m_pPostProcessRenderTarget->AttachColourBuffer(0, GL_RGB8);
	m_pPostProcessRenderTarget->AttachDepthBuffer();
	m_pPostProcessRenderTarget->SetDrawBuffers();

	//make post process stuff later

	//m_pPostProcessRenderTarget = new RenderTarget();
	//m_pPostProcessRenderTarget->SetSize(1280, 720);
	//m_pPostProcessRenderTarget->AttachColourBuffer(0, GL_RGBA8);
	//m_pPostProcessRenderTarget->AttachDepthBuffer();
	//m_pPostProcessRenderTarget->SetDrawBuffers();

	CreateFullScreenQuad();
	CreateCube();
}

void Renderer::LoadTexture(unsigned int &texture, const char* path)
{
	int imageWidth = 0, imageHeight = 0, imageFormat = 0;

	unsigned char* data = stbi_load(path, &imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);
}

void Renderer::LoadTextureRGBA(unsigned int &texture, const char* path)
{
	int imageWidth = 0, imageHeight = 0, imageFormat = 0;

	unsigned char* data = stbi_load(path, &imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);
}

unsigned int Renderer::LoadShader(unsigned int type, const char* path)
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

void Renderer::CreateShader(unsigned int &shader, const char* vert, const char* frag)
{
	unsigned int vs = LoadShader(GL_VERTEX_SHADER, vert);
	unsigned int fs = LoadShader(GL_FRAGMENT_SHADER, frag);

	int success = GL_FALSE;

	shader = glCreateProgram();
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
}

void Renderer::CreateShader(unsigned int &shader, const char* vert, const char* geom, const char* frag)
{
	unsigned int vs = LoadShader(GL_VERTEX_SHADER, vert);
	unsigned int gs = LoadShader(GL_GEOMETRY_SHADER, geom);
	unsigned int fs = LoadShader(GL_FRAGMENT_SHADER, frag);


	int success = GL_FALSE;
	//
	//unsigned int shader;

	shader = glCreateProgram();
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
}

void Renderer::CreateOpenGLBuffers(FBXFile* fbx)
{
	int numMeshes = fbx->getMeshCount();

	for (int i = 0; i < numMeshes; i++)
	{
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);

		unsigned int* glData = new unsigned int[3];

		glGenVertexArrays(1, &glData[0]);
		glBindVertexArray(glData[0]);

		glGenBuffers(1, &glData[1]);
		glGenBuffers(1, &glData[2]);

		glBindBuffer(GL_ARRAY_BUFFER, glData[1]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData[2]);

		glBufferData(GL_ARRAY_BUFFER,
			mesh->m_vertices.size() * sizeof(FBXVertex),
			mesh->m_vertices.data(), GL_STATIC_DRAW);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			mesh->m_indices.size() * sizeof(unsigned int),
			mesh->m_indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); //position
		glEnableVertexAttribArray(1); //normal
		glEnableVertexAttribArray(2); //texture coordinates
		glEnableVertexAttribArray(3); //tangentsss
		glEnableVertexAttribArray(4); //weights
		glEnableVertexAttribArray(5); //indices

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), 0);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::NormalOffset);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char*)0) + FBXVertex::TexCoord1Offset);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::TangentOffset);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::WeightsOffset);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::IndicesOffset);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		mesh->m_userData = glData;
	}
}

void Renderer::CleanupOpenGLBuffers(FBXFile* fbx)
{
	for (unsigned int i = 0; i < fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);

		unsigned int* glData = (unsigned int*)mesh->m_userData;

		glDeleteVertexArrays(1, &glData[0]);
		glDeleteBuffers(1, &glData[1]);
		glDeleteBuffers(1, &glData[2]);

		delete[] glData;
	}
}

void Renderer::CreateFullScreenQuad()
{
	glm::vec2 halfTexel = 1.0f / glm::vec2(1280, 720) * 0.5f;

	float vertexData[] = {
		-1, -1, 0, 1, halfTexel.x, halfTexel.y,
		1, 1, 0, 1, 1 - halfTexel.x, 1 - halfTexel.y,
		-1, 1, 0, 1, halfTexel.x, 1 - halfTexel.y,

		-1, -1, 0, 1, halfTexel.x, halfTexel.y,
		1, -1, 0, 1, 1 - halfTexel.x, halfTexel.y,
		1, 1, 0, 1, 1 - halfTexel.x, 1 - halfTexel.y,
	};

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 6, vertexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, ((char*)0) + 16);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	m_VAOfullScreenQuad = m_VAO;

}

void Renderer::CreateCube()
{
	float vertexData[] = {
		-1, -1, 1, 1,
		1, -1, 1, 1,
		1, -1, -1, 1,
		-1, -1, -1, 1,
		-1, 1, 1, 1,
		1, 1, 1, 1,
		1, 1, -1, 1,
		-1, 1, -1, 1,
	};

	unsigned int indexData[] = {
		0, 5, 4,
		0, 1, 5,
		1, 6, 5,
		1, 2, 6,
		2, 7, 6,
		2, 3, 7,
		3, 4, 7,
		3, 0, 4,
		4, 6, 7,
		4, 5, 6,
		3, 1, 0,
		3, 2, 1
	};

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 8, vertexData, GL_STATIC_DRAW);

	glGenBuffers(1, &m_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6 * 6, indexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	m_VAOCube = m_VAO;
}

void Renderer::AddAssetManager(AssetManager* assetManager)
{
	m_pAssetManager = assetManager;
}

void Renderer::AddEntityManager(EntityManager* entityManager)
{
	m_pEntityManager = entityManager;
}

void Renderer::AddCheckersManager(CheckersManager* checkersManager)
{
	m_pCheckersManager = checkersManager;
}

void Renderer::AddRagdollRendering(Ragdoll* pRagdoll) 
{ 
	m_pRagdoll = pRagdoll; 
};//ragdollrendering

void Renderer::AddFluidRendering(ParticleFluidEmitter* pFluidEmitter)
{
	m_pFluidEmitter = pFluidEmitter;
}