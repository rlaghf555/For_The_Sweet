#define WIN32_LEAN_AND_MEAN  
#define INITGUID
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "header.h"
#include "Protocol.h"
#include "Player.h"
#include "Room.h"
#include "Physx.h"
#include "Timer.h"
#include "Util.h"

#pragma comment(lib, "ws2_32.lib")

#define MAX_BUFFER 1024

HANDLE g_iocp;

vector<vector<PxVec3>> gMapVertexs;
vector<vector<int>> gMapIndexs;

vector<CRoom> gRoom;
mutex room_l;

unordered_set<int> gLobby;
mutex lobby_l;

CGameTimer gGameTimer;
CPhysx *gPhysx;

PxVec3 PlayerInitPos_1[8] = {
   PxVec3(0, 10.1, 100), PxVec3(50, 10.1, 100), PxVec3(-50, 10.1, 100), PxVec3(100, 10.1, 100), PxVec3(-100, 10.1, 100),
   PxVec3(150, 10.1, 100), PxVec3(-150, 10.1, 100), PxVec3(200, 10.1, 100)
};
PxVec3 PlayerInitPos_2[8] = {
	PxVec3(-299, 18.0011, 46),PxVec3(-299, 18.0011, 13),PxVec3(-299, 18.0011, -23),PxVec3(-299, 18.0011, -58),
	PxVec3(299, 18.0011, 46),PxVec3(299, 18.0011, 13),PxVec3(299, 18.0011, -23),PxVec3(299, 18.0011, -58)
};
PxVec3 PlayerInitPos_3[8] = {
	PxVec3(-120, 1.31468, 53),PxVec3(120, 1.31468, 53),PxVec3(-134, 1.31468, -122),PxVec3(134 , 1.31468, -122),
	PxVec3(-368, 1.31468, -132),PxVec3(368, 1.31468,-132),PxVec3(-248, 76.6323,-32),PxVec3(248, 76.6323,-32)
};

PxVec3 OreoTeleport[2] = {
	PxVec3(-186, 3.77634, -1.5),
	PxVec3(186, 3.77634, -1.5)
};

PxVec3 WeaponInitPos_1[10] = {
	PxVec3(-200, 10, -100), PxVec3(-200, 10, 0), PxVec3(-200, 10, 100), PxVec3(-100, 10, -50), PxVec3(-100, 10, 50),
	PxVec3(100, 10, -50), PxVec3(100, 10, 50), PxVec3(200, 10, -100), PxVec3(200, 10, 0), PxVec3(200, 10, 100)
};
PxVec3 WeaponInitPos_2[10] = {
	PxVec3(0, 5.68, 115), PxVec3(-76, 5.68, 95), PxVec3(-116, 5.68, 52), PxVec3(-116, 5.68, -52), PxVec3(-76, 5.68, -95),
	PxVec3(0, 5.68, -115), PxVec3(76, 5.68, 95), PxVec3(116, 5.68, 52), PxVec3(116, 5.68, -52), PxVec3(76, 5.68, -95)
};
PxVec3 WeaponInitPos_3[10] = {
   PxVec3(-165.951, 1.31468, -127.585), PxVec3(165.951, 1.31468, -127.585), PxVec3(-306.981, 1.31468, -127.585), PxVec3(306.981, 1.31468, -127.585), PxVec3(-249.68, 76.6323, -13.0885),
   PxVec3(249.68, 76.6323, -13.0885), PxVec3(-300.596, 500.751, 27.5226), PxVec3(310.442, 500.928, 22.9703), PxVec3(-184.102, 500.927, 22.4771), PxVec3(195.677, 500.928, 22.9703)
};

PxVec3 LightningInitPos_12[10] = {
   PxVec3(-100.f, 100.f, 100.f), PxVec3(-120.f, 100.f, 110.f), PxVec3(60.f, 100.f, 110.f), PxVec3(180.f, 100.f, 110.f), PxVec3(-150.f, 100.f, 0.f),
   PxVec3(90.f, 100.f, 0.f), PxVec3(-240.f, 100.f, -110.f), PxVec3(-120.f, 100.f, -110.f), PxVec3(60.f, 100.f, -110.f), PxVec3(180.f, 100.f, -110.f)
};

PxVec3 DoorPos[8] = {
	PxVec3(-250.f, 10.f, -40.f),    //LEFT_DOWN_OUT
	PxVec3(-250.f, 530.f, -80.f),    //LEFT_DOWN_IN
	PxVec3(-250.f, 85.f, 40.f),        //LEFT_UP_OUT
	PxVec3(-250.f, 585.f, 10.f),    //LEFT_UP_IN

	PxVec3(250.f, 10.f, -40.f),        //RIGHT_DOWN_OUT
	PxVec3(250.f, 530.f, -80.f),    //RIGHT_DOWN_IN
	PxVec3(250.f, 85.f, 40.f),        //RIGHT_UP_OUT
	PxVec3(250.f, 585.f, 10.f)        //RIGHT_UP_IN
};
PxVec3 DoorTelepos[8] = {
	PxVec3(-250.f, 530.f - 17.5f, -80.f + 30.f),    //LEFT_DOWN_OUT
	PxVec3(-250.f, 10.f - 17.5f, -40.f - 30.f),    //LEFT_DOWN_IN
	PxVec3(-250.f, 585.f - 17.5f, 10.f+ 30.f),    //LEFT_UP_OUT
	PxVec3(-250.f, 85.f - 17.5f, 40.f- 30.f),        //LEFT_UP_IN

	PxVec3(250.f, 530.f - 17.5f, -80.f + 30.f),    //RIGHT_DOWN_OUT
	PxVec3(250.f, 10.f - 17.5f, -40.f - 30.f),        //RIGHT_DOWN_IN
	PxVec3(250.f, 585.f - 17.5f, 10.f + 30.f),        //RIGHT_UP_OUT
	PxVec3(250.f, 85.f - 17.5f, 40.f- 30.f)        //RIGHT_UP_IN
};

enum SITUATION
{
	ST_LOBBY, ST_ROOM
};

priority_queue<EVENT_ST> timer_queue;

mutex timer_l;

struct OVER_EX {
	WSAOVERLAPPED overlapped;
	WSABUF dataBuffer;
	char messageBuffer[MAX_BUFFER];
	bool is_recv;
};

class SOCKETINFO
{
public:
	OVER_EX over_ex;
	SOCKET socket;
	char packetBuffer[MAX_BUFFER];
	int prev_size;
	CPlayer *playerinfo;
	//float vx, vy, vz;
	volatile bool connected;
	char id[15];
	char situation;
	char slot;
	int room_num;

	SOCKETINFO() {
		over_ex.dataBuffer.len = MAX_BUFFER;
		over_ex.dataBuffer.buf = over_ex.messageBuffer;
		ZeroMemory(&over_ex.overlapped, sizeof(WSAOVERLAPPED));
		//playerinfo = new CPlayer();
		prev_size = 0;
		over_ex.is_recv = true;
		connected = false;
	}
};

SOCKETINFO clients[MAX_USER];
int roomNum = 1;

void error_display(const char *msg, int err_no)
{
	WCHAR *lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	std::cout << msg;
	std::wcout << L"  에러" << lpMsgBuf << std::endl;
	LocalFree(lpMsgBuf);
	while (true);
}

void ErrorDisplay(const char * location)
{
	error_display(location, WSAGetLastError());
}

void add_timer(int room_num, int id, EVENT_TYPE et, high_resolution_clock::time_point start_time, char attack_count = 0)
{
	room_l.lock();
	auto it = find(gRoom.begin(), gRoom.end(), room_num);
	room_l.unlock();

	it->m_timer_l.lock();
	it->m_timer_queue.push(EVENT_ST{ id, et, start_time, attack_count });
	it->m_timer_l.unlock();
}

void do_recv(char id)
{
	DWORD flags = 0;

	ZeroMemory(&clients[id].over_ex.overlapped, sizeof(WSAOVERLAPPED));

	int retval = WSARecv(clients[id].socket, &clients[id].over_ex.dataBuffer, 1,
		NULL, &flags, &clients[id].over_ex.overlapped, 0);

	if (0 != retval) {
		int err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no)
		{
			cout << "Error1 - IO pending Recv Failure\n";
		}
	}
	else {
		//cout << "Non Overlapped Recv~~~~~~~~~~~~.\n";
	}
}

void sendPacket(char key, void *ptr)
{
	char *packet = reinterpret_cast<char *>(ptr);
	OVER_EX *over = new OVER_EX;
	over->is_recv = false;
	memcpy(over->messageBuffer, packet, packet[0]);
	over->dataBuffer.buf = over->messageBuffer;
	over->dataBuffer.len = over->messageBuffer[0];

	//cout << int(over->messageBuffer[0]) << ", " << int(over->messageBuffer[1]) << endl;

	ZeroMemory(&over->overlapped, sizeof(WSAOVERLAPPED));
	int res = WSASend(clients[key].socket, &over->dataBuffer, 1, NULL, 0,
		&over->overlapped, NULL);
	if (0 != res) {
		int err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no)
		{
			cout << "Error1 - IO pending Send Failure\n";
		}
	}
	else {
		//cout << "Non Overlapped Send~~~~~~~~~~~~.\n";
	}
}

void send_room_info_packet(char client, const CRoom& room, int slot)
{
	sc_packet_room_info p_room_info;
	p_room_info.size = sizeof(sc_packet_room_info);
	p_room_info.type = SC_ROOM_INFO;
	p_room_info.slot = slot;
	strcpy_s(p_room_info.name, _countof(p_room_info.name), room.name);
	p_room_info.current_num = room.current_num;
	p_room_info.room_num = room.room_num;
	p_room_info.room_mode = room.room_mode;

	sendPacket(client, &p_room_info);
}

void send_room_datail_info_packet(int client, int player, char slot, int room_num, char room_mode, char room_map, char room_user, char host)
{
	sc_packet_room_detail_info p_room_detail_info;
	p_room_detail_info.size = sizeof(sc_packet_room_detail_info);
	p_room_detail_info.type = SC_ROOM_DETAIL_INFO;
	p_room_detail_info.room_num = room_num;
	p_room_detail_info.room_index = slot;
	p_room_detail_info.room_mode = room_mode;
	p_room_detail_info.room_map = room_map;
	p_room_detail_info.room_user = room_user;
	p_room_detail_info.host = host;
	strcpy_s(p_room_detail_info.player_name, _countof(p_room_detail_info.player_name), clients[player].id);

	sendPacket(client, &p_room_detail_info);
}

void send_room_detail_delete_packet(int client, char slot)
{
	sc_packet_room_detail_delete p_room_detail_delete;
	p_room_detail_delete.size = sizeof(sc_packet_room_detail_delete);
	p_room_detail_delete.type = SC_ROOM_DETAIL_DELETE;
	p_room_detail_delete.room_index = slot;

	sendPacket(client, &p_room_detail_delete);
}

void send_room_option_packet(int client, char room_mode, char room_map, char room_member)
{
	sc_packet_room_option p_room_option;
	p_room_option.size = sizeof(sc_packet_room_option);
	p_room_option.type = SC_ROOM_OPTION;
	p_room_option.room_mode = room_mode;
	p_room_option.room_map = room_map;
	p_room_option.room_member = room_member;

	sendPacket(client, &p_room_option);
}

void send_start_load_room_packet(int client)
{
	sc_packet_room_load p_room_load;
	p_room_load.size = sizeof(sc_packet_room_load);
	p_room_load.type = SC_ROOM_LOAD;

	sendPacket(client, &p_room_load);
}

void send_start_game_packet(int client)
{
	sc_packet_room_start p_room_start;
	p_room_start.size = sizeof(sc_packet_room_start);
	p_room_start.type = SC_ROOM_START;

	sendPacket(client, &p_room_start);
}

void send_put_player_packet(char client, char new_id) {
	sc_packet_put_player p_put;
	p_put.id = clients[new_id].slot;
	p_put.x = clients[new_id].playerinfo->m_Pos.x;
	p_put.y = clients[new_id].playerinfo->m_Pos.y;
	p_put.z = clients[new_id].playerinfo->m_Pos.z;
	p_put.vx = clients[new_id].playerinfo->m_Vel.x;
	p_put.vy = clients[new_id].playerinfo->m_Vel.y;
	p_put.vz = clients[new_id].playerinfo->m_Vel.z;
	p_put.ani_index = clients[new_id].playerinfo->m_AniIndex;
	p_put.dashed = clients[new_id].playerinfo->m_dashed;
	p_put.skill = clients[new_id].playerinfo->m_skill;
	p_put.type = SC_PUT_PLAYER;
	p_put.size = sizeof(sc_packet_put_player);

	sendPacket(client, &p_put);
}

void send_pos_packet(char client, char id) {
	sc_packet_pos p_pos;
	p_pos.id = clients[id].slot;
	p_pos.x = clients[id].playerinfo->m_Pos.x;
	p_pos.y = clients[id].playerinfo->m_Pos.y;
	p_pos.z = clients[id].playerinfo->m_Pos.z;
	p_pos.vx = clients[id].playerinfo->m_Vel.x;
	p_pos.vy = clients[id].playerinfo->m_Vel.y;
	p_pos.vz = clients[id].playerinfo->m_Vel.z;
	p_pos.ani_index = clients[id].playerinfo->m_AniIndex;
	p_pos.dashed = clients[id].playerinfo->m_dashed;
	p_pos.type = SC_POS;
	p_pos.size = sizeof(sc_packet_pos);

	sendPacket(client, &p_pos);
}

void send_remove_player_packet(char client, int id) {

}

void send_anim_packet(char client, char id) {
	sc_packet_anim p_anim;
	p_anim.type = SC_ANIM;
	p_anim.size = sizeof(sc_packet_anim);
	p_anim.id = clients[id].slot;
	p_anim.ani_index = clients[id].playerinfo->m_AniIndex;

	sendPacket(client, &p_anim);
}

void send_hit_packet(char client, char id, int hp, int mp)
{
	sc_packet_hit p_hit;
	p_hit.type = SC_HIT;
	p_hit.size = sizeof(sc_packet_hit);
	p_hit.id = clients[id].slot;
	p_hit.hp = hp;
	p_hit.mp = mp;

	sendPacket(client, &p_hit);
}

void send_critical_hit_packet(char client, char id, int hp, int mp, const PxVec3& dir)
{
	sc_packet_critical_hit p_cri_hit;
	p_cri_hit.type = SC_CRITICAL_HIT;
	p_cri_hit.size = sizeof(sc_packet_critical_hit);
	p_cri_hit.id = clients[id].slot;
	p_cri_hit.hp = hp;
	p_cri_hit.mp = mp;
	p_cri_hit.x = dir.x;
	p_cri_hit.y = dir.y;
	p_cri_hit.z = dir.z;

	sendPacket(client, &p_cri_hit);
}

void send_stun_packet(char client, char id, int hp, int mp)
{
	sc_packet_stun p_stun;
	p_stun.type = SC_STUN;
	p_stun.size = sizeof(sc_packet_stun);
	p_stun.id = clients[id].slot;
	p_stun.hp = hp;
	p_stun.mp = mp;

	sendPacket(client, &p_stun);
}

