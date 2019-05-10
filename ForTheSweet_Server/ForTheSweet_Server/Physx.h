#pragma once
#include "header.h"
#include "PxPhysicsAPI.h"
#include "Util.h"

using namespace physx;

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

	} //트리거박스 충돌 체크
	void onAdvance(const PxRigidBody*const*, const PxTransform*, const PxU32) {}
	void onConstraintBreak(PxConstraintInfo*, PxU32) {}
	void onWake(PxActor**, PxU32) {}
	void onSleep(PxActor**, PxU32) {}
	void onContact(const PxContactPairHeader&, const PxContactPair*, PxU32) { }

	//void setPlayer(GameObject* object) { player = object; }
};

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
	//physx::PxController *m_PlayerController;

	// 요리
	physx::PxCooking* m_Cooking;

	//PlayerHitReport hitreport;

	PhysSimulation		m_Simulator;

	// Player 충돌 모형을 Capsule Or Box
	//physx::PxCapsuleControllerDesc m_CapsuleDesc;
	//physx::PxBoxControllerDesc m_BoxDesc;

public:
	CPhysx();
	~CPhysx() = default;

	void initPhysics();
	void move(int direction, float distance);

	PxTriangleMesh*	GetTriangleMesh(vector<PxVec3> ver, vector<int> index);
	PxCapsuleController* getCapsuleController(PxVec3 pos, float height, float radius, PxUserControllerHitReport* collisionCallback);
};

inline PxExtendedVec3 PXtoPXEx(const PxVec3& pos) {
	return PxExtendedVec3(pos.x, pos.y, pos.z);
}

inline PxVec3 PXExtoPX(const PxExtendedVec3& pos) {
	return PxVec3(pos.x, pos.y, pos.z);
}

inline PxVec3 Normalize(PxVec3& vec)
{
	PxVec3 Normal;
	
	float vec_size = sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
	if (vec_size > 0)
	{
		Normal.x = vec.x / vec_size;
		Normal.y = vec.y / vec_size;
		Normal.z = vec.z / vec_size;
	}
	else {
		Normal.x = 0.0f;
		Normal.y = 0.0f;
		Normal.z = 0.0f;
	}

	return(Normal);
}
