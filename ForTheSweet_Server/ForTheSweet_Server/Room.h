#pragma once

#include "Player.h"
#include "Weapon.h"
#include "Physx.h"
#include "Timer.h"

enum EVENT_TYPE {
	EV_FREE, EV_HIT, EV_PICK, EV_WEAPON_SKILL, EV_WEAPON_REMOVE,		// 플레이어에 해당
	EV_LOLLIPOP_HEAL,
	EV_PEPERO_MOVE,
	EV_CANDY_MOVE,
	EV_CUPCAKE_REMOVE,
	EV_KING_OFF,
	EV_RFR_WEAPON, EV_RFR_FOG, EV_RFR_FEVER, EV_RFR_LIGHTNING, EV_RFR_SLIME,				// 심판에 해당
	EV_RFR_FOG_END, EV_RFR_FEVER_END, EV_RFR_LIGHTNING_END,
	EV_RFR_LIGHTNING_INDEX, EV_RFR_LIGHTNING_DELETE,
	EV_TIME,
	EV_END
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

struct Referee {
	char patern_type = 0;
	char patern_count = 0;
};

class CRoom
{
public:
	int room_num;					// 방 넘버
	char name[MAX_ROOM_NAME_LEN];	// 방 이름

	int clientNum[MAX_ROOM_USER];	// 각 슬롯의 클라에 대한 id
	char current_num;				// 현재 인원
	char max_num;					// 최대 인원
	char host_num;					// host의 slot
	char room_mode;					// 0 : 팀전, 1 : 개인전, 2 : 대장전
	char room_map;

	char room_status;				// 방 상태 (대기 = 0, 게임 로딩 = 1, 게임 로딩완료 = 2, 게임 중 = 3, 게임 끝 = 4)

	CPhysx *m_pPhysx;

	float PosBroadCastTime;

	priority_queue<EVENT_ST> m_timer_queue;
	mutex m_timer_l;

	bool load_complete[MAX_ROOM_USER];
	bool setting_complete[MAX_ROOM_USER];

	CWeapon weapon_list[MAX_WEAPON_TYPE][MAX_WEAPON_NUM];
	CWeapon_Respawn weapon_respawn[RESPAWN_WEAPON_NUM];

	int timer;

	PxRigidActor *move_actor;
	bool move_actor_flag = false;

	vector<Skill_Actor> m_skillTrigger;
	char trigger_order;

	// 심판
	Referee referee;
	bool fever = false;
	bool lighting = false;
	char light_count = 0;
	char light_index1 = 0;
	char light_index2 = 0;

	char solo_dead_count = 0;												// 개인전에 사용
	bool team_dead[8] = { false, false, false, false, false, false, false };	// 팀전에 사용(false : 없는 플레이어, true : 있는 플레이어)
	bool team_victory = false;												// true: 1팀 우승, false : 2팀 우승

public:
	CRoom();
	CRoom(const CRoom &);
	void init(char *nm, int hostId, int roomNum);
	bool attend(int Num);
	void start(int map_type, const vector<vector<PxVec3>>& vectex, const vector<vector<int>>& index);
	bool operator==(const int num) { return (num == room_num); }
	bool all_load_complete();
	bool all_setting_complete();
	bool is_game_end();
	~CRoom();
};

