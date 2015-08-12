#include "Application.h"

#include "core/gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>

#include "core/Camera.h"

#include <stb_image.h>

#include "core/AssetManager.h"

//#include "vld.h"

using namespace physx;

using glm::vec3;
using glm::vec4;
using glm::mat4;

class Camera;
class FlyCamera;
struct Vertex;

Application::Application()
{}

Application::~Application()
{
	delete m_entityManager;
	delete m_renderer;
	delete m_camera;
}

int Application::Run()
{
	if (glfwInit() == false)
		return -1;

	m_window = glfwCreateWindow(1280, 720, "Engine", nullptr, nullptr);

	if (m_window == nullptr) {
		glfwTerminate();
		return -2;
	}

	glfwMakeContextCurrent(m_window);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED) {
		glfwDestroyWindow(m_window);
		glfwTerminate();
		return -3;
	}

	auto major = ogl_GetMajorVersion();
	auto minor = ogl_GetMinorVersion();
	printf("GL: %i.%i\n", major, minor);

	glClearColor(0.25f, 0.25f, 1.0f, 1);
	glEnable(GL_DEPTH_TEST);

	//float previousTime = 0.0f;

	InitialisePhysX();
	InitialiseVisualDebugger();
	InitialisePlayerController();

	Load();

	float previousTime = (float)glfwGetTime();
	float currentTime = (float)glfwGetTime();
	float deltaTime;

	glfwSwapInterval(1);

	while (glfwWindowShouldClose(m_window) == false && glfwGetKey(m_window, GLFW_KEY_ESCAPE) != GLFW_PRESS) {

		currentTime = (float)glfwGetTime();
		deltaTime = currentTime - previousTime;
		previousTime = currentTime;

		FPSRefresh += deltaTime;
		if (FPSRefresh > 1)
		{
			FPSRefresh = 0;
			previousFPS = framesPerSecond;
			framesPerSecond = 0;
			char buffer[50];
			int n = sprintf(buffer, "Engine: %f", previousFPS);
			glfwSetWindowTitle(m_window, buffer);
		}
		framesPerSecond = framesPerSecond + 1;

		Update(deltaTime);
		Draw();
		
		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}

	delete m_physicsManager;
	delete m_entityManager;
	delete m_assetManager;
	delete m_renderer;
	delete m_camera;

	g_PhysicsScene->release();
	g_Physics->release();
	g_PhysicsFoundation->release();

	glfwDestroyWindow(m_window);
	glfwTerminate();
	return 0;
}