void send_heal_packet(char client, char id, int hp, int mp)
{
	sc_packet_heal p_heal;
	p_heal.type = SC_HEAL;
	p_heal.size = sizeof(sc_packet_heal);
	p_heal.id = clients[id].slot;
	p_heal.hp = hp;
	p_heal.mp = mp;

	sendPacket(client, &p_heal);
}

void send_fall_packet(char client, char id, int hp)
{
	sc_packet_fall p_fall;
	p_fall.type = SC_FALL;
	p_fall.size = sizeof(sc_packet_fall);
	p_fall.id = clients[id].slot;
	p_fall.hp = hp;

	sendPacket(client, &p_fall);
}

void send_put_weapon_packet(char client, char wp_type, char wp_index, const PxVec3& pos) {
	sc_packet_put_weapon p_put_weapon;

	p_put_weapon.type = SC_PUT_WEAPON;
	p_put_weapon.size = sizeof(sc_packet_put_weapon);
	p_put_weapon.weapon_type = wp_type;
	p_put_weapon.weapon_index = wp_index;
	p_put_weapon.x = pos.x;
	p_put_weapon.y = pos.y;
	p_put_weapon.z = pos.z;

	sendPacket(client, &p_put_weapon);
}

void send_pos_weapon_packet(char client, char wp_type, char wp_index, float x, float y, float z) {
	sc_packet_pos_weapon p_pos_weapon;

	p_pos_weapon.type = SC_POS_WEAPON;
	p_pos_weapon.size = sizeof(sc_packet_put_weapon);
	p_pos_weapon.weapon_type = wp_type;
	p_pos_weapon.weapon_index = wp_index;
	p_pos_weapon.x = x;
	p_pos_weapon.y = y;
	p_pos_weapon.z = z;

	//cout << "Weapon Pos : " << x << "," << y << "," << z << endl;

	sendPacket(client, &p_pos_weapon);
}

void send_remove_weapon_packet(char client, char wp_type, char wp_index) {
	sc_packet_remove_weapon p_remove_weapon;

	p_remove_weapon.type = SC_REMOVE_WEAPON;
	p_remove_weapon.size = sizeof(sc_packet_remove_weapon);
	p_remove_weapon.weapon_type = wp_type;
	p_remove_weapon.weapon_index = wp_index;

	sendPacket(client, &p_remove_weapon);
}

void send_pick_weapon_packet(char client, char id, char wp_type, char wp_index) {
	sc_packet_pick_weapon p_pick_weapon;
	p_pick_weapon.type = SC_PICK_WEAPON;
	p_pick_weapon.size = sizeof(sc_packet_pick_weapon);
	p_pick_weapon.id = clients[id].slot;
	p_pick_weapon.weapon_type = wp_type;
	p_pick_weapon.weapon_index = wp_index;

	sendPacket(client, &p_pick_weapon);
}

void send_unpick_weapon_packet(char client, char id, char wp_type, char wp_index, const PxVec3& pos) {
	sc_packet_unpick_weapon p_unpick_weapon;

	p_unpick_weapon.type = SC_UNPICK_WEAPON;
	p_unpick_weapon.size = sizeof(sc_packet_unpick_weapon);
	p_unpick_weapon.id = clients[id].slot;
	p_unpick_weapon.weapon_type = wp_type;
	p_unpick_weapon.weapon_index = wp_index;
	p_unpick_weapon.x = pos.x;
	p_unpick_weapon.y = pos.y;
	p_unpick_weapon.z = pos.z;

	sendPacket(client, &p_unpick_weapon);
}

void send_time_packet(char client, int timer)
{
	sc_packet_timer p_time;
	p_time.type = SC_TIMER;
	p_time.size = sizeof(sc_packet_timer);
	p_time.timer = timer;

	sendPacket(client, &p_time);
}

void send_patern_message_packet(char client_id, char patern)
{
	sc_packet_patern_message p_patern_mss;
	p_patern_mss.type = SC_PATERN_MESSAGE;
	p_patern_mss.size = sizeof(sc_packet_patern_message);
	p_patern_mss.patern = patern;

	sendPacket(client_id, &p_patern_mss);
}

void send_patern_packet(char client_id, char type) {
	sc_packet_patern p_patern;
	p_patern.type = type;
	p_patern.size = sizeof(sc_packet_patern);

	sendPacket(client_id, &p_patern);
}

void send_light_index_packet(char client_id, char index1, char index2) {
	sc_packet_light_index p_light_index;
	p_light_index.type = SC_LIGHT_INDEX;
	p_light_index.size = sizeof(sc_packet_light_index);
	p_light_index.index1 = index1;
	p_light_index.index2 = index2;

	sendPacket(client_id, &p_light_index);
}

void send_king_packet(char client_id, char king) {
	sc_packet_king p_king;
	p_king.type = SC_KING;
	p_king.size = sizeof(sc_packet_king);
	p_king.king = king;

	sendPacket(client_id, &p_king);
}

void send_king_off_packet(char client_id, char king) {
	sc_packet_king_off p_king_off;
	p_king_off.type = SC_KING_OFF;
	p_king_off.size = sizeof(sc_packet_king_off);
	p_king_off.king = king;

	sendPacket(client_id, &p_king_off);
}

void send_win_packet(char client_id) {
	sc_packet_win_lose_end p_win;
	p_win.type = SC_WIN;
	p_win.size = sizeof(sc_packet_win_lose_end);

	sendPacket(client_id, &p_win);
}

void send_lose_packet(char client_id) {
	sc_packet_win_lose_end p_lose;
	p_lose.type = SC_LOSE;
	p_lose.size = sizeof(sc_packet_win_lose_end);

	sendPacket(client_id, &p_lose);
}

void send_end_packet(char client_id) {
	sc_packet_win_lose_end p_end;
	p_end.type = SC_END;
	p_end.size = sizeof(sc_packet_win_lose_end);

	sendPacket(client_id, &p_end);
}

void send_teleport_packet(char client_id, char slot, PxVec3 pos) {
	sc_packet_teleport p_teleport;
	p_teleport.type = SC_TELEPORT;
	p_teleport.size = sizeof(sc_packet_teleport);
	p_teleport.id = slot;
	p_teleport.x = pos.x;
	p_teleport.y = pos.y;
	p_teleport.z = pos.z;

	sendPacket(client_id, &p_teleport);
}

void send_ch_speed_packet(char client_id, char speeder, char type) {
	sc_packet_ch_speed p_speed;
	p_speed.type = type;
	p_speed.size = sizeof(sc_packet_ch_speed);
	p_speed.id = clients[speeder].slot;

	sendPacket(client_id, &p_speed);
}

void send_room_setting_weapon(int room_num)
{
	room_l.lock();
	auto it = find(gRoom.begin(), gRoom.end(), room_num);
	room_l.unlock();

	for (int i = 0; i < MAX_ROOM_USER; ++i)
	{
		int client_id = it->clientNum[i];

		if (client_id != -1)
		{
			if (clients[client_id].connected == true)
			{
				for (int j = 0; j < RESPAWN_WEAPON_NUM; ++j)
				{
					char type = it->weapon_respawn[j].type;
					char index = it->weapon_respawn[j].index;
					const PxVec3 pos = it->weapon_list[type][index].pos;

					send_put_weapon_packet(client_id, type, index, pos);
				}
			}
		}
	}
	cout << "put weapon packet setting end\n";
}

void send_room_setting_player(int room_num)
{
	room_l.lock();
	auto it = find(gRoom.begin(), gRoom.end(), room_num);
	room_l.unlock();

	for (int i = 0; i < MAX_ROOM_USER; ++i)
	{
		// i : 현재 플레이어
		int client_id = it->clientNum[i];

		if (client_id != -1)
		{
			if (clients[client_id].connected == true)
			{
				for (int j = 0; j < MAX_ROOM_USER; ++j)
				{
					// j : 받을 플레이어
					int client_id2 = it->clientNum[j];
					if (client_id2 != -1)
					{
						if (clients[client_id2].connected == true)
						{
							//cout << client_id << ", " << j << endl;
							send_put_player_packet(client_id2, client_id);
						}
					}
				}
			}
		}
	}
	cout << "put player packet setting end\n";
}

void send_room_start_game(int room_num)
{
	room_l.lock();
	auto it = find(gRoom.begin(), gRoom.end(), room_num);
	room_l.unlock();

	for (int i = 0; i < MAX_ROOM_USER; ++i)
	{
		int client_id = it->clientNum[i];

		if (client_id != -1)
		{
			if (clients[client_id].connected == true)
			{
				send_start_game_packet(client_id);
			}
		}
	}
	cout << "[" << it->room_num << "] room game start\n";
}

