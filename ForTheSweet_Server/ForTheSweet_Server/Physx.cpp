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
										// -0.707... ~ -1.0 : 아머 가능
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
}//트리거박스 충돌 체크

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
	//m_Physics = NULL;

	//m_Material = NULL;

	//m_Dispatcher = NULL;
	//m_Scene = NULL;


	//m_PlayerManager = NULL;
	//m_PlayerController = NULL;
}

void CPhysx::initPhysics()
{
	m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_Allocator, m_ErrorCallback);

	m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, PxTolerancesScale(), true, 0);

	m_Material = m_Physics->createMaterial(0.5f, 0.5f, 0.6f);

	m_Cooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_Foundation, PxCookingParams(PxTolerancesScale()));

	//PxSceneDesc sceneDesc(m_Physics->getTolerancesScale());
	//sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	//
	//m_Dispatcher = PxDefaultCpuDispatcherCreate(1);
	//sceneDesc.cpuDispatcher = m_Dispatcher;
	//sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	//sceneDesc.simulationEventCallback = &m_Simulator;
	//
	//m_Scene = m_Physics->createScene(sceneDesc);

	//m_PlayerManager = PxCreateControllerManager(*m_Scene);
}