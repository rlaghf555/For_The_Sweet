#include "Room.h"

CRoom::CRoom()
{
	room_num = 0;
	current_num = 0;
	max_num = MAX_ROOM_USER;

	room_status = 0;
	room_mode = ROOM_MODE_INDIVIDUAL;
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

void CRoom::init(char *nm, int hostId, int roomNum)
{
	strcpy_s(name, _countof(name), nm);
	host_num = 0;
	current_num = 1;
	room_num = roomNum;
	room_mode = ROOM_MODE_INDIVIDUAL;
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

	// ¸Ê ·Îµù
	if (map_type == MAP_Wehas)
	{
		// ¸Ê 1 ±âº» ¹Ù´Ú
		PxTriangleMesh* triMesh = m_pPhysx->GetTriangleMesh(vectex[MAP_1_BASE], index[MAP_1_BASE]);

		PxVec3 scaleTmp = PxVec3(1.0f, 1.0f, 1.0f);
		PxMeshScale PxScale;
		PxScale.scale = scaleTmp;
		PxTriangleMeshGeometry meshGeo(triMesh, PxScale);
		PxTransform location(0, 0, 0);

		PxMaterial* mat = m_pPhysx->m_Physics->createMaterial(0.2f, 0.2f, 0.2f);
		PxRigidActor* base = PxCreateStatic(*m_pPhysx->m_Physics, location, meshGeo, *mat);

		m_pPhysx->m_Scene->addActor(*base);

		// ¸Ê 1 ¸¶Ä«·Õ
		PxTriangleMesh* triMesh2 = m_pPhysx->GetTriangleMesh(vectex[MAP_MACARON], index[MAP_MACARON]);
		PxTriangleMeshGeometry meshGeo2(triMesh2, PxScale);
		PxTransform location2(PxVec3(0.f, -50.f, 0.f));

		// À§Ä¡ Á¶Á¤

		PxRigidActor* macaron = PxCreateStatic(*m_pPhysx->m_Physics, location2, meshGeo2, *mat);
		m_pPhysx->m_Scene->addActor(*macaron);

		move_actor = macaron;
		move_actor->userData = (void *)(int)1;

		// ÃÊÄÝ¸´ º® »ý¼º
		PxVec3 wall_size(3, 100, 60);
		PxVec3 wall_pos(167, 40, 0);

		m_pPhysx->getBox(wall_pos, wall_size);
		wall_pos.x *= -1.f;
		m_pPhysx->getBox(wall_pos, wall_size);

		// ¼¼ÆÃ
		room_mode = 0;
		room_status = 1;
		timer = MAX_TIMER;
	}
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

CRoom::~CRoom()
{
}
