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

vector<PxVec3> gMapVertex;
vector<int> gMapIndex;

vector<CRoom> gRoom;
mutex room_l;

unordered_set<int> gLobby;
mutex lobby_l;

CGameTimer gGameTimer;

PxVec3 PlayerInitPosition[8] = {
   PxVec3(0, 10.1, 100), PxVec3(50, 10.1, 100), PxVec3(-50, 10.1, 100), PxVec3(100, 10.1, 100), PxVec3(-100, 10.1, 100),
   PxVec3(150, 10.1, 100), PxVec3(-150, 10.1, 100), PxVec3(200, 10.1, 100)
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

void send_login_packet(char client) {
	sc_packet_login p_login;
	p_login.id = client;
	p_login.x = PlayerInitPosition[client].x;
	p_login.y = PlayerInitPosition[client].y;
	p_login.z = PlayerInitPosition[client].z;
	p_login.vx = 0.f;
	p_login.vy = 0.f;
	p_login.vz = 0.f;
	p_login.type = SC_LOGIN;
	p_login.size = sizeof(sc_packet_login);

	sendPacket(client, &p_login);
}

void send_room_info_packet(char client, const CRoom& room)
{
	sc_packet_room_info p_room_info;
	p_room_info.size = sizeof(sc_packet_room_info);
	p_room_info.type = SC_ROOM_INFO;
	strcpy_s(p_room_info.name, _countof(p_room_info.name), room.name);
	p_room_info.current_num = room.current_num;
	p_room_info.room_num = room.room_num;

	sendPacket(client, &p_room_info);
}

void send_room_datail_info_packet(int client, int player, char slot, int room_num, char host)
{
	sc_packet_room_detail_info p_room_detail_info;
	p_room_detail_info.size = sizeof(sc_packet_room_detail_info);
	p_room_detail_info.type = SC_ROOM_DETAIL_INFO;
	p_room_detail_info.room_num = room_num;
	p_room_detail_info.room_index = slot;
	p_room_detail_info.host = host;
	strcpy_s(p_room_detail_info.name, _countof(p_room_detail_info.name), clients[player].id);

	sendPacket(client, &p_room_detail_info);
}

void send_put_player_packet(char client, char new_id) {
	sc_packet_put_player p_put;
	p_put.id = new_id;
	p_put.x = clients[new_id].playerinfo->m_Pos.x;
	p_put.y = clients[new_id].playerinfo->m_Pos.y;
	p_put.z = clients[new_id].playerinfo->m_Pos.z;
	p_put.vx = clients[new_id].playerinfo->m_Vel.x;
	p_put.vy = clients[new_id].playerinfo->m_Vel.y;
	p_put.vz = clients[new_id].playerinfo->m_Vel.z;
	p_put.ani_index = clients[new_id].playerinfo->m_AniIndex;
	p_put.dashed = clients[new_id].playerinfo->m_dashed;
	p_put.type = SC_PUT_PLAYER;
	p_put.size = sizeof(sc_packet_put_player);

	sendPacket(client, &p_put);
}

void send_pos_packet(char client, char id) {
	sc_packet_pos p_pos;
	p_pos.id = id;
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
	p_anim.id = id;
	p_anim.ani_index = clients[id].playerinfo->m_AniIndex;

	sendPacket(client, &p_anim);
}

void send_weapon_packet(char client, char id, char wp_type, char wp_index) {
	sc_packet_weapon p_weapon;
	p_weapon.type = SC_WEAPON;
	p_weapon.size = sizeof(sc_packet_weapon);
	p_weapon.id = id;
	p_weapon.weapon_type = wp_type;
	p_weapon.weapon_index = wp_index;

	sendPacket(client, &p_weapon);
}

void send_room_setting_weapon(int room_num)
{

}

void send_room_setting_player(int room_num)
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
				for (int j = 0; j < MAX_ROOM_USER; ++j)
				{
					int client_id2 = it->clientNum[j];
					if (client_id2 != -1)
					{
						if (clients[client_id2].connected == true)
						{
							cout << client_id << ", " << j << endl;
							send_put_player_packet(client_id, j);
						}
					}
				}
			}
		}
	}
	cout << "put player packet setting end\n";
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

		//room_l.lock();
		//auto room = gRoom;
		//room_l.unlock();

		int count = 0;

		for (auto it = gRoom.begin(); it != gRoom.end(); ++it)
		{
			if (count < 6)
			{
				send_room_info_packet(key, *it);
				count++;
			}
			else {
				break;
			}
		}

		//clients[key].playerinfo->setPosition(PlayerInitPosition[key]);
		//clients[key].playerinfo->setVelocity(PxVec3(0, 0, 0));
		//clients[key].playerinfo->setLook(PxVec3(0, 0, 1));
		//clients[key].playerinfo->setDashed(false);
		//gPhysx->m_Scene->lockWrite();
		//clients[key].playerinfo->setPlayerController(gPhysx);
		//clients[key].playerinfo->setTrigger(gPhysx);
		//gPhysx->m_Scene->unlockWrite();
		//gPhysx->registerPlayer(clients[key].playerinfo, key);
		//clients[key].connected = true;
		//
		//send_login_packet(key);
		//
		//// 자신(key)과 다른 클라에게 자기 위치 정보 Send
		//for (int i = 0; i < MAX_USER; ++i)
		//{
		//	if (clients[i].connected == true)
		//	{
		//		if (i != key)
		//		{
		//			//cout << "Put Packet Send\n";
		//			send_put_player_packet(i, key);
		//		}
		//	}
		//}
		//// 로그인 시, 접속 중인 다른 클라 위치 정보를 자신에게 Send
		//for (int i = 0; i < MAX_USER; ++i)
		//{
		//	if (clients[i].connected == true)
		//	{
		//		if (i != key)
		//		{
		//			send_put_player_packet(key, i);
		//		}
		//	}
		//}
		//break;
	}
	case CS_DISCONNECT:
	{
		cout << "[" << int(key) << "] Clients Disconnect\n";
		break;
	}

	case CS_MAKE_ROOM:
	{
		cs_packet_make_room *p_make_room;
		p_make_room = reinterpret_cast<cs_packet_make_room*>(buffer);

		CRoom *room = new CRoom();
		room->init(p_make_room->name, key, roomNum);

		clients[key].room_num = roomNum;

		roomNum++;

		room_l.lock();
		gRoom.push_back(*room);
		room_l.unlock();

		lobby_l.lock();
		gLobby.erase(key);
		auto lobby = gLobby;
		lobby_l.unlock();

		send_room_datail_info_packet(key, key, 0, room->room_num, 1);


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


		int count = 0;
		// 참가한 플레이어에게 자신을 제외한 클라의 정보를 전송
		for (int i = 0; i < MAX_ROOM_USER; ++i)
		{
			char host = 0;
			if (count < it->current_num)
			{
				if (it->clientNum[i] != -1)
				{
					if (i == it->host_num) host = 1;
					send_room_datail_info_packet(key, it->clientNum[i], i, room_num, host);
					count++;
				}
			}
		}

		count = 0;
		// 참가한 플레이어의 정보를 방 내부 클라에게 전송
		for (int i = 0; i < MAX_ROOM_USER; ++i)
		{
			if (count < it->current_num - 1)
			{
				if (it->clientNum[i] != -1)
				{
					if (it->clientNum[i] != key)
					{
						send_room_datail_info_packet(it->clientNum[i], key, i, room_num, 0);
						count++;
					}
				}
			}
		}

		cout << "ATTEND ROOM : " << int(it->current_num) << endl;
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

		it->start(gMapVertex, gMapIndex);			// Physx에 초기화 및 맵 / 무기

		for (int i = 0; i < MAX_ROOM_USER; ++i)		// 플레이어 Physx Capsule 적용
		{
			int client_id = it->clientNum[i];
			if (client_id != -1)
			{
				if (clients[client_id].connected == true)
				{
					clients[client_id].playerinfo = new CPlayer();

					clients[client_id].playerinfo->setPosition(PlayerInitPosition[key]);
					clients[client_id].playerinfo->setVelocity(PxVec3(0, 0, 0));
					clients[client_id].playerinfo->setLook(PxVec3(0, 0, 1));
					clients[client_id].playerinfo->setDashed(false);
					clients[client_id].playerinfo->setAniIndex(Anim::Idle);
					it->m_pPhysx->m_Scene->lockWrite();
					clients[client_id].playerinfo->setPlayerController(it->m_pPhysx);
					clients[client_id].playerinfo->setTrigger(it->m_pPhysx);
					it->m_pPhysx->m_Scene->unlockWrite();
					it->m_pPhysx->registerPlayer(clients[client_id].playerinfo, key);
				}
			}
		}

		cout << "Map loading complete\n";

		break;
	}

	case CS_READY_ROOM:
	{
		break;
	}

	case CS_LOAD_COMPLETE:
	{
		int room_num = clients[key].room_num;

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

		cout << "load_complete\n";
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
			clients[key].playerinfo->setLook(Normalize(clients[key].playerinfo->m_Vel));
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
					send_pos_packet(client_id, i);
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
			clients[key].playerinfo->setAniIndex(Anim::Guard);
			clients[key].playerinfo->setStatus(STATUS::DEFENSE);
		}

		if (p_anim->key == CS_GUARD_OFF) {
			cout << "guard off" << endl;
			clients[key].playerinfo->setAniIndex(Anim::Idle);
			clients[key].playerinfo->setStatus(STATUS::FREE);
		}

		if (p_anim->key == CS_WEAK) {
			cout << "weak attack ";
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
			clients[key].playerinfo->setStatus(STATUS::WEAK_ATTACK);
			cout << int(p_anim->count) << endl;

		}

		if (p_anim->key == CS_HARD) {
			cout << "hard attack ";
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
			clients[key].playerinfo->setStatus(STATUS::HARD_ATTACK);
			cout << int(p_anim->count) << endl;
		}

		/*if (p_anim->key == CS_WEAK) {
			if (clients[key].playerinfo->weapon_type == -1) {
				if (Anim_Index == Anim::Idle || Anim_Index == Anim::Walk || Anim_Index == Anim::Run) {
					clients[key].playerinfo->setAniIndex(Anim::Weak_Attack1);
				}
				if (Anim_Index == Anim::Weak_Attack1 && (Anim_Time > 10 && Anim_Time < 15)) {
					clients[key].playerinfo->setAniIndex(Anim::Weak_Attack2);
				}
				if (Anim_Index == Anim::Weak_Attack2 && (Anim_Time > 20 && Anim_Time < 25)) {
					clients[key].playerinfo->setAniIndex(Anim::Weak_Attack3);
				}
			}
			else {
				if (Anim_Index == Anim::Idle || Anim_Index == Anim::Walk || Anim_Index == Anim::Run) {
					clients[key].playerinfo->setAniIndex(Anim::Lollipop_Attack1);
				}
				if (Anim_Index == Anim::Lollipop_Attack1 && (Anim_Time > 10 && Anim_Time < 15)) {
					clients[key].playerinfo->setAniIndex(Anim::Lollipop_Attack2);
				}

			}
		}
		if (p_anim->key == CS_HARD) {
			if (clients[key].playerinfo->weapon_type == -1) {
				if (Anim_Index == Anim::Idle || Anim_Index == Anim::Walk || Anim_Index == Anim::Run) {
					clients[key].playerinfo->setAniIndex(Anim::Hard_Attack1);
				}
				if (Anim_Index == Anim::Hard_Attack1 && (Anim_Time > 10 && Anim_Time < 20)) {
					clients[key].playerinfo->setAniIndex(Anim::Hard_Attack2);
				}
			}
			else {
				if (Anim_Index == Anim::Idle || Anim_Index == Anim::Walk || Anim_Index == Anim::Run) {
					clients[key].playerinfo->setAniIndex(Anim::Lollipop_Hard_Attack);
				}
			}
		}*/

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

		clients[key].playerinfo->weapon_type = p_weapon->weapon_type;
		clients[key].playerinfo->weapon_index = p_weapon->weapon_index;
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
	}
}