void Application::Update(float deltaTime)
{
	if (glfwGetKey(m_window, GLFW_KEY_V) == GLFW_PRESS)
	{
		FPSinterval++;
		if (FPSinterval > 2)
			FPSinterval = 0;

		glfwSwapInterval(FPSinterval);

	}

	if (glfwGetKey(m_window, GLFW_KEY_T) == GLFW_PRESS)
	{
		m_deltaTimeModifier += m_deltaTimeModifier * 0.1f;
	}
	if (glfwGetKey(m_window, GLFW_KEY_Y) == GLFW_PRESS)
	{
		m_deltaTimeModifier -= m_deltaTimeModifier * 0.1f;
		if (m_deltaTimeModifier < 0.1f)
		{
			m_deltaTimeModifier = 0.1f;
		}
	}
	if (glfwGetKey(m_window, GLFW_KEY_U) == GLFW_PRESS)
	{
		m_deltaTimeModifier = 1;
	}
	if (m_deltaTimeModifier < 0.1f)
	{
		m_deltaTimeModifier = 0.1f;
	}
	gameWorldDeltaTime = deltaTime / m_deltaTimeModifier;
	if (glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS)
	{
		m_entityManager->DeleteEntities();
	}

	//m_renderer->Update(deltaTime);
	m_camera->Update(deltaTime);
	m_physicsManager->Update(gameWorldDeltaTime);
	m_entityManager->Update(gameWorldDeltaTime);
	UpdatePhysx(gameWorldDeltaTime);

	if (glfwGetKey(m_window, GLFW_KEY_E) == GLFW_PRESS)
	{
		Entity* pointLight = m_entityManager->GetEntity("Test3");
		if(pointLight != nullptr)
			pointLight->SetWorldTranslation(m_camera->GetPosition());

	}

	if (glfwGetKey(m_window, GLFW_KEY_G) == GLFW_PRESS)
	{
		Entity* entity = m_entityManager->GetEntity("Physicstestsphere");
		if (entity != nullptr)
		{
			static_cast<PhysicsComponent*>(entity->GetComponentOfType("Physics"))->ApplyForce(glm::vec3(1, 0, 0));
		}
	}

	testTimer += deltaTime;
	if (testTimer > 0.5f)
	{
		m_entityManager->CreateEntity("Testall");
		m_entityManager->GetNewEntity()->Initialise(m_assetManager);
		m_entityManager->GetNewEntity()->SetWorldTranslation(vec3(-2, 50, 0));
	
		if (rand() % 2 == 1)
		{
			m_entityManager->AttachRenderable(m_entityManager->GetNewEntity(), "Cube1", "GeometryPass", "White");
			m_entityManager->AttachPhysxPhysics(m_entityManager->GetNewEntity(), g_Physics, g_PhysicsScene);
			static_cast<PhysxPhysicsComponent*>(m_entityManager->GetNewEntity()->GetComponentOfType("PhysxPhysics"))->SetBox(glm::vec3(0.5f, 0.5f, 0.5f), 10, true);
		}
		else
		{
			m_entityManager->AttachRenderable(m_entityManager->GetNewEntity(), "Sphere", "GeometryPass", "Red");
			static_cast<RenderableComponent*>(m_entityManager->GetNewEntity()->GetComponentOfType("Renderable"))->SetRenderableTransform(glm::mat4(0.5f, 0, 0, 0,
				0, 0.5f, 0, 0,
				0, 0, 0.5f, 0,
				0, 0, 0, 1));
			m_entityManager->AttachPhysxPhysics(m_entityManager->GetNewEntity(), g_Physics, g_PhysicsScene);
			static_cast<PhysxPhysicsComponent*>(m_entityManager->GetNewEntity()->GetComponentOfType("PhysxPhysics"))->SetSphere(0.5f, 10, true);
		}
		m_entityManager->AttachPointLight(m_entityManager->GetNewEntity(), "PointLight", glm::vec3(((double)rand() / (RAND_MAX)) + 1, ((double)rand() / (RAND_MAX)) + 1, ((double)rand() / (RAND_MAX)) + 1), 5);
		testTimer = 0;


		m_entityManager->CreateEntity("Physicstestsphere3");
		m_entityManager->GetNewEntity()->Initialise(m_assetManager);
		m_entityManager->GetNewEntity()->SetWorldTranslation(vec3((rand() % 100) / 10 + 10, 26, (rand() % 100) / 10 + 10));
		m_entityManager->AttachRenderable(m_entityManager->GetNewEntity(), "Sphere", "GeometryPass", "Gold");
		static_cast<RenderableComponent*>(m_entityManager->GetNewEntity()->GetComponentOfType("Renderable"))->SetRenderableTransform(glm::mat4(0.5f, 0, 0, 0,
			0, 0.5f, 0, 0,
			0, 0, 0.5f, 0,
			0, 0, 0, 1));
		m_entityManager->AttachPhysics(m_entityManager->GetNewEntity());
		static_cast<PhysicsComponent*>(m_entityManager->GetNewEntity()->GetComponentOfType("Physics"))->SetSphere(10, 0.7f);
	}

	/*PxTransform trans = dynamicActor->getGlobalPose();

	PxVec3 vX = trans.q.getBasisVector0();
	PxVec3 vY = trans.q.getBasisVector1();
	PxVec3 vZ = trans.q.getBasisVector2();

	glm::mat4 mattransform;
	mattransform[0].xyz = glm::vec3(vX.x, vX.y, vX.z);
	mattransform[1].xyz = glm::vec3(vY.x, vY.y, vY.z);
	mattransform[2].xyz = glm::vec3(vZ.x, vZ.y, vZ.z);
	mattransform[3].xyz = glm::vec3(trans.p.x, trans.p.y, trans.p.z);
	m_entityManager->GetEntity("Test1")->SetWorldTranslation(mattransform[3].xyz);*/
	
	//CHARACTER CONTROLLER
	bool onGround; //set to true if we are on the ground
	float movementSpeed = 1.0f; //forward and back movement speed
	float rotationSpeed = 1.0f; //turn speed
	//check if we have a contact normal. if y is greater than .3 we assume this is	solid ground.This is a rather primitive way to do this.Can you do better ?
		if (myHitReport->getPlayerContactNormal().y > 0.3f)
		{
			_characterYVelocity = -0.1f;
			onGround = true;
		}
		else
		{
			_characterYVelocity += _playerGravity * deltaTime;
			onGround = false;
		}
	myHitReport->clearPlayerContactNormal();
	const PxVec3 up(0, 1, 0);
	//scan the keys and set up our intended velocity based on a global transform
	PxVec3 velocity(0, _characterYVelocity, 0);
	if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
	{
		velocity.z -= movementSpeed*deltaTime;
	}
	if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
	{
		velocity.z += movementSpeed*deltaTime;
	}
	if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
	{
		velocity.x += movementSpeed*deltaTime;
	}
	if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
	{
		velocity.x -= movementSpeed*deltaTime;
	}
	if ((glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS) && onGround == true)
	{
		_characterYVelocity = 0.1f;
	}
	//To do.. add code to control z movement and jumping
	float minDistance = 0.001f;
	PxControllerFilters filter;
	//make controls relative to player facing
	//PxQuat rotation(_characterRotation, PxVec3(0, 1, 0));
	glm::vec3 forward = m_camera->GetTransform()[2].xyz;
	glm::vec3 right = m_camera->GetTransform()[0].xyz;
	forward.y = 0;
	forward = glm::normalize(forward);

	right.y = 0;
	right = glm::normalize(right);

	glm::vec3 moveVec = forward * velocity.z;
	moveVec += right * velocity.x;
	moveVec.y = _characterYVelocity;

	//PxVec3 velocity(0, _characterYVelocity, 0);
	//move the controller
	if ((glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) && onGround == true)
	{
		moveVec *= 10;
	}
	g_PlayerController->move(PxVec3(moveVec.x, moveVec.y, moveVec.z) * deltaTime * 1000, minDistance, deltaTime, filter);
	m_camera->SetPosition(glm::vec3(g_PlayerController->getPosition().x, g_PlayerController->getPosition().y, g_PlayerController->getPosition().z));
	//Entity* playerVisual = m_entityManager->GetEntity("Capsuletest");
	//if (playerVisual != nullptr)
	//	m_entityManager->GetEntity("Capsuletest")->SetWorldTranslation(glm::vec3(g_PlayerController->getPosition().x, g_PlayerController->getPosition().y, g_PlayerController->getPosition().z));

	if (m_particleEmitter)
	{
		m_particleEmitter->update(deltaTime);
		//render all our particles
		//m_particleEmitter->renderParticles();
	}

	static_cast<PhysicsComponent*>(m_entityManager->GetEntity("Jointtestfixed")->GetComponentOfType("Physics"))->SetPosition(glm::vec3(sinf(glfwGetTime()) * 5 + 30, 15, 40));
	m_testSpringJoint->Update(glm::vec3(0, -1, 0), deltaTime);

	//RAGDOLLS
	

	
	//m_entityManager->GetEntity("ragdollrenderer")->SetWorldTranslation


}

