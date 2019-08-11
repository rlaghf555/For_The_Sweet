#pragma once
#include "header.h"
#include "PxPhysicsAPI.h"
#include "Util.h"
#include "Protocol.h"

class CPlayer;
class CRoom;

using namespace physx;

struct UserData {
	char order = 0;
	char type = 0;
};

class PhysSimulation : public PxSimulationEventCallback
{
private:

	CPlayer* player[8];
	CRoom* room;

public:
	void onTrigger(PxTriggerPair* pairs, PxU32 count);
	void onAdvance(const PxRigidBody*const*, const PxTransform*, const PxU32) {}
	void onConstraintBreak(PxConstraintInfo*, PxU32) {}
	void onWake(PxActor**, PxU32) {}
	void onSleep(PxActor**, PxU32) {}
	void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs);
	void setPlayer(CPlayer* pl, int index) { player[index] = pl; }
	void setRoom(CRoom* ro) { room = ro; }
	//void removePlayer(int index) { delete player[index]; player[index] = nullptr; }
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

	// Physics가 쓸 matarial(정적 마찰계수, 동적 마찰계수, 탄성계수)
	physx::PxMaterial* m_Material;

	// 요리
	physx::PxCooking* m_Cooking;


	// 충돌발생 여부를 알려주는 dispatcher역할
	//physx::PxDefaultCpuDispatcher* m_Dispatcher;

	// 물리적인 활동을 책임질 Scene
	//physx::PxScene*	m_Scene;

	// Player를 관리할 manager
	//physx::PxControllerManager *m_PlayerManager;

	// Player를 관리하는 manager가 생성할 controller
	//physx::PxController *m_PlayerController;

	//PlayerHitReport hitreport;

	//PhysSimulation		m_Simulator;

	// Player 충돌 모형을 Capsule Or Box
	//physx::PxCapsuleControllerDesc m_CapsuleDesc;
	//physx::PxBoxControllerDesc m_BoxDesc;

public:
	CPhysx();
	~CPhysx() = default;

	void initPhysics();

};

inline PxExtendedVec3 PXtoPXEx(const PxVec3& pos) {
	return PxExtendedVec3(pos.x, pos.y, pos.z);
}

inline PxVec3 PXExtoPX(const PxExtendedVec3& pos) {
	return PxVec3(pos.x, pos.y, pos.z);
}
