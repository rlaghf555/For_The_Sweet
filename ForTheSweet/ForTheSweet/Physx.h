#pragma once

#include <ctype.h>
#include "PxPhysicsAPI.h"

#include "Physx.h"
using namespace physx;
class CPhysx {
public:
	// PxFoundation이 관리하는 Allocator, ErrorCallback
	physx::PxDefaultAllocator m_Allocator;
	physx::PxDefaultErrorCallback m_ErrorCallback;

	// Px의 최상위 루트(Foundation), Px의 차상위 루트(Physics)
	physx::PxFoundation* m_Foundation;
	physx::PxPhysics* m_Physics;

	// 충돌발생 여부를 알려주는 dispatcher역할
	physx::PxDefaultCpuDispatcher* m_Dispatcher;

	// 물리적인 활동을 책임질 Scene
	physx::PxScene*	m_Scene;

	// Physics가 쓸 matarial(정적 마찰계수, 동적 마찰계수, 탄성계수)
	physx::PxMaterial* m_Material;

	// Player를 관리할 manager
	physx::PxControllerManager *m_PlayerManager;
	// Player를 관리하는 manager가 생성할 controller
	physx::PxController *m_PlayerController;

	// Player 충돌 모형을 Capsule Or Box
	//physx::PxCapsuleControllerDesc m_CapsuleDesc;
	//physx::PxBoxControllerDesc m_BoxDesc;

public:
	CPhysx();
	~CPhysx() = default;

	void initPhysics();
	void move(DWORD direction, float distance);
};