void process_packet(char key, char *buffer)
{
	char Anim_Index;

	switch (buffer[1])
	{
	case CS_CONNECT:
	{
		cout << "[" << int(key) << "] Clients Login\n";

		cs_packet_connect *p_connect;
		p_connect = reinterpret_cast<cs_packet_connect*>(buffer);

		strcpy_s(clients[key].id, _countof(clients[key].id), p_connect->id);

		cout << clients[key].id << endl;

		lobby_l.lock();
		gLobby.insert(key);
		lobby_l.unlock();

		cout << "room num : " << gRoom.size() << endl;

		int count = 0;

		room_l.lock();
		for (auto it = gRoom.begin(); it != gRoom.end(); ++it)
		{
			if (count < 6)
			{
				cout << "send lobby : " << it->room_num << endl;
				send_room_info_packet(key, *it, count);
				count++;
			}
			else {
				break;
			}
		}

		room_l.unlock();
		break;
	}
	case CS_DISCONNECT:
	{
		cout << "[" << int(key) << "] Clients Disconnect\n";
		break;
	}

	case CS_UPDATE_ROOM:
	{
		cout << "Update Room\n";
		cout << gRoom.size() << endl;
		int count = 0;

		room_l.lock();
		for (auto it = gRoom.begin(); it != gRoom.end(); ++it)
		{
			if (count < 6)
			{
				send_room_info_packet(key, *it, count);
				count++;
			}
			else {
				break;
			}
		}
		room_l.unlock();

		break;
	}

	case CS_MAKE_ROOM:
	{
		cs_packet_make_room *p_make_room;
		p_make_room = reinterpret_cast<cs_packet_make_room*>(buffer);

		CRoom room;
		room.init(p_make_room->name, key, roomNum);

		clients[key].room_num = roomNum;
		clients[key].slot = 0;

		roomNum++;

		room_l.lock();
		gRoom.push_back(room);
		room_l.unlock();

		lobby_l.lock();
		gLobby.erase(key);
		lobby_l.unlock();

		send_room_datail_info_packet(key, key, 0, room.room_num, room.room_mode, room.room_map, room.max_num, 1);

		break;
	}

	case CS_ATTEND_ROOM:
	{
		cs_packet_attend_room *p_attend_room;
		p_attend_room = reinterpret_cast<cs_packet_attend_room*>(buffer);

		int room_num = p_attend_room->room_num;
		clients[key].room_num = room_num;

		room_l.lock();
		auto it = find(gRoom.begin(), gRoom.end(), room_num);

		if (it != gRoom.end()) {
			bool result = it->attend(key);
		}
		room_l.unlock();

		lobby_l.lock();
		gLobby.erase(key);
		lobby_l.unlock();

		char room_mode = it->room_mode;
		char room_map = it->room_map;
		char room_user = it->max_num;

		char my_slot;

		// 참가한 플레이어에게 자신을 제외한 클라의 정보를 전송
		for (int i = 0; i < MAX_ROOM_USER; ++i)
		{
			char host = 0;
			int client_id = it->clientNum[i];
			if (client_id != -1)
			{
				if (client_id != key)
				{
					if (i == it->host_num) host = 1;
					send_room_datail_info_packet(key, client_id, i, room_num, room_mode, room_map, room_user, host);
				}
				else {
					my_slot = i;
				}
			}
		}

		// 참가한 플레이어의 정보를 방 내부 클라(자신도 포함)에게 전송
		for (int i = 0; i < MAX_ROOM_USER; ++i)
		{
			int client_id = it->clientNum[i];
			if (client_id != -1)
			{
				send_room_datail_info_packet(client_id, key, my_slot, room_num, room_mode, room_map, room_user, 0);
			}
		}

		clients[key].slot = my_slot;
		break;
	}

	case CS_OPTION_ROOM:
	{
		cs_packet_room_option *p_room_option;
		p_room_option = reinterpret_cast<cs_packet_room_option*>(buffer);

		int room_num = p_room_option->room_num;
		char room_map = p_room_option->room_map;
		char room_mode = p_room_option->room_mode;
		char room_member = p_room_option->room_member;

		room_l.lock();
		auto it = find(gRoom.begin(), gRoom.end(), room_num);
		room_l.unlock();

		it->room_map = room_map;
		it->room_mode = room_mode;
		it->max_num = room_member;

		for (int i = 0; i < MAX_ROOM_USER; ++i)
		{
			int client_id = it->clientNum[i];
			if (client_id != -1)
			{
				if (clients[client_id].connected == true)
				{
					if (i != it->host_num)
					{
						send_room_option_packet(client_id, it->room_mode, it->room_map, it->max_num);
					}
				}
			}
		}
		break;
	}
	case CS_TEAM_CHANGE_ROOM:
	{
		cs_packet_room_team_change *p_team_change;
		p_team_change = reinterpret_cast<cs_packet_room_team_change*>(buffer);

		int room_num = p_team_change->room_num;

		room_l.lock();
		auto it = find(gRoom.begin(), gRoom.end(), room_num);
		room_l.unlock();

		int room_mode = it->room_mode;
		char room_map = it->room_map;
		char room_user = it->max_num;

		char oldslot = clients[key].slot;
		char newslot = -1;

		cout << "Old Slot : " << int(oldslot) << endl;

		int start = 0;
		if (oldslot < 4 && oldslot >= 0) start += 4;
		for (int i = start; i < start + (MAX_ROOM_USER / 2); ++i)
		{
			if (it->clientNum[i] == -1) {
				newslot = i;
				break;
			}
		}

		cout << "New Slot : " << int(newslot) << endl;

		char host = 0;

		if (newslot != -1)
		{
			clients[key].slot = newslot;
			if (it->host_num == oldslot) {
				it->host_num = newslot;
				host = 1;
			}
			for (int i = 0; i < MAX_ROOM_USER; ++i)
			{
				int client_id = it->clientNum[i];
				if (client_id != -1)
				{
					if (clients[client_id].connected == true)
					{
						send_room_detail_delete_packet(client_id, oldslot);
						send_room_datail_info_packet(client_id, key, newslot, room_num, room_mode, room_map, room_user, host);
					}
				}
			}
			it->clientNum[oldslot] = -1;
			it->clientNum[newslot] = key;
		}
		break;
	}
	case CS_ROOM_EXIT:
	{
		cs_packet_room_exit *p_room_exit;
		p_room_exit = reinterpret_cast<cs_packet_room_exit*>(buffer);

		int room_num = p_room_exit->room_num;

		room_l.lock();
		auto it = find(gRoom.begin(), gRoom.end(), room_num);
		room_l.unlock();

		char myslot = clients[key].slot;
		int room_mode = it->room_mode;
		char room_map = it->room_map;
		char room_user = it->max_num;

		//cout << "OLD HOST : " << int(it->host_num) << endl;

		if (it->host_num == myslot) 
		{
			if (it->current_num == 1)
			{
				room_l.lock();
				gRoom.erase(it);
				room_l.unlock();
				break;
			}
			else
			{
				// 방장 재적용
				it->clientNum[myslot] = -1;
				for (int i = 0; i < MAX_ROOM_USER; ++i)
				{
					int client_id = it->clientNum[i];
					if (client_id != -1)
					{
						if (clients[client_id].connected == true)
						{
							if (i != myslot) 
							{
								it->host_num = i;
								//cout << "NEW HOST : " << int(it->host_num) << endl;

								int host_id = it->clientNum[i];
								for (int j = 0; j < MAX_ROOM_USER; ++j)
								{
									int client_id = it->clientNum[j];
									if (client_id != -1)
									{
										if (clients[client_id].connected == true)
										{
											send_room_datail_info_packet(client_id, host_id, i, room_num, room_mode, room_map, room_user, 1);
											send_room_detail_delete_packet(client_id, myslot);
										}
									}
								}
							}
						}
					}
				}
				it->current_num -= 1;
			}
		}
		else
		{
			it->clientNum[myslot] = -1;
			for (int i = 0; i < MAX_ROOM_USER; ++i)
			{
				int client_id = it->clientNum[i];
				if (client_id != -1)
				{
					if (clients[client_id].connected == true)
					{
						send_room_detail_delete_packet(client_id, myslot);
					}
				}
			}
			it->current_num -= 1;
		}

		// EXIT 후 로비에서 받을 방들의 정보 Send
		int count = 0;

		room_l.lock();
		cout << gRoom.size() << endl;
		for (auto it = gRoom.begin(); it != gRoom.end(); ++it)
		{
			if (count < 6)
			{
				send_room_info_packet(key, *it, count);
				count++;
			}
			else {
				break;
			}
		}
		room_l.unlock();

		break;
	}
	case CS_START_ROOM:
	{
		cs_packet_start_room *p_start_room;
		p_start_room = reinterpret_cast<cs_packet_start_room*>(buffer);

		int room_num = p_start_room->room_num;

		room_l.lock();
		auto it = find(gRoom.begin(), gRoom.end(), room_num);
		room_l.unlock();

		// Physx에 맵 초기화
		it->start(it->room_map, gMapVertexs, gMapIndexs, gPhysx);

		it->registerRoom(&(*it));

		// 무기 초기화
		for (int i = 0; i < RESPAWN_WEAPON_NUM; ++i) {
			if (it->weapon_respawn[i].respawn_able == true) {

				it->weapon_respawn[i].respawn_able = false;

				int type = rand() % (MAX_WEAPON_TYPE - 1);
				//int type = (rand() % 2) + 2;

				it->weapon_respawn[i].type = type;

				for (int j = 0; j < MAX_WEAPON_NUM; ++j) {
					if (it->weapon_list[type][j].empty == true) {
						it->weapon_respawn[i].index = j;

						it->weapon_list[type][j].empty = false;
						it->weapon_list[type][j].owner = -1;
						if (it->room_map == MAP_Wehas)
							it->weapon_list[type][j].pos = WeaponInitPos_1[i];
						if (it->room_map == MAP_Oreo)
							it->weapon_list[type][j].pos = WeaponInitPos_2[i];
						if (it->room_map == MAP_Cake)
							it->weapon_list[type][j].pos = WeaponInitPos_3[i];
						it->weapon_list[type][j].respawn_index = i;

						break;
					}
				}
			}
		}

		// 플레이어 Physx Capsule 적용
		for (int i = 0; i < MAX_ROOM_USER; ++i)
		{
			int client_id = it->clientNum[i];
			if (client_id != -1)
			{
				if (clients[client_id].connected == true)
				{
					char slot = clients[client_id].slot;
					// 자신을 제외한 다른 클라도 게임 업로드 시작하도록 packet send
					send_start_load_room_packet(client_id);

					// Player Basic Setting
					clients[client_id].playerinfo = new CPlayer();

					if (it->room_map == MAP_Wehas)
						clients[client_id].playerinfo->setPosition(PlayerInitPos_1[slot]);
					if (it->room_map == MAP_Oreo)
						clients[client_id].playerinfo->setPosition(PlayerInitPos_2[slot]);
					if (it->room_map == MAP_Cake)
						clients[client_id].playerinfo->setPosition(PlayerInitPos_3[slot]);
					clients[client_id].playerinfo->setVelocity(PxVec3(0, 0, 0));
					clients[client_id].playerinfo->setLook(PxVec3(0, 0, 1));
					clients[client_id].playerinfo->setDashed(false);
					clients[client_id].playerinfo->setAniIndex(Anim::Idle);
					it->m_Scene->lockWrite();
					clients[client_id].playerinfo->setPlayerController(&(*it), gPhysx);
					clients[client_id].playerinfo->setTrigger(&(*it), gPhysx);
					it->m_Scene->unlockWrite();
					it->registerPlayer(clients[client_id].playerinfo, slot);

					// Player Team/King Setting
					if (it->room_mode == ROOM_MODE_KING || it->room_mode == ROOM_MODE_TEAM)
					{
						it->team_dead[slot] = true;
					}
				}
			}
		}

		cout << it->room_num << "Room : Map, Weapon, Player Init complete\n";

		break;
	}

	case CS_READY_ROOM:
	{
		break;
	}

	case CS_LOAD_COMPLETE:
	{
		cs_packet_load_complete *p_load_complete;
		p_load_complete = reinterpret_cast<cs_packet_load_complete*>(buffer);

		int room_num = clients[key].room_num;
		char skill = p_load_complete->skill;
		clients[key].playerinfo->m_skill = skill;

		room_l.lock();
		auto it = find(gRoom.begin(), gRoom.end(), room_num);
		room_l.unlock();

		for (int i = 0; i < MAX_ROOM_USER; ++i)
		{
			if (it->clientNum[i] == key)
			{
				it->load_complete[i] = true;
			}
		}

		break;
	}

	case CS_SETTING_COMPLETE:
	{
		int room_num = clients[key].room_num;

		room_l.lock();
		auto it = find(gRoom.begin(), gRoom.end(), room_num);
		room_l.unlock();

		for (int i = 0; i < MAX_ROOM_USER; ++i)
		{
			if (it->clientNum[i] == key)
			{
				it->setting_complete[i] = true;
			}
		}
		break;
	}

	case CS_MOVE:
	{
		cs_packet_move *p_move;
		p_move = reinterpret_cast<cs_packet_move*>(buffer);

		if (p_move->state == 0)
		{
			if (p_move->key == CS_UP) {  // forward
				clients[key].playerinfo->m_Vel.z -= 1.f;
			}
			else if (p_move->key == CS_DOWN) {   // backward
				clients[key].playerinfo->m_Vel.z += 1.f;
			}
			else if (p_move->key == CS_LEFT) {   // left
				clients[key].playerinfo->m_Vel.x += 1.f;
			}
			else if (p_move->key == CS_RIGHT) {   // right
				clients[key].playerinfo->m_Vel.x -= 1.f;
			}
		}
		else if (p_move->state == 1) {
			if (p_move->key == CS_UP) {  // forward
				clients[key].playerinfo->m_Vel.z += 1.f;
			}
			if (p_move->key == CS_DOWN) {   // backward
				clients[key].playerinfo->m_Vel.z -= 1.f;
			}
			if (p_move->key == CS_LEFT) {   // left
				clients[key].playerinfo->m_Vel.x -= 1.f;
			}
			if (p_move->key == CS_RIGHT) {   // right
				clients[key].playerinfo->m_Vel.x += 1.f;
			}
		}

		if (p_move->key == CS_DASH)
		{
			if (p_move->state == 1) {
				clients[key].playerinfo->m_dashed = true;
			}
			else if (p_move->state == 0) {
				clients[key].playerinfo->m_dashed = false;
			}
		}

		PxVec3 vel;

		vel = clients[key].playerinfo->m_Vel;


		if (vel.isZero()) {
			clients[key].playerinfo->setAniIndex(Anim::Idle);
		}
		else {
			if (clients[key].playerinfo->m_dashed) {
				clients[key].playerinfo->setAniIndex(Anim::Run);
			}
			else {
				clients[key].playerinfo->setAniIndex(Anim::Walk);
			}
			clients[key].playerinfo->setLook(clients[key].playerinfo->m_Vel.getNormalized());
		}

		//cout << "[" << int(key) << "] Clients Move => " << clients[key].playerinfo->m_Vel.x << ","
		//	<< clients[key].playerinfo->m_Vel.y << "," << clients[key].playerinfo->m_Vel.z << "\n";
		//cout << p_pos.vx << "," << p_pos.vy << "," << p_pos.vz << endl;

		// Move한 정보를 브로드캐스팅

		int room_num = clients[key].room_num;

		room_l.lock();
		auto it = find(gRoom.begin(), gRoom.end(), room_num);
		room_l.unlock();

		for (int i = 0; i < MAX_ROOM_USER; ++i)
		{
			int client_id = it->clientNum[i];

			if (client_id != -1)
			{
				if (clients[client_id].connected == true)
				{
					send_pos_packet(client_id, key);
				}
			}
		}
		break;
	}

	case CS_ATTACK:
	{
		cs_packet_anim *p_anim;
		p_anim = reinterpret_cast<cs_packet_anim*>(buffer);

		Anim_Index = clients[key].playerinfo->m_AniIndex;
		int room_num = clients[key].room_num;

		if (p_anim->key == CS_JUMP) {
			cout << "jump" << endl;

			PxVec3 jumpVel = clients[key].playerinfo->m_Vel;
			jumpVel = jumpVel.getNormalized();
			if (clients[key].playerinfo->m_dashed)
			{
				jumpVel *= 2;
			}

			clients[key].playerinfo->setAniIndex(Anim::Jump);
			clients[key].playerinfo->jumpstart();
			clients[key].playerinfo->setJumpVelocity(jumpVel);
			clients[key].playerinfo->setStatus(STATUS::JUMP);
		}

		if (p_anim->key == CS_GUARD) {
			cout << "guard" << endl;

			if (clients[key].playerinfo->weapon_type == -1 || clients[key].playerinfo->weapon_type == Weapon_cupcake
				|| clients[key].playerinfo->weapon_type == Weapon_King) {
				clients[key].playerinfo->setAniIndex(Anim::Guard);
			}
			else if (clients[key].playerinfo->weapon_type >= Weapon_Lollipop && clients[key].playerinfo->weapon_type <= Weapon_pepero) {
				clients[key].playerinfo->setAniIndex(Anim::Lollipop_Guard);
			}
			else if (clients[key].playerinfo->weapon_type == Weapon_chocolate) {
				clients[key].playerinfo->setAniIndex(Anim::Chocolate_Guard);
			}
			clients[key].playerinfo->setStatus(STATUS::DEFENSE);
		}

		if (p_anim->key == CS_GUARD_OFF) {
			cout << "guard off" << endl;
			clients[key].playerinfo->setAniIndex(Anim::Idle);
			clients[key].playerinfo->setStatus(STATUS::FREE);
		}

		if (p_anim->key == CS_WEAK) {
			if (clients[key].playerinfo->weapon_type == -1 || clients[key].playerinfo->weapon_type == Weapon_cupcake
				|| clients[key].playerinfo->weapon_type == Weapon_King)
			{
				cout << "Idle weak attack ";
				if (p_anim->count == 0) {
					clients[key].playerinfo->setAniIndex(Anim::Weak_Attack1);
					clients[key].playerinfo->attack_time = high_resolution_clock::now();			// 최초 공격 시작
					clients[key].playerinfo->attack_count = 1;
					add_timer(room_num, key, EV_FREE, clients[key].playerinfo->attack_time + 660ms, 1);
					add_timer(room_num, key, EV_HIT, clients[key].playerinfo->attack_time + 400ms, 0);
				}
				else if (p_anim->count == 1) {
					clients[key].playerinfo->setAniIndex(Anim::Weak_Attack2);
					clients[key].playerinfo->attack_count = 2;
					add_timer(room_num, key, EV_FREE, clients[key].playerinfo->attack_time + 1330ms, 2);
					add_timer(room_num, key, EV_HIT, clients[key].playerinfo->attack_time + 800ms, 0);
				}
				else if (p_anim->count == 2) {
					clients[key].playerinfo->setAniIndex(Anim::Weak_Attack3);
					clients[key].playerinfo->attack_count = 3;
					add_timer(room_num, key, EV_FREE, clients[key].playerinfo->attack_time + 1330ms, 3);
					add_timer(room_num, key, EV_HIT, clients[key].playerinfo->attack_time + 1160ms, 0);
				}
			}
			else if (clients[key].playerinfo->weapon_type >= Weapon_Lollipop && clients[key].playerinfo->weapon_type <= Weapon_pepero)
			{
				cout << "Lollipop weak attack ";
				if (p_anim->count == 0) {
					clients[key].playerinfo->setAniIndex(Anim::Lollipop_Attack1);
					clients[key].playerinfo->attack_time = high_resolution_clock::now();			// 최초 공격 시작
					clients[key].playerinfo->attack_count = 1;
					add_timer(room_num, key, EV_FREE, clients[key].playerinfo->attack_time + 660ms, 1);
					add_timer(room_num, key, EV_HIT, clients[key].playerinfo->attack_time + 500ms, 0);
				}
				else if (p_anim->count == 1) {
					clients[key].playerinfo->setAniIndex(Anim::Lollipop_Attack2);
					clients[key].playerinfo->attack_count = 2;
					add_timer(room_num, key, EV_FREE, clients[key].playerinfo->attack_time + 1330ms, 2);
					add_timer(room_num, key, EV_HIT, clients[key].playerinfo->attack_time + 1130ms, 0);
				}
			}
			else if (clients[key].playerinfo->weapon_type == Weapon_chocolate)
			{
				cout << "Chocolate weak attack ";
				if (p_anim->count == 0) {
					clients[key].playerinfo->setAniIndex(Anim::Chocolate_Attack);
					clients[key].playerinfo->attack_time = high_resolution_clock::now();			// 최초 공격 시작
					clients[key].playerinfo->attack_count = 1;
					add_timer(room_num, key, EV_FREE, clients[key].playerinfo->attack_time + 800ms, 1);
					add_timer(room_num, key, EV_HIT, clients[key].playerinfo->attack_time + 530ms, 0);
				}
			}
			else if (clients[key].playerinfo->weapon_type == Weapon_cupcake)
			{

			}
			clients[key].playerinfo->setStatus(STATUS::WEAK_ATTACK);
			cout << int(p_anim->count) << endl;
		}

		if (p_anim->key == CS_HARD) {
			if (clients[key].playerinfo->weapon_type == -1 || clients[key].playerinfo->weapon_type == Weapon_cupcake
				|| clients[key].playerinfo->weapon_type == Weapon_King)
			{
				cout << "Idle hard attack ";
				if (p_anim->count == 0) {
					clients[key].playerinfo->setAniIndex(Anim::Hard_Attack1);
					clients[key].playerinfo->attack_time = high_resolution_clock::now();
					clients[key].playerinfo->attack_count = 1;
					add_timer(room_num, key, EV_FREE, clients[key].playerinfo->attack_time + 660ms, 1);
					add_timer(room_num, key, EV_HIT, clients[key].playerinfo->attack_time + 500ms, 0);
				}
				else if (p_anim->count == 1) {
					clients[key].playerinfo->setAniIndex(Anim::Hard_Attack2);
					clients[key].playerinfo->attack_count = 2;
					add_timer(room_num, key, EV_FREE, clients[key].playerinfo->attack_time + 1330ms, 2);
					add_timer(room_num, key, EV_HIT, clients[key].playerinfo->attack_time + 1000ms, 0);
				}
			}
			else if (clients[key].playerinfo->weapon_type == Weapon_Lollipop)
			{
				cout << "Lollipop hard attack ";
				clients[key].playerinfo->setAniIndex(Anim::Lollipop_Hard_Attack);
				clients[key].playerinfo->attack_time = high_resolution_clock::now();
				clients[key].playerinfo->attack_count = 1;
				add_timer(room_num, key, EV_FREE, clients[key].playerinfo->attack_time + 1000ms, 1);
				add_timer(room_num, key, EV_HIT, clients[key].playerinfo->attack_time + 500ms, 0);
			}
			else if (clients[key].playerinfo->weapon_type == Weapon_chupachupse)
			{
				cout << "Candy hard attack ";
				clients[key].playerinfo->setAniIndex(Anim::Candy_Hard_Attack);
				clients[key].playerinfo->attack_time = high_resolution_clock::now();
				clients[key].playerinfo->attack_count = 1;
				add_timer(room_num, key, EV_FREE, clients[key].playerinfo->attack_time + 1000ms, 1);
				add_timer(room_num, key, EV_HIT, clients[key].playerinfo->attack_time + 600ms, 0);
			}
			else if (clients[key].playerinfo->weapon_type == Weapon_pepero)
			{
				cout << "Pepero hard attack ";
				clients[key].playerinfo->setAniIndex(Anim::Pepero_Hard_Attack1);
				clients[key].playerinfo->attack_time = high_resolution_clock::now();
				clients[key].playerinfo->attack_count = 1;
				add_timer(room_num, key, EV_FREE, clients[key].playerinfo->attack_time + 1000ms, 1);
				add_timer(room_num, key, EV_HIT, clients[key].playerinfo->attack_time + 500ms, 0);
			}
			else if (clients[key].playerinfo->weapon_type == Weapon_chocolate)
			{
				cout << "Chocolate hard attack ";
				clients[key].playerinfo->setAniIndex(Anim::Chocolate_Hard_Attack);
				clients[key].playerinfo->attack_time = high_resolution_clock::now();
				clients[key].playerinfo->attack_count = 1;
				add_timer(room_num, key, EV_FREE, clients[key].playerinfo->attack_time + 660ms, 1);
				add_timer(room_num, key, EV_HIT, clients[key].playerinfo->attack_time + 500ms, 0);
			}

			clients[key].playerinfo->setStatus(STATUS::HARD_ATTACK);
			cout << int(p_anim->count) << endl;
		}

		room_l.lock();
		auto it = find(gRoom.begin(), gRoom.end(), room_num);
		room_l.unlock();

		for (int i = 0; i < MAX_ROOM_USER; ++i)
		{
			int client_id = it->clientNum[i];

			if (client_id != -1)
			{
				if (clients[client_id].connected == true)
				{
					send_anim_packet(client_id, key);
				}
			}
		}
		break;
	}

	case CS_WEAPON:
	{
		cs_packet_weapon *p_weapon;
		p_weapon = reinterpret_cast<cs_packet_weapon*>(buffer);

		char type = p_weapon->weapon_type;
		char index = p_weapon->weapon_index;

		clients[key].playerinfo->weapon_type = type;
		clients[key].playerinfo->weapon_index = index;
		clients[key].playerinfo->setAniIndex(Anim::Pick_Up);

		int room_num = clients[key].room_num;

		//cout << int(key) << " Player Weapon Success : " << int(p_weapon->weapon_type) << ", " << int(p_weapon->weapon_index) << endl;;

		add_timer(room_num, key, EV_PICK, high_resolution_clock::now() + 660ms, 0);

		room_l.lock();
		auto it = find(gRoom.begin(), gRoom.end(), room_num);
		room_l.unlock();


		for (int i = 0; i < MAX_ROOM_USER; ++i)
		{
			int client_id = it->clientNum[i];

			if (client_id != -1)
			{
				if (clients[client_id].connected == true)
				{
					send_anim_packet(client_id, key);
				}
			}
		}
		break;
	}
	case CS_WEAPON_SKILL:
	{
		cs_packet_weapon_skill *p_weapon_skill;
		p_weapon_skill = reinterpret_cast<cs_packet_weapon_skill*>(buffer);

		int room_num = clients[key].room_num;

		int weapon_type = clients[key].playerinfo->weapon_type;

		// 롤리팝
		if (weapon_type == Weapon_Lollipop) {
			clients[key].playerinfo->setAniIndex(Anim::Lollipop_Skill);
			add_timer(room_num, key, EV_FREE, high_resolution_clock::now() + 1000ms, 0);
			add_timer(room_num, key, EV_WEAPON_SKILL, high_resolution_clock::now() + 760ms, Weapon_Lollipop);
			clients[key].playerinfo->setStatus(STATUS::SKILL_WEAPON_NO_MOVE);
		}
		// 츄파춥스
		else if (weapon_type == Weapon_chupachupse) {
			clients[key].playerinfo->setAniIndex(Anim::Candy_Skill);
			add_timer(room_num, key, EV_FREE, high_resolution_clock::now() + 3000ms, 0);
			add_timer(room_num, key, EV_WEAPON_SKILL, high_resolution_clock::now() + 600ms, Weapon_chupachupse);
			clients[key].playerinfo->setStatus(STATUS::SKILL_WEAPON_MOVE);
		}
		// 빼빼로
		else if (weapon_type == Weapon_pepero) {
			clients[key].playerinfo->setAniIndex(Anim::Pepero_Skill);
			add_timer(room_num, key, EV_FREE, high_resolution_clock::now() + 1000ms, 0);
			add_timer(room_num, key, EV_WEAPON_SKILL, high_resolution_clock::now() + 630ms, Weapon_pepero);
			clients[key].playerinfo->setStatus(STATUS::SKILL_WEAPON_NO_MOVE);
		}
		// 초콜릿
		else if (weapon_type == Weapon_chocolate) {
			clients[key].playerinfo->setAniIndex(Anim::Chocolate_Skill);
			add_timer(room_num, key, EV_FREE, high_resolution_clock::now() + 660ms, 0);
			add_timer(room_num, key, EV_WEAPON_SKILL, high_resolution_clock::now() + 360ms, Weapon_chocolate);
			clients[key].playerinfo->setStatus(STATUS::SKILL_WEAPON_NO_MOVE);
		}
		else if (weapon_type == Weapon_cupcake) {
			clients[key].playerinfo->setAniIndex(Anim::CupCake_Eat);
			add_timer(room_num, key, EV_FREE, high_resolution_clock::now() + 1330ms, 0);
			add_timer(room_num, key, EV_WEAPON_SKILL, high_resolution_clock::now() + 1260ms, Weapon_cupcake);
			add_timer(room_num, key, EV_CUPCAKE_REMOVE, high_resolution_clock::now() + 660ms);
			clients[key].playerinfo->setStatus(STATUS::SKILL_WEAPON_NO_MOVE);
		}

		room_l.lock();
		auto it = find(gRoom.begin(), gRoom.end(), room_num);
		room_l.unlock();

		for (int i = 0; i < MAX_ROOM_USER; ++i)
		{
			int client_id = it->clientNum[i];

			if (client_id != -1)
			{
				if (clients[client_id].connected == true)
				{
					send_anim_packet(client_id, key);
				}
			}
		}

		break;
	}
	case CS_CH_SKILL:
	{
		char skill = clients[key].playerinfo->m_skill;
		int room_num = clients[key].room_num;
		if (skill == Skill_Attack)
		{
			clients[key].playerinfo->m_attack = 5;
			add_timer(room_num, key, EV_CH_ATTACK_END, high_resolution_clock::now() + 10s);
			add_timer(room_num, key, EV_FREE, high_resolution_clock::now() + 660ms);
			clients[key].playerinfo->m_mp = 0;
			clients[key].playerinfo->setAniIndex(Anim::PowerUp);
			clients[key].playerinfo->setStatus(STATUS::CH_SKILL);

			int room_num = clients[key].room_num;

			room_l.lock();
			auto it = find(gRoom.begin(), gRoom.end(), room_num);
			room_l.unlock();

			for (int i = 0; i < MAX_ROOM_USER; ++i)
			{
				int client_id = it->clientNum[i];
				if (client_id != -1)
				{
					if (clients[client_id].connected == true)
					{
						send_heal_packet(client_id, key, clients[key].playerinfo->m_hp, clients[key].playerinfo->m_mp);
						send_anim_packet(client_id, key);
					}
				}
			}
		}
		else if (skill == Skill_Speed)
		{
			clients[key].playerinfo->speed_skill = 1.5f;
			add_timer(room_num, key, EV_CH_SPEED_END, high_resolution_clock::now() + 10s);
			add_timer(room_num, key, EV_FREE, high_resolution_clock::now() + 660ms);
			clients[key].playerinfo->m_mp = 0;
			clients[key].playerinfo->setAniIndex(Anim::PowerUp);
			clients[key].playerinfo->setStatus(STATUS::CH_SKILL);

			int room_num = clients[key].room_num;

			room_l.lock();
			auto it = find(gRoom.begin(), gRoom.end(), room_num);
			room_l.unlock();

			for (int i = 0; i < MAX_ROOM_USER; ++i)
			{
				int client_id = it->clientNum[i];
				if (client_id != -1)
				{
					if (clients[client_id].connected == true)
					{
						send_ch_speed_packet(client_id, key, SC_CH_SPEED_UP);
						send_heal_packet(client_id, key, clients[key].playerinfo->m_hp, clients[key].playerinfo->m_mp);
						send_anim_packet(client_id, key);
					}
				}
			}
		}
		else if (skill == Skill_Heal)
		{
			int hp = clients[key].playerinfo->m_hp;
			hp += 20;
			if (hp > 100) hp = 100;
			clients[key].playerinfo->m_hp = hp;
			add_timer(room_num, key, EV_FREE, high_resolution_clock::now() + 660ms);
			clients[key].playerinfo->m_mp = 0;
			clients[key].playerinfo->setAniIndex(Anim::PowerUp);
			clients[key].playerinfo->setStatus(STATUS::CH_SKILL);

			int room_num = clients[key].room_num;

			room_l.lock();
			auto it = find(gRoom.begin(), gRoom.end(), room_num);
			room_l.unlock();

			for (int i = 0; i < MAX_ROOM_USER; ++i)
			{
				int client_id = it->clientNum[i];
				if (client_id != -1)
				{
					if (clients[client_id].connected == true)
					{
						send_heal_packet(client_id, key, hp, clients[key].playerinfo->m_mp);
						send_anim_packet(client_id, key);
					}
				}
			}
		}
		break;
	}
	case CS_TELEPORT:
	{
		cs_packet_teleport *p_teleport;
		p_teleport = reinterpret_cast<cs_packet_teleport*>(buffer);

		int room_num = clients[key].room_num;

		room_l.lock();
		auto it = find(gRoom.begin(), gRoom.end(), room_num);
		room_l.unlock();

		int index = p_teleport->index;
		PxVec3 telepos = DoorTelepos[index];
		PxExtendedVec3 pos = PXtoPXEx(telepos);
		clients[key].playerinfo->m_PlayerController->setFootPosition(pos);

		for (int i = 0; i < MAX_ROOM_USER; ++i)
		{
			int client_id = it->clientNum[i];
			if (client_id != -1)
			{
				if (clients[client_id].connected == true)
				{
					send_pos_packet(client_id, key);
				}
			}
		}
		break;
	}
	}
}

