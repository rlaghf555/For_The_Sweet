#include "stdafx.h"
#include "Physx.h"
#include "Player.h"

PxControllerBehaviorFlags getBehaviorFlags(const PxShape& shape, const PxActor& actor)
{
	cout << "1\n";
}
PxControllerBehaviorFlags getBehaviorFlags(const PxController& controller)
{
	cout << "2\n";
}
PxControllerBehaviorFlags getBehaviorFlags(const PxObstacle& obstacle) 
{
	cout << "3\n";
}
void PhysSimulation::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	//cout << "Trigger Count : " << count << endl;

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
				if (player[j] != NULL) {
					//cout << j << endl;
					if (pairs[i].triggerActor != player[j]->m_AttackTrigger)
					{
						if (pairs[i].otherActor == player[j]->getControllerActor()) {
							cout << j << " Player Hitted\n";
							player[j]->ChangeAnimation(Anim_Small_React);
							player[j]->DisableLoop();
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
	m_PlayerController = NULL;
}

CPhysx::~CPhysx()
{
	if (m_PlayerController)
		m_PlayerController->release();

	if (m_PlayerManager)
		m_PlayerManager->release();

	if (m_Dispatcher)
		m_Dispatcher->release();

	if (m_Material)
		m_Material->release();

	if (m_Scene)
		m_Scene->release();

	if (m_Physics)
		m_Physics->release();

	if (m_Cooking)
		m_Cooking->release();

	if (m_Foundation)
		m_Foundation->release();
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

	m_PlayerManager = PxCreateControllerManager(*(m_Scene));
}

void CPhysx::move(DWORD direction, float distance)
{
	if (m_PlayerController)
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		XMFLOAT3 xmf3Direction = XMFLOAT3(0, 0, 0);
		//XMFLOAT3 Look = XMFLOAT3(0, 0, 0);

		if (direction & DIR_FORWARD) {
			//	Look.z = -1.f;
			//	xmf3Direction = Look;
			xmf3Direction.z = 1.f;
			xmf3Shift = Vector3::Add(xmf3Shift, xmf3Direction, distance);
		}
		if (direction & DIR_BACKWARD) {
			//	Look.z = 1.f;
			//	xmf3Direction = Look;
			xmf3Direction.z = -1.f;
			xmf3Shift = Vector3::Add(xmf3Shift, xmf3Direction, distance);
		}
		//화살표 키 ‘→’를 누르면 로컬 x-축 방향으로 이동한다. ‘←’를 누르면 반대 방향으로 이동한다.
		if (direction & DIR_RIGHT) {
			//	Look.x = -1.f;
			//	xmf3Direction = Look;
			xmf3Direction.x = 1.f;
			xmf3Shift = Vector3::Add(xmf3Shift, xmf3Direction, distance);
		}
		if (direction & DIR_LEFT) {
			//	Look.x = 1.f;
			//	xmf3Direction = Look;
			xmf3Direction.x = -1.f;
			xmf3Shift = Vector3::Add(xmf3Shift, xmf3Direction, distance);
		}

		PxVec3 disp;
		disp.x = xmf3Shift.x;
		disp.y = xmf3Shift.y;
		disp.z = xmf3Shift.z;

		PxControllerFilters filters;
		m_PlayerController->move(disp, 0, 1 / 60, filters);
	}
}

inline PxVec3* fromVertex(vertexDatas* vertex, int size)
{
	PxVec3* mem = new PxVec3[size];
	for (int i = 0; i < size; ++i) {
		mem[i] = PxVec3(vertex[i].m_pos.x, vertex[i].m_pos.y, vertex[i].m_pos.z);
	}
	return mem;
}

PxTriangleMesh*	CPhysx::GetTriangleMesh(mesh* meshes, UINT count) {
	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = meshes->m_vertices.size();
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = fromVertex(meshes->m_vertices.data(), meshes->m_vertices.size());

	//cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
	//
	//char title[20];
	//char test[8];
	//
	//sprintf_s(title, sizeof(title), "%d", type);
	//sprintf_s(test, sizeof(test), ".txt");
	//
	//strcat_s(title, sizeof(title), test);
	//
	//ofstream out(title);
	//out << meshes->m_vertices.size() << endl;

	//for (auto d : meshes->m_vertices)
	//{
	//	//cout << d.m_pos.x << "," << d.m_pos.y << "," << d.m_pos.z << endl;
	//	out << d.m_pos.x << endl << d.m_pos.y << endl << d.m_pos.z << endl;
	//}
	//cout << "size : " << meshes->m_vertices.size() << endl;
	//cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

	meshDesc.triangles.count = meshes->m_indices.size() / 3;
	meshDesc.triangles.stride = sizeof(int) * 3;
	meshDesc.triangles.data = meshes->m_indices.data();

	//cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
	//out << meshes->m_indices.size() << endl;
	//for (auto d : meshes->m_indices)
	//{
	//	//cout << d << endl;
	//	out << d << endl;;
	//}
	////cout << "size : " << meshes->m_indices.size() << endl;
	////cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
	//
	//out.close();
	//cout << type << endl;
	//type += 1;

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
	//triMesh = gCooking->createTriangleMesh(meshDesc, gPhysics->getPhysicsInsertionCallback());

	//PxU32 meshSize = 0;

	PxDefaultMemoryOutputStream outBuffer;
	m_Cooking->cookTriangleMesh(meshDesc, outBuffer);

	PxDefaultMemoryInputData stream(outBuffer.getData(), outBuffer.getSize());
	triMesh = m_Physics->createTriangleMesh(stream);
	//meshSize = outBuffer.getSize();

	return triMesh;
}

PxCapsuleController* CPhysx::getCapsuleController(PxExtendedVec3 pos, PxUserControllerHitReport* collisionCallback, CPlayer *player)
{
	PxCapsuleControllerDesc capsuleDesc;
	capsuleDesc.height = 15; //Height of capsule
	capsuleDesc.radius = 10; //Radius of casule
	capsuleDesc.position = pos; //Initial position of capsule
	capsuleDesc.material = m_Physics->createMaterial(1.0f, 1.0f, 1.0f); //Material for capsule shape
	//capsuleDesc.density = 1.0f; //Desity of capsule shape
	//capsuleDesc.contactOffset = 1.0f; //외부 물체와 상호작용하는 크기 (지정한 충돌캡슐보다 조금 더 크게 형성위해)
	//capsuleDesc.slopeLimit = cosf(XMConvertToRadians(1.0f)); //경사 허용도(degree) 0에 가까울수록 경사를 못올라감
	//capsuleDesc.stepOffset = 0.0f;	//자연스러운 이동 (약간의 고저에 부딫혔을 때 이동가능 여부)
													//stepoffset보다 큰 높이에 부딛치면 멈춤
	//capsuleDesc.maxJumpHeight = 2.0f; //최대 점프 높이
	//capsuleDesc.climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;
	//capsuleDesc.invisibleWallHeight = 0.0f;

	//충돌 콜백 함수
	capsuleDesc.reportCallback = collisionCallback;
	capsuleDesc.behaviorCallback = player;

	PxCapsuleController* controller = static_cast<PxCapsuleController*>(m_PlayerManager->createController(capsuleDesc));

	return controller;
}

PxRigidStatic * CPhysx::getTrigger(PxVec3 & t, XMFLOAT3 size)
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

PxRigidDynamic* CPhysx::getRotateBox(PxVec3& t, PxVec3& ro, PxVec3 size)
{
	PxShape* shape = m_Physics->createShape(PxBoxGeometry(size.x, size.y, size.z), *m_Physics->createMaterial(0.2f, 0.2f, 0.2f));
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);	//시물레이션 off
	//shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);			//트리거링 on

	PxVec3 init(1, 0, 0);

	//cout << "weapon look : " << ro.x << "," << ro.y << "," << ro.z << endl;

	PxVec3 a = init.cross(ro);

	PxQuat q;
	q.x = a.x;
	q.y = a.y;
	q.z = a.z;
	q.w = sqrt((init.magnitude() * init.magnitude()) * (ro.magnitude()*ro.magnitude())) + init.dot(ro);

	q = q.getNormalized();

	PxTransform temp(t, q);

	PxRigidDynamic * staticActor = m_Physics->createRigidDynamic(temp);

	//staticActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	//staticActor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
	//staticActor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
	//staticActor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);
	staticActor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, true);
	staticActor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, true);
	staticActor->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, true);


	//UserData* udata = new UserData;
	//udata->type = Player_Trigger;
	//
	staticActor->attachShape(*shape);
	//staticActor->userData = udata;
	//float* num = new float(0.0f);
	//staticActor->userData = (void*)num;
	/*int* tmp = (int*)staticActor->userData;
	*tmp = 1;*/

	m_Scene->addActor(*staticActor);

	return staticActor;
}