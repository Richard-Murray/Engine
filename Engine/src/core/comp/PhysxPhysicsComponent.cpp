#include "PhysxPhysicsComponent.h"

//NOT BEING DELETED!
PhysxPhysicsComponent::~PhysxPhysicsComponent()
{
	if (m_physicsObjectType != "Undefined")
	{
		if (m_dynamic)
		{
			m_physicsScene->removeActor(*m_dynamicActor, true);
		}
		else
		{
			m_physicsScene->removeActor(*m_staticActor, true);
		}
	}
}

void PhysxPhysicsComponent::Initialise()
{
	SetComponentType("PhysxPhysics");
	//m_dynamicActor->setName("unnamed");

	
}

void PhysxPhysicsComponent::SetName(const char* name, bool dynamic)
{
	if (dynamic)
	{
		m_dynamicActor->setName(name);
	}
	else
	{
		m_staticActor->setName(name);
	}
}

void PhysxPhysicsComponent::SetPhysicsScene(PxPhysics* physics, PxScene* physicsScene)
{
	m_physics = physics;
	m_physicsScene = physicsScene;
	m_material = m_physics->createMaterial(0.5f, 0.5f, 0.5f);
	SetPhysicsObjectType("Undefined"); //If the object is uninitialised past this point it remains undefined and not used
}

void PhysxPhysicsComponent::SetPhysicsObjectType(const char* type)
{
	m_physicsObjectType = type;
}

void PhysxPhysicsComponent::Update(float deltaTime)
{
	if (m_physicsObjectType != "Undefined")
	{
		PxTransform trans;
		if (m_dynamic)
		{
			trans = m_dynamicActor->getGlobalPose();
		}
		else
		{
			trans = m_staticActor->getGlobalPose();
		}

		PxVec3 vX = trans.q.getBasisVector0();
		PxVec3 vY = trans.q.getBasisVector1();
		PxVec3 vZ = trans.q.getBasisVector2();

		glm::mat4 matrixTransform;
		matrixTransform[0].xyz = glm::vec3(vX.x, vX.y, vX.z);
		matrixTransform[1].xyz = glm::vec3(vY.x, vY.y, vY.z);
		matrixTransform[2].xyz = glm::vec3(vZ.x, vZ.y, vZ.z);
		matrixTransform[3].xyz = glm::vec3(trans.p.x, trans.p.y, trans.p.z);

		m_parent->SetWorldTransform(matrixTransform);
	}
}

void PhysxPhysicsComponent::SetBox(glm::vec3 boxGeometry, float density, bool dynamic)
{
	PxBoxGeometry box(boxGeometry.x, boxGeometry.y, boxGeometry.z);

	glm::mat4 parentTransform = m_parent->GetWorldTransform();
	PxTransform transform(*(PxMat44*)(&parentTransform[0])); //cast between
		/*PxMat44(	PxVec4(parentTransform[0].x, parentTransform[0].y, parentTransform[0].z, parentTransform[0].w),
									PxVec4(parentTransform[1].x, parentTransform[1].y, parentTransform[1].z, parentTransform[1].w),
									PxVec4(parentTransform[2].x, parentTransform[2].y, parentTransform[2].z, parentTransform[2].w),
									PxVec4(parentTransform[3].x, parentTransform[3].y, parentTransform[3].z, parentTransform[3].w)));*/

	if (dynamic)
	{
		m_dynamicActor = PxCreateDynamic(*m_physics, transform, box, *m_material, density);
		m_dynamicActor->setName("unnamed");
		m_physicsScene->addActor(*m_dynamicActor);
		m_dynamic = dynamic;

	}
	else
	{
		m_staticActor = PxCreateStatic(*m_physics, transform, box, *m_material);
		m_staticActor->setName("unnamed");
		m_physicsScene->addActor(*m_staticActor);
		m_dynamic = dynamic;
	}
	
	SetPhysicsObjectType("Box");

	if (m_dynamic)
		SetUpFiltering(m_dynamicActor, FilterGroup::eGENERICOBJECT, FilterGroup::eGROUND);
	else
		SetUpFiltering(m_staticActor, FilterGroup::eGENERICOBJECT, FilterGroup::eGROUND);
}