void worker_thread()
{
	srand((unsigned int)time(NULL));

	while (true)
	{
		ULONG io_byte;
		ULONGLONG io_key;
		OVER_EX *over_ex;

		BOOL is_error = GetQueuedCompletionStatus(g_iocp, &io_byte, (PULONG_PTR)&io_key, reinterpret_cast<LPWSAOVERLAPPED *>(&over_ex), INFINITE);

		int key = static_cast<char>(io_key);

		if (is_error == FALSE) {
			clients[key].connected = false;
			////cout << "Error in GQCS\n";
			//cout << "[" << int(key) << "] Clients Disconnect\n";
			//sc_packet_remove p_remove;
			//p_remove.id = key;
			//p_remove.type = SC_REMOVE;
			//p_remove.size = sizeof(sc_packet_remove);

			// 접속 종료한 클라
			//for (int i = 0; i < MAX_USER; ++i)
			//{
			//	if (clients[i].connected == true)
			//		if (i != key)
			//			sendPacket(i, &p_remove);
			//		else {
			//			closesocket(clients[i].socket);
			//			clients[i].connected = false;
			//			gPhysx->m_Scene->lockWrite();
			//			clients[i].playerinfo->m_PlayerController->release();  clients[i].playerinfo->m_PlayerController = nullptr;
			//			clients[i].playerinfo->m_AttackTrigger->release(); clients[i].playerinfo->m_AttackTrigger = nullptr;
			//			gPhysx->m_Scene->unlockWrite();
			//			//gPhysx->removePlayer(i);
			//		}
			//}
			continue;
		}
		if (io_byte == 0)
		{
			clients[key].connected = false;
			//cout << "[" << int(key) << "] Clients Disconnect\n";
			//sc_packet_remove p_remove;
			//p_remove.id = key;
			//p_remove.type = SC_REMOVE;
			//p_remove.size = sizeof(sc_packet_remove);

			//for (int i = 0; i < MAX_USER; ++i)
			//{
			//	if (clients[i].connected == true)
			//		if (i != key)
			//			sendPacket(i, &p_remove);
			//		else {
			//			clients[i].connected = false;
			//			clients[i].playerinfo->m_PlayerController->release();
			//			clients[i].playerinfo->m_AttackTrigger->release();
			//		}
			//}
			continue;
		}

		if (true == over_ex->is_recv) {
			//cout << int(key) << endl;

			// 패킷 조립
			int rest = io_byte;
			char *wptr = over_ex->messageBuffer;

			int packet_size = 0;

			if (0 < clients[key].prev_size) {
				packet_size = int(clients[key].packetBuffer[0]);
			}

			while (0 < rest) {
				if (0 == packet_size)
					packet_size = wptr[0];

				int required = packet_size - clients[key].prev_size;

				if (required <= rest) {
					memcpy(clients[key].packetBuffer + clients[key].prev_size, wptr, required);
					process_packet(key, clients[key].packetBuffer);
					rest -= required;
					wptr += required;
					packet_size = 0;
					clients[key].prev_size = 0;
				}
				else {
					memcpy(clients[key].packetBuffer + clients[key].prev_size,
						wptr, rest);
					rest = 0;
					clients[key].prev_size += rest;
				}
			}
			do_recv(key);
		}
		else {
			//cout << "[" << int(key) << "] Clients Send Packet\n";
			delete over_ex;
		}
	}
}

