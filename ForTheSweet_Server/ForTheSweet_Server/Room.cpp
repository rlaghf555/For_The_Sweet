#include "Room.h"

CRoom::CRoom()
{
	room_num = 0;
	current_num = 0;
	max_num = MAX_ROOM_USER;

	room_status = 0;
	room_mode = ROOM_MODE_SOLO;
	room_map = MAP_Wehas;

	//m_pPhysx = nullptr;
	//m_Dispatcher = NULL;
	//m_Scene = NULL;

	//m_PlayerManager = NULL;
	//m_PlayerController = NULL;

	m_Dispatcher = NULL;
	m_Scene = NULL;
	m_PlayerManager = NULL;
	m_Simulator = NULL;

	PosBroadCastTime = 0.0f;

	trigger_order = 0;

	for (int i = 0; i < MAX_ROOM_USER; ++i)
	{
		clientNum[i] = -1;
		load_complete[i] = false;
		setting_complete[i] = false;
	}

	for (int i = 0; i < RESPAWN_WEAPON_NUM; ++i)
	{
		weapon_respawn[i].respawn_able = true;
		weapon_respawn[i].type = 0;
		weapon_respawn[i].index = 0;
	}

	for (int i = 0; i < MAX_WEAPON_TYPE; ++i)
	{
		for (int j = 0; j < MAX_WEAPON_NUM; ++j)
		{
			weapon_list[i][j].SetEmpty(true);
			weapon_list[i][j].SetOwner(-1);
		}
	}
}

CRoom::CRoom(const CRoom& other) {
	room_num = other.room_num;
	strcpy_s(name, _countof(name), other.name);

	current_num = other.current_num;
	max_num = other.max_num;
	host_num = other.host_num;
	room_mode = other.room_mode;
	room_status = other.room_status;
	room_map = other.room_map;

	PosBroadCastTime = other.PosBroadCastTime;

	trigger_order = other.trigger_order;

	m_Dispatcher = other.m_Dispatcher;
	m_Scene = other.m_Scene;
	m_PlayerManager = other.m_PlayerManager;
	m_Simulator = other.m_Simulator;

	for (int i = 0; i < MAX_ROOM_USER; ++i)
	{
		clientNum[i] = other.clientNum[i];
		load_complete[i] = other.load_complete[i];
		setting_complete[i] = other.setting_complete[i];
	}

	for (int i = 0; i < RESPAWN_WEAPON_NUM; ++i)
	{
		weapon_respawn[i].respawn_able = other.weapon_respawn[i].respawn_able;
		weapon_respawn[i].type = other.weapon_respawn[i].type;
		weapon_respawn[i].index = other.weapon_respawn[i].index;
	}

	for (int i = 0; i < MAX_WEAPON_TYPE; ++i)
	{
		for (int j = 0; j < MAX_WEAPON_NUM; ++j)
		{
			weapon_list[i][j] = other.weapon_list[i][j];
		}
	}
}

CRoom& CRoom::operator=(const CRoom& other)
{
	room_num = other.room_num;
	strcpy_s(name, _countof(name), other.name);

	current_num = other.current_num;
	max_num = other.max_num;
	host_num = other.host_num;
	room_mode = other.room_mode;
	room_status = other.room_status;
	room_map = other.room_map;

	PosBroadCastTime = other.PosBroadCastTime;

	trigger_order = other.trigger_order;

	m_Dispatcher = other.m_Dispatcher;
	m_Scene = other.m_Scene;
	m_PlayerManager = other.m_PlayerManager;
	m_Simulator = other.m_Simulator;

	for (int i = 0; i < MAX_ROOM_USER; ++i)
	{
		clientNum[i] = other.clientNum[i];
		load_complete[i] = other.load_complete[i];
		setting_complete[i] = other.setting_complete[i];
	}

	for (int i = 0; i < RESPAWN_WEAPON_NUM; ++i)
	{
		weapon_respawn[i].respawn_able = other.weapon_respawn[i].respawn_able;
		weapon_respawn[i].type = other.weapon_respawn[i].type;
		weapon_respawn[i].index = other.weapon_respawn[i].index;
	}

	for (int i = 0; i < MAX_WEAPON_TYPE; ++i)
	{
		for (int j = 0; j < MAX_WEAPON_NUM; ++j)
		{
			weapon_list[i][j] = other.weapon_list[i][j];
		}
	}

	return *this;
}

