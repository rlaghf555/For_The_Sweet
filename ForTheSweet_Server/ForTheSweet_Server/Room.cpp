#include "Room.h"

CRoom::CRoom()
{
	room_num = 0;
	current_num = 0;
	max_num = MAX_ROOM_USER;

	room_status = 0;
	room_mode = ROOM_MODE_INDIVIDUAL;

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

	// room : PhysicSimulation에 등록

	PxVec3 wall_size(3, 100, 60);
	PxVec3 wall_pos(167, 40, 0);

	test2 = m_pPhysx->getBox(wall_pos, wall_size);
	wall_pos.x *= -1.f;
	m_pPhysx->getBox(wall_pos, wall_size);

	//m_pPhysx->setBoxController(wall_pos, wall_size);
	//wall_pos.x *= -1.f;
	//m_pPhysx->setBoxController(wall_pos, wall_size);

	// 무기 로딩

	//Skill_Trigger *s_trigger = new Skill_Trigger(0, 0, 0);
	//Skill_Trigger *s_trigger2 = new Skill_Trigger(0, 0, 0);
	//
	//PxVec3 pos(-50, 27.5, 100);
	//PxVec3 pos2(50, 27.5, 100);
	//PxVec3 init(1, 0, 0);
	//PxVec3 look(0, 0, 1);			// 캐릭터의 look.z만 * -1 해줄 것
	//
	//look = look.getNormalized();
	//
	//cout << look.x << "," << look.y << "," << look.z << endl;
	//
	//PxVec3 a = init.cross(look);
	//
	//PxQuat q;
	//q.x = a.x;
	//q.y = a.y;
	//q.z = a.z;
	//q.w = sqrt((init.magnitude() * init.magnitude()) * (look.magnitude()*look.magnitude())) + init.dot(look);
	//
	//q = q.getNormalized();
	//
	//s_trigger->skillTrigger = m_pPhysx->getBoxTrigger(pos, PxVec3(20, 1, 1));
	//s_trigger2->skillTrigger = m_pPhysx->getBoxTrigger(pos2, PxVec3(20, 1, 1));
	//
	//PxTransform t = s_trigger->skillTrigger->getGlobalPose();
	//
	//cout << t.q.x << "," << t.q.y << "," << t.q.z << "," << t.q.w << endl;
	//
	//float test = q.getAngle(t.q);
	//
	//
	//cout << "quat : " << q.x << "," << q.y << "," << q.z << "," << q.w << endl;
	//cout << "test  : " << test << endl;
	//
	//float angle = t.q.getAngle();
	//
	//cout << "quat : " << t.q.x << "," << t.q.y << "," << t.q.z << "," << t.q.w << endl;
	//cout << "angle : " << angle << endl;
	//PxVec3 result = t.rotate(look.getNormalized());
	//
	//t.q = q;
	//
	//angle = t.q.getAngle();
	//cout << "quat : " << t.q.x << "," << t.q.y << "," << t.q.z << "," << t.q.w << endl;
	//cout << "angle2 : " << angle << endl;
	//
	//s_trigger->skillTrigger->setGlobalPose(t);
	//
	//m_skillTrigger.push_back(*s_trigger);
	//m_skillTrigger.push_back(*s_trigger2);


	// 플레이어 로딩은 Server_FTS에서 해줘야 해서 여기서 안함



	// 세팅
	room_mode = 0;
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

CRoom::~CRoom()
{
}