void worker_thread()
{
	while (true)
	{
		ULONG io_byte;
		ULONGLONG io_key;
		OVER_EX *over_ex;

		BOOL is_error = GetQueuedCompletionStatus(g_iocp, &io_byte, (PULONG_PTR)&io_key, reinterpret_cast<LPWSAOVERLAPPED *>(&over_ex), INFINITE);

		int key = static_cast<char>(io_key);

		if (is_error == FALSE) {
			//cout << "Error in GQCS\n";
			cout << "[" << int(key) << "] Clients Disconnect\n";
			sc_packet_remove p_remove;
			p_remove.id = key;
			p_remove.type = SC_REMOVE;
			p_remove.size = sizeof(sc_packet_remove);

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
			//cout << "[" << int(key) << "] Clients Disconnect\n";
			sc_packet_remove p_remove;
			p_remove.id = key;
			p_remove.type = SC_REMOVE;
			p_remove.size = sizeof(sc_packet_remove);

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

			PxTransform triggerpos(PxVec3(0, 0, 0));
			PxExtendedVec3 playerpos = clients[ev.id].playerinfo->m_PlayerController->getPosition();
			PxVec3 look = clients[ev.id].playerinfo->m_Look;

			triggerpos.p.x = playerpos.x + (look.x * 10);
			triggerpos.p.y = playerpos.y + (look.y * 10);
			triggerpos.p.z = playerpos.z + (look.z * 10);

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

		if (it->weapon_list[wp_type][wp_index] == -1)
		{
			it->weapon_list[wp_type][wp_index] = ev.id;

			for (int i = 0; i < MAX_ROOM_USER; ++i)
			{
				int client_id = it->clientNum[i];

				if (client_id != -1)
				{
					if (clients[client_id].connected == true)
					{
						send_weapon_packet(i, ev.id, wp_type, wp_index);
					}
				}
			}
		}
		else {
			clients[ev.id].playerinfo->weapon_index = -1;
			clients[ev.id].playerinfo->weapon_type = -1;
		}
		break;
	}
	// 심판
	case EV_WEAPON:
	{
		break;
	}
	case EV_FOG:
	{
		break;
	}
	case EV_FEVER:
	{
		break;
	}
	case EV_LIGHTNING:
	{
		break;
	}
	case EV_SLIME:
	{
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

	for (int i = 0; i < MAX_ROOM_USER; ++i)
	{
		int client_id = it->clientNum[i];
		if (client_id != -1)
		{
			if (clients[client_id].connected == true)
			{
				int Ani_Index = clients[client_id].playerinfo->m_AniIndex;

				clients[client_id].playerinfo->m_AttackTrigger->setGlobalPose(PxTransform(100, 100, 100));

				char status = clients[client_id].playerinfo->m_status;

				PxVec3 direction;
				PxVec3 velocity = clients[client_id].playerinfo->m_Vel;

				direction = velocity.getNormalized();

				if (clients[client_id].playerinfo->m_dashed) {
					direction *= 2;
				}

				if (status == STATUS::DEFENSE || status == STATUS::WEAK_ATTACK || status == STATUS::HARD_ATTACK || status == STATUS::HITTED)
				{
					continue;
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

					PxVec3 distance = direction * elapsedTime * 20.f;
					distance.y += jump_height;

					PxControllerFilters filters;
					if (clients[client_id].playerinfo->m_PlayerController) {
						const PxU32 flags = clients[client_id].playerinfo->m_PlayerController->move(distance, 0.001, 1 / 60, filters);

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

	for (int i = 0; i < MAX_ROOM_USER; ++i)
	{
		int client_id = it->clientNum[i];
		if (client_id != -1)
		{
			if (clients[client_id].connected == true)
			{
				if (clients[client_id].playerinfo->hitted == true)
				{
					clients[client_id].playerinfo->setStatus(STATUS::HITTED);
					add_timer(room_num, client_id, EV_FREE, high_resolution_clock::now() + 660ms);

					sc_packet_anim p_anim;
					p_anim.type = SC_ANIM;
					p_anim.size = sizeof(sc_packet_anim);
					p_anim.id = client_id;
					p_anim.ani_index = clients[client_id].playerinfo->m_AniIndex;

					for (int j = 0; j < MAX_ROOM_USER; ++j)
					{
						if (clients[it->clientNum[j]].connected == true)
						{
							if (it->clientNum[j] != -1)
							{
								if (clients[it->clientNum[j]].connected == true)
								{
									send_anim_packet(it->clientNum[j], client_id);
								}
							}
						}
					}
					clients[client_id].playerinfo->hitted = false;
				}

				//clients[i].playerinfo->animate(fTime);

				if (clients[client_id].playerinfo->m_PlayerController != nullptr) {
					PxExtendedVec3 position = clients[client_id].playerinfo->m_PlayerController->getPosition();
					//cout << int(i) << " Client Pos : " << position.x << "," << position.y - 17.5 << "," << position.z << endl;

					clients[client_id].playerinfo->m_Pos.x = position.x;
					clients[client_id].playerinfo->m_Pos.y = position.y - 17.5;
					clients[client_id].playerinfo->m_Pos.z = position.z;
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
				for (int j = 0; j < MAX_ROOM_USER; ++j)
				{
					if (it->clientNum[j] != -1)
					{
						if (clients[it->clientNum[j]].connected == true)
						{
							send_pos_packet(it->clientNum[i], j);
						}
					}
				}
			}
		}
	}
}

void logic()
{
	gGameTimer.Reset();
	while (true)
	{
		gGameTimer.Tick(60.0f);
		for (auto it = gRoom.begin(); it != gRoom.end(); ++it)
		{
			if (it->room_status == 0)
			{
				continue;
			}
			else if (it->room_status == 1)
			{
				bool result = it->all_load_complete();

				if (result == true)
				{
					send_room_setting_weapon(it->room_num);
					send_room_setting_player(it->room_num);

					it->room_status = 2;

					cout << "방 준비 끝\n";
					continue;
				}
			}
			else if (it->room_status == 2)
			{
				clientInputProcess(it->room_num);

				it->m_pPhysx->m_Scene->simulate(1.f / 60.f);
				it->m_pPhysx->m_Scene->fetchResults(true);

				clientUpdateProcess(it->room_num);

				it->PosBroadCastTime += 1.f / 60.f;
				if (it->PosBroadCastTime > 0.1f)
				{
					int room_num = it->room_num;
					broadcastPosPacket(room_num);
					it->PosBroadCastTime = 0.0f;
				}
			}
		}
	}
}

void mapLoad()
{
	ifstream in("map.txt");
	int i;
	float f;
	PxVec3 vertex;

	while (true) {
		in >> i;
		int ver_size = i;
		for (int j = 0; j < ver_size; ++j) {
			in >> f;
			vertex.x = f;
			in >> f;
			vertex.y = f;
			in >> f;
			vertex.z = f;

			gMapVertex.push_back(vertex);
		}

		in >> i;
		int index_size = i;
		for (int j = 0; j < index_size; ++j) {
			in >> i;
			gMapIndex.push_back(i);
		}
		break;
	}
	in.close();
}

int main()
{
	mapLoad();

	for (int i = 0; i < MAX_USER; ++i)
	{
		clients[i].connected = false;
	}

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