void Application::Draw()
{
	m_renderer->Draw();
}

void Application::Load()
{
	m_camera = new FlyCamera(10.0f);
	m_camera->SetInputWindow(m_window);
	m_camera->SetUpPerspective(glm::pi<float>() * 0.25f, 16 / 9.f, 0.1f, 10000.f);
	//m_camera->SetPosition(glm::vec3(-5, 5, 5));
	m_camera->SetTransform(glm::mat4(0, 0, 1, 0,
		0, 1, 0, 0,
		-1, 0, 0, 0,
		-5, 5, 5, 1));

	m_renderer = new Renderer(m_camera, m_window);

	m_assetManager = new AssetManager(m_renderer);
	m_assetManager->Initialise();

	m_assetManager->LoadShader("GeometryPass", "./data/shaders/deferredGbuffer.vert", "./data/shaders/deferredGbuffer.frag");
	m_assetManager->LoadShader("DirectionalLight", "./data/shaders/deferredDirectionalLight.vert", "./data/shaders/deferredDirectionalLight.frag");
	m_assetManager->LoadShader("PointLight", "./data/shaders/deferredPointLight.vert", "./data/shaders/deferredPointLight.frag");
	m_assetManager->LoadShader("CompositePass", "./data/shaders/deferredComposite.vert", "./data/shaders/deferredComposite.frag");

	m_assetManager->LoadModel("Cube1", "data/models/Cube.fbx");
	m_assetManager->LoadModel("Crate1", "data/models/Crate.fbx");
	m_assetManager->LoadModel("Sphere", "data/models/only_quad_sphere.fbx");
	//m_assetManager->LoadModel("Sword", "data/models/SCHWERT.fbx");
	m_assetManager->LoadModel("Sword2", "data/models/Sword Of Carnage.fbx");
	//m_assetManager->LoadModel("Crossbow", "data/models/Merciless Crossbow.fbx");

	m_assetManager->LoadTexture("Snow", "data/textures/snow.jpg");
	m_assetManager->LoadTexture("Rock", "data/textures/rock.jpg");
	m_assetManager->LoadTexture("SwordTex", "data/textures/FbxTemp_0001.png");

	m_assetManager->LoadTexture("Red", "data/textures/colour/red.png");
	m_assetManager->LoadTexture("Blue", "data/textures/colour/blue.png");
	m_assetManager->LoadTexture("Orange", "data/textures/colour/orange.png");
	m_assetManager->LoadTexture("Cyan", "data/textures/colour/cyan.png");
	m_assetManager->LoadTexture("Gold", "data/textures/colour/gold.png");
	m_assetManager->LoadTexture("Black", "data/textures/colour/black.png");
	m_assetManager->LoadTexture("White", "data/textures/colour/white.png");

	m_entityManager = new EntityManager();
	m_renderer->AddEntityManager(m_entityManager);
	m_renderer->AddAssetManager(m_assetManager);
	m_renderer->Load();

	m_physicsManager = new PhysicsManager();
	m_physicsManager->Initialise(m_entityManager);

	m_entityManager->CreateEntity("Test1");
	m_entityManager->GetNewEntity()->Initialise(m_assetManager);
	m_entityManager->GetNewEntity()->SetWorldTranslation(vec3(0, 20, 0));
	m_entityManager->AttachRenderable(m_entityManager->GetNewEntity(), "Cube1", "GeometryPass", "Blue");
	m_entityManager->AttachPhysxPhysics(m_entityManager->GetNewEntity(), g_Physics, g_PhysicsScene);
	static_cast<PhysxPhysicsComponent*>(m_entityManager->GetNewEntity()->GetComponentOfType("PhysxPhysics"))->SetBox(glm::vec3(0.5f, 0.5f, 0.5f), 10, true);
	m_entityManager->AttachPointLight(m_entityManager->GetNewEntity(), "PointLight", glm::vec3(0, 0, 1), 5);

	m_entityManager->CreateEntity("Test5");
	m_entityManager->GetNewEntity()->Initialise(m_assetManager);
	m_entityManager->GetNewEntity()->SetWorldTranslation(vec3(4, 10, 0));
	m_entityManager->AttachRenderable(m_entityManager->GetNewEntity(), "Cube1", "GeometryPass", "Blue");
	m_entityManager->AttachPhysxPhysics(m_entityManager->GetNewEntity(), g_Physics, g_PhysicsScene);
	static_cast<PhysxPhysicsComponent*>(m_entityManager->GetNewEntity()->GetComponentOfType("PhysxPhysics"))->SetBox(glm::vec3(0.5f, 0.5f, 0.5f), 10, true);

	/*m_entityManager->CreateEntity("SwordTest");
	m_entityManager->GetNewEntity()->Initialise(m_assetManager);
	m_entityManager->GetNewEntity()->SetWorldTranslation(vec3(0, 1, 0));
	m_entityManager->AttachRenderable(m_entityManager->GetNewEntity(), "Sword2", "GeometryPass", "SwordTex");
	static_cast<RenderableComponent*>(m_entityManager->GetNewEntity()->GetComponentOfType("Renderable"))->SetRenderableTransform(glm::mat4(0.1f, 0, 0, 0,
		0, 0.1f, 0, 0,
		0, 0, 0.1f, 0,
		0, 0, 0, 1));
	m_entityManager->AttachPhysxPhysics(m_entityManager->GetNewEntity(), g_Physics, g_PhysicsScene);
	static_cast<PhysxPhysicsComponent*>(m_entityManager->GetNewEntity()->GetComponentOfType("PhysxPhysics"))->SetBox(glm::vec3(1, 1, 3), 10, false);	*/

	m_entityManager->CreateEntity("SwordTest2");
	m_entityManager->GetNewEntity()->Initialise(m_assetManager);
	m_entityManager->GetNewEntity()->SetWorldTranslation(vec3(0, 7, 0));
	m_entityManager->AttachRenderable(m_entityManager->GetNewEntity(), "Sword2", "GeometryPass", "SwordTex");
	static_cast<RenderableComponent*>(m_entityManager->GetNewEntity()->GetComponentOfType("Renderable"))->SetRenderableTransform(glm::mat4(0.1f, 0, 0, 0,
		0, 0.1f, 0, 0,
		0, 0, 0.1f, 0,
		0, 0, 0, 1));
	m_entityManager->AttachPhysxPhysics(m_entityManager->GetNewEntity(), g_Physics, g_PhysicsScene);
	//static_cast<PhysxPhysicsComponent*>(m_entityManager->GetNewEntity()->GetComponentOfType("PhysxPhysics"))->SetBox(glm::vec3(1, 1, 3), 10, true);
	PhysxPhysicsComponent* test = static_cast<PhysxPhysicsComponent*>(m_entityManager->GetNewEntity()->GetComponentOfType("PhysxPhysics"));
	test->AttachRigidBodyConvex(m_assetManager->GetModel("Sword2")->GetModel(), 10, g_PhysicsMaterial, g_PhysicsCooker);

	/*m_entityManager->CreateEntity("Test2");
	m_entityManager->GetNewEntity()->Initialise(m_assetManager);
	m_entityManager->AttachRenderable(m_entityManager->GetNewEntity(), "Crate1", "GeometryPass", nullptr);*/

	m_entityManager->CreateEntity("Test3");
	m_entityManager->GetNewEntity()->Initialise(m_assetManager);
	m_entityManager->AttachPointLight(m_entityManager->GetNewEntity(), "PointLight", glm::vec3(0, 1, 0), 10);

	m_entityManager->CreateEntity("Test4");
	m_entityManager->GetNewEntity()->Initialise(m_assetManager);
	m_entityManager->GetNewEntity()->SetWorldTranslation(vec3(3, 3, 3));
	m_entityManager->AttachPointLight(m_entityManager->GetNewEntity(), "PointLight", glm::vec3(1, 0, 0), 10);

	m_entityManager->CreateEntity("Ground");
	m_entityManager->GetNewEntity()->Initialise(m_assetManager);
	m_entityManager->GetNewEntity()->SetWorldTranslation(vec3(0, -500, 0));
	m_entityManager->AttachRenderable(m_entityManager->GetNewEntity(), "Cube1", "GeometryPass", "Snow");
	m_entityManager->GetNewEntity()->SetWorldTransform(m_entityManager->GetNewEntity()->GetWorldTransform() * glm::mat4(1000, 0, 0, 0,
		0, 1000, 0, 0,
		0, 0, 1000, 0,
		0, 0, 0, 1));

	//m_entityManager->CreateEntity("Physicstestsphere");
	//m_entityManager->GetNewEntity()->Initialise(m_assetManager);
	//m_entityManager->GetNewEntity()->SetWorldTranslation(vec3(10, 26.5, 2));
	//m_entityManager->AttachRenderable(m_entityManager->GetNewEntity(), "Sphere", "GeometryPass", "Gold");
	//static_cast<RenderableComponent*>(m_entityManager->GetNewEntity()->GetComponentOfType("Renderable"))->SetRenderableTransform(glm::mat4(0.5f, 0, 0, 0,
	//	0, 0.5f, 0, 0,
	//	0, 0, 0.5f, 0,
	//	0, 0, 0, 1));
	//m_entityManager->AttachPhysics(m_entityManager->GetNewEntity());
	////static_cast<PhysicsComponent*>(m_entityManager->GetNewEntity()->GetComponentOfType("Physics"))->SetAABB(10, glm::vec3(1, 1, 1));
	//static_cast<PhysicsComponent*>(m_entityManager->GetNewEntity()->GetComponentOfType("Physics"))->SetSphere(10, 0.7f);

	//m_entityManager->CreateEntity("Physicstestsphere2");
	//m_entityManager->GetNewEntity()->Initialise(m_assetManager);
	//m_entityManager->GetNewEntity()->SetWorldTranslation(vec3(20, 26, 2));
	//m_entityManager->AttachRenderable(m_entityManager->GetNewEntity(), "Sphere", "GeometryPass", "Gold");
	//static_cast<RenderableComponent*>(m_entityManager->GetNewEntity()->GetComponentOfType("Renderable"))->SetRenderableTransform(glm::mat4(0.5f, 0, 0, 0,
	//	0, 0.5f, 0, 0,
	//	0, 0, 0.5f, 0,
	//	0, 0, 0, 1));
	//m_entityManager->AttachPhysics(m_entityManager->GetNewEntity());
	//static_cast<PhysicsComponent*>(m_entityManager->GetNewEntity()->GetComponentOfType("Physics"))->SetSphere(10, 0.7f);

	/*m_entityManager->CreateEntity("PhysicstestAABB");
	m_entityManager->GetNewEntity()->Initialise(m_assetManager);
	m_entityManager->GetNewEntity()->SetWorldTranslation(vec3(15, 26, 2.5));
	m_entityManager->AttachRenderable(m_entityManager->GetNewEntity(), "Cube1", "GeometryPass", "Gold");
	m_entityManager->AttachPhysics(m_entityManager->GetNewEntity());
	static_cast<PhysicsComponent*>(m_entityManager->GetNewEntity()->GetComponentOfType("Physics"))->SetAABB(10, glm::vec3(1, 1, 1));*/

	m_entityManager->CreateEntity("Physicstestplane");
	m_entityManager->GetNewEntity()->Initialise(m_assetManager);
	m_entityManager->GetNewEntity()->SetWorldTranslation(vec3(0, 0, 0));
	m_entityManager->AttachPhysics(m_entityManager->GetNewEntity());
	static_cast<PhysicsComponent*>(m_entityManager->GetNewEntity()->GetComponentOfType("Physics"))->SetPlane(glm::vec3(0, 1, 0), 0);

	m_entityManager->CreateEntity("Physicstestspherefixed");
	m_entityManager->GetNewEntity()->Initialise(m_assetManager);
	m_entityManager->GetNewEntity()->SetWorldTranslation(vec3(10, 10, 10));
	m_entityManager->AttachRenderable(m_entityManager->GetNewEntity(), "Sphere", "GeometryPass", "Gold");
	static_cast<RenderableComponent*>(m_entityManager->GetNewEntity()->GetComponentOfType("Renderable"))->SetRenderableTransform(glm::mat4(5, 0, 0, 0,
		0, 5, 0, 0,
		0, 0, 5, 0,
		0, 0, 0, 1));
	m_entityManager->AttachPhysics(m_entityManager->GetNewEntity());
	static_cast<PhysicsComponent*>(m_entityManager->GetNewEntity()->GetComponentOfType("Physics"))->SetSphere(10, 7);
	static_cast<PhysicsComponent*>(m_entityManager->GetNewEntity()->GetComponentOfType("Physics"))->SetStatic(true);

	m_entityManager->CreateEntity("Jointtestfixed");
	m_entityManager->GetNewEntity()->Initialise(m_assetManager);
	m_entityManager->GetNewEntity()->SetWorldTranslation(vec3(40, 15, 40));
	m_entityManager->AttachRenderable(m_entityManager->GetNewEntity(), "Sphere", "GeometryPass", "Gold");
	static_cast<RenderableComponent*>(m_entityManager->GetNewEntity()->GetComponentOfType("Renderable"))->SetRenderableTransform(glm::mat4(2.5f, 0, 0, 0,
		0, 2.5f, 0, 0,
		0, 0, 2.5f, 0,
		0, 0, 0, 1));
	m_entityManager->AttachPhysics(m_entityManager->GetNewEntity());
	static_cast<PhysicsComponent*>(m_entityManager->GetNewEntity()->GetComponentOfType("Physics"))->SetSphere(10, 3.5f);
	static_cast<PhysicsComponent*>(m_entityManager->GetNewEntity()->GetComponentOfType("Physics"))->SetStatic(true);

	m_entityManager->CreateEntity("Jointtestmoving");
	m_entityManager->GetNewEntity()->Initialise(m_assetManager);
	m_entityManager->GetNewEntity()->SetWorldTranslation(vec3(40, 7, 40));
	m_entityManager->AttachRenderable(m_entityManager->GetNewEntity(), "Sphere", "GeometryPass", "Gold");
	/*static_cast<RenderableComponent*>(m_entityManager->GetNewEntity()->GetComponentOfType("Renderable"))->SetRenderableTransform(glm::mat4(2.5f, 0, 0, 0,
		0, 2.5f, 0, 0,
		0, 0, 2.5f, 0,
		0, 0, 0, 1));*/
	m_entityManager->AttachPhysics(m_entityManager->GetNewEntity());
	static_cast<PhysicsComponent*>(m_entityManager->GetNewEntity()->GetComponentOfType("Physics"))->SetSphere(10, 0.7f);

	m_testSpringJoint = new SpringJoint(static_cast<PhysicsComponent*>(m_entityManager->GetEntity("Jointtestfixed")->GetComponentOfType("Physics")),
										static_cast<PhysicsComponent*>(m_entityManager->GetEntity("Jointtestmoving")->GetComponentOfType("Physics")), 1, 0.999f); //coefficient?

	static_cast<PhysicsComponent*>(m_entityManager->GetEntity("Jointtestfixed")->GetComponentOfType("Physics"))->SetPosition(glm::vec3(30, 15, 40));

	/*for (int i = 0; i < 5000; ++i)
	{
		m_entityManager->CreateEntity("Testall");
		m_entityManager->GetNewEntity()->Initialise(m_assetManager);
		m_entityManager->GetNewEntity()->SetWorldTranslation(vec3(-2, i * 2, 0));
		m_entityManager->AttachRenderable(m_entityManager->GetNewEntity(), "Cube1", "GeometryPass", "White");
		m_entityManager->AttachPhysxPhysics(m_entityManager->GetNewEntity(), "Box", g_Physics, g_PhysicsScene);
		m_entityManager->AttachPointLight(m_entityManager->GetNewEntity(), "PointLight", glm::vec3(((double)rand() / (RAND_MAX)) + 1, ((double)rand() / (RAND_MAX)) + 1, ((double)rand() / (RAND_MAX)) + 1), 5);
	}*/
	//static_cast<PointLightComponent*>(m_entityManager->GetNewEntity()->GetComponentOfType("PointLight"))->SetDiffuse(vec3(1, 0, 0));

	AddTestPhysxContent();

	//Particle emitter stuff
	PxParticleFluid* pf;

	PxU32 maxParticles = 4000;
	bool perParticleRestOffset = false;
	pf = g_Physics->createParticleFluid(maxParticles, perParticleRestOffset);

	pf->setRestParticleDistance(0.3f);
	pf->setDynamicFriction(0.1);
	pf->setStaticFriction(0.1);
	pf->setDamping(0.1);
	pf->setParticleMass(0.1);
	pf->setRestitution(0);
	
	pf->setParticleBaseFlag(PxParticleBaseFlag::eCOLLISION_TWOWAY, true);
	pf->setStiffness(100);

	if (pf)
	{
		g_PhysicsScene->addActor(*pf);
		m_particleEmitter = new ParticleFluidEmitter(maxParticles, PxVec3(0, 10, 0), pf, .1);
		m_particleEmitter->setStartVelocityRange(-2, -250, -2, 2, -250, 2);
	}

	m_ragdollTest = new Ragdoll(g_Physics, PxTransform(PxVec3(-5, 50, 0)), 0.1f, g_PhysicsMaterial, g_PhysicsScene);

	//std::string test = "ragdollrenderer" + std::to_string(5); //test.c_str()
	m_entityManager->CreateEntity("ragdollrenderer");
	m_entityManager->GetNewEntity()->Initialise(m_assetManager);
	m_entityManager->AttachRenderable(m_entityManager->GetNewEntity(), "Sphere", "GeometryPass", "Cyan");
	static_cast<RenderableComponent*>(m_entityManager->GetNewEntity()->GetComponentOfType("Renderable"))->SetRenderableTransform(glm::mat4(	0.25f, 0, 0, 0,
																																			0, 0.25f, 0, 0,
																																			0, 0, 0.25f, 0,
																																			0, 0, 0, 1));
	m_ragdollTest->SetUpRendering(m_entityManager);

	m_renderer->AddRagdollRendering(m_ragdollTest); //ragdollrendering
	m_renderer->AddFluidRendering(m_particleEmitter);

	m_entityManager->CreateEntity("Triggervolume");
	m_entityManager->GetNewEntity()->Initialise(m_assetManager);
	m_entityManager->GetNewEntity()->SetWorldTranslation(glm::vec3(-10, 0, -10));
	m_entityManager->AttachRenderable(m_entityManager->GetNewEntity(), "Cube1", "GeometryPass", "Red");
	static_cast<RenderableComponent*>(m_entityManager->GetNewEntity()->GetComponentOfType("Renderable"))->SetRenderableTransform(glm::mat4(	5, 0, 0, 0,
																																			0, 5, 0, 0,
																																			0, 0, 5, 0,
																																			0, 0, 0, 1));
	m_entityManager->AttachPhysxPhysics(m_entityManager->GetNewEntity(), g_Physics, g_PhysicsScene);
	static_cast<PhysxPhysicsComponent*>(m_entityManager->GetNewEntity()->GetComponentOfType("PhysxPhysics"))->SetBox(glm::vec3(2.5, 2.5, 2.5), 10, false);
	static_cast<PhysxPhysicsComponent*>(m_entityManager->GetNewEntity()->GetComponentOfType("PhysxPhysics"))->SetShapeAsTrigger();
}

