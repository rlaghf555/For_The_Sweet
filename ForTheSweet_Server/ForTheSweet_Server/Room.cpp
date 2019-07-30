#include "Room.h"

CRoom::CRoom()
{
	room_num = 0;
	current_num = 0;
	max_num = MAX_ROOM_USER;
	
	room_status = 0;
	room_mode = ROOM_MODE_INDIVIDUAL;

	weapon_num = 0;

	m_pPhysx = nullptr;

	PosBroadCastTime = 0.0f;

	for (int i = 0; i < MAX_ROOM_USER; ++i)
	{
		clientNum[i] = -1;
		load_complete[i] = false;
	}

	for (int i = 0; i < MAX_WEAPON_TYPE; ++i)
	{
		for (int j = 0; j < MAP_1_MAX_WEAPON_NUM; ++j)
		{
			weapon_list[i][j] = -1;
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
	weapon_num = other.weapon_num;

	PosBroadCastTime = other.PosBroadCastTime;

	for (int i = 0; i < MAX_ROOM_USER; ++i)
	{
		clientNum[i] = other.clientNum[i];
		load_complete[i] = other.load_complete[i];
	}

	for (int i = 0; i < MAX_WEAPON_TYPE; ++i)
	{
		for (int j = 0; j < MAP_1_MAX_WEAPON_NUM; ++j)
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

void CRoom::start(const vector<PxVec3>& vectex, const vector<int>& index)
{
	m_pPhysx = new CPhysx();
	m_pPhysx->initPhysics();

	// 맵 로딩
	PxTriangleMesh* triMesh = m_pPhysx->GetTriangleMesh(vectex, index);

	PxVec3 scaleTmp = PxVec3(1.0f, 1.0f, 1.0f);

	PxMeshScale PxScale;
	PxScale.scale = scaleTmp;

	PxTriangleMeshGeometry meshGeo(triMesh, PxScale);
	PxTransform location(0, 0, 0);

	PxMaterial* mat = m_pPhysx->m_Physics->createMaterial(0.2f, 0.2f, 0.2f);

	PxRigidActor* m_Actor = PxCreateStatic(*m_pPhysx->m_Physics, location, meshGeo, *mat);
	m_pPhysx->m_Scene->addActor(*m_Actor);

	// 무기 로딩

	// 플레이어 로딩은 Server_FTS에서 해줘야 해서 여기서 안함

	// 세팅
	room_mode = 0;
	room_status = 1;
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

CRoom::~CRoom()
{
}
