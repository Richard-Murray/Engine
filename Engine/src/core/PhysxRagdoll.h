#ifndef PHYSX_RAGDOLL_H
#define PHYSX_RAGDOLL_H

#include <PxPhysicsAPI.h>
#include <PxScene.h>
#include <pvd/PxVisualDebugger.h>

#include "EntityManager.h"

using namespace physx;


enum RagDollParts
{
	NO_PARENT = -1,
	LOWER_SPINE,
	LEFT_PELVIS,
	RIGHT_PELVIS,
	LEFT_UPPER_LEG,
	RIGHT_UPPER_LEG,
	LEFT_LOWER_LEG,
	RIGHT_LOWER_LEG,
	UPPER_SPINE,
	LEFT_CLAVICLE,
	RIGHT_CLAVICLE,
	NECK,
	HEAD,
	LEFT_UPPER_ARM,
	RIGHT_UPPER_ARM,
	LEFT_LOWER_ARM,
	RIGHT_LOWER_ARM,
};

//create some constants for axis of rotation to make definition of quaternions a bit neater
const PxVec3 X_AXIS = PxVec3(1, 0, 0);
const PxVec3 Y_AXIS = PxVec3(0, 1, 0);
const PxVec3 Z_AXIS = PxVec3(0, 0, 1);

struct RagdollNode
{
	PxQuat globalRotation; 
	PxVec3 scaledGobalPos;

	int parentNodeIdx;
	float halfLength;
	float radius;
	float parentLinkPos; 
	float childLinkPos;
	char* name;

	PxArticulationLink* linkPtr;
	
	RagdollNode(PxQuat _globalRotation, int _parentNodeIdx, float _halfLength, float
		_radius, float _parentLinkPos, float _childLinkPos, char* _name){
		globalRotation = _globalRotation, parentNodeIdx = _parentNodeIdx; halfLength = _halfLength; radius = _radius;
		parentLinkPos = _parentLinkPos; childLinkPos = _childLinkPos; name = _name;
	};
};

class Ragdoll
{
public:
	Ragdoll(PxPhysics* physics, PxTransform transform, float scaleFactor, PxMaterial* material, PxScene* scene);
	~Ragdoll();

	PxArticulation* MakeRagdoll(PxPhysics* physics, PxTransform transform, float scaleFactor, PxMaterial* material);
	void SetUpRendering(EntityManager* pEntityManager);

	PxArticulation* GetArticulation(){ return m_articulation; };

private:
	RagdollNode** m_ragdollData;
	PxArticulation* m_articulation;

	EntityManager* m_pEntityManager;
	Entity* m_renderingEntity;

};



#endif