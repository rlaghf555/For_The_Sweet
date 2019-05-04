#pragma once

#include <ctype.h>
#include "PxPhysicsAPI.h"

#include "Physx.h"
#include "Model.h"

using namespace physx;

class PlayerHitReport : public PxUserControllerHitReport {
public:
	void	onShapeHit(const PxControllerShapeHit &hit) {
		cout << "ShapedHit!!!\n";
	}
	void 	onControllerHit(const PxControllersHit &hit) {
		cout << "ControllerHit!!!\n";
	}
	void 	onObstacleHit(const PxControllerObstacleHit &hit) {
		cout << "ObstacleHit!!!\n";
	}

};

class PhysSimulation : public PxSimulationEventCallback
{
private:
	//void PlayerToEnemy(PxTriggerPair* trigger);
	//void EnemyToPlayer(PxTriggerPair* trigger);
	//
	//GameObject* player = nullptr;
public:
	void onTrigger(PxTriggerPair* pairs, PxU32 count)
	{
		cout << "Trigger Count : " << count << endl;

		for (PxU32 i = 0; i < count; ++i) {
			if (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				PxTransform tmp = pairs[i].triggerActor->getGlobalPose();

				cout << "Trigger Actor Pos : " << tmp.p.x << "," << tmp.p.y << "," << tmp.p.z << endl;

				tmp = pairs[i].otherActor->getGlobalPose();
				cout << "Other Actor Pos : " << tmp.p.x << "," << tmp.p.y << "," << tmp.p.z << endl;
			}
		}

	} //Ʈ���Źڽ� �浹 üũ
	void onAdvance(const PxRigidBody*const*, const PxTransform*, const PxU32) {}
	void onConstraintBreak(PxConstraintInfo*, PxU32) {}
	void onWake(PxActor**, PxU32) {}
	void onSleep(PxActor**, PxU32) {}
	void onContact(const PxContactPairHeader&, const PxContactPair*, PxU32) { }

	//void setPlayer(GameObject* object) { player = object; }
};

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

	// �丮
	physx::PxCooking* m_Cooking;

	PlayerHitReport hitreport;

	PhysSimulation		m_Simulator;
	// Player �浹 ������ Capsule Or Box
	//physx::PxCapsuleControllerDesc m_CapsuleDesc;
	//physx::PxBoxControllerDesc m_BoxDesc;

public:
	CPhysx();
	~CPhysx() = default;

	void initPhysics();
	void move(DWORD direction, float distance);

	PxTriangleMesh*	GetTriangleMesh(mesh* meshes, UINT count);
};