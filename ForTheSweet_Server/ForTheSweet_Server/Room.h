#pragma once

#include "Player.h"
#include "Physx.h"
#include "Timer.h"

enum EVENT_TYPE {
	EV_FREE, EV_HIT, EV_PICK,											// 플레이어에 해당
	EV_WEAPON, EV_FOG, EV_FEVER, EV_LIGHTNING, EV_SLIME,				// 심판에 해당
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

class CRoom
{
public:
	int room_num;					// 방 넘버
	char name[MAX_ROOM_NAME_LEN];	// 방 이름

	int clientNum[MAX_ROOM_USER];	// 각 슬롯의 클라에 대한 id
	char current_num;				// 현재 인원
	char max_num;					// 최대 인원
	char host_num;					// host의 slot
	char room_mode;					// 0 : 웨하스, 1 : 오레오, 2 : 케이크

	char room_status;				// 방 상태 (대기 = 0, 게임 로딩 = 1, 게임 중 = 2)

	CPhysx *m_pPhysx;

	float PosBroadCastTime;

	priority_queue<EVENT_ST> m_timer_queue;
	mutex m_timer_l;

	bool load_complete[MAX_ROOM_USER];
	char weapon_num;
	char weapon_list[MAX_WEAPON_TYPE][MAP_1_MAX_WEAPON_NUM];

public:
	CRoom();
	CRoom(const CRoom &);
	void init(char *nm, int hostId, int roomNum);
	bool attend(int Num);
	void start(const vector<PxVec3>& vectex, const vector<int>& index);
	bool operator==(const int num) { return (num == room_num); }
	bool all_load_complete();
	~CRoom();
};

