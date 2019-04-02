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

	m_Dispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = m_Dispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;

	m_Scene = m_Physics->createScene(sceneDesc);

	m_Material = m_Physics->createMaterial(0.5f, 0.5f, 0.6f);
}

void CPhysx::move(DWORD direction, float distance)
{
	if (m_PlayerController)
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		XMFLOAT3 xmf3Direction = XMFLOAT3(0, 0, 0);
		XMFLOAT3 Look = XMFLOAT3(0, 0, 0);

		if (direction & DIR_FORWARD) {
			Look.z = -1.f;
			xmf3Direction = Look;
			xmf3Direction.z = 1.f;
			xmf3Shift = Vector3::Add(xmf3Shift, xmf3Direction, distance);
		}
		if (direction & DIR_BACKWARD) {
			Look.z = 1.f;
			xmf3Direction = Look;
			xmf3Direction.z = -1.f;
			xmf3Shift = Vector3::Add(xmf3Shift, xmf3Direction, distance);
		}
		//화살표 키 ‘→’를 누르면 로컬 x-축 방향으로 이동한다. ‘←’를 누르면 반대 방향으로 이동한다.
		if (direction & DIR_RIGHT) {
			Look.x = -1.f;
			xmf3Direction = Look;
			xmf3Direction.x = 1.f;
			xmf3Shift = Vector3::Add(xmf3Shift, xmf3Direction, distance);
		}
		if (direction & DIR_LEFT) {
			Look.x = 1.f;
			xmf3Direction = Look;
			xmf3Direction.x = -1.f;
			xmf3Shift = Vector3::Add(xmf3Shift, xmf3Direction, distance);
		}

		PxVec3 disp;
		disp.x = xmf3Shift.x;
		disp.y = xmf3Shift.y;
		disp.z = xmf3Shift.z;

		PxControllerFilters filters;
		m_PlayerController->move(disp, 0, 1/60, filters);
	}
}

