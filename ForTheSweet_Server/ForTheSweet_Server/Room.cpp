#include "Room.h"

CRoom::CRoom()
{
	room_num = 0;
	current_num = 0;
	max_num = MAX_ROOM_USER;

	room_status = 0;
	room_mode = ROOM_MODE_SOLO;
	room_map = MAP_Wehas;

	m_pPhysx = nullptr;

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

void CRoom::start(int map_type, const vector<vector<PxVec3>>& vectex, const vector<vector<int>>& index)
{
	m_pPhysx = new CPhysx();
	m_pPhysx->initPhysics();

	// ∏  ∑Œµ˘
	if (map_type == MAP_Wehas)
	{
		// ∏  1 ±‚∫ª πŸ¥⁄
		PxVec3 scaleTmp = PxVec3(1.0f, 1.0f, 1.0f);
		PxMeshScale PxScale;
		PxScale.scale = scaleTmp;
		PxMaterial* mat = m_pPhysx->m_Physics->createMaterial(0.2f, 0.2f, 0.2f);

		PxTriangleMesh* triMesh = m_pPhysx->GetTriangleMesh(vectex[MAP_1_BASE], index[MAP_1_BASE]);
		PxTriangleMeshGeometry meshGeo(triMesh, PxScale);
		PxTransform location(0, 0, 0);
		PxRigidActor* base = PxCreateStatic(*m_pPhysx->m_Physics, location, meshGeo, *mat);
		m_pPhysx->m_Scene->addActor(*base);

		// ∏  1 ∏∂ƒ´∑’
		PxTriangleMesh* triMesh2 = m_pPhysx->GetTriangleMesh(vectex[MAP_MACARON], index[MAP_MACARON]);
		PxTriangleMeshGeometry meshGeo2(triMesh2, PxScale);
		PxTransform location2(PxVec3(0.f, -50.f, 0.f));
		PxRigidActor* macaron = PxCreateStatic(*m_pPhysx->m_Physics, location2, meshGeo2, *mat);
		m_pPhysx->m_Scene->addActor(*macaron);

		move_actor = macaron;
		move_actor->userData = (void *)(int)1;

		// √ ƒ›∏¥ ∫Æ ª˝º∫
		PxVec3 wall_size(3, 100, 60);
		PxVec3 wall_pos(167, 40, 0);
		m_pPhysx->getBox(wall_pos, wall_size);
		wall_pos.x *= -1.f;
		m_pPhysx->getBox(wall_pos, wall_size);
	}
	else if (room_map == MAP_Oreo)
	{

	}
	else if (room_map == MAP_Cake)
	{

	}

	if (room_mode == ROOM_MODE_SOLO)
	{
		solo_dead_count = 0;
	}
	else if (room_mode == ROOM_MODE_TEAM || ROOM_MODE_KING)
	{
		// ∆¿, ¥Î¿Â¿¸¿∫ π€ø°º≠ ºº∆√
		for (int i = 0; i < MAX_USER; ++i)
		{
			team_dead[i] = false;
		}
	}

	// ∏∂¡ˆ∏∑ Room ºº∆√
	room_status = 1;
	timer = MAX_TIMER;
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
			if (team_dead[i] == true)		// true : ªÏæ∆ ¿÷¿Ω
			{
				is_Team1_end = false;
				break;
			}
		}
		// 2team check
		bool is_Team2_end = true;
		for (int i = 4; i < MAX_ROOM_USER / 2; ++i)
		{
			if (team_dead[i] == true)		// true : ªÏæ∆ ¿÷¿Ω
			{
				is_Team2_end = false;
				break;
			}
		}

		if (is_Team1_end || is_Team2_end) {
			if (is_Team1_end == true)
			{
				team_victory = true;			// 1∆¿ øÏΩ¬
			}
			if (is_Team2_end = true) {
				team_victory = false;			// 2∆¿ øÏΩ¬
			}
			return true;
		}
		return false;
	}
	return false;
}

CRoom::~CRoom()
{
}