void do_accept()
{
	// 1. 소켓생성  
	SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	// 마지막 인자로 비동기식이라는 것을 알려주는 WSA_FLAG_OVERLAPPED
	if (listenSocket == INVALID_SOCKET)
	{
		cout << "Error - Invalid socket\n";
		return;
	}

	// 서버정보 객체설정
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	// 2. 소켓설정
	if (::bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		printf("Error - Fail bind\n");
		// 6. 소켓종료
		closesocket(listenSocket);
		// Winsock End
		WSACleanup();
		return;
	}

	// 3. 수신대기열생성
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		printf("Error - Fail listen\n");
		// 6. 소켓종료
		closesocket(listenSocket);
		// Winsock End
		WSACleanup();
		return;
	}

	while (1)
	{
		SOCKADDR_IN clientAddr;
		ZeroMemory(&clientAddr, sizeof(SOCKADDR_IN));
		int addrLen = sizeof(SOCKADDR_IN);
		DWORD flags;

		SOCKET clientSocket = accept(listenSocket, (struct sockaddr *)&clientAddr, &addrLen);
		// listenSocket이 비동기여야 clientSocket도 비동기소켓으로 된다
		if (clientSocket == INVALID_SOCKET)
		{
			cout << "Error - Accept Failure\n";
			return;
		}

		int new_id = -1;
		for (int i = 0; i < MAX_USER; ++i) {
			if (!clients[i].connected) {
				new_id = i;
				break;
			}
		}

		if (-1 == new_id) {
			cout << "MAX USER overflow\n";
			continue;
		}

		clients[new_id].socket = clientSocket;
		clients[new_id].prev_size = 0;
		ZeroMemory(&clients[new_id].over_ex.overlapped, sizeof(clients[new_id].over_ex.overlapped));
		flags = 0;

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket), g_iocp, new_id, 0);

		clients[new_id].connected = true;

		cout << "[" << new_id << "] Client Connect" << endl;

		do_recv(new_id);
	}

	// 6-2. 리슨 소켓종료
	closesocket(listenSocket);

	// Winsock End
	WSACleanup();

	return;
}

