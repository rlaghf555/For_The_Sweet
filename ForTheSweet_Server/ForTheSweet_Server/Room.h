#pragma once

#include "Player.h"
#include "Weapon.h"
#include "Physx.h"
#include "Timer.h"

enum EVENT_TYPE {
	EV_FREE, EV_HIT, EV_PICK, EV_WEAPON_SKILL, EV_WEAPON_REMOVE,		// �÷��̾ �ش�
	EV_LOLLIPOP_HEAL,
	EV_PEPERO_MOVE,
	EV_CANDY_MOVE,
	EV_WEAPON, EV_FOG, EV_FEVER, EV_LIGHTNING, EV_SLIME,				// ���ǿ� �ش�
	EV_TIME
};

struct EVENT_ST {
	int id;
	EVENT_TYPE type;
	high_resolution_clock::time_point start_time;
	char attack_count;

	constexpr bool operator < (const EVENT_ST& left) const
	{
		return (start_time > left.start_time);
	}
};

struct Skill_Actor {
	char order;
	char type;
	char index;
	char owner;
	PxRigidActor* skillTrigger;
	PxVec3 vel;
	PxVec3 look;

public:
	Skill_Actor(char tp, char in, char ow, char ord) {
		type = tp;
		owner = ow;
		index = in;
		order = ord;
	}

	bool operator==(const char ord) { return (order == ord); }
};

class CRoom
{
public:
	int room_num;					// �� �ѹ�
	char name[MAX_ROOM_NAME_LEN];	// �� �̸�

	int clientNum[MAX_ROOM_USER];	// �� ������ Ŭ�� ���� id
	char current_num;				// ���� �ο�
	char max_num;					// �ִ� �ο�
	char host_num;					// host�� slot
	char room_mode;					// 0 : ����, 1 : ������, 2 : ������

	char room_status;				// �� ���� (��� = 0, ���� �ε� = 1, ���� �� = 2)

	CPhysx *m_pPhysx;

	float PosBroadCastTime;

	priority_queue<EVENT_ST> m_timer_queue;
	mutex m_timer_l;

	bool load_complete[MAX_ROOM_USER];
	bool setting_complete[MAX_ROOM_USER];

	CWeapon weapon_list[MAX_WEAPON_TYPE][MAX_WEAPON_NUM];
	CWeapon_Respawn weapon_respawn[RESPAWN_WEAPON_NUM];

	int timer;

	PxBoxController *test;
	PxRigidActor *test2;

	vector<Skill_Actor> m_skillTrigger;
	char trigger_order;

public:
	CRoom();
	CRoom(const CRoom &);
	void init(char *nm, int hostId, int roomNum);
	bool attend(int Num);
	void start(const vector<PxVec3>& vectex, const vector<int>& index);
	bool operator==(const int num) { return (num == room_num); }
	bool all_load_complete();
	bool all_setting_complete();
	~CRoom();
};

