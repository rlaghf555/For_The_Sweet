#include "stdafx.h"
#include "Physx.h"

CPhysx::CPhysx()
{

	m_Foundation = NULL;
	m_Physics = NULL;

	m_Dispatcher = NULL;
	m_Scene = NULL;

	m_Material = NULL;

	m_PlayerManager = NULL;
	m_PlayerController = NULL;
}

void CPhysx::initPhysics()
{
	m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);

	m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, PxTolerancesScale(), true, 0);

	PxSceneDesc sceneDesc(m_Physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);

	m_Dispatcher = PxDefaultCpuDispatcherCreate(1);
	sceneDesc.cpuDispatcher = m_Dispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	sceneDesc.simulationEventCallback = &m_Simulator;

	m_Scene = m_Physics->createScene(sceneDesc);

	m_Material = m_Physics->createMaterial(0.5f, 0.5f, 0.6f);

	m_Cooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_Foundation, PxCookingParams(PxTolerancesScale()));
}

void CPhysx::move(DWORD direction, float distance)
{
	if (m_PlayerController)
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		XMFLOAT3 xmf3Direction = XMFLOAT3(0, 0, 0);
		//XMFLOAT3 Look = XMFLOAT3(0, 0, 0);

		if (direction & DIR_FORWARD) {
			//	Look.z = -1.f;
			//	xmf3Direction = Look;
			xmf3Direction.z = 1.f;
			xmf3Shift = Vector3::Add(xmf3Shift, xmf3Direction, distance);
		}
		if (direction & DIR_BACKWARD) {
			//	Look.z = 1.f;
			//	xmf3Direction = Look;
			xmf3Direction.z = -1.f;
			xmf3Shift = Vector3::Add(xmf3Shift, xmf3Direction, distance);
		}
		//화살표 키 ‘→’를 누르면 로컬 x-축 방향으로 이동한다. ‘←’를 누르면 반대 방향으로 이동한다.
		if (direction & DIR_RIGHT) {
			//	Look.x = -1.f;
			//	xmf3Direction = Look;
			xmf3Direction.x = 1.f;
			xmf3Shift = Vector3::Add(xmf3Shift, xmf3Direction, distance);
		}
		if (direction & DIR_LEFT) {
			//	Look.x = 1.f;
			//	xmf3Direction = Look;
			xmf3Direction.x = -1.f;
			xmf3Shift = Vector3::Add(xmf3Shift, xmf3Direction, distance);
		}

		PxVec3 disp;
		disp.x = xmf3Shift.x;
		disp.y = xmf3Shift.y;
		disp.z = xmf3Shift.z;

		PxControllerFilters filters;
		m_PlayerController->move(disp, 0, 1 / 60, filters);
	}
}

inline PxVec3* fromVertex(vertexDatas* vertex, int size)
{
	PxVec3* mem = new PxVec3[size];
	for (int i = 0; i < size; ++i) {
		mem[i] = PxVec3(vertex[i].m_pos.x, vertex[i].m_pos.y, vertex[i].m_pos.z);
	}
	return mem;
}

PxTriangleMesh*	CPhysx::GetTriangleMesh(mesh* meshes, UINT count) {
	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = meshes->m_vertices.size();
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = fromVertex(meshes->m_vertices.data(), meshes->m_vertices.size());

	/*cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

	ofstream out("map.txt");
	out << meshes->m_vertices.size() << endl;

	for (auto d : meshes->m_vertices)
	{
		cout << d.m_pos.x << "," << d.m_pos.y << "," << d.m_pos.z << endl;
		out << d.m_pos.x << endl << d.m_pos.y << endl << d.m_pos.z << endl;
	}
	cout << "size : " << meshes->m_vertices.size() << endl;
	cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";*/

	meshDesc.triangles.count = meshes->m_indices.size() / 3;
	meshDesc.triangles.stride = sizeof(int) * 3;
	meshDesc.triangles.data = meshes->m_indices.data();

	/*cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
	out << meshes->m_indices.size() << endl;
	for (auto d : meshes->m_indices)
	{
		cout << d << endl;
		out << d << endl;;
	}
	cout << "size : " << meshes->m_indices.size() << endl;
	cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

	out.close();*/

	meshDesc.flags = PxMeshFlags(0);
	PxCookingParams params = m_Cooking->getParams();
	params.midphaseDesc = PxMeshMidPhase::eBVH33;
	params.suppressTriangleMeshRemapTable = true;
	params.meshPreprocessParams &= ~static_cast<PxMeshPreprocessingFlags>(PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH);
	params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
	params.midphaseDesc.mBVH33Desc.meshCookingHint = PxMeshCookingHint::eCOOKING_PERFORMANCE;
	params.midphaseDesc.mBVH33Desc.meshSizePerformanceTradeOff = 0.0f;
	m_Cooking->setParams(params);

	PxTriangleMesh* triMesh = nullptr;
	//triMesh = gCooking->createTriangleMesh(meshDesc, gPhysics->getPhysicsInsertionCallback());

	//PxU32 meshSize = 0;

	PxDefaultMemoryOutputStream outBuffer;
	m_Cooking->cookTriangleMesh(meshDesc, outBuffer);

	PxDefaultMemoryInputData stream(outBuffer.getData(), outBuffer.getSize());
	triMesh = m_Physics->createTriangleMesh(stream);
	//meshSize = outBuffer.getSize();

	return triMesh;
}