void CRoom::init(char *nm, int hostId, int roomNum)
{
	strcpy_s(name, _countof(name), nm);
	host_num = 0;
	current_num = 1;
	room_num = roomNum;
	room_mode = ROOM_MODE_SOLO;
	room_map = MAP_Wehas;

	clientNum[0] = hostId;
}

bool CRoom::attend(int Num)
{
	int slot = -1;
	for (int i = 0; i < max_num; ++i)
	{
		if (clientNum[i] == -1)
		{
			slot = i;
			break;
		}
	}

	if (slot == -1)
	{
		return false;
	}

	clientNum[slot] = Num;
	current_num++;
	return true;
}

void CRoom::start(int map_type, const vector<vector<PxVec3>>& vectex, const vector<vector<int>>& index, CPhysx *physx)
{
	//m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *(physx->m_Foundation), PxTolerancesScale(), true, 0);
	//m_Material = m_Physics->createMaterial(0.5f, 0.5f, 0.6f);
	m_Simulator = new PhysSimulation();
	m_Scene = getScene(m_Simulator, m_PlayerManager, physx);
	//m_Cooking = PxCreateCooking(PX_PHYSICS_VERSION, *(physx->m_Foundation), PxCookingParams(PxTolerancesScale()));
	m_PlayerManager = PxCreateControllerManager(*m_Scene);

	// 맵 로딩
	if (map_type == MAP_Wehas)
	{
		PxVec3 scaleTmp = PxVec3(1.0f, 1.0f, 1.0f);
		PxMeshScale PxScale;
		PxScale.scale = scaleTmp;
		PxMaterial* mat = physx->m_Physics->createMaterial(0.2f, 0.2f, 0.2f);

		// 맵 1 기본 바닥
		PxTriangleMesh* triMesh = GetTriangleMesh(vectex[MAP_1_BASE], index[MAP_1_BASE], physx);
		PxTriangleMeshGeometry meshGeo(triMesh, PxScale);
		PxTransform location(0, 0, 0);
		PxRigidActor* base = PxCreateStatic(*physx->m_Physics, location, meshGeo, *mat);
		m_Scene->addActor(*base);

		// 맵 1 마카롱
		PxTriangleMesh* triMesh2 = GetTriangleMesh(vectex[MAP_MACARON], index[MAP_MACARON], physx);
		PxTriangleMeshGeometry meshGeo2(triMesh2, PxScale);
		PxTransform location2(PxVec3(0.f, -50.f, 0.f));
		PxRigidActor* macaron = PxCreateStatic(*physx->m_Physics, location2, meshGeo2, *mat);
		m_Scene->addActor(*macaron);

		move_actor = macaron;
		move_actor->userData = (void *)(int)1;

		// 초콜릿 벽 생성
		PxVec3 wall_size(3, 100, 60);
		PxVec3 wall_pos(167, 40, 0);
		getBox(wall_pos, wall_size, physx);
		wall_pos.x *= -1.f;
		getBox(wall_pos, wall_size, physx);
	}
	else if (room_map == MAP_Oreo)
	{
		PxVec3 scaleTmp = PxVec3(1.0f, 1.0f, 1.0f);
		PxMeshScale PxScale;
		PxScale.scale = scaleTmp;
		PxMaterial* mat = physx->m_Physics->createMaterial(0.2f, 0.2f, 0.2f);
		PxTriangleMesh* triMesh;

		// 맵 2 기본 바닥
		{
			triMesh = GetTriangleMesh(vectex[MAP_2_BASE], index[MAP_2_BASE], physx);
			PxTriangleMeshGeometry meshGeo(triMesh, PxScale);
			PxTransform location(0, -20, 0);
			PxRigidActor* base = PxCreateStatic(*physx->m_Physics, location, meshGeo, *mat);
			m_Scene->addActor(*base);
		}
		{
			// 맵 2 마카롱 2개
			triMesh = GetTriangleMesh(vectex[MAP_MACARON], index[MAP_MACARON], physx);
			PxTriangleMeshGeometry meshGeo(triMesh, PxScale);
			PxTransform location(-185, -10, 0);
			PxRigidActor* base = PxCreateStatic(*physx->m_Physics, location, meshGeo, *mat);
			m_Scene->addActor(*base);
		}
		{
			triMesh = GetTriangleMesh(vectex[MAP_MACARON], index[MAP_MACARON], physx);
			PxTriangleMeshGeometry meshGeo(triMesh, PxScale);
			PxTransform location(185, -10, 0);
			PxRigidActor* base = PxCreateStatic(*physx->m_Physics, location, meshGeo, *mat);
			m_Scene->addActor(*base);
		}
		{
			// 맵 2 초콜릿바 2개
			triMesh = GetTriangleMesh(vectex[MAP_2_CHOCOHAR], index[MAP_2_CHOCOHAR], physx);
			PxTriangleMeshGeometry meshGeo(triMesh, PxScale);
			PxTransform location(-300, -20, 0);
			PxRigidActor* base = PxCreateStatic(*physx->m_Physics, location, meshGeo, *mat);
			m_Scene->addActor(*base);
		}
		{
			triMesh = GetTriangleMesh(vectex[MAP_2_CHOCOHAR], index[MAP_2_CHOCOHAR], physx);
			PxTriangleMeshGeometry meshGeo(triMesh, PxScale);
			PxTransform location(300, -20, 0);
			PxRigidActor* base = PxCreateStatic(*physx->m_Physics, location, meshGeo, *mat);
			m_Scene->addActor(*base);
		}
	}
	else if (room_map == MAP_Cake)
	{
		PxVec3 scaleTmp = PxVec3(1.0f, 1.0f, 1.0f);
		PxMeshScale PxScale;
		PxScale.scale = scaleTmp;
		PxMaterial* mat = physx->m_Physics->createMaterial(0.2f, 0.2f, 0.2f);
		PxTriangleMesh* triMesh;

		{
			// 맵 3 1층 2개
			triMesh = GetTriangleMesh(vectex[MAP_3_FLOOR_1], index[MAP_3_FLOOR_1], physx);
			PxTriangleMeshGeometry meshGeo(triMesh, PxScale);
			PxTransform location(-250, -132, 0);
			PxRigidActor* base = PxCreateStatic(*physx->m_Physics, location, meshGeo, *mat);
			m_Scene->addActor(*base);
		}
		{
			triMesh = GetTriangleMesh(vectex[MAP_3_FLOOR_1], index[MAP_3_FLOOR_1], physx);
			PxTriangleMeshGeometry meshGeo(triMesh, PxScale);
			PxTransform location(250, -132, 0);
			PxRigidActor* base = PxCreateStatic(*physx->m_Physics, location, meshGeo, *mat);
			m_Scene->addActor(*base);
		}
		{
			// 맵 3 2층 2개
			triMesh = GetTriangleMesh(vectex[MAP_3_FLOOR_2], index[MAP_3_FLOOR_2], physx);
			PxTriangleMeshGeometry meshGeo(triMesh, PxScale);
			PxTransform location(-250, 0, 50);
			PxRigidActor* base = PxCreateStatic(*physx->m_Physics, location, meshGeo, *mat);
			m_Scene->addActor(*base);
		}
		{
			triMesh = GetTriangleMesh(vectex[MAP_3_FLOOR_2], index[MAP_3_FLOOR_2], physx);
			PxTriangleMeshGeometry meshGeo(triMesh, PxScale);
			PxTransform location(250, 0, 50);
			PxRigidActor* base = PxCreateStatic(*physx->m_Physics, location, meshGeo, *mat);
			m_Scene->addActor(*base);
		}
		{
			// 맵 3 3층 2개
			triMesh = GetTriangleMesh(vectex[MAP_3_FLOOR_3], index[MAP_3_FLOOR_3], physx);
			PxTriangleMeshGeometry meshGeo(triMesh, PxScale);
			PxTransform location(-250, 80, 80);
			PxRigidActor* base = PxCreateStatic(*physx->m_Physics, location, meshGeo, *mat);
			m_Scene->addActor(*base);
		}
		{
			triMesh = GetTriangleMesh(vectex[MAP_3_FLOOR_3], index[MAP_3_FLOOR_3], physx);
			PxTriangleMeshGeometry meshGeo(triMesh, PxScale);
			PxTransform location(250, 80, 80);
			PxRigidActor* base = PxCreateStatic(*physx->m_Physics, location, meshGeo, *mat);
			m_Scene->addActor(*base);
		}
		{
			// 맵 3 내부 2개
			triMesh = GetTriangleMesh(vectex[MAP_3_IN], index[MAP_3_IN], physx);
			PxTriangleMeshGeometry meshGeo(triMesh, PxScale);
			PxTransform location(-250, 500, 40);
			PxRigidActor* base = PxCreateStatic(*physx->m_Physics, location, meshGeo, *mat);
			m_Scene->addActor(*base);
		}
		{
			triMesh = GetTriangleMesh(vectex[MAP_3_IN], index[MAP_3_IN], physx);
			PxTriangleMeshGeometry meshGeo(triMesh, PxScale);
			PxTransform location(250, 500, 40);
			PxRigidActor* base = PxCreateStatic(*physx->m_Physics, location, meshGeo, *mat);
			m_Scene->addActor(*base);
		}
		{
			// 맵 3 내부 계단 1층 2개
			triMesh = GetTriangleMesh(vectex[MAP_3_IN_STAIR_1], index[MAP_3_IN_STAIR_1], physx);
			PxTriangleMeshGeometry meshGeo(triMesh, PxScale);
			PxTransform location(-250, 500, 40);
			PxRigidActor* base = PxCreateStatic(*physx->m_Physics, location, meshGeo, *mat);
			m_Scene->addActor(*base);
		}
		{
			triMesh = GetTriangleMesh(vectex[MAP_3_IN_STAIR_1], index[MAP_3_IN_STAIR_1], physx);
			PxTriangleMeshGeometry meshGeo(triMesh, PxScale);
			PxTransform location(250, 500, 40);
			PxRigidActor* base = PxCreateStatic(*physx->m_Physics, location, meshGeo, *mat);
			m_Scene->addActor(*base);
		}
		{
			// 맵 3 내부 계단 2층 2개
			triMesh = GetTriangleMesh(vectex[MAP_3_IN_STAIR_2], index[MAP_3_IN_STAIR_2], physx);
			PxTriangleMeshGeometry meshGeo(triMesh, PxScale);
			PxTransform location(-250, 500, 40);
			PxRigidActor* base = PxCreateStatic(*physx->m_Physics, location, meshGeo, *mat);
			m_Scene->addActor(*base);
		}
		{
			triMesh = GetTriangleMesh(vectex[MAP_3_IN_STAIR_2], index[MAP_3_IN_STAIR_2], physx);
			PxTriangleMeshGeometry meshGeo(triMesh, PxScale);
			PxTransform location(250, 500, 40);
			PxRigidActor* base = PxCreateStatic(*physx->m_Physics, location, meshGeo, *mat);
			m_Scene->addActor(*base);
		}
		{
			// 맵 3 내부 벽 2개
			triMesh = GetTriangleMesh(vectex[MAP_3_IN_WALL], index[MAP_3_IN_WALL], physx);
			PxTriangleMeshGeometry meshGeo(triMesh, PxScale);
			PxTransform location(-250, 500, 40);
			PxRigidActor* base = PxCreateStatic(*physx->m_Physics, location, meshGeo, *mat);
			m_Scene->addActor(*base);
		}
		{
			triMesh = GetTriangleMesh(vectex[MAP_3_IN_WALL], index[MAP_3_IN_WALL], physx);
			PxTriangleMeshGeometry meshGeo(triMesh, PxScale);
			PxTransform location(250, 500, 40);
			PxRigidActor* base = PxCreateStatic(*physx->m_Physics, location, meshGeo, *mat);
			m_Scene->addActor(*base);
		}
		{
			// 맵 3 마카롱
			PxTriangleMesh* macaron = GetTriangleMesh(vectex[MAP_MACARON], index[MAP_MACARON], physx);
			PxTriangleMeshGeometry meshGeo(macaron, PxScale);
			PxTransform location(0, 80, 50);
			PxRigidActor* base = PxCreateStatic(*physx->m_Physics, location, meshGeo, *mat);
			m_Scene->addActor(*base);
			move_actor = base;
		}

		// 맵 3 가운데 다리
		for (int i = 0; i < 4; ++i)
		{
			triMesh = GetTriangleMesh(vectex[MAP_3_BRIDGE], index[MAP_3_BRIDGE], physx);
			PxTriangleMeshGeometry meshGeo(triMesh, PxScale);
			PxTransform location(0.f, 0.f, float(i) * 20.f + 30.f);
			PxRigidActor* base = PxCreateStatic(*physx->m_Physics, location, meshGeo, *mat);
			m_Scene->addActor(*base);
		}

		// 맵 3 좌 계단
		for (int i = 0; i < 11; ++i)
		{
			if (i == 10)
			{
				triMesh = GetTriangleMesh(vectex[MAP_3_STAIR], index[MAP_3_STAIR], physx);
				PxTriangleMeshGeometry meshGeo(triMesh, PxScale);
				PxTransform location(-378, (i - 1) * 8, i * 18 - 50);
				PxRigidActor* base = PxCreateStatic(*physx->m_Physics, location, meshGeo, *mat);
				m_Scene->addActor(*base);
			}
			else
			{
				triMesh = GetTriangleMesh(vectex[MAP_3_STAIR], index[MAP_3_STAIR], physx);
				PxTriangleMeshGeometry meshGeo(triMesh, PxScale);
				PxTransform location(-378, i * 8, i * 18 - 50);
				PxRigidActor* base = PxCreateStatic(*physx->m_Physics, location, meshGeo, *mat);
				m_Scene->addActor(*base);
			}
		}

		// 맵 3 우 계단
		for (int i = 0; i < 11; ++i)
		{
			if (i == 10)
			{
				triMesh = GetTriangleMesh(vectex[MAP_3_STAIR], index[MAP_3_STAIR], physx);
				PxTriangleMeshGeometry meshGeo(triMesh, PxScale);
				PxTransform location(378, (i - 1) * 8, i * 18 - 50);
				PxRigidActor* base = PxCreateStatic(*physx->m_Physics, location, meshGeo, *mat);
				m_Scene->addActor(*base);
			}
			else
			{
				triMesh = GetTriangleMesh(vectex[MAP_3_STAIR], index[MAP_3_STAIR], physx);
				PxTriangleMeshGeometry meshGeo(triMesh, PxScale);
				PxTransform location(378, i * 8, i * 18 - 50);
				PxRigidActor* base = PxCreateStatic(*physx->m_Physics, location, meshGeo, *mat);
				m_Scene->addActor(*base);
			}
		}
	}

	if (room_mode == ROOM_MODE_SOLO)
	{
		solo_dead_count = 0;
	}
	else if (room_mode == ROOM_MODE_TEAM || ROOM_MODE_KING)
	{
		// 팀, 대장전은 밖에서 세팅
		for (int i = 0; i < MAX_USER; ++i)
		{
			team_dead[i] = false;
		}
	}

	// 마지막 Room 세팅
	for (int i = 0; i < MAX_ROOM_USER; ++i)
	{
		load_complete[i] = false;
		setting_complete[i] = false;
	}
	for (int i = 0; i < MAX_WEAPON_TYPE; ++i)
	{
		for (int j = 0; j < MAX_WEAPON_NUM; ++j)
		{
			weapon_list[i][j].init();
		}
	}
	for (int i = 0; i < RESPAWN_WEAPON_NUM; ++i)
	{
		weapon_respawn[i].respawn_able = true;
		weapon_respawn[i].index = 0;
		weapon_respawn[i].type = 0;
	}

	PosBroadCastTime = 0.0f;
	trigger_order = 0;
	move_actor_flag = false;
	timer = MAX_TIMER;
	end_timer = 0.0f;
	fever = false;
	lighting = false;
	light_count = 0;
	light_index1 = 0;
	light_index2 = 0;
	oreo_team1_spawn = false;
	oreo_team2_spawn = false;
	referee.patern_count = 0;
	referee.patern_type = Rfr_Start_Num;
	room_status = ROOM_ST_LOADING;
}

