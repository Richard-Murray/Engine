#ifndef APPLICATION7_H
#define APPLICATION7_H

#include "core/gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "FBXFile.h"

#include "core/Renderer.h"
#include "core/EntityManager.h"
#include "core/AssetManager.h"
#include "core/PhysicsManager.h"

#include <PxPhysicsAPI.h>
#include <PxScene.h>
#include <pvd/PxVisualDebugger.h>

#include "core/ParticleEmitter.h"
#include "core/ParticleFluidEmitter.h"

#include "core/comp/PhysicsComponent.h"

#include "core/PhysxRagdoll.h"

using namespace physx;

class FlyCamera;
class MyControllerHitReport;

class Application
{
public:
	Application();
	~Application();

	int Run();

	void Load();

	void Update(float deltaTime);
	void Draw();

private:

	void InitialisePhysX();
	void InitialisePlayerController();
	void InitialiseVisualDebugger();

	void UpdatePhysx(float deltaTime);
	void AddTestPhysxContent();
	//void createOpenGLBuffers(FBXFile* fbx);
	//void cleanupOpenGLBuffers(FBXFile* fbx);

	//TEST CONTENT
	PxRigidDynamic* dynamicActor;
	glm::mat4 physicsTransform;
	float testTimer = 0;

private:
	Renderer* m_renderer;

	FlyCamera* m_camera;
	GLFWwindow* m_window;

	EntityManager* m_entityManager;
	AssetManager* m_assetManager;
	PhysicsManager* m_physicsManager;
	
	PxFoundation* g_PhysicsFoundation;
	PxPhysics* g_Physics;
	PxScene* g_PhysicsScene;
	PxDefaultErrorCallback gDefaultErrorCallback;
	PxDefaultAllocator gDefaultAllocatorCallback;
	PxSimulationFilterShader gDefaultFilterShader = PxDefaultSimulationFilterShader;
	PxMaterial* g_PhysicsMaterial;
	PxMaterial* g_boxMaterial;
	PxCooking* g_PhysicsCooker;

	PxControllerManager* g_CharacterManager;

	MyControllerHitReport* myHitReport;
	PxController* g_PlayerController;

	ParticleFluidEmitter* m_particleEmitter;
	SpringJoint* m_testSpringJoint;

	Ragdoll* m_ragdollTest;

	float _characterYVelocity;
	float _characterXVelocity;
	float _playerGravity;
	float _characterRotation;

	float gameWorldDeltaTime = 0;
	float deltaTimeModifier = 1;

	float FPSRefresh = 0;
	float framesPerSecond = 0;
	float previousFPS = 0;
	float FPSinterval = 1;
};

class myAllocator : public PxAllocatorCallback
{
public:
	virtual ~myAllocator() {}
	virtual void* allocate(size_t size, const char* typeName, const char* filename, int line)
	{
		void* pointer = _aligned_malloc(size, 16);
		return pointer;
	}
	virtual void deallocate(void* ptr)
	{
		_aligned_free(ptr);
	}
};

class MyControllerHitReport : public PxUserControllerHitReport
{
public:
	//overload the onShapeHit function
	virtual void onShapeHit(const PxControllerShapeHit &hit);
	//other collision functions which we must overload //these handle collision	with other controllers and hitting obstacles
	virtual void onControllerHit(const PxControllersHit &hit){};
	//Called when current controller hits another controller. More...
	virtual void onObstacleHit(const PxControllerObstacleHit &hit){};
	//Called when current controller hits a user-defined obstacl
	MyControllerHitReport() :PxUserControllerHitReport(){};
	PxVec3 getPlayerContactNormal(){ return _playerContactNormal; };
	void clearPlayerContactNormal(){ _playerContactNormal = PxVec3(0, 0, 0); };

private:
	PxVec3 _playerContactNormal;
};




#endif