void process_event(EVENT_ST &ev)
{
	switch (ev.type)
	{
		// 플레이어
	case EV_FREE:
	{
		cout << "FREE\n";

		if (clients[ev.id].playerinfo->attack_count != ev.attack_count)
			break;

		clients[ev.id].playerinfo->setStatus(STATUS::FREE);
		clients[ev.id].playerinfo->attack_count = 0;
		break;
	}
	case EV_HIT:
	{
		if (clients[ev.id].playerinfo->m_AttackTrigger)
		{
			clients[ev.id].playerinfo->m_AttackTrigger->setGlobalPose(PxTransform(100, 100, 100));

			float hit_dist = 10.f;		// 기본, 컵케이크, 방패

			int type = clients[ev.id].playerinfo->weapon_type;
			if (type == Weapon_Lollipop || type == Weapon_chupachupse || type == Weapon_King) {
				hit_dist = 20.f;
			}
			else if (type == Weapon_pepero) {
				hit_dist = 30.f;
			}

			PxTransform triggerpos(PxVec3(0, 0, 0));
			PxExtendedVec3 playerpos = clients[ev.id].playerinfo->m_PlayerController->getPosition();

			PxVec3 look = clients[ev.id].playerinfo->m_Look;

			triggerpos.p.x = playerpos.x + (look.x * hit_dist);
			triggerpos.p.y = playerpos.y + (look.y * hit_dist);
			triggerpos.p.z = playerpos.z + (look.z * hit_dist);

			clients[ev.id].playerinfo->m_AttackTrigger->setGlobalPose(triggerpos);
		}
		break;
	}
	case EV_PICK:
	{
		char wp_index = clients[ev.id].playerinfo->weapon_index;
		char wp_type = clients[ev.id].playerinfo->weapon_type;

		int room_num = clients[ev.id].room_num;

		room_l.lock();
		auto it = find(gRoom.begin(), gRoom.end(), room_num);
		room_l.unlock();

		int my_slot = clients[ev.id].slot;

		if (it->weapon_list[wp_type][wp_index].owner == -1)
		{
			it->weapon_list[wp_type][wp_index].SetOwner(my_slot);
			if (wp_type != Weapon_cupcake)
			{
				char index = it->weapon_list[wp_type][wp_index].respawn_index;
				it->weapon_respawn[index].respawn_able = true;
			}

			for (int i = 0; i < MAX_ROOM_USER; ++i)
			{
				int client_id = it->clientNum[i];

				if (client_id != -1)
				{
					if (clients[client_id].connected == true)
					{
						send_pick_weapon_packet(client_id, ev.id, wp_type, wp_index);
					}
				}
			}
		}
		else {
			if (it->weapon_list[wp_type][wp_index].owner != my_slot)
			{
				clients[ev.id].playerinfo->weapon_index = -1;
				clients[ev.id].playerinfo->weapon_type = -1;
			}
		}
		break;
	}
	case EV_WEAPON_SKILL:
	{
		int room_num = clients[ev.id].room_num;
		char weapon_type = ev.attack_count;
		char weapon_index = clients[ev.id].playerinfo->weapon_index;
		char owner = clients[ev.id].slot;

		room_l.lock();
		auto it = find(gRoom.begin(), gRoom.end(), room_num);
		room_l.unlock();

		if (weapon_type == Weapon_Lollipop) {

			// 롤리팝 스킬
			char order = it->trigger_order;

			Skill_Actor *s_lollipop = new Skill_Actor(weapon_type, weapon_index, owner, order);

			it->trigger_order += 1;

			PxVec3 look = clients[ev.id].playerinfo->m_Look.getNormalized();
			PxVec3 pos = clients[ev.id].playerinfo->m_Pos;
			pos += look * Lollipop_Pos_Gap;

			it->weapon_list[weapon_type][weapon_index].pos = pos;

			//it->m_pPhysx->m_Scene->lockWrite();
			s_lollipop->skillTrigger = it->getSphereTrigger(pos, Lollipop_Radius, Lollipop_Trigger, order, gPhysx);
			//it->m_pPhysx->m_Scene->unlockWrite();

			it->m_skillTrigger.push_back(*s_lollipop);

			clients[ev.id].playerinfo->weapon_type = -1;

			for (int i = 0; i < MAX_ROOM_USER; ++i)
			{
				int client_id = it->clientNum[i];

				if (client_id != -1)
				{
					if (clients[client_id].connected == true)
					{
						send_unpick_weapon_packet(client_id, ev.id, weapon_type, weapon_index, pos);
					}
				}
			}

			add_timer(room_num, room_num + ROOM_TIMER_START, EV_LOLLIPOP_HEAL, high_resolution_clock::now() + 500ms, order);
			add_timer(room_num, room_num + ROOM_TIMER_START, EV_WEAPON_REMOVE, high_resolution_clock::now() + 5000ms, order);
		}
		else if (weapon_type == Weapon_chupachupse) {

			// 츄파춥스 스킬
			char order = it->trigger_order;

			Skill_Actor *s_candy = new Skill_Actor(weapon_type, weapon_index, owner, order);

			it->trigger_order += 1;

			PxVec3 pos = clients[ev.id].playerinfo->m_Pos;
			pos.y += CH_HALF_HEIGHT;

			//it->m_pPhysx->m_Scene->lockWrite();
			s_candy->skillTrigger = it->getSphereTrigger(pos, Candy_Radius, Candy_Trigger, order, gPhysx);
			//it->m_pPhysx->m_Scene->unlockWrite();

			it->m_skillTrigger.push_back(*s_candy);

			add_timer(room_num, room_num + ROOM_TIMER_START, EV_CANDY_MOVE, high_resolution_clock::now() + 16ms, order);
			add_timer(room_num, room_num + ROOM_TIMER_START, EV_WEAPON_REMOVE, high_resolution_clock::now() + 1860ms, order);
		}
		else if (weapon_type == Weapon_pepero) {

			// 빼빼로 투창 스킬
			PxVec3 look = clients[ev.id].playerinfo->m_Look;
			cout << "player look : " << look.x << ", " << look.y << ", " << look.z << endl;
			look = look.getNormalized();

			char order = it->trigger_order;

			Skill_Actor *s_pepero = new Skill_Actor(weapon_type, weapon_index, owner, order);

			it->trigger_order += 1;

			PxVec3 pos = clients[ev.id].playerinfo->m_Pos;
			pos += look * Pepero_Trigger_Len;
			pos.y += CH_HALF_HEIGHT;

			PxVec3 weapon_look = look;
			if (weapon_look == PxVec3(-1, 0, 0)) {
				weapon_look.x *= -1.f;
			}

			//cout << "weapon pos : " << pos.x << ", " << pos.y << ", " << pos.z << endl;

			//it->m_pPhysx->m_Scene->lockWrite();
			s_pepero->skillTrigger = it->getRotateBoxTrigger(pos, weapon_look,
				PxVec3(Pepero_Length, Pepero_Width, Pepero_Width), Pepero_Trigger, order, gPhysx);
			//it->m_pPhysx->m_Scene->unlockWrite();

			s_pepero->vel = look * Pepero_Vel;
			s_pepero->look = look;

			it->m_skillTrigger.push_back(*s_pepero);

			clients[ev.id].playerinfo->weapon_type = -1;

			pos -= look * Pepero_Pos_Gap;

			for (int i = 0; i < MAX_ROOM_USER; ++i)
			{
				int client_id = it->clientNum[i];

				if (client_id != -1)
				{
					if (clients[client_id].connected == true)
					{
						send_unpick_weapon_packet(client_id, ev.id, weapon_type, weapon_index, pos);
					}
				}
			}

			add_timer(room_num, room_num + ROOM_TIMER_START, EV_PEPERO_MOVE, high_resolution_clock::now() + 16ms, order);
			add_timer(room_num, room_num + ROOM_TIMER_START, EV_WEAPON_REMOVE, high_resolution_clock::now() + 5000ms, order);
		}
		else if (weapon_type == Weapon_chocolate) {

			// 빼빼로 투창 스킬
			PxVec3 look = clients[ev.id].playerinfo->m_Look;
			//look = PxVec3(1, 0, 0);

			cout << "player look : " << look.x << ", " << look.y << ", " << look.z << endl;
			look = look.getNormalized();

			char order = it->trigger_order;

			Skill_Actor *s_choco = new Skill_Actor(weapon_type, weapon_index, owner, order);

			it->trigger_order += 1;

			PxVec3 pos = clients[ev.id].playerinfo->m_Pos;
			PxVec3 size(Chocolate_Depth, Chocolate_Height, Chocolate_Width);
			size *= Chocolate_Scale;

			pos += look * Chocolate_Len;
			pos.y += size.y;

			PxVec3 weapon_look = look;
			if (weapon_look == PxVec3(-1, 0, 0)) {
				weapon_look.x *= -1.f;
			}

			cout << "weapon pos : " << pos.x << ", " << pos.y << ", " << pos.z << endl;

			//it->m_pPhysx->m_Scene->lockWrite();
			s_choco->skillTrigger = it->getRotateBox(pos, weapon_look, size, gPhysx);
			//it->m_pPhysx->m_Scene->unlockWrite();

			it->m_skillTrigger.push_back(*s_choco);

			//pos -= look * Pepero_Pos_Gap;

			clients[ev.id].playerinfo->weapon_type = -1;

			for (int i = 0; i < MAX_ROOM_USER; ++i)
			{
				int client_id = it->clientNum[i];

				if (client_id != -1)
				{
					if (clients[client_id].connected == true)
					{
						send_unpick_weapon_packet(client_id, ev.id, weapon_type, weapon_index, pos);
					}
				}
			}

			add_timer(room_num, room_num + ROOM_TIMER_START, EV_WEAPON_REMOVE, high_resolution_clock::now() + 5000ms, order);
		}
		else if (weapon_type == Weapon_cupcake)
		{
			// 컵케이크 스킬 : 커진다.

			clients[ev.id].playerinfo->m_PlayerController->setRadius(CH_CAPSULE_RADIUS * 2.f);
			clients[ev.id].playerinfo->m_PlayerController->setHeight(CH_CAPSULE_HEIGHT * 2.f);
			PxExtendedVec3 pos = clients[ev.id].playerinfo->m_PlayerController->getPosition();
			pos.y += 17.5f;
			clients[ev.id].playerinfo->m_PlayerController->setPosition(pos);

			clients[ev.id].playerinfo->weapon_type = Weapon_King;

			for (int i = 0; i < MAX_ROOM_USER; ++i)
			{
				int client_id = it->clientNum[i];

				if (client_id != -1)
				{
					if (clients[client_id].connected == true)
					{
						send_king_packet(client_id, owner);
					}
				}
			}

			add_timer(room_num, ev.id, EV_KING_OFF, high_resolution_clock::now() + 10000ms);
		}
		break;
	}
	case EV_KING_OFF:
	{
		int room_num = clients[ev.id].room_num;
		char slot = clients[ev.id].slot;

		room_l.lock();
		auto it = find(gRoom.begin(), gRoom.end(), room_num);
		room_l.unlock();

		clients[ev.id].playerinfo->m_PlayerController->setRadius(CH_CAPSULE_RADIUS);
		clients[ev.id].playerinfo->m_PlayerController->setHeight(CH_CAPSULE_HEIGHT);
		PxExtendedVec3 pos = clients[ev.id].playerinfo->m_PlayerController->getPosition();
		pos.y -= 17.5f;
		clients[ev.id].playerinfo->m_PlayerController->setPosition(pos);

		clients[ev.id].playerinfo->weapon_type = -1;

		for (int i = 0; i < MAX_ROOM_USER; ++i)
		{
			int client_id = it->clientNum[i];

			if (client_id != -1)
			{
				if (clients[client_id].connected == true)
				{
					send_king_off_packet(client_id, slot);
				}
			}
		}
		break;
	}
	case EV_LOLLIPOP_HEAL:
	{
		int room_num = ev.id - ROOM_TIMER_START;
		char order = ev.attack_count;

		room_l.lock();
		auto it = find(gRoom.begin(), gRoom.end(), room_num);
		room_l.unlock();

		auto it2 = find(it->m_skillTrigger.begin(), it->m_skillTrigger.end(), order);			//order

		if (it2 != it->m_skillTrigger.end())
		{
			for (int i = 0; i < MAX_ROOM_USER; ++i)
			{
				int client_id = it->clientNum[i];
				if (client_id != -1)
				{
					if (clients[client_id].connected == true)
					{
						if (clients[client_id].playerinfo->lollipop_heal == true)
						{
							cout << "Heal~~~~~ : " << client_id << endl;
							int hp = clients[client_id].playerinfo->m_hp + 2;
							if (hp > MAX_HP) hp = MAX_HP;
							clients[client_id].playerinfo->setHP(hp);
							
							for (int j = 0; j < MAX_ROOM_USER; ++j)
							{
								int client_id2 = it->clientNum[j];

								if (client_id2 != -1)
								{
									if (clients[client_id2].connected == true)
									{
										send_heal_packet(client_id2, client_id, hp, clients[client_id].playerinfo->m_mp);
									}
								}
							}
						}
					}
				}
			}
			add_timer(room_num, room_num + ROOM_TIMER_START, EV_LOLLIPOP_HEAL, high_resolution_clock::now() + 500ms, order);
		}
		break;
	}
	case EV_CANDY_MOVE:
	{
		int room_num = ev.id - ROOM_TIMER_START;
		char order = ev.attack_count;

		room_l.lock();
		auto it = find(gRoom.begin(), gRoom.end(), room_num);
		room_l.unlock();

		auto it2 = find(it->m_skillTrigger.begin(), it->m_skillTrigger.end(), order);			//order

		if (it2 != it->m_skillTrigger.end())
		{
			int owner = it->clientNum[it2->owner];
			PxVec3 owner_pos = clients[owner].playerinfo->m_Pos;
			it2->skillTrigger->setGlobalPose(PxTransform(owner_pos));

			add_timer(room_num, room_num + ROOM_TIMER_START, EV_CANDY_MOVE, high_resolution_clock::now() + 16ms, order);
		}
		break;
	}
	case EV_PEPERO_MOVE:
	{
		int room_num = ev.id - ROOM_TIMER_START;
		char order = ev.attack_count;

		room_l.lock();
		auto it = find(gRoom.begin(), gRoom.end(), room_num);
		room_l.unlock();

		auto it2 = find(it->m_skillTrigger.begin(), it->m_skillTrigger.end(), order);			//order

		if (it2 != it->m_skillTrigger.end())
		{
			char type = it2->type;
			char index = it2->index;
			PxVec3 vel = it2->vel;
			PxVec3 look = it2->look;

			//cout << "vel : " << vel.x << "," << vel.y << "," << vel.z << endl;

			PxTransform trans = it2->skillTrigger->getGlobalPose();

			trans.p += vel * gGameTimer.GetTimeElapsed();

			it2->skillTrigger->setGlobalPose(trans);

			trans.p -= look * Pepero_Pos_Gap;

			//cout << "send weapon pos\n";
			for (int i = 0; i < MAX_ROOM_USER; ++i)
			{
				int client_id = it->clientNum[i];

				if (client_id != -1)
				{
					if (clients[client_id].connected == true)
					{
						send_pos_weapon_packet(client_id, type, index, trans.p.x, trans.p.y, trans.p.z);
					}
				}
			}
			add_timer(room_num, room_num + ROOM_TIMER_START, EV_PEPERO_MOVE, high_resolution_clock::now() + 16ms, order);
		}
		break;
	}
	case EV_WEAPON_REMOVE:
	{
		int room_num = ev.id - ROOM_TIMER_START;

		room_l.lock();
		auto it = find(gRoom.begin(), gRoom.end(), room_num);
		room_l.unlock();

		cout << int(ev.attack_count) << "번 무기를 없애라!!\n";

		auto it2 = find(it->m_skillTrigger.begin(), it->m_skillTrigger.end(), ev.attack_count);			//order

		if (it2 != it->m_skillTrigger.end())
		{
			char type = it2->type;
			char index = it2->index;
			char slot = it2->owner;

			it->weapon_list[type][index].init();
			it2->skillTrigger->release();

			if (type == Weapon_chupachupse) {
				int id = it->clientNum[slot];
				clients[id].playerinfo->weapon_type = -1;
			}
			if (type == Weapon_cupcake) {
				int id = it->clientNum[slot];
				clients[id].playerinfo->weapon_type = -1;
			}

			for (int i = 0; i < MAX_ROOM_USER; ++i)
			{
				int client_id = it->clientNum[i];

				if (client_id != -1)
				{
					if (clients[client_id].connected == true)
					{
						if (type == Weapon_chupachupse) {
							PxVec3 pos(0, 0, 0);
							send_unpick_weapon_packet(client_id, it->clientNum[slot], type, index, pos);
						}
						if (type == Weapon_Lollipop) {
							clients[client_id].playerinfo->setLollipopHeal(false);
						}
						send_remove_weapon_packet(client_id, type, index);
					}
				}
			}
			it->m_skillTrigger.erase(it2);
		}
		break;
	}
	case EV_CUPCAKE_REMOVE:
	{
		int room_num = clients[ev.id].room_num;

		room_l.lock();
		auto it = find(gRoom.begin(), gRoom.end(), room_num);
		room_l.unlock();

		for (int i = 0; i < MAX_ROOM_USER; ++i)
		{
			int client_id = it->clientNum[i];

			if (client_id != -1)
			{
				if (clients[client_id].connected == true)
				{
					send_unpick_weapon_packet(client_id, ev.id, Weapon_cupcake, 0, PxVec3(1000, 1000, 1000));
				}
			}
		}
		break;
	}
	// 심판
	case EV_RFR_WEAPON:
	{
		int room_num = ev.id - ROOM_TIMER_START;

		room_l.lock();
		auto it = find(gRoom.begin(), gRoom.end(), room_num);
		room_l.unlock();

		for (int i = 0; i < MAX_ROOM_USER; ++i) {
			int client_id = it->clientNum[i];
			if (client_id != -1)
			{
				if (clients[client_id].connected == true)
				{
					send_patern_message_packet(client_id, EV_RFR_WEAPON);
				}
			}
		}

		for (int i = 0; i < RESPAWN_WEAPON_NUM; ++i) {
			if (it->weapon_respawn[i].respawn_able == true) {

				it->weapon_respawn[i].respawn_able = false;

				//int type = rand() % (MAX_WEAPON_TYPE - 1);
				int type = (rand() % 2) + 2;

				it->weapon_respawn[i].type = type;

				for (int j = 0; j < MAX_WEAPON_NUM; ++j) {
					if (it->weapon_list[type][j].empty == true) {
						char index = j;
						PxVec3 pos;
						if (it->room_map == MAP_Wehas)
							pos = WeaponInitPos_1[i];
						//if (it->room_map == MAP_Oreo)
						//	pos = WeaponInitPos_2[i];
						if (it->room_map == MAP_Cake)
							pos = WeaponInitPos_3[i];


						it->weapon_respawn[i].index = index;
						it->weapon_list[type][j].empty = false;
						it->weapon_list[type][j].owner = -1;
						it->weapon_list[type][j].pos = pos;
						it->weapon_list[type][j].respawn_index = i;


						for (int k = 0; k < MAX_ROOM_USER; ++k) {
							int client_id = it->clientNum[k];
							if (client_id != -1)
							{
								if (clients[client_id].connected == true)
								{
									send_put_weapon_packet(client_id, type, j, pos);
								}
							}
						}

						break;
					}
				}
			}
		}
		break;
	}
	case EV_RFR_FOG:
	{
		int room_num = ev.id - ROOM_TIMER_START;

		room_l.lock();
		auto it = find(gRoom.begin(), gRoom.end(), room_num);
		room_l.unlock();

		for (int i = 0; i < MAX_ROOM_USER; ++i)
		{
			int client_id = it->clientNum[i];

			if (client_id != -1)
			{
				if (clients[client_id].connected == true)
				{
					send_patern_packet(client_id, SC_FOG);
				}
			}
		}

		add_timer(room_num, room_num + ROOM_TIMER_START, EV_RFR_FOG_END, high_resolution_clock::now() + 10s, 0);
		break;
	}
	case EV_RFR_FOG_END:
	{
		int room_num = ev.id - ROOM_TIMER_START;

		room_l.lock();
		auto it = find(gRoom.begin(), gRoom.end(), room_num);
		room_l.unlock();

		for (int i = 0; i < MAX_ROOM_USER; ++i)
		{
			int client_id = it->clientNum[i];

			if (client_id != -1)
			{
				if (clients[client_id].connected == true)
				{
					send_patern_packet(client_id, SC_FOG_END);
				}
			}
		}
		break;
	}
	case EV_RFR_FEVER:
	{
		int room_num = ev.id - ROOM_TIMER_START;

		room_l.lock();
		auto it = find(gRoom.begin(), gRoom.end(), room_num);
		room_l.unlock();

		it->fever = true;

		for (int i = 0; i < MAX_ROOM_USER; ++i)
		{
			int client_id = it->clientNum[i];

			if (client_id != -1)
			{
				if (clients[client_id].connected == true)
				{
					send_patern_packet(client_id, SC_FEVER);
				}
			}
		}

		add_timer(room_num, room_num + ROOM_TIMER_START, EV_RFR_FEVER_END, high_resolution_clock::now() + 10s, 0);
		break;
	}
	case EV_RFR_FEVER_END:
	{
		int room_num = ev.id - ROOM_TIMER_START;

		room_l.lock();
		auto it = find(gRoom.begin(), gRoom.end(), room_num);
		room_l.unlock();

		it->fever = false;

		for (int i = 0; i < MAX_ROOM_USER; ++i)
		{
			int client_id = it->clientNum[i];

			if (client_id != -1)
			{
				if (clients[client_id].connected == true)
				{
					send_patern_packet(client_id, SC_FEVER_END);
				}
			}
		}
		break;
	}
	case EV_RFR_LIGHTNING:
	{
		int room_num = ev.id - ROOM_TIMER_START;

		add_timer(room_num, room_num + ROOM_TIMER_START, EV_RFR_LIGHTNING_END, high_resolution_clock::now() + 10s, 0);
		break;
	}
	case EV_RFR_LIGHTNING_END:
	{
		int room_num = ev.id - ROOM_TIMER_START;

		room_l.lock();
		auto it = find(gRoom.begin(), gRoom.end(), room_num);
		room_l.unlock();

		it->lighting = false;

		for (int i = 0; i < MAX_ROOM_USER; ++i)
		{
			int client_id = it->clientNum[i];

			if (client_id != -1)
			{
				if (clients[client_id].connected == true)
				{
					send_patern_packet(client_id, SC_LIGHTNING_END);
				}
			}
		}
		break;
	}
	case EV_RFR_LIGHTNING_INDEX:
	{
		int room_num = ev.id - ROOM_TIMER_START;

		room_l.lock();
		auto it = find(gRoom.begin(), gRoom.end(), room_num);
		room_l.unlock();

		if (it->lighting == true)
		{
			if (it->light_count < 5)
			{
				//int index1 = rand() % 5;
				//int index2 = (rand() % 5) + 5;
				int index1 = 0;
				int index2 = 5;
				PxVec3 pos1;
				PxVec3 pos2;

				if (it->room_map == MAP_Wehas || it->room_map == MAP_Oreo)
				{
					pos1 = LightningInitPos_12[index1];
					pos2 = LightningInitPos_12[index2];
				}
				if (it->room_map == MAP_Cake)
				{
					pos1 = PlayerInitPos_3[index1];
					pos2 = PlayerInitPos_3[index2];
				}

				Skill_Actor *s_light1 = new Skill_Actor(0, 0, 0, Lightning_Type);
				Skill_Actor *s_light2 = new Skill_Actor(0, 0, 0, Lightning_Type);

				PxVec3 size(Lightning_Width, Lightning_Height, Lightning_Width);

				//it->m_pPhysx->m_Scene->lockWrite();
				s_light1->skillTrigger = it->getBoxTrigger(pos1, size, Light_Trigger, gPhysx);
				s_light2->skillTrigger = it->getBoxTrigger(pos2, size, Light_Trigger, gPhysx);
				//it->m_pPhysx->m_Scene->unlockWrite();

				it->m_skillTrigger.push_back(*s_light1);
				it->m_skillTrigger.push_back(*s_light2);

				for (int i = 0; i < MAX_ROOM_USER; ++i)
				{
					int client_id = it->clientNum[i];

					if (client_id != -1)
					{
						if (clients[client_id].connected == true)
						{
							send_light_index_packet(client_id, index1, index2);
						}
					}
				}
				it->light_count += 1;
				//cout << "count : " << int(it->light_count) << endl;

				add_timer(room_num, room_num + ROOM_TIMER_START, EV_RFR_LIGHTNING_INDEX, high_resolution_clock::now() + 2s, 0);
				add_timer(room_num, room_num + ROOM_TIMER_START, EV_RFR_LIGHTNING_DELETE, high_resolution_clock::now() + 1s, 0);
			}
		}

		break;
	}
	case EV_RFR_LIGHTNING_DELETE:
	{
		int room_num = ev.id - ROOM_TIMER_START;

		room_l.lock();
		auto it = find(gRoom.begin(), gRoom.end(), room_num);
		room_l.unlock();

		auto it2 = find(it->m_skillTrigger.begin(), it->m_skillTrigger.end(), Lightning_Type);			//order

		if (it2 != it->m_skillTrigger.end())
		{
			it2->skillTrigger->release();
			it->m_skillTrigger.erase(it2);
		}

		auto it3 = find(it->m_skillTrigger.begin(), it->m_skillTrigger.end(), Lightning_Type);			//order

		if (it3 != it->m_skillTrigger.end())
		{
			it3->skillTrigger->release();
			it->m_skillTrigger.erase(it3);
		}
		break;
	}
	case EV_RFR_SLIME:
	{
		int room_num = ev.id - ROOM_TIMER_START;

		room_l.lock();
		auto it = find(gRoom.begin(), gRoom.end(), room_num);
		room_l.unlock();

		PxVec3 pos;
		if (it->room_map == MAP_Wehas)
		{
			pos = PxVec3(0.f, 15.f, 0.f);
		}
		else if (it->room_map == MAP_Cake)
		{
			pos = PxVec3(0.f, 20.f, 30.f);
		}

		for (int i = 0; i < MAX_ROOM_USER; ++i)
		{
			int client_id = it->clientNum[i];

			if (client_id != -1)
			{
				if (clients[client_id].connected == true)
				{
					send_put_weapon_packet(client_id, Weapon_cupcake, 0, pos);
				}
			}
		}
		break;
	}
	case EV_CH_ATTACK_END:
	{
		clients[ev.id].playerinfo->m_attack = 0;
		break;
	}
	case EV_CH_SPEED_END:
	{
		clients[ev.id].playerinfo->speed_skill = 1.f;
		int room_num = clients[ev.id].room_num;

		room_l.lock();
		auto it = find(gRoom.begin(), gRoom.end(), room_num);
		room_l.unlock();

		for (int i = 0; i < MAX_ROOM_USER; ++i)
		{
			int client_id = it->clientNum[i];

			if (client_id != -1)
			{
				if (clients[client_id].connected == true)
				{
					send_ch_speed_packet(client_id, ev.id, SC_CH_SPEED_RESET);
				}
			}
		}
		break;
	}
	case EV_TIME:
	{
		int room_num = ev.id - ROOM_TIMER_START;

		room_l.lock();
		auto it = find(gRoom.begin(), gRoom.end(), room_num);
		room_l.unlock();

		int timer = it->timer;
		timer -= 1;
		it->timer = timer;

		for (int i = 0; i < MAX_ROOM_USER; ++i)
		{
			int client_id = it->clientNum[i];

			if (client_id != -1)
			{
				if (clients[client_id].connected == true)
				{
					send_time_packet(client_id, timer);
				}
			}
		}

		int timer_event = timer % MINUTE;

		if (timer_event == Rfr_Init) {
			// 심판 패턴 정하기
			char count = it->referee.patern_count;
			char patern;
			count += 1;

			if (count == 1)
			{
				patern = Rfr_Start_Num;
			}
			else if(count == 2){
				patern = Rfr_Start_Num + 1;
			}
			else if (count == 3) {
				if (it->room_map != MAP_Oreo)
					patern = EV_RFR_SLIME;	// 12. 컵케이크
				else
					patern = (rand() % 3) + Rfr_Start_Num;
			}
			else if (count == 4) {
				patern = Rfr_Start_Num + 2;
			}
			else if (count == 5)
			{
				patern = (rand() % 3) + Rfr_Start_Num;
			}
			//if (count == 3) {	// 컵케이크
			//	if(it->room_map != MAP_Oreo)
			//		patern = EV_RFR_SLIME;	// 12. 컵케이크
			//	else
			//		patern = (rand() % 3) + Rfr_Start_Num;
			//}
			//else {				// 나머지
			//	patern = (rand() % 3) + Rfr_Start_Num;	// 11. 안개, 12. 피버, 13.번개
			//	//patern = EV_RFR_FEVER;
			//}

			it->referee.patern_count = count;
			it->referee.patern_type = patern;

			for (int i = 0; i < MAX_ROOM_USER; ++i) {
				int client_id = it->clientNum[i];

				if (client_id != -1)
				{
					if (clients[client_id].connected == true)
					{
						send_patern_message_packet(client_id, patern);
					}
				}
			}

			add_timer(room_num, room_num + ROOM_TIMER_START, EVENT_TYPE(patern), high_resolution_clock::now() + 3s, 0);
			if (patern == EV_RFR_SLIME) {
				if(it->room_map == MAP_Wehas)
					it->move_actor_flag = true;
			}
			if (patern == EV_RFR_LIGHTNING) {
				it->lighting = true;
				it->light_count = 0;
				add_timer(room_num, room_num + ROOM_TIMER_START, EV_RFR_LIGHTNING_INDEX, high_resolution_clock::now() + 3s, 0);
			}
		}
		else if (timer_event == Rfr_Weapon) {
			add_timer(room_num, room_num + ROOM_TIMER_START, EV_RFR_WEAPON, high_resolution_clock::now(), 0);
		}

		if (timer > 0) {
			// 시간 종료
			add_timer(room_num, room_num + ROOM_TIMER_START, EV_TIME, high_resolution_clock::now() + 1s, 0);
		}
		else {
			add_timer(room_num, room_num + ROOM_TIMER_START, EV_END, high_resolution_clock::now(), 0);
		}

		break;
	}
	case EV_END:
	{
		int room_num = ev.id - ROOM_TIMER_START;

		room_l.lock();
		auto it = find(gRoom.begin(), gRoom.end(), room_num);
		room_l.unlock();

		cout << "게임 종료\n";

		// 게임 시간 끝 : HP 총합으로 승패 결정
		if (it->room_mode == ROOM_MODE_SOLO)
		{
			int winer_id;
			int high_hp = 0;
			for (int i = 0; i < MAX_ROOM_USER; ++i)
			{
				int client_id = it->clientNum[i];
				if (client_id != -1)
				{
					if (clients[client_id].connected == true)
					{
						int hp = clients[client_id].playerinfo->m_hp;
						if (hp > high_hp)
						{
							high_hp = hp;
							winer_id = client_id;
						}
					}
				}
			}
			for (int i = 0; i < MAX_ROOM_USER; ++i)
			{
				int client_id = it->clientNum[i];
				if (client_id != -1)
				{
					if (clients[client_id].connected == true)
					{
						if (winer_id == client_id)
							send_win_packet(client_id);
						else
							send_lose_packet(client_id);
					}
				}
			}
		}
		else {
			int team1_hp_total = 0;
			int team2_hp_total = 0;

			for (int i = 0; i < MAX_ROOM_USER / 2; ++i)
			{
				int team1_id = it->clientNum[i];
				int team2_id = it->clientNum[i + 4];
				if (team1_id != -1)
				{
					if (clients[team1_id].connected == true)
					{
						team1_hp_total += clients[team1_id].playerinfo->m_hp;
					}
				}
				if (team2_id != -1)
				{
					if (clients[team2_id].connected == true)
					{
						team2_hp_total += clients[team2_id].playerinfo->m_hp;
					}
				}
			}

			if (team1_hp_total >= team2_hp_total)
				it->team_victory = true;
			else
				it->team_victory = false;

			bool win_team = it->team_victory;
			for (int i = 0; i < MAX_ROOM_USER / 2; ++i)
			{
				int winner, loser;
				if (win_team == true) {
					winner = it->clientNum[i];
					loser = it->clientNum[i + 4];
				}
				else {
					winner = it->clientNum[i + 4];
					loser = it->clientNum[i];
				}
				if (winner != -1)
				{
					if (clients[winner].connected == true)
					{
						send_win_packet(winner);
					}
				}
				if (loser != -1)
				{
					if (clients[loser].connected == true)
					{
						send_lose_packet(loser);
					}
				}
			}
		}
		it->room_status = ROOM_ST_END;
		break;
	}
	default:
		cout << "Event Error!" << endl;
		while (true);
	}
}

