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

	// 요리
	physx::PxCooking* m_Cooking;

	//PlayerHitReport hitreport;

	PhysSimulation		m_Simulator;
	// Player 충돌 모형을 Capsule Or Box
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
	PxRigidDynamic* getRotateBox(PxVec3& t, PxVec3& ro, PxVec3 size);	// 초콜릿 방패세팅

	void registerPlayer(CPlayer* player, int index) { m_Simulator.setPlayer(player, index); }

};