void PhysxPhysicsComponent::SetSphere(float radius, float density, bool dynamic)
{
	PxSphereGeometry sphere(radius);

	glm::mat4 parentTransform = m_parent->GetWorldTransform();
	PxTransform transform(*(PxMat44*)(&parentTransform[0]));
	//parentTransform = (*(glm::mat4*)(&transform));
		/*PxMat44(PxVec4(parentTransform[0].x, parentTransform[0].y, parentTransform[0].z, parentTransform[0].w),
		PxVec4(parentTransform[1].x, parentTransform[1].y, parentTransform[1].z, parentTransform[1].w),
		PxVec4(parentTransform[2].x, parentTransform[2].y, parentTransform[2].z, parentTransform[2].w),
		PxVec4(parentTransform[3].x, parentTransform[3].y, parentTransform[3].z, parentTransform[3].w)));*/

	if (dynamic)
	{
		m_dynamicActor = PxCreateDynamic(*m_physics, transform, sphere, *m_material, density);
		m_dynamicActor->setName("unnamed");
		m_physicsScene->addActor(*m_dynamicActor);
		m_dynamic = dynamic;
	}
	else
	{
		m_staticActor = PxCreateStatic(*m_physics, transform, sphere, *m_material);
		m_staticActor->setName("unnamed");
		m_physicsScene->addActor(*m_staticActor);
		m_dynamic = dynamic;
	}
	//m_dynamicActor = PxCreateDynamic(*m_physics, transform, sphere, *m_material, density);
	//m_physicsScene->addActor(*m_dynamicActor);
	SetPhysicsObjectType("Sphere");

	if (m_dynamic)
		SetUpFiltering(m_dynamicActor, FilterGroup::eGENERICOBJECT, FilterGroup::eGROUND);
	else
		SetUpFiltering(m_staticActor, FilterGroup::eGENERICOBJECT, FilterGroup::eGROUND);
}

void PhysxPhysicsComponent::SetCapsule(float radius, float halfHeight, float density, bool dynamic)
{
	PxCapsuleGeometry capsule(radius, halfHeight);

	glm::mat4 parentTransform = m_parent->GetWorldTransform();
	PxTransform transform(*(PxMat44*)(&parentTransform[0]));

	if (dynamic)
	{
		m_dynamicActor = PxCreateDynamic(*m_physics, transform, capsule, *m_material, density);
		m_dynamicActor->setName("unnamed");
		m_physicsScene->addActor(*m_dynamicActor);
		m_dynamic = dynamic;
	}
	else
	{
		m_staticActor = PxCreateStatic(*m_physics, transform, capsule, *m_material);
		m_staticActor->setName("unnamed");
		m_physicsScene->addActor(*m_staticActor);
		m_dynamic = dynamic;
	}
	//m_dynamicActor = PxCreateDynamic(*m_physics, transform, sphere, *m_material, density);
	//m_physicsScene->addActor(*m_dynamicActor);
	SetPhysicsObjectType("Capsule");
}