void Application::InitialisePhysX()
{
	PxAllocatorCallback *myCallback = new myAllocator();
	g_PhysicsFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *myCallback, gDefaultErrorCallback);
	g_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *g_PhysicsFoundation,  PxTolerancesScale());
	PxInitExtensions(*g_Physics);
	//create physics material 
	g_PhysicsMaterial = g_Physics->createMaterial(0.5f, 0.5f, .5f);
	PxSceneDesc sceneDesc(g_Physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0, -10.0f, 0);
	sceneDesc.filterShader = myFliterShader;
	//sceneDesc.filterShader = &physx::PxDefaultSimulationFilterShader; //filter shader
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
	g_PhysicsScene = g_Physics->createScene(sceneDesc);

	g_PhysicsCooker = PxCreateCooking(PX_PHYSICS_VERSION, *g_PhysicsFoundation, PxCookingParams(PxTolerancesScale()));

	//TRIGGER VOLUMES
	PxSimulationEventCallback* myCollisionCallBack = new MyCollisionCallBack();
	g_PhysicsScene->setSimulationEventCallback(myCollisionCallBack);	static_cast<MyCollisionCallBack*>(myCollisionCallBack)->SetApplication(this);
}

void Application::InitialisePlayerController()
{
	myHitReport = new MyControllerHitReport();
	g_CharacterManager = PxCreateControllerManager(*g_PhysicsScene);
	//describe our controller...
	PxCapsuleControllerDesc desc;
	desc.height = 1.6f;
	desc.radius = 0.4f;
	desc.position.set(0, 0, 0);
	desc.material = g_PhysicsMaterial; //g_PhysicsMaterial?
	desc.reportCallback = myHitReport; //connect it to our collision detection routine
	desc.density = 10;
	//create the layer controller
	g_PlayerController = g_CharacterManager->createController(desc);

	g_PlayerController->setPosition(PxExtendedVec3(0, 0, 0));
	//set up some variables to control our player with
	_characterYVelocity = 0; //initialize character velocity
	_characterRotation = 0; //and rotation
	_playerGravity = -0.5f; //set up the player gravity
	myHitReport->clearPlayerContactNormal(); //initialize the contact normal (what we are in contact with)
	//	addToActorList(gPlayerController->getActor()); //so we can draw it's gizmo

	g_PlayerController->getActor()->setName("PlayerActor");
}