bool CRoom::all_load_complete()
{
	int count = 0;

	for (int i = 0; i < MAX_ROOM_USER; ++i)
	{
		if (load_complete[i] == true)
		{
			count++;
		}
	}

	if (count == current_num)
	{
		cout << "ALL load\n";
		return true;
	}
	else
	{
		//cout << "ALL not yet load\n";
		return false;
	}
}

bool CRoom::all_setting_complete()
{
	int count = 0;

	for (int i = 0; i < MAX_ROOM_USER; ++i)
	{
		if (setting_complete[i] == true)
		{
			count++;
		}
	}

	if (count == current_num)
	{
		cout << "ALL load\n";
		return true;
	}
	else
	{
		//cout << "ALL not yet load\n";
		return false;
	}
}

bool CRoom::is_game_end()
{
	if (room_mode == ROOM_MODE_SOLO)
	{
		if (solo_dead_count == current_num - 1) {
			return true;
		}
	}
	else if (room_mode == ROOM_MODE_TEAM || room_mode == ROOM_MODE_KING)
	{
		bool is_Team1_end = true;
		// 1 team check
		for (int i = 0; i < MAX_ROOM_USER / 2; ++i)
		{
			if (team_dead[i] == true)		// true : 살아 있음
			{
				is_Team1_end = false;
				break;
			}
		}
		// 2team check
		bool is_Team2_end = true;
		for (int i = 4; i < 4 + (MAX_ROOM_USER / 2); ++i)
		{
			if (team_dead[i] == true)		// true : 살아 있음
			{
				is_Team2_end = false;
				break;
			}
		}

		if (is_Team1_end || is_Team2_end) {
			if (is_Team1_end == true)
			{
				team_victory = false;			// 2팀 우승
			}
			if (is_Team2_end == true) {
				team_victory = true;			// 1팀 우승
			}
			return true;
		}
		return false;
	}
	return false;
}

