#include <vector>
#include "Physx.h"
#include "Player.h"
#include "Room.h"

void add_timer(CRoom *room, char order, high_resolution_clock::time_point start_time)
{
	int room_num = room->room_num + ROOM_TIMER_START;
	cout << "Crush\n";

	room->m_timer_l.lock();
	room->m_timer_queue.push(EVENT_ST{ room_num, EV_WEAPON_REMOVE, start_time, order });
	room->m_timer_l.unlock();
}

void PhysSimulation::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	for (PxU32 i = 0; i < count; ++i) {
		if (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_LOST)
		{
			for (int j = 0; j < MAX_ROOM_USER; ++j) {
				if (Lollipop_Trigger == reinterpret_cast<UserData *>(pairs[i].triggerActor->userData)->type)
				{
					if (player[j])
					{
						if (player[j]->m_PlayerController != nullptr)
						{
							if (pairs[i].otherActor == player[j]->getControllerActor()) {
								cout << j << " Player : Lollipop Heal Fail\n";
								//cout << player[j]->getControllerActor()->getGlobalPose().p.x << "," 
								//	<< player[j]->getControllerActor()->getGlobalPose().p.y << "," 
								//	<< player[j]->getControllerActor()->getGlobalPose().p.z << endl;

								player[j]->setLollipopHeal(false);
							}
						}
					}
				}
			}
		}

		if (pairs[i].status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			//PxTransform tmp = pairs[i].triggerActor->getGlobalPose();
			//cout << "Trigger Actor Pos : " << tmp.p.x << "," << tmp.p.y << "," << tmp.p.z << endl;
			//
			//tmp = pairs[i].otherActor->getGlobalPose();
			//cout << "Other Actor Pos : " << tmp.p.x << "," << tmp.p.y << "," << tmp.p.z << endl;

			PxVec3 mylook(0, 0, 0);
			PxVec3 otherlook(0, 0, 0);
			float dot;
			int my;

			for (int j = 0; j < MAX_ROOM_USER; ++j)
			{
				if (player[j])
				{
					if (pairs[i].triggerActor == player[j]->m_AttackTrigger)
					{
						mylook = player[j]->m_Look;
						my = j;
						break;
					}
				}
			}

			bool pepero_status = false;
			char order = 0;

			for (int j = 0; j < MAX_ROOM_USER; ++j)
			{
				if (Player_Trigger == reinterpret_cast<UserData *>(pairs[i].triggerActor->userData)->type)
				{
					if (player[j])
					{
						if (pairs[i].triggerActor != player[j]->m_AttackTrigger)
						{
							if (player[j]->m_PlayerController != nullptr)
							{
								if (pairs[i].otherActor == player[j]->getControllerActor()) {
									cout << j << " Player : Hitted\n";
									//cout << player[j]->getControllerActor()->getGlobalPose().p.x << "," 
									//	<< player[j]->getControllerActor()->getGlobalPose().p.y << "," 
									//	<< player[j]->getControllerActor()->getGlobalPose().p.z << endl;
									otherlook = player[j]->m_Look;
									dot = mylook.dot(otherlook);

									if (player[my]->weapon_type == Weapon_King)
									{
										PxVec3 knockback;
										knockback = player[j]->m_Pos - player[my]->m_Pos;

										player[j]->m_Knockback = knockback.getNormalized();
										player[j]->setStatus(STATUS::CRI_HITTED);
										player[j]->hitted = true;
									}
									else
									{
										// -0.707... ~ -1.0 : �Ƹ� ����
										if (player[j]->m_status == STATUS::DEFENSE) {
											if (dot <= -0.7f && dot >= -1.0f) {
												continue;
											}
											else {
												player[j]->setStatus(STATUS::HITTED);
												player[j]->hitted = true;
											}
										}
										else {
											player[j]->setStatus(STATUS::HITTED);
											player[j]->hitted = true;
										}
									}
								}
							}
						}
					}
				}
				else if (Pepero_Trigger == reinterpret_cast<UserData *>(pairs[i].triggerActor->userData)->type)
				{
					order = reinterpret_cast<UserData *>(pairs[i].triggerActor->userData)->order;

					if (player[j])
					{
						if (player[j]->m_PlayerController != nullptr)
						{
							if (pairs[i].otherActor == player[j]->getControllerActor()) {
								cout << j << " Player : Pepero Skill Hitted\n";
								//cout << player[j]->getControllerActor()->getGlobalPose().p.x << "," 
								//	<< player[j]->getControllerActor()->getGlobalPose().p.y << "," 
								//	<< player[j]->getControllerActor()->getGlobalPose().p.z << endl;

								auto it = find(room->m_skillTrigger.begin(), room->m_skillTrigger.end(), order);

								player[j]->m_Knockback = it->look;
								player[j]->setStatus(STATUS::CRI_HITTED);
								player[j]->hitted = true;
							}
						}
						add_timer(room, order, high_resolution_clock::now());
					}
				}
				else if (Lollipop_Trigger == reinterpret_cast<UserData *>(pairs[i].triggerActor->userData)->type)
				{
					if (player[j])
					{
						if (player[j]->m_PlayerController != nullptr)
						{
							if (pairs[i].otherActor == player[j]->getControllerActor()) {
								cout << j << " Player : Lollipop Heal Success\n";
								//cout << player[j]->getControllerActor()->getGlobalPose().p.x << "," 
								//	<< player[j]->getControllerActor()->getGlobalPose().p.y << "," 
								//	<< player[j]->getControllerActor()->getGlobalPose().p.z << endl;

								player[j]->setLollipopHeal(true);
							}
						}
					}
				}
				else if (Candy_Trigger == reinterpret_cast<UserData *>(pairs[i].triggerActor->userData)->type)
				{
					order = reinterpret_cast<UserData *>(pairs[i].triggerActor->userData)->order;

					auto it = find(room->m_skillTrigger.begin(), room->m_skillTrigger.end(), order);

					int slot = it->owner;

					if (player[j])
					{
						if (player[j]->m_PlayerController != nullptr)
						{
							if (slot != j)
							{
								if (pairs[i].otherActor == player[j]->getControllerActor()) {
									cout << j << " Player : Candy Skill Hit\n";
									//cout << player[j]->getControllerActor()->getGlobalPose().p.x << "," 
									//	<< player[j]->getControllerActor()->getGlobalPose().p.y << "," 
									//	<< player[j]->getControllerActor()->getGlobalPose().p.z << endl;
									// cout << mylook.x << "," << mylook.y << "," << mylook.z << endl;
									PxVec3 knockback;

									knockback = player[j]->m_Pos - player[slot]->m_Pos;

									player[j]->m_Knockback = knockback.getNormalized();
									//player[j]->m_Knockback = PxVec3(1, 0, 0);
									player[j]->setStatus(STATUS::CRI_HITTED);
									player[j]->hitted = true;
								}
							}
						}
					}
				}
				else if (Light_Trigger == reinterpret_cast<UserData *>(pairs[i].triggerActor->userData)->type)
				{
					if (player[j])
					{
						if (player[j]->m_PlayerController != nullptr)
						{
							if (pairs[i].otherActor == player[j]->getControllerActor()) {
								cout << j << " Player : Lightning Stun\n";
								//cout << player[j]->getControllerActor()->getGlobalPose().p.x << "," 
								//	<< player[j]->getControllerActor()->getGlobalPose().p.y << "," 
								//	<< player[j]->getControllerActor()->getGlobalPose().p.z << endl;
								// cout << mylook.x << "," << mylook.y << "," << mylook.z << endl;

								player[j]->setStatus(STATUS::STUN);
								player[j]->hitted = true;
							}
						}
					}
				}
			}
		}
	}
}//Ʈ���Źڽ� �浹 üũ