void Application::InitialiseVisualDebugger()
{
	// check if PvdConnection manager is available on this platform
	if (g_Physics->getPvdConnectionManager() == NULL)
		return;

	// setup connection parameters
	const char* pvd_host_ip = "127.0.0.1";

	// IP of the PC which is running PVD
	int port = 5425;

	// TCP port to connect to, where PVD is listening
	unsigned int timeout = 100;

	// timeout in milliseconds to wait for PVD to respond,
	//consoles and remote PCs need a higher timeout.
	PxVisualDebuggerConnectionFlags connectionFlags = PxVisualDebuggerExt::getAllConnectionFlags();

	// and now try to connectPxVisualDebuggerExt
	auto theConnection = PxVisualDebuggerExt::createConnection(
		g_Physics->getPvdConnectionManager(), pvd_host_ip, port, timeout, connectionFlags);
}

void Application::UpdatePhysx(float deltaTime)
{
	if (deltaTime <= 0)
	{
		return;
	}
	g_PhysicsScene->simulate(deltaTime);
	while (g_PhysicsScene->fetchResults() == false)
	{

	}
}

void Application::AddTestPhysxContent()
{
	//add a plane
	PxTransform pose = PxTransform(PxVec3(0.0f, 0, 0.0f), PxQuat(PxHalfPi*1.0f,	PxVec3(0.0f, 0.0f, 1.0f)));
	PxRigidStatic* plane = PxCreateStatic(*g_Physics, pose, PxPlaneGeometry(), *g_PhysicsMaterial);

	//add it to the physX scene
	g_PhysicsScene->addActor(*plane);

	/*
	//add a box
	float density = 10;
	PxBoxGeometry box(2, 2, 2);
	PxTransform transform(PxVec3(0, 5, 0));
	dynamicActor = PxCreateDynamic(*g_Physics, transform, box, *g_PhysicsMaterial, density);

	//PxTransform t = dynamicActor->getGlobalPose();
	//PxMat44 m = PxMat44(t);
	// physicsTransform =

	//temp[0] = t.q.getBasisVector0();

	//add it to the physX scene
	g_PhysicsScene->addActor(*dynamicActor);*/
}

void MyControllerHitReport::onShapeHit(const PxControllerShapeHit &hit)
{
	//gets a reference to a structure which tells us what has been hit and where
	//get the acter from the shape we hit
	PxRigidActor* actor = hit.shape->getActor();
	//get the normal of the thing we hit and store it so the player controller can respond correctly
	_playerContactNormal = hit.worldNormal;
	//try to cast to a dynamic actor
	PxRigidDynamic* myActor = actor->is<PxRigidDynamic>();
	if (myActor)
	{
		//this is where we can apply forces to things we hit
	}
}