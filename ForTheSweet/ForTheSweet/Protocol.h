#pragma once
constexpr int MAX_PACKET_SIZE = 1024;

constexpr int SERVER_PORT = 7000;
constexpr int MAX_USER = 8;

constexpr int CS_CONNECT = 1;
constexpr int CS_DISCONNECT = 2;
constexpr int CS_MOVE = 3;
constexpr int CS_ATTACK = 4;
constexpr int CS_WEAPON = 5;

constexpr int CS_UP = 10;
constexpr int CS_DOWN = 11;
constexpr int CS_LEFT = 12;
constexpr int CS_RIGHT = 13;

constexpr int CS_GUARD = 14;
constexpr int CS_WEAK = 15;
constexpr int CS_HARD = 16;
constexpr int CS_JUMP = 17;
constexpr int CS_DASH = 18;
constexpr int CS_GUARD_OFF = 19;
constexpr int CS_WEAPON_SKILL = 20;

constexpr int CS_MAKE_ROOM = 100;
constexpr int CS_ATTEND_ROOM = 101;
constexpr int CS_START_ROOM = 102;
constexpr int CS_READY_ROOM = 103;
constexpr int CS_LOAD_COMPLETE = 104;
constexpr int CS_UPDATE_ROOM = 105;
constexpr int CS_SETTING_COMPLETE = 106;

constexpr int SC_LOGIN = 1;
constexpr int SC_POS = 2;
constexpr int SC_PUT_PLAYER = 3;
constexpr int SC_REMOVE = 4;
constexpr int SC_ANIM = 5;
constexpr int SC_PICK_WEAPON = 6;
constexpr int SC_HIT = 7;
constexpr int SC_PUT_WEAPON = 8;
constexpr int SC_TIMER = 9;

constexpr int SC_ROOM_INFO = 100;
constexpr int SC_ROOM_DETAIL_INFO = 101;
constexpr int SC_ROOM_LOAD = 102;
constexpr int SC_ROOM_START = 103;

#pragma pack(push, 1)

struct cs_packet_connect {
	char size;
	char type;
	char id[15];
};

struct cs_packet_disconnect {
	char size;
	char type;
};

struct cs_packet_update_room {
	char size;
	char type;
};

struct cs_packet_make_room {
	char size;
	char type;
	char name[10];
};

struct cs_packet_attend_room {
	char size;
	char type;
	int room_num;
};

struct cs_packet_start_room {
	char size;
	char type;
	int room_num;
};

struct cs_packet_ready_room {
	char size;
	char type;
	int room_num;
};

struct cs_packet_load_complete {
	char size;
	char type;
};

struct cs_packet_settting_complete {
	char size;
	char type;
};

struct cs_packet_move {
	char size;
	char type;
	char key;			// 좌우상하 누릴는 키값
	char state;			// 처음 눌렀을 떄(1), 때졋을때(0)
};

struct cs_packet_anim {
	char size;
	char type;
	char key;
	char count;
};

struct cs_packet_weapon {
	char size;
	char type;
	char weapon_type;
	char weapon_index;
};

struct cs_packet_weapon_skill {
	char size;
	char type;
	//char weapon_type;
};

struct sc_packet_login {
	char size;
	char type;
	char id;
	float x, y, z;
	float vx, vy, vz;
};

struct sc_packet_room_info {
	char size;
	char type;
	char name[10];
	char slot;
	char current_num;
	char room_mode;
	int room_num;
};

struct sc_packet_room_detail_info {
	char size;
	char type;
	int room_num;
	char room_index;
	char room_mode;
	char player_name[15];
	char host;
};

struct sc_packet_room_load {
	char size;
	char type;
};

struct sc_packet_room_start {
	char size;
	char type;
};

struct sc_packet_pos {
	char size;
	char type;
	char id;
	float x, y, z;
	float vx, vy, vz;
	char ani_index;
	char dashed;
};

struct sc_packet_put_player {
	char size;
	char type;
	char id;
	float x, y, z;
	float vx, vy, vz;
	char ani_index;
	char dashed;
};

struct sc_packet_anim {
	char size;
	char type;
	char id;
	char ani_index;
};

struct sc_packet_hit {
	char size;
	char type;
	char id;
	char hp;
};

struct sc_packet_put_weapon {
	char size;
	char type;
	char weapon_type;
	char weapon_index;
	float x, y, z;
};

struct sc_packet_pick_weapon {
	char size;
	char type;
	char id;
	char weapon_type;
	char weapon_index;
};

struct sc_packet_remove {
	char size;
	char type;
	char id;
};

struct sc_packet_timer {
	char size;
	char type;
	int timer;
};




#pragma pack (pop)