void PhysSimulation::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
	for (PxU32 i = 0; i < nbPairs; i++)
	{
		const PxContactPair& cp = pairs[i];

		if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			cout << "contack~~\n";
		}
	}
}

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

PxCapsuleController* CPhysx::getCapsuleController(PxVec3 pos, float height, float radius, PxUserControllerHitReport* collisionCallback, CPlayer *player)
{
	PxCapsuleControllerDesc capsuleDesc;
	capsuleDesc.height = height; //Height of capsule
	capsuleDesc.radius = radius; //Radius of casule
	capsuleDesc.position = PXtoPXEx(pos) + PxExtendedVec3(0, 17.5, 0); //Initial position of capsule
	capsuleDesc.material = m_Physics->createMaterial(1.0f, 1.0f, 1.0f); //Material for capsule shape
	//capsuleDesc.density = 1.0f; //Desity of capsule shape
	//capsuleDesc.contactOffset = 1.0f; //�ܺ� ��ü�� ��ȣ�ۿ��ϴ� ũ�� (������ �浹ĸ������ ���� �� ũ�� ��������)
	////capsuleDesc.slopeLimit = cosf(XMConvertToRadians(1.0f)); //��� ��뵵(degree) 0�� �������� ��縦 ���ö�
	//capsuleDesc.stepOffset = 0.0f;	//�ڿ������� �̵� (�ణ�� ������ �΋H���� �� �̵����� ����)
	//												//stepoffset���� ū ���̿� �ε�ġ�� ����
	////capsuleDesc.maxJumpHeight = 2.0f; //�ִ� ���� ����
	//capsuleDesc.climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;
	//capsuleDesc.invisibleWallHeight = 0.0f;

	//�浹 �ݹ� �Լ�
	capsuleDesc.reportCallback = collisionCallback;
	capsuleDesc.behaviorCallback = player;

	PxCapsuleController* controller = static_cast<PxCapsuleController*>(m_PlayerManager->createController(capsuleDesc));

	return controller;
}