void clientInputProcess(int room_num)
{
	room_l.lock();
	auto it = find(gRoom.begin(), gRoom.end(), room_num);
	room_l.unlock();

	if (it->room_map == MAP_Wehas)
	{
		if (it->move_actor_flag == true)
		{
			PxTransform pos = it->move_actor->getGlobalPose();
			pos.p.y += 25.f * gGameTimer.GetTimeElapsed();
			if (pos.p.y > 0.0f) {
				pos.p.y = 0.0f;
				it->move_actor_flag = false;
			}

			it->move_actor->setGlobalPose(PxTransform(pos));
		}
	}
	else if (it->room_map == MAP_Cake)
	{
		if (it->move_actor_flag == true) {
			PxTransform pos = it->move_actor->getGlobalPose();
			pos.p.x += 0.5f;
			if (pos.p.x > 120.0f) {
				it->move_actor_flag = false;
			}
			it->move_actor->setGlobalPose(PxTransform(pos));
		}
		else {
			PxTransform pos = it->move_actor->getGlobalPose();
			pos.p.x -= 0.5f;
			if (pos.p.x < -120.0f) {
				it->move_actor_flag = true;
			}
			it->move_actor->setGlobalPose(PxTransform(pos));
		}
	}

	if (it->room_map == MAP_Oreo)
	{
		if (it->oreo_team1_spawn == true)
		{
			for (int i = 0; i < MAX_ROOM_USER / 2; ++i)
			{
				int client_id = it->clientNum[i];
				if (client_id != -1)
				{
					if (clients[client_id].connected == true)
					{
						if (it->team_dead[i] == true)
						{
							// Team1 다음 주자
							PxExtendedVec3 pos = PXtoPXEx(OreoTeleport[0]);
							clients[client_id].playerinfo->m_PlayerController->setFootPosition(pos);
							for (int j = 0; j < MAX_ROOM_USER; ++j)
							{
								int client_id2 = it->clientNum[j];
								if (client_id2 != -1)
								{
									if (clients[client_id2].connected == true)
									{
										send_teleport_packet(client_id2, i, OreoTeleport[0]);
									}
								}
							}
							it->oreo_team1_spawn = false;
							break;
						}
					}
				}
			}
		}
		if (it->oreo_team2_spawn == true)
		{
			for (int i = 4; i < 4 + (MAX_ROOM_USER / 2); ++i)
			{
				int client_id = it->clientNum[i];
				if (client_id != -1)
				{
					if (clients[client_id].connected == true)
					{
						if (it->team_dead[i] == true)
						{
							// Team2 다음 주자
							PxExtendedVec3 pos = PXtoPXEx(OreoTeleport[1]);
							clients[client_id].playerinfo->m_PlayerController->setFootPosition(pos);
							for (int j = 0; j < MAX_ROOM_USER; ++j)
							{
								int client_id2 = it->clientNum[j];
								if (client_id2 != -1)
								{
									if (clients[client_id2].connected == true)
									{
										send_teleport_packet(client_id2, i, OreoTeleport[1]);
									}
								}
							}
							it->oreo_team2_spawn = false;
							break;
						}
					}
				}
			}
		}
	}

	for (int i = 0; i < MAX_ROOM_USER; ++i)
	{
		int client_id = it->clientNum[i];
		if (client_id != -1)
		{
			if (clients[client_id].connected == true)
			{
				if (clients[client_id].playerinfo->m_hp > 0)
				{
					int Ani_Index = clients[client_id].playerinfo->m_AniIndex;

					clients[client_id].playerinfo->m_AttackTrigger->setGlobalPose(PxTransform(100, 100, 100));

					char status = clients[client_id].playerinfo->m_status;

					PxVec3 direction;
					PxVec3 velocity = clients[client_id].playerinfo->m_Vel;

					direction = velocity.getNormalized();

					if (clients[client_id].playerinfo->m_dashed) {
						if (status != STATUS::SKILL_WEAPON_MOVE) {
							direction *= 2.f;
						}
					}

					if (it->fever == true)
					{
						direction *= 2.f;
					}

					direction *= clients[client_id].playerinfo->speed_skill;

					if (status == STATUS::DEFENSE || status == STATUS::WEAK_ATTACK || status == STATUS::HARD_ATTACK
						|| status == STATUS::HITTED || status == STATUS::SKILL_WEAPON_NO_MOVE || status == STATUS::STUN || status == STATUS::CH_SKILL)
					{
						continue;
					}
					else if (status == STATUS::CRI_HITTED)
					{
						PxVec3 knockback_dir = clients[client_id].playerinfo->m_Knockback;
						PxVec3 dist = knockback_dir * gGameTimer.GetTimeElapsed() * KnockBack_Vel;

						float jump_height = 0.0f;

						if (clients[client_id].playerinfo->m_Jump.mJump == true) {
							clients[client_id].playerinfo->m_Jump.stopJump();
						}
						else {
							clients[client_id].playerinfo->m_Fall.startJump(0);
							jump_height = clients[client_id].playerinfo->m_Fall.getHeight(gGameTimer.GetTimeElapsed());
						}

						dist.y += jump_height;

						PxControllerFilters filters;
						const PxU32 flags = clients[client_id].playerinfo->m_PlayerController->move(dist, 0.001, 1 / 60, filters);

						if (flags & PxControllerCollisionFlag::eCOLLISION_DOWN)
						{
							if (clients[client_id].playerinfo->m_Fall.mJump) {
								clients[client_id].playerinfo->m_Fall.stopJump();
							}
						}
					}
					else
					{
						float jump_height;

						if (clients[client_id].playerinfo->m_Jump.mJump == true) {
							jump_height = clients[client_id].playerinfo->m_Jump.getHeight(gGameTimer.GetTimeElapsed());
							direction = clients[client_id].playerinfo->m_JumpVel;
						}
						else {
							clients[client_id].playerinfo->m_Fall.startJump(0);
							jump_height = clients[client_id].playerinfo->m_Fall.getHeight(gGameTimer.GetTimeElapsed());
						}

						//cout << int(i) << " Vel : " << direction.x << ", " << direction.y << ", " << direction.z << endl;
						float elapsedTime = gGameTimer.GetTimeElapsed();

						PxVec3 distance = direction * elapsedTime * NORMAL_SPEED;
						distance.y += jump_height;

						PxControllerFilters filters;
						if (clients[client_id].playerinfo->m_PlayerController) {
							const PxU32 flags = clients[client_id].playerinfo->m_PlayerController->move(distance, 0.001, 1 / 60, filters);

							//PxExtendedVec3 pos = clients[client_id].playerinfo->m_PlayerController->getPosition();
							//
							//cout << pos.x << "," << pos.y << "," << pos.z << endl;

							if (flags & PxControllerCollisionFlag::eCOLLISION_DOWN)
							{
								//cout << "충돌\n";
								if (clients[client_id].playerinfo->m_Jump.mJump) {
									clients[client_id].playerinfo->m_Jump.stopJump();
									clients[client_id].playerinfo->setJumpVelocity(PxVec3(0, 0, 0));
									if (velocity.magnitude() > 0.f)
									{
										if (clients[client_id].playerinfo->m_dashed == true)
											clients[client_id].playerinfo->setAniIndex(Anim::Run);
										else
											clients[client_id].playerinfo->setAniIndex(Anim::Walk);
									}
									else {
										clients[client_id].playerinfo->setAniIndex(Anim::Idle);
									}
									clients[client_id].playerinfo->setStatus(STATUS::FREE);
								}
								if (clients[client_id].playerinfo->m_Fall.mJump) {
									clients[client_id].playerinfo->m_Fall.stopJump();
								}
							}
						}
					}
				}
			}
		}
	}

	while (true)
	{
		it->m_timer_l.lock();
		if (true == it->m_timer_queue.empty()) {
			it->m_timer_l.unlock();
			break;
		}
		EVENT_ST ev = it->m_timer_queue.top();
		if (ev.start_time > high_resolution_clock::now()) {
			it->m_timer_l.unlock();
			break;
		}
		it->m_timer_queue.pop();
		it->m_timer_l.unlock();

		process_event(ev);
	}
}