PxScene* CRoom::getScene(PhysSimulation* simul, PxControllerManager *manager, CPhysx *physx)
{
	PxSceneDesc sceneDesc(physx->m_Physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);

	PxDefaultCpuDispatcher* dispatcher = PxDefaultCpuDispatcherCreate(1);
	sceneDesc.cpuDispatcher = dispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	sceneDesc.simulationEventCallback = simul;

	PxScene* scene = physx->m_Physics->createScene(sceneDesc);

	return scene;
}

inline PxVec3* fromVertex(PxVec3 *ver, int size)
{
	PxVec3* mem = new PxVec3[size];
	for (int i = 0; i < size; ++i) {
		mem[i] = ver[i];
	}
	return mem;
}

PxTriangleMesh*	CRoom::GetTriangleMesh(vector<PxVec3> ver, vector<int> index, CPhysx* physx) {

	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = ver.size();
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = fromVertex(ver.data(), ver.size());

	meshDesc.triangles.count = index.size() / 3;
	meshDesc.triangles.stride = sizeof(int) * 3;
	meshDesc.triangles.data = index.data();

	meshDesc.flags = PxMeshFlags(0);

	PxCookingParams params = physx->m_Cooking->getParams();
	params.midphaseDesc = PxMeshMidPhase::eBVH33;
	params.suppressTriangleMeshRemapTable = true;
	params.meshPreprocessParams &= ~static_cast<PxMeshPreprocessingFlags>(PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH);
	params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
	params.midphaseDesc.mBVH33Desc.meshCookingHint = PxMeshCookingHint::eCOOKING_PERFORMANCE;
	params.midphaseDesc.mBVH33Desc.meshSizePerformanceTradeOff = 0.0f;
	physx->m_Cooking->setParams(params);

	PxTriangleMesh* triMesh = nullptr;
	PxDefaultMemoryOutputStream outBuffer;
	physx->m_Cooking->cookTriangleMesh(meshDesc, outBuffer);

	PxDefaultMemoryInputData stream(outBuffer.getData(), outBuffer.getSize());
	triMesh = physx->m_Physics->createTriangleMesh(stream);

	return triMesh;
}