void PhysxPhysicsComponent::AttachRigidBodyConvex(FBXFile* m_FBX, float density, PxMaterial* g_PhysicsMaterial, PxCooking* physicsCooker)
{
	//need a placeholder box
	PxBoxGeometry box = PxBoxGeometry(1, 1, 1);
	PxTransform transform(*(PxMat44*)(&m_parent->GetWorldTransform()[0])); //PhysX and GLM
	//matricies are the same internally so we simply cast between them;
	m_dynamicActor = PxCreateDynamic(*m_physics, transform, box, *g_PhysicsMaterial, density);
	m_dynamicActor->userData = this; //link the PhysX actor to our FBX model
	int numberVerts = 0;
	//find out how many verts there are in total in tank model
	for (unsigned int i = 0; i < m_FBX->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = m_FBX->getMeshByIndex(i);
		numberVerts += mesh->m_vertices.size();
	}
	//reserve space for vert buffer
	PxVec3 *verts = new PxVec3[numberVerts]; //temporary buffer for our verts
	int vertIDX = 0;
	//copy our verts from all the sub meshes
	//and tranform them into the same space
	for (unsigned int i = 0; i < m_FBX->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = m_FBX->getMeshByIndex(i);
		int numberVerts = mesh->m_vertices.size();
		for (int vertCount = 0; vertCount< numberVerts; vertCount++)
		{
			glm::vec4 temp = mesh->m_globalTransform * glm::mat4(10, 0, 0, 0, 0, 10, 0, 0, 0, 0, 10, 0, 0, 0, 0, 1) * glm::rotate(glm::pi<float>() * 0.25f, glm::vec3(1, 0, 0)) * glm::rotate(glm::pi<float>() * 0.5f, glm::vec3(0, 0, 1)) * mesh->m_vertices[vertCount].position;
			verts[vertIDX++] = PxVec3(temp.x, temp.y, temp.z);
		}
	}
	PxConvexMeshDesc convexDesc;
	convexDesc.points.count = numberVerts;
	convexDesc.points.stride = sizeof(PxVec3);
	convexDesc.points.data = verts;
	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;
	convexDesc.vertexLimit = 128;

	PxDefaultMemoryOutputStream* buf = new PxDefaultMemoryOutputStream();
	bool bSucceed = physicsCooker->cookConvexMesh(convexDesc, *buf);
	assert(bSucceed);

	PxU8* contents = buf->getData();
	PxU32 size = buf->getSize();
	PxDefaultMemoryInputData input(contents, size);
	PxConvexMesh* convexMesh = m_physics->createConvexMesh(input);
	PxTransform pose = PxTransform(PxVec3(0.0f, 0, 0.0f));

	//remove the placeholder box we started with
	int numberShapes = m_dynamicActor->getNbShapes();
	PxShape** shapes = (PxShape**)_aligned_malloc(sizeof(PxShape*)*numberShapes, 16);
	m_dynamicActor->getShapes(shapes, numberShapes);
	m_dynamicActor->detachShape(**shapes);

	PxShape* convexShape = m_dynamicActor->createShape(PxConvexMeshGeometry(convexMesh), *g_PhysicsMaterial, pose);
	
	delete(verts); //delete our temporary vert buffer.
	//Add it to the scene
	m_physicsScene->addActor(*m_dynamicActor);

	m_dynamic = true;
	SetPhysicsObjectType("Mesh");
}

//helper function to set up filtering
void PhysxPhysicsComponent::SetUpFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask)
{
	PxFilterData filterData;
	filterData.word0 = filterGroup; // word0 = own ID
	filterData.word1 = filterMask; // word1 = ID mask to filter pairs that trigger a contact callback;
	const PxU32 numShapes = actor->getNbShapes();
	PxShape** shapes = (PxShape**)_aligned_malloc(sizeof(PxShape*)*numShapes, 16);
	actor->getShapes(shapes, numShapes);
	for (PxU32 i = 0; i < numShapes; i++)
	{
		PxShape* shape = shapes[i];
		shape->setSimulationFilterData(filterData);
	}
	_aligned_free(shapes);
}

void PhysxPhysicsComponent::SetShapeAsTrigger(/*PxRigidActor* actorIn*/)
{
	PxRigidStatic* staticActor = m_staticActor;//actorIn->is<PxRigidStatic>();
	assert(staticActor);
	const PxU32 numShapes = staticActor->getNbShapes();
	PxShape** shapes = (PxShape**)_aligned_malloc(sizeof(PxShape*)*numShapes, 16);
	staticActor->getShapes(shapes, numShapes);
	for (PxU32 i = 0; i < numShapes; i++)
	{
		shapes[i]->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		shapes[i]->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	}
}