void clientUpdateProcess(int room_num)
{
	room_l.lock();
	auto it = find(gRoom.begin(), gRoom.end(), room_num);
	room_l.unlock();

	//PxTransform t = it->test2->getGlobalPose();
	//cout << t.p.x << "," << t.p.y << "," << t.p.z << endl;

	for (int i = 0; i < MAX_ROOM_USER; ++i)
	{
		int client_id = it->clientNum[i];
		if (client_id != -1)
		{
			if (clients[client_id].connected == true)
			{
				if (clients[client_id].playerinfo->m_hp > 0)
				{
					if (clients[client_id].playerinfo->hitted == true)
					{
						char status = clients[client_id].playerinfo->m_status;

						if (status == STATUS::HITTED) {

							add_timer(room_num, client_id, EV_FREE, high_resolution_clock::now() + 660ms);

							int hp = clients[client_id].playerinfo->m_hp;
							int mp = clients[client_id].playerinfo->m_mp;
							if (hp > 0) {
								hp -= 10;
								mp += 20;
								if (mp > 100) {
									mp = 100;
								}
								if (hp <= 0) {
									hp = 0;
									mp = 0;
									clients[client_id].playerinfo->setAniIndex(Anim::Death);
									clients[client_id].playerinfo->setVelocity(PxVec3(0, 0, 0));
									clients[client_id].playerinfo->m_PlayerController->release();
									clients[client_id].playerinfo->m_PlayerController = NULL;

									if (it->room_mode == ROOM_MODE_SOLO)
									{
										it->solo_dead_count += 1;
									}
									else if (it->room_mode == ROOM_MODE_TEAM || it->room_mode == ROOM_MODE_KING)
									{
										char slot = clients[client_id].slot;
										it->team_dead[slot] = false;
										if (it->room_mode == ROOM_MODE_KING) {
											if (slot >= 0 && slot < 4) {
												it->oreo_team1_spawn = true;
											}
											else {
												it->oreo_team2_spawn = true;
											}
										}
									}
								}
								else {
									clients[client_id].playerinfo->setAniIndex(Anim::Small_React);
								}
								clients[client_id].playerinfo->setHP(hp);
								clients[client_id].playerinfo->m_mp = mp;

								for (int j = 0; j < MAX_ROOM_USER; ++j)
								{
									int client_id2 = it->clientNum[j];
									if (client_id2 != -1)
									{
										if (clients[client_id2].connected == true)
										{
											send_hit_packet(client_id2, client_id, hp, mp);
										}
									}
								}
							}
						}
						if (status == STATUS::CRI_HITTED) {

							add_timer(room_num, client_id, EV_FREE, high_resolution_clock::now() + 2000ms);

							int hp = clients[client_id].playerinfo->m_hp;
							int mp = clients[client_id].playerinfo->m_mp;
							if (hp > 0) {
								hp -= 20;
								mp += 20;
								if (mp > 100) {
									mp = 100;
								}
								if (hp <= 0) {
									hp = 0;
									clients[client_id].playerinfo->setAniIndex(Anim::Death);
									clients[client_id].playerinfo->setVelocity(PxVec3(0, 0, 0));
									clients[client_id].playerinfo->m_PlayerController->release();
									clients[client_id].playerinfo->m_PlayerController = NULL;

									if (it->room_mode == ROOM_MODE_SOLO)
									{
										it->solo_dead_count += 1;
									}
									else if (it->room_mode == ROOM_MODE_TEAM || it->room_mode == ROOM_MODE_KING)
									{
										char slot = clients[client_id].slot;
										it->team_dead[slot] = false;
										if (it->room_mode == ROOM_MODE_KING) {
											if (slot >= 0 && slot < 4) {
												it->oreo_team1_spawn = true;
											}
											else {
												it->oreo_team2_spawn = true;
											}
										}
									}
								}
								else {
									clients[client_id].playerinfo->setAniIndex(Anim::Hard_React);
								}
								clients[client_id].playerinfo->setHP(hp);
								clients[client_id].playerinfo->m_mp = mp;
								PxVec3 dir = clients[client_id].playerinfo->m_Knockback;

								for (int j = 0; j < MAX_ROOM_USER; ++j)
								{
									int client_id2 = it->clientNum[j];
									if (client_id2 != -1)
									{
										if (clients[client_id2].connected == true)
										{
											send_critical_hit_packet(client_id2, client_id, hp, mp, dir);
										}
									}
								}
							}
						}

						if (status == STATUS::STUN) {

							add_timer(room_num, client_id, EV_FREE, high_resolution_clock::now() + 1330ms);

							int hp = clients[client_id].playerinfo->m_hp;
							int mp = clients[client_id].playerinfo->m_mp;
							if (hp > 0) {
								hp -= 10;
								mp += 20;
								if (mp > 100)
								{
									mp = 100;
								}
								if (hp <= 0) {
									hp = 0;
									clients[client_id].playerinfo->setAniIndex(Anim::Death);
									clients[client_id].playerinfo->setVelocity(PxVec3(0, 0, 0));
									clients[client_id].playerinfo->m_PlayerController->release();
									clients[client_id].playerinfo->m_PlayerController = NULL;

									if (it->room_mode == ROOM_MODE_SOLO)
									{
										it->solo_dead_count += 1;
									}
									else if (it->room_mode == ROOM_MODE_TEAM || it->room_mode == ROOM_MODE_KING)
									{
										char slot = clients[client_id].slot;
										it->team_dead[slot] = false;
										if (it->room_mode == ROOM_MODE_KING) {
											if (slot >= 0 && slot < 4) {
												it->oreo_team1_spawn = true;
											}
											else {
												it->oreo_team2_spawn = true;
											}
										}
									}
								}
								else {
									clients[client_id].playerinfo->setAniIndex(Anim::Stun);
								}
								clients[client_id].playerinfo->setHP(hp);
								clients[client_id].playerinfo->m_mp = mp;
								for (int j = 0; j < MAX_ROOM_USER; ++j)
								{
									int client_id2 = it->clientNum[j];
									if (client_id2 != -1)
									{
										if (clients[client_id2].connected == true)
										{
											send_stun_packet(client_id2, client_id, hp, mp);
										}
									}
								}
							}
						}

						clients[client_id].playerinfo->hitted = false;
					}

					// 위치 동기화
					if (clients[client_id].playerinfo->m_PlayerController != nullptr) {
						PxExtendedVec3 position = clients[client_id].playerinfo->m_PlayerController->getPosition();
						//cout << int(i) << " Client Pos : " << position.x << "," << position.y - 17.5 << "," << position.z << endl;

						clients[client_id].playerinfo->m_Pos.x = position.x;
						if (clients[client_id].playerinfo->weapon_type == Weapon_King)
						{
							clients[client_id].playerinfo->m_Pos.y = position.y - 35.f;
						}
						else {
							clients[client_id].playerinfo->m_Pos.y = position.y - 17.5f;
						}
						clients[client_id].playerinfo->m_Pos.z = position.z;
					}

					// 낙사 체크(-100 이하는 낙사로 인정)
					PxVec3 pos = clients[client_id].playerinfo->m_Pos;
					if (pos.y < FALL_HEIGHT) {
						cout << "FALL\n";

						int hp = 0;
						clients[client_id].playerinfo->setAniIndex(Anim::Death);
						clients[client_id].playerinfo->setVelocity(PxVec3(0, 0, 0));
						clients[client_id].playerinfo->m_PlayerController->release();
						clients[client_id].playerinfo->m_PlayerController = NULL;

						if (it->room_mode == ROOM_MODE_SOLO)
						{
							it->solo_dead_count += 1;
						}
						else if (it->room_mode == ROOM_MODE_TEAM || it->room_mode == ROOM_MODE_KING)
						{
							char slot = clients[client_id].slot;
							it->team_dead[slot] = false;
							if (it->room_mode == ROOM_MODE_KING) {
								if (slot >= 0 && slot < 4) {
									it->oreo_team1_spawn = true;
								}
								else {
									it->oreo_team2_spawn = true;
								}
							}
						}

						clients[client_id].playerinfo->setHP(hp);

						for (int j = 0; j < MAX_ROOM_USER; ++j)
						{
							int client_id2 = it->clientNum[j];
							if (client_id2 != -1)
							{
								if (clients[client_id2].connected == true)
								{
									send_fall_packet(client_id2, client_id, hp);
								}
							}
						}
					}
				}
			}
		}
	}
}

void broadcastPosPacket(int room_num)
{
	//cout << "BroadCast\n";
	room_l.lock();
	auto it = find(gRoom.begin(), gRoom.end(), room_num);
	room_l.unlock();

	for (int i = 0; i < MAX_ROOM_USER; ++i)
	{
		int client_id = it->clientNum[i];
		if (client_id != -1)
		{
			if (clients[client_id].connected == true)
			{
				if (clients[client_id].playerinfo->m_hp > 0)
				{
					for (int j = 0; j < MAX_ROOM_USER; ++j)
					{
						int client_id2 = it->clientNum[j];
						if (it->clientNum[j] != -1)
						{
							if (clients[it->clientNum[j]].connected == true)
							{
								send_pos_packet(client_id2, client_id);
							}
						}
					}
				}
			}
		}
	}
}

void logic()
{
	srand((unsigned int)time(NULL));
	gGameTimer.Reset();
	while (true)
	{
		gGameTimer.Tick(60.0f);
		for (auto it = gRoom.begin(); it != gRoom.end(); ++it)
		{
			if (it->room_status == ROOM_ST_WAITING)
			{
				continue;
			}
			else if (it->room_status == ROOM_ST_LOADING)
			{
				bool result = it->all_load_complete();

				if (result == true)
				{
					send_room_setting_weapon(it->room_num);
					send_room_setting_player(it->room_num);

					cout << it->room_num << " Room Loding Complete\n";
					it->room_status = 2;
				}
			}
			else if (it->room_status == ROOM_ST_LOADING_COMPLETE)
			{
				bool result = it->all_setting_complete();

				send_room_start_game(it->room_num);

				add_timer(it->room_num, it->room_num + ROOM_TIMER_START, EV_TIME, high_resolution_clock::now() + 5s, 0);

				cout << it->room_num << " Room Start Game\n";

				if (it->room_mode == MAP_Cake)
					it->move_actor_flag = true;
				if (it->room_map == MAP_Oreo) {
					it->oreo_team1_spawn = true;
					it->oreo_team2_spawn = true;
				}
				it->room_status = 3;
			}
			else if (it->room_status == ROOM_ST_PLAYING)
			{
				bool is_end = it->is_game_end();
				if (is_end) {
					if (it->room_mode == ROOM_MODE_SOLO)
					{
						for (int i = 0; i < MAX_ROOM_USER; ++i)
						{
							int client_id = it->clientNum[i];
							if (client_id != -1)
							{
								if (clients[client_id].connected == true)
								{
									if (clients[client_id].playerinfo->m_hp > 0)
									{
										send_win_packet(client_id);
									}
									else
									{
										send_lose_packet(client_id);
									}
								}
							}
						}
					}
					else
					{
						bool win_team = it->team_victory;			// false : 1팀 우승, true : 2팀 우승
						for (int i = 0; i < MAX_ROOM_USER / 2; ++i)
						{
							int winner, loser;
							if (win_team == false) {				// 1팀 우승
								winner = it->clientNum[i];
								loser = it->clientNum[i + 4];
							}
							else {
								winner = it->clientNum[i + 4];
								loser = it->clientNum[i];
							}
							if (winner != -1)
							{
								if (clients[winner].connected == true)
								{
									send_win_packet(winner);
								}
							}
							if (loser != -1)
							{
								if (clients[loser].connected == true)
								{
									send_lose_packet(loser);
								}
							}
						}
					}
					cout << it->room_num << " Room Game Over\n";
					it->room_status = ROOM_ST_END;
				}
				else {
					clientInputProcess(it->room_num);

					it->m_Scene->simulate(1.f / 60.f);
					it->m_Scene->fetchResults(true);

					clientUpdateProcess(it->room_num);

					it->PosBroadCastTime += gGameTimer.GetTimeElapsed();
					if (it->PosBroadCastTime > 0.05f)
					{
						int room_num = it->room_num;
						broadcastPosPacket(room_num);
						it->PosBroadCastTime = 0.0f;
					}
				}
			}
			else if (it->room_status == ROOM_ST_END)
			{
				if (it->m_Scene)
					it->m_Scene->release();

				it->m_timer_l.lock();
				while (!it->m_timer_queue.empty())
				{
					it->m_timer_queue.pop();
				}
				it->m_timer_l.unlock();

				cout << it->room_num << " Room Release\n";
				it->room_status = ROOM_ST_EXIT;
			}
			else if (it->room_status == ROOM_ST_EXIT)
			{
				it->end_timer += gGameTimer.GetTimeElapsed();
				if (it->end_timer > END_TIMER)
				{
					for (int i = 0; i < MAX_ROOM_USER; ++i)
					{
						int client_id = it->clientNum[i];
						if (client_id != -1)
						{
							if (clients[client_id].connected == true)
							{
								send_end_packet(client_id);
							}
						}
					}
					cout << it->room_num << " Room End\n";
					it->room_status = ROOM_ST_WAITING;
				}
			}
		}
	}
}

void mapLoad()
{
	char title[13][40];
	char txt[8];

	sprintf_s(txt, sizeof(txt), ".txt");
	sprintf_s(title[0], sizeof(title[0]), "map_1_base");
	sprintf_s(title[1], sizeof(title[1]), "map_2_base");
	sprintf_s(title[2], sizeof(title[2]), "map_2_chocobar");
	sprintf_s(title[3], sizeof(title[3]), "map_3_bridge");
	sprintf_s(title[4], sizeof(title[4]), "map_3_floor1");
	sprintf_s(title[5], sizeof(title[5]), "map_3_floor2");
	sprintf_s(title[6], sizeof(title[6]), "map_3_floor3");
	sprintf_s(title[7], sizeof(title[7]), "map_3_in");
	sprintf_s(title[8], sizeof(title[8]), "map_3_in_stair1");
	sprintf_s(title[9], sizeof(title[9]), "map_3_in_stair2");
	sprintf_s(title[10], sizeof(title[10]), "map_3_in_wall");
	sprintf_s(title[11], sizeof(title[11]), "map_3_stair");
	sprintf_s(title[12], sizeof(title[12]), "map_macaron");

	for (int i = 0; i < 13; ++i)
	{
		strcat_s(title[i], sizeof(title[i]), txt);

		ifstream in(title[i]);
		int size;
		float pos;
		PxVec3 vertex;

		vector<PxVec3> mapVertex;
		vector<int> mapIndex;

		while (true) {
			in >> size;
			int ver_size = size;
			for (int j = 0; j < ver_size; ++j) {
				in >> pos;
				vertex.x = pos;
				in >> pos;
				vertex.y = pos;
				in >> pos;
				vertex.z = pos;

				mapVertex.push_back(vertex);
			}

			in >> size;
			int index_size = size;
			for (int j = 0; j < index_size; ++j) {
				in >> size;
				mapIndex.push_back(size);
			}
			break;
		}
		in.close();

		gMapVertexs.push_back(mapVertex);
		gMapIndexs.push_back(mapIndex);
	}
}

int main()
{
	mapLoad();

	for (int i = 0; i < MAX_USER; ++i)
	{
		clients[i].connected = false;
	}

	gPhysx = new CPhysx();
	gPhysx->initPhysics();

	vector<thread> worker_threads;
	g_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);

	// Winsock Start - windock.dll 로드
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
	{
		cout << "Error - Can not load 'winsock.dll' file\n";
		return 0;
	}

	for (int i = 0; i < 4; ++i)
		worker_threads.push_back(thread{ worker_thread });

	thread accept_thread{ do_accept };

	thread logic_thread{ logic };

	for (auto &th : worker_threads) th.join();
	accept_thread.join();
	logic_thread.join();

	//while (true);
}