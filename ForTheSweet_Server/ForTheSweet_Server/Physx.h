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
	//void PlayerToEnemy(PxTriggerPair* trigger);
	//void EnemyToPlayer(PxTriggerPair* trigger);
	//
	//GameObject* player = nullptr;
	CPlayer* player[8];
	CRoom* room;

public:
	void onTrigger(PxTriggerPair* pairs, PxU32 count);
	void onAdvance(const PxRigidBody*const*, const PxTransform*, const PxU32) {}
	void onConstraintBreak(PxConstraintInfo*, PxU32) {}
	void onWake(PxActor**, PxU32) {}
	void onSleep(PxActor**, PxU32) {}
	void onContact(const PxContactPairHeader&, const PxContactPair*, PxU32) { }

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

	PxTriangleMesh*	GetTriangleMesh(vector<PxVec3> ver, vector<int> index);
	void getBoxController(PxVec3 pos, PxVec3 size);
	PxCapsuleController* getCapsuleController(PxVec3 pos, float height, float radius, PxUserControllerHitReport* collisionCallback);
	PxRigidStatic* getBoxTrigger(PxVec3& t, PxVec3 size);
	PxRigidStatic* getRotateBoxTrigger(PxVec3& t, PxVec3& ro, PxVec3 size, int trigger_type, int order);
	PxRigidStatic* getSphereTrigger(PxVec3& t, PxReal rad);

	PxRigidStatic* getBox(PxVec3& t, PxVec3 size);

	void registerPlayer(CPlayer* player, int index) { m_Simulator.setPlayer(player, index); }
	void registerRoom(CRoom* room) { m_Simulator.setRoom(room); }
	//void removePlayer(int index) { m_Simulator.removePlayer(index); }
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
