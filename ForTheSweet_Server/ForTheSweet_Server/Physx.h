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
	// PxFoundation�� �����ϴ� Allocator, ErrorCallback
	physx::PxDefaultAllocator m_Allocator;
	physx::PxDefaultErrorCallback m_ErrorCallback;

	// Px�� �ֻ��� ��Ʈ(Foundation), Px�� ������ ��Ʈ(Physics)
	physx::PxFoundation* m_Foundation;
	physx::PxPhysics* m_Physics;

	// Physics�� �� matarial(���� �������, ���� �������, ź�����)
	physx::PxMaterial* m_Material;

	// �丮
	physx::PxCooking* m_Cooking;


	// �浹�߻� ���θ� �˷��ִ� dispatcher����
	//physx::PxDefaultCpuDispatcher* m_Dispatcher;

	// �������� Ȱ���� å���� Scene
	//physx::PxScene*	m_Scene;

	// Player�� ������ manager
	//physx::PxControllerManager *m_PlayerManager;

	// Player�� �����ϴ� manager�� ������ controller
	//physx::PxController *m_PlayerController;

	//PlayerHitReport hitreport;

	//PhysSimulation		m_Simulator;

	// Player �浹 ������ Capsule Or Box
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