void CPhysx::setBoxController(PxVec3 pos, PxVec3 size)
{
	PxBoxControllerDesc boxDesc;
	boxDesc.halfForwardExtent = size.z;
	boxDesc.halfHeight = size.y;
	boxDesc.halfSideExtent = size.x;

	boxDesc.position = PXtoPXEx(pos);

	//boxDesc.density = 1.0f;
	boxDesc.material = m_Physics->createMaterial(1.0f, 1.0f, 1.0f);
	//boxDesc.contactOffset = 5.0f;
	//boxDesc.slopeLimit = slopeDegree;
	//boxDesc.stepOffset = step;

	//boxDesc.reportCallback = collisionCallback;

	PxBoxController* controller = static_cast<PxBoxController*>(m_PlayerManager->createController(boxDesc));

	//return controller;
}

PxRigidDynamic* CPhysx::getBox(PxVec3& t, PxVec3 size)
{
	PxShape* shape = m_Physics->createShape(PxBoxGeometry(size.x, size.y, size.z), *m_Physics->createMaterial(0.2f, 0.2f, 0.2f));
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);	//�ù����̼� off
	//shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);			//Ʈ���Ÿ� on

	PxRigidDynamic * staticActor = m_Physics->createRigidDynamic(PxTransform(t));

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

PxRigidDynamic* CPhysx::getRotateBox(PxVec3& t, PxVec3& ro, PxVec3 size)
{
	PxShape* shape = m_Physics->createShape(PxBoxGeometry(size.x, size.y, size.z), *m_Physics->createMaterial(0.2f, 0.2f, 0.2f));
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);	//�ù����̼� off
	//shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);			//Ʈ���Ÿ� on

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

PxRigidStatic* CPhysx::getBoxTrigger(PxVec3& t, PxVec3 size)
{
	PxShape* shape = m_Physics->createShape(PxBoxGeometry(size.x, size.y, size.z), *m_Physics->createMaterial(0.2f, 0.2f, 0.2f));
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);	//�ù����̼� off
	shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);			//Ʈ���Ÿ� on

	PxRigidStatic * staticActor = m_Physics->createRigidStatic(PxTransform(t));

	UserData* udata = new UserData;
	udata->type = Player_Trigger;

	staticActor->attachShape(*shape);
	staticActor->userData = udata;
	//float* num = new float(0.0f);
	//staticActor->userData = (void*)num;
	/*int* tmp = (int*)staticActor->userData;
	*tmp = 1;*/

	m_Scene->addActor(*staticActor);

	return staticActor;
}

PxRigidStatic* CPhysx::getBoxTrigger(PxVec3& t, PxVec3 size, int trigger_type)
{
	PxShape* shape = m_Physics->createShape(PxBoxGeometry(size.x, size.y, size.z), *m_Physics->createMaterial(0.2f, 0.2f, 0.2f));
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);	//�ù����̼� off
	shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);			//Ʈ���Ÿ� on

	PxRigidStatic * staticActor = m_Physics->createRigidStatic(PxTransform(t));

	UserData* udata = new UserData;
	udata->order = 0;
	udata->type = trigger_type;

	staticActor->attachShape(*shape);
	staticActor->userData = (void *)udata;
	//float* num = new float(0.0f);
	//staticActor->userData = (void*)num;
	/*int* tmp = (int*)staticActor->userData;
	*tmp = 1;*/
	m_Scene->addActor(*staticActor);

	return staticActor;
}

PxRigidStatic* CPhysx::getRotateBoxTrigger(PxVec3& t, PxVec3& ro, PxVec3 size, int trigger_type, int order)
{
	PxShape* shape = m_Physics->createShape(PxBoxGeometry(size.x, size.y, size.z), *m_Physics->createMaterial(0.2f, 0.2f, 0.2f));
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);	//�ù����̼� off
	shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);			//Ʈ���Ÿ� on

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

	PxRigidStatic * staticActor = m_Physics->createRigidStatic(PxTransform(t, q));

	UserData* udata = new UserData;
	udata->order = order;
	udata->type = trigger_type;

	staticActor->attachShape(*shape);
	staticActor->userData = (void *)udata;
	//float* num = new float(0.0f);
	//staticActor->userData = (void*)num;
	/*int* tmp = (int*)staticActor->userData;
	*tmp = 1;*/
	m_Scene->addActor(*staticActor);

	return staticActor;
}

PxRigidStatic* CPhysx::getSphereTrigger(PxVec3& t, PxReal rad, int trigger_type, int order)
{
	PxShape* shape = m_Physics->createShape(PxSphereGeometry(rad), *m_Physics->createMaterial(0.2f, 0.2f, 0.2f));
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);	//�ù����̼� off
	shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);			//Ʈ���Ÿ� on

	PxRigidStatic * staticActor = m_Physics->createRigidStatic(PxTransform(t));

	UserData* udata = new UserData;
	udata->order = order;
	udata->type = trigger_type;

	staticActor->attachShape(*shape);
	staticActor->userData = (void *)udata;

	//float* num = new float(0.0f);
	//staticActor->userData = (void*)num;
	/*int* tmp = (int*)staticActor->userData;
	*tmp = 1;*/
	m_Scene->addActor(*staticActor);

	return staticActor;
}