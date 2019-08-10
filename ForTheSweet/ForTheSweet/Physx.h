#pragma once

#include <ctype.h>
#include "PxPhysicsAPI.h"

#include "Model.h"

using namespace physx;

class CPlayer;

//class PlayerHitReport : public PxUserControllerHitReport {
//public:
//	void	onShapeHit(const PxControllerShapeHit &hit) {
//		cout << "ShapedHit!!!\n";
//	}
//	void 	onControllerHit(const PxControllersHit &hit) {
//		cout << "ControllerHit!!!\n";
//	}
//	void 	onObstacleHit(const PxControllerObstacleHit &hit) {
//		cout << "ObstacleHit!!!\n";
//	}
//
//};

class PhysSimulation : public PxSimulationEventCallback
{
private:
	//void PlayerToEnemy(PxTriggerPair* trigger);
	//void EnemyToPlayer(PxTriggerPair* trigger);
	//
	CPlayer* player[8];

public:
	void onTrigger(PxTriggerPair* pairs, PxU32 count);
	void onAdvance(const PxRigidBody*const*, const PxTransform*, const PxU32) {}
	void onConstraintBreak(PxConstraintInfo*, PxU32) {}
	void onWake(PxActor**, PxU32) {}
	void onSleep(PxActor**, PxU32) {}
	void onContact(const PxContactPairHeader&, const PxContactPair*, PxU32) { }

	void setPlayer(CPlayer* pl, int index) { player[index] = pl; }
};

//class PhysBehavior : public PxControllerBehaviorCallback
//{
//public:
//	PxControllerBehaviorFlags getBehaviorFlags(const PxShape& shape, const PxActor& actor) = 0;
//	PxControllerBehaviorFlags getBehaviorFlags(const PxController& controller) = 0;
//	PxControllerBehaviorFlags getBehaviorFlags(const PxObstacle& obstacle) = 0;
//};


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

	//PlayerHitReport hitreport;

	PhysSimulation		m_Simulator;
	// Player �浹 ������ Capsule Or Box
	//physx::PxCapsuleControllerDesc m_CapsuleDesc;
	//physx::PxBoxControllerDesc m_BoxDesc;

	PxRigidActor *move_actor;

public:
	CPhysx();
	~CPhysx();

	void initPhysics();
	void move(DWORD direction, float distance);

	PxTriangleMesh*	GetTriangleMesh(mesh* meshes, UINT count);
	PxCapsuleController* getCapsuleController(PxExtendedVec3 pos, PxUserControllerHitReport* collisionCallback, CPlayer *player);
	PxRigidStatic*	getTrigger(PxVec3& t, XMFLOAT3 size);
	PxRigidDynamic* getRotateBox(PxVec3& t, PxVec3& ro, PxVec3 size);	// ���ݸ� ���м���

	void registerPlayer(CPlayer* player, int index) { m_Simulator.setPlayer(player, index); }

};