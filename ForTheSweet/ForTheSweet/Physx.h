#pragma once

#include <ctype.h>
#include "PxPhysicsAPI.h"

#include "Physx.h"
using namespace physx;
class CPhysx {
public:
	// PxFoundation�� �����ϴ� Allocator, ErrorCallback
	physx::PxDefaultAllocator m_Allocator;
	physx::PxDefaultErrorCallback m_ErrorCallback;

	// Px�� �ֻ��� ��Ʈ(Foundation), Px�� ������ ��Ʈ(Physics)
	physx::PxFoundation* m_Foundation;
	physx::PxPhysics* m_Physics;

	// �浹�߻� ���θ� �˷��ִ� dispatcher����
	physx::PxDefaultCpuDispatcher* m_Dispatcher;

	// �������� Ȱ���� å���� Scene
	physx::PxScene*	m_Scene;

	// Physics�� �� matarial(���� �������, ���� �������, ź�����)
	physx::PxMaterial* m_Material;

	// Player�� ������ manager
	physx::PxControllerManager *m_PlayerManager;
	// Player�� �����ϴ� manager�� ������ controller
	physx::PxController *m_PlayerController;

	// Player �浹 ������ Capsule Or Box
	//physx::PxCapsuleControllerDesc m_CapsuleDesc;
	//physx::PxBoxControllerDesc m_BoxDesc;

public:
	CPhysx();
	~CPhysx() = default;

	void initPhysics();
	void move(DWORD direction, float distance);
};

