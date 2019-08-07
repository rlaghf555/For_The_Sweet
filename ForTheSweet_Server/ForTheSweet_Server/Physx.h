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
	//physx::PxController *m_PlayerController;

	// �丮
	physx::PxCooking* m_Cooking;

	//PlayerHitReport hitreport;

	PhysSimulation		m_Simulator;

	// Player �浹 ������ Capsule Or Box
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