PxCapsuleController* CRoom::getCapsuleController(PxVec3 pos, float height, float radius, PxUserControllerHitReport* collisionCallback, CPlayer *player, CPhysx *physx)
{
	PxCapsuleControllerDesc capsuleDesc;
	capsuleDesc.height = height; //Height of capsule
	capsuleDesc.radius = radius; //Radius of casule
	capsuleDesc.position = PXtoPXEx(pos) + PxExtendedVec3(0, 17.5, 0); //Initial position of capsule
	capsuleDesc.material = physx->m_Physics->createMaterial(1.0f, 1.0f, 1.0f); //Material for capsule shape
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
	capsuleDesc.behaviorCallback = player;

	PxCapsuleController* controller = static_cast<PxCapsuleController*>(m_PlayerManager->createController(capsuleDesc));

	return controller;
}

void CRoom::setBoxController(PxVec3 pos, PxVec3 size, CPhysx *physx)
{
	PxBoxControllerDesc boxDesc;
	boxDesc.halfForwardExtent = size.z;
	boxDesc.halfHeight = size.y;
	boxDesc.halfSideExtent = size.x;

	boxDesc.position = PXtoPXEx(pos);

	//boxDesc.density = 1.0f;
	boxDesc.material = physx->m_Physics->createMaterial(1.0f, 1.0f, 1.0f);
	//boxDesc.contactOffset = 5.0f;
	//boxDesc.slopeLimit = slopeDegree;
	//boxDesc.stepOffset = step;

	//boxDesc.reportCallback = collisionCallback;

	PxBoxController* controller = static_cast<PxBoxController*>(m_PlayerManager->createController(boxDesc));

	//return controller;
}

