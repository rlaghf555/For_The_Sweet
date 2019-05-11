#pragma once
constexpr int MAX_PACKET_SIZE = 1024;

constexpr int SERVER_PORT = 7000;
constexpr int MAX_USER = 8;

constexpr int CS_CONNECT = 1;
constexpr int CS_DISCONNECT = 2;
constexpr int CS_MOVE = 3;
constexpr int CS_ATTACK = 4;

constexpr int CS_UP = 10;
constexpr int CS_DOWN = 11;
constexpr int CS_LEFT = 12;
constexpr int CS_RIGHT = 13;

constexpr int CS_GUARD = 14;
constexpr int CS_WEAK = 15;
constexpr int CS_HARD = 16;
constexpr int CS_JUMP = 17;

constexpr int SC_LOGIN = 1;
constexpr int SC_POS = 2;
constexpr int SC_PUT_PLAYER = 3;
constexpr int SC_REMOVE = 4;
constexpr int SC_ANIM = 5;


#pragma pack(push, 1)

struct cs_packet_connect {
	char size;
	char type;
};

struct cs_packet_disconnect {
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
};

struct sc_packet_login {
	char size;
	char type;
	char id;
	float x, y, z;
	float vx, vy, vz;
};

struct sc_packet_pos {
	char size;
	char type;
	char id;
	float x, y, z;
	float vx, vy, vz;
	char ani_index;
	float ani_frame;
};

struct sc_packet_put_player {
	char size;
	char type;
	char id;
	float x, y, z;
	float vx, vy, vz;
	char ani_index;
	float ani_frame;
};

struct sc_packet_anim {
	char size;
	char type;
	char id;
	char ani_index;
	float ani_frame;
};

struct sc_packet_remove {
	char size;
	char type;
	char id;
};

#pragma pack (pop)