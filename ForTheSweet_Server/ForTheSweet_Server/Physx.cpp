#include <vector>
#include "Physx.h"
#include "Player.h"

void PhysSimulation::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	for (PxU32 i = 0; i < count; ++i) {
		if (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			//PxTransform tmp = pairs[i].triggerActor->getGlobalPose();
			//cout << "Trigger Actor Pos : " << tmp.p.x << "," << tmp.p.y << "," << tmp.p.z << endl;
			//
			//tmp = pairs[i].otherActor->getGlobalPose();
			//cout << "Other Actor Pos : " << tmp.p.x << "," << tmp.p.y << "," << tmp.p.z << endl;

			for (int j = 0; j < 8; ++j)
			{
				//cout << j << endl;
				if (player[j]) 
				{
					if (pairs[i].triggerActor != player[j]->m_AttackTrigger)
					{
						if (player[j]->m_PlayerController != nullptr)
						{
							if (pairs[i].otherActor == player[j]->getControllerActor()) {
								//cout << j << " Player Hitted\n";
								player[j]->setAniIndex(Anim::Small_React);
								player[j]->hitted = true;
							}
						}
					}
				}
			}
		}
	}
} //트리거박스 충돌 체크

CPhysx::CPhysx()
{

	m_Foundation = NULL;
	m_Physics = NULL;

	m_Dispatcher = NULL;
	m_Scene = NULL;

	m_Material = NULL;

	m_PlayerManager = NULL;
	//m_PlayerController = NULL;
}

void CPhysx::initPhysics()
{
	m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);

	m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, PxTolerancesScale(), true, 0);

	PxSceneDesc sceneDesc(m_Physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);

	m_Dispatcher = PxDefaultCpuDispatcherCreate(1);
	sceneDesc.cpuDispatcher = m_Dispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	sceneDesc.simulationEventCallback = &m_Simulator;

	m_Scene = m_Physics->createScene(sceneDesc);

	m_Material = m_Physics->createMaterial(0.5f, 0.5f, 0.6f);

	m_Cooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_Foundation, PxCookingParams(PxTolerancesScale()));

	m_PlayerManager = PxCreateControllerManager(*m_Scene);
}

inline PxVec3* fromVertex(PxVec3 *ver, int size)
{
	PxVec3* mem = new PxVec3[size];
	for (int i = 0; i < size; ++i) {
		mem[i] = ver[i];
	}
	return mem;
}

PxTriangleMesh*	CPhysx::GetTriangleMesh(vector<PxVec3> ver, vector<int> index) {

	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = ver.size();
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = fromVertex(ver.data(), ver.size());

	meshDesc.triangles.count = index.size() / 3;
	meshDesc.triangles.stride = sizeof(int) * 3;
	meshDesc.triangles.data = index.data();

	meshDesc.flags = PxMeshFlags(0);
	PxCookingParams params = m_Cooking->getParams();
	params.midphaseDesc = PxMeshMidPhase::eBVH33;
	params.suppressTriangleMeshRemapTable = true;
	params.meshPreprocessParams &= ~static_cast<PxMeshPreprocessingFlags>(PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH);
	params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
	params.midphaseDesc.mBVH33Desc.meshCookingHint = PxMeshCookingHint::eCOOKING_PERFORMANCE;
	params.midphaseDesc.mBVH33Desc.meshSizePerformanceTradeOff = 0.0f;
	m_Cooking->setParams(params);

	PxTriangleMesh* triMesh = nullptr;
	PxDefaultMemoryOutputStream outBuffer;
	m_Cooking->cookTriangleMesh(meshDesc, outBuffer);

	PxDefaultMemoryInputData stream(outBuffer.getData(), outBuffer.getSize());
	triMesh = m_Physics->createTriangleMesh(stream);

	return triMesh;
}

PxCapsuleController* CPhysx::getCapsuleController(PxVec3 pos, float height, float radius, PxUserControllerHitReport* collisionCallback)
{
	PxCapsuleControllerDesc capsuleDesc;
	capsuleDesc.height = height; //Height of capsule
	capsuleDesc.radius = radius; //Radius of casule
	capsuleDesc.position = PXtoPXEx(pos) + PxExtendedVec3(0, 17.5, 0); //Initial position of capsule
	capsuleDesc.material = m_Physics->createMaterial(1.0f, 1.0f, 1.0f); //Material for capsule shape
	//capsuleDesc.density = 1.0f; //Desity of capsule shape
	//capsuleDesc.contactOffset = 1.0f; //외부 물체와 상호작용하는 크기 (지정한 충돌캡슐보다 조금 더 크게 형성위해)
	////capsuleDesc.slopeLimit = cosf(XMConvertToRadians(1.0f)); //경사 허용도(degree) 0에 가까울수록 경사를 못올라감
	//capsuleDesc.stepOffset = 0.0f;	//자연스러운 이동 (약간의 고저에 부딫혔을 때 이동가능 여부)
	//												//stepoffset보다 큰 높이에 부딛치면 멈춤
	////capsuleDesc.maxJumpHeight = 2.0f; //최대 점프 높이
	//capsuleDesc.climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;
	//capsuleDesc.invisibleWallHeight = 0.0f;

	//충돌 콜백 함수
	capsuleDesc.reportCallback = collisionCallback;

	PxCapsuleController* controller = static_cast<PxCapsuleController*>(m_PlayerManager->createController(capsuleDesc));

	return controller;
}

PxRigidStatic* CPhysx::getTrigger(PxVec3& t, PxVec3 size)
{
	PxShape* shape = m_Physics->createShape(PxBoxGeometry(size.x, size.y, size.z), *m_Physics->createMaterial(0.2f, 0.2f, 0.2f));
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);	//시물레이션 off
	shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);			//트리거링 on

	PxRigidStatic * staticActor = m_Physics->createRigidStatic(PxTransform(t));
	staticActor->attachShape(*shape);
	//float* num = new float(0.0f);
	//staticActor->userData = (void*)num;
	/*int* tmp = (int*)staticActor->userData;
	*tmp = 1;*/
	m_Scene->addActor(*staticActor);

	return staticActor;
}