PxRigidDynamic* CRoom::getBox(PxVec3& t, PxVec3 size, CPhysx *physx)
{
	PxShape* shape = physx->m_Physics->createShape(PxBoxGeometry(size.x, size.y, size.z), *physx->m_Physics->createMaterial(0.2f, 0.2f, 0.2f));
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);	//시물레이션 off
	//shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);			//트리거링 on

	PxRigidDynamic * staticActor = physx->m_Physics->createRigidDynamic(PxTransform(t));

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

PxRigidDynamic* CRoom::getRotateBox(PxVec3& t, PxVec3& ro, PxVec3 size, CPhysx *physx)
{
	PxShape* shape = physx->m_Physics->createShape(PxBoxGeometry(size.x, size.y, size.z), *physx->m_Physics->createMaterial(0.2f, 0.2f, 0.2f));
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

	PxRigidDynamic * staticActor = physx->m_Physics->createRigidDynamic(temp);

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

PxRigidStatic* CRoom::getBoxTrigger(PxVec3& t, PxVec3 size, CPhysx *physx)
{
	PxShape* shape = physx->m_Physics->createShape(PxBoxGeometry(size.x, size.y, size.z), *physx->m_Physics->createMaterial(0.2f, 0.2f, 0.2f));
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);	//시물레이션 off
	shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);			//트리거링 on

	PxRigidStatic * staticActor = physx->m_Physics->createRigidStatic(PxTransform(t));

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

PxRigidStatic* CRoom::getBoxTrigger(PxVec3& t, PxVec3 size, int trigger_type, CPhysx *physx)
{
	PxShape* shape = physx->m_Physics->createShape(PxBoxGeometry(size.x, size.y, size.z), *physx->m_Physics->createMaterial(0.2f, 0.2f, 0.2f));
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);	//시물레이션 off
	shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);			//트리거링 on

	PxRigidStatic * staticActor = physx->m_Physics->createRigidStatic(PxTransform(t));

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

PxRigidStatic* CRoom::getRotateBoxTrigger(PxVec3& t, PxVec3& ro, PxVec3 size, int trigger_type, int order, CPhysx *physx)
{
	PxShape* shape = physx->m_Physics->createShape(PxBoxGeometry(size.x, size.y, size.z), *physx->m_Physics->createMaterial(0.2f, 0.2f, 0.2f));
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);	//시물레이션 off
	shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);			//트리거링 on

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

	PxRigidStatic * staticActor = physx->m_Physics->createRigidStatic(PxTransform(t, q));

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

PxRigidStatic* CRoom::getSphereTrigger(PxVec3& t, PxReal rad, int trigger_type, int order, CPhysx *physx)
{
	PxShape* shape = physx->m_Physics->createShape(PxSphereGeometry(rad), *physx->m_Physics->createMaterial(0.2f, 0.2f, 0.2f));
	shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);	//시물레이션 off
	shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);			//트리거링 on

	PxRigidStatic * staticActor = physx->m_Physics->createRigidStatic(PxTransform(t));

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

CRoom::~CRoom()
{
}
