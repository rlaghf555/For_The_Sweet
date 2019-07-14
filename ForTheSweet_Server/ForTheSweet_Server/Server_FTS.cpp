#define WIN32_LEAN_AND_MEAN  
#define INITGUID
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "header.h"
#include "Protocol.h"
#include "Player.h"
#include "Physx.h"
#include "Timer.h"
#include "Util.h"

#pragma comment(lib, "ws2_32.lib")

#define MAX_BUFFER 1024

PxVec3 PlayerInitPosition[8] = {
   PxVec3(0, 10.1, 100), PxVec3(50, 10.1, 100), PxVec3(-50, 10.1, 100), PxVec3(100, 10.1, 100), PxVec3(-100, 10.1, 100),
   PxVec3(150, 10.1, 100), PxVec3(-150, 10.1, 100), PxVec3(200, 10.1, 100)
};

char Weapon[MAX_WEAPON_TYPE][MAX_WEAPON_NUM];

HANDLE g_iocp;

CPhysx *gPhysx;

vector<PxVec3> gMapVertex;
vector<int> gMapIndex;
float gAniInfo[MAX_ANIM];

CGameTimer gGameTimer;
volatile bool start = false;

enum EVENT_TYPE {
	EV_FREE, EV_HIT,												// 플레이어에 해당
	EV_WEAPON, EV_FOG, EV_FEVER, EV_LIGHTNING, EV_SLIME				// 심판에 해당
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

void add_timer(int id, EVENT_TYPE et, high_resolution_clock::time_point start_time, char attack_count = 0)
{
	timer_l.lock();
	timer_queue.push(EVENT_ST{ id, et, start_time, attack_count });
	timer_l.unlock();
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

void process_packet(char key, char *buffer)
{
	char Anim_Index;

	switch (buffer[1]) {
	case CS_CONNECT:
		cout << "[" << int(key) << "] Clients Login\n";

		clients[key].playerinfo->setPosition(PlayerInitPosition[key]);
		clients[key].playerinfo->setVelocity(PxVec3(0, 0, 0));
		clients[key].playerinfo->setLook(PxVec3(0, 0, 1));
		clients[key].playerinfo->setDashed(false);
		clients[key].playerinfo->setPlayerController(gPhysx);
		clients[key].playerinfo->setTrigger(gPhysx);
		gPhysx->registerPlayer(clients[key].playerinfo, key);
		clients[key].connected = true;

		send_login_packet(key);

		// 자신(key)과 다른 클라에게 자기 위치 정보 Send
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (clients[i].connected == true)
			{
				if (i != key)
				{
					//cout << "Put Packet Send\n";
					send_put_player_packet(i, key);
				}
			}
		}
		// 로그인 시, 접속 중인 다른 클라 위치 정보를 자신에게 Send
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (clients[i].connected == true)
			{
				if (i != key)
				{
					send_put_player_packet(key, i);
				}
			}
		}
		break;

	case CS_DISCONNECT:
		cout << "[" << int(key) << "] Clients Disconnect\n";
		break;

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
				clients[key].playerinfo->setLook(Normalize(clients[key].playerinfo->m_Vel));
			}
			else {
				clients[key].playerinfo->setAniIndex(Anim::Walk);
				clients[key].playerinfo->setLook(Normalize(clients[key].playerinfo->m_Vel));
			}

		}

		//cout << "[" << int(key) << "] Clients Move => " << clients[key].playerinfo->m_Vel.x << ","
		//	<< clients[key].playerinfo->m_Vel.y << "," << clients[key].playerinfo->m_Vel.z << "\n";
		//cout << p_pos.vx << "," << p_pos.vy << "," << p_pos.vz << endl;

		// Move한 정보를 브로드캐스팅
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (clients[i].connected == true)
				send_pos_packet(i, key);
		}
		break;
	}

	case CS_ATTACK:
		cs_packet_anim *p_anim;
		p_anim = reinterpret_cast<cs_packet_anim*>(buffer);

		Anim_Index = clients[key].playerinfo->m_AniIndex;

		if (p_anim->key == CS_JUMP) {
			cout << "jump" << endl;
			clients[key].playerinfo->setAniIndex(Anim::Jump);
			clients[key].playerinfo->jumpstart();
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
				add_timer(key, EV_FREE, clients[key].playerinfo->attack_time + 660ms, 1);
			}
			else if (p_anim->count == 1) {
				clients[key].playerinfo->setAniIndex(Anim::Weak_Attack2);
				clients[key].playerinfo->attack_count = 2;
				add_timer(key, EV_FREE, clients[key].playerinfo->attack_time + 1330ms, 2);
			}
			else if (p_anim->count == 2) {
				clients[key].playerinfo->setAniIndex(Anim::Weak_Attack3);
				clients[key].playerinfo->attack_count = 3;
				add_timer(key, EV_FREE, clients[key].playerinfo->attack_time + 1330ms, 3);
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
				add_timer(key, EV_FREE, clients[key].playerinfo->attack_time + 660ms, 1);
			}
			else if (p_anim->count == 1) {
				clients[key].playerinfo->setAniIndex(Anim::Hard_Attack2);
				clients[key].playerinfo->attack_count = 2;
				add_timer(key, EV_FREE, clients[key].playerinfo->attack_time + 1330ms, 2);
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

		for (int i = 0; i < MAX_USER; ++i)
		{
			if (clients[i].connected == true)
				send_anim_packet(i, key);
		}
		break;

	case CS_WEAPON:
		cs_packet_weapon *p_weapon;
		p_weapon = reinterpret_cast<cs_packet_weapon*>(buffer);

		clients[key].playerinfo->weapon_type = p_weapon->weapon_type;
		clients[key].playerinfo->weapon_index = p_weapon->weapon_index;
		clients[key].playerinfo->setAniIndex(Anim::Pick_Up);

		//cout << int(key) << " Player Weapon Success : " << int(p_weapon->weapon_type) << ", " << int(p_weapon->weapon_index) << endl;;

		for (int i = 0; i < MAX_USER; ++i)
		{
			if (clients[i].connected == true)
				send_anim_packet(i, key);
		}
		break;
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

			for (int i = 0; i < MAX_USER; ++i)
			{
				if (clients[i].connected == true)
					if (i != key)
						sendPacket(i, &p_remove);
					else {
						closesocket(clients[i].socket);
						clients[i].connected = false;
						clients[i].playerinfo->m_PlayerController->release();  clients[i].playerinfo->m_PlayerController = nullptr;
						clients[i].playerinfo->m_AttackTrigger->release(); clients[i].playerinfo->m_AttackTrigger = nullptr;
						//gPhysx->removePlayer(i);
					}
			}
			continue;
		}
		if (io_byte == 0)
		{
			//cout << "[" << int(key) << "] Clients Disconnect\n";
			sc_packet_remove p_remove;
			p_remove.id = key;
			p_remove.type = SC_REMOVE;
			p_remove.size = sizeof(sc_packet_remove);

			for (int i = 0; i < MAX_USER; ++i)
			{
				if (clients[i].connected == true)
					if (i != key)
						sendPacket(i, &p_remove);
					else {
						clients[i].connected = false;
						clients[i].playerinfo->m_PlayerController->release();
						clients[i].playerinfo->m_AttackTrigger->release();
					}
			}
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

		start = true;

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

		//clients[new_id].connected = true;

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

void clientInputProcess()
{
	for (char i = 0; i < MAX_USER; ++i)
	{
		if (clients[i].connected == true)
		{
			int Ani_Index = clients[i].playerinfo->m_AniIndex;

			/*
			float Anim_Time = clients[i].playerinfo->m_AniFrame;

			if (Ani_Index == Anim::Pick_Up) {
				if (Anim_Time > 20.f) {
					char weapon_type = clients[i].playerinfo->weapon_type;
					char weapon_index = clients[i].playerinfo->weapon_index;

					if (Weapon[weapon_type][weapon_index] == -1)
					{
						Weapon[weapon_type][weapon_index] = i;

						sc_packet_weapon p_weapon2;
						p_weapon2.type = SC_WEAPON;
						p_weapon2.size = sizeof(sc_packet_weapon);
						p_weapon2.id = i;
						p_weapon2.weapon_type = weapon_type;
						p_weapon2.weapon_index = weapon_index;
						p_weapon2.weapon_success = 1;

						for (int j = 0; j < MAX_USER; ++j)
						{
							if (clients[j].connected == true)
							{
								sendPacket(j, &p_weapon2);
							}
						}
					}
				}
			}
			*/

			if (clients[i].playerinfo->m_AttackTrigger)
			{
				clients[i].playerinfo->m_AttackTrigger->setGlobalPose(PxTransform(100, 100, 100));

				PxTransform triggerpos(PxVec3(0, 0, 0));
				PxExtendedVec3 playerpos = clients[i].playerinfo->m_PlayerController->getPosition();
				PxVec3 look = clients[i].playerinfo->m_Look;

				/*
				if (Ani_Index == Anim::Weak_Attack1 || Ani_Index == Anim::Weak_Attack2 || Ani_Index == Anim::Weak_Attack3) {
					if ((Anim_Time > 10 && Anim_Time < 15) || (Anim_Time > 22 && Anim_Time < 27) || (Anim_Time > 32 && Anim_Time < 37))
					{
						triggerpos.p.x = playerpos.x + (look.x * 10);
						triggerpos.p.y = playerpos.y + (look.y * 10);
						triggerpos.p.z = playerpos.z + (look.z * 10);

						clients[i].playerinfo->m_AttackTrigger->setGlobalPose(triggerpos);
					}
				}
				else if (Ani_Index == Anim::Hard_Attack1 || Ani_Index == Anim::Hard_Attack2) {
					if ((Anim_Time > 14 && Anim_Time < 19) || (Anim_Time > 28 && Anim_Time < 33))
					{
						triggerpos.p.x = playerpos.x + (look.x * 10);
						triggerpos.p.y = playerpos.y + (look.y * 10);
						triggerpos.p.z = playerpos.z + (look.z * 10);

						clients[i].playerinfo->m_AttackTrigger->setGlobalPose(triggerpos);
					}
				}
				else if (Ani_Index == Anim::Lollipop_Attack1 || Ani_Index == Anim::Lollipop_Attack2) {
					if ((Anim_Time > 13 && Anim_Time < 18) || (Anim_Time > 28 && Anim_Time < 33))
					{
						triggerpos.p.x = playerpos.x + (look.x * 10);
						triggerpos.p.y = playerpos.y + (look.y * 10);
						triggerpos.p.z = playerpos.z + (look.z * 10);

						clients[i].playerinfo->m_AttackTrigger->setGlobalPose(triggerpos);
					}
				}
				else if (Ani_Index == Anim::Lollipop_Hard_Attack) {
					if (Anim_Time > 13 && Anim_Time < 18)
					{
						triggerpos.p.x = playerpos.x + (look.x * 10);
						triggerpos.p.y = playerpos.y + (look.y * 10);
						triggerpos.p.z = playerpos.z + (look.z * 10);

						clients[i].playerinfo->m_AttackTrigger->setGlobalPose(triggerpos);
					}
				}
				*/
				char status = clients[i].playerinfo->m_status;

				if (status == STATUS::DEFENSE || status == STATUS::WEAK_ATTACK || status == STATUS::HARD_ATTACK)
				{
					continue;
				}
				else
				{
					float jump_height;

					if (clients[i].playerinfo->m_Jump.mJump == true) {
						jump_height = clients[i].playerinfo->m_Jump.getHeight(gGameTimer.GetTimeElapsed());
					}
					else {
						clients[i].playerinfo->m_Fall.startJump(0);
						jump_height = clients[i].playerinfo->m_Fall.getHeight(gGameTimer.GetTimeElapsed());
					}

					PxVec3 direction;
					PxVec3 velocity = clients[i].playerinfo->m_Vel;

					direction = velocity.getNormalized();

					if (clients[i].playerinfo->m_dashed) {
						direction *= 2;
					}

					//cout << int(i) << " Vel : " << direction.x << ", " << direction.y << ", " << direction.z << endl;
					float elapsedTime = gGameTimer.GetTimeElapsed();

					PxVec3 distance = direction * elapsedTime * 20.f;
					distance.y += jump_height;

					PxControllerFilters filters;
					if (clients[i].playerinfo->m_PlayerController) {
						const PxU32 flags = clients[i].playerinfo->m_PlayerController->move(distance, 0.001, 1 / 60, filters);

						if (flags & PxControllerCollisionFlag::eCOLLISION_DOWN)
						{
							//cout << "충돌\n";
							if (clients[i].playerinfo->m_Jump.mJump) {
								clients[i].playerinfo->m_Jump.stopJump();
								if (velocity.magnitude() > 0.f)
								{
									if (clients[i].playerinfo->m_dashed == true)
										clients[i].playerinfo->setAniIndex(Anim::Run);
									else
										clients[i].playerinfo->setAniIndex(Anim::Walk);
								}
								else {
									clients[i].playerinfo->setAniIndex(Anim::Idle);
								}
								clients[i].playerinfo->setStatus(STATUS::FREE);
							}
							if (clients[i].playerinfo->m_Fall.mJump) {
								clients[i].playerinfo->m_Fall.stopJump();
							}
						}
					}
				}
			}
		}
	}

	while (true)
	{
		timer_l.lock();
		if (true == timer_queue.empty()) {
			timer_l.unlock();
			break;
		}
		EVENT_ST ev = timer_queue.top();
		if (ev.start_time > high_resolution_clock::now()) {
			timer_l.unlock();
			break;
		}
		timer_queue.pop();
		timer_l.unlock();

		process_event(ev);
	}
}

void clientUpdateProcess(float fTime)
{
	for (int i = 0; i < MAX_USER; ++i)
	{
		if (clients[i].connected == true)
		{
			if (clients[i].playerinfo->hitted == true) {

				sc_packet_anim p_anim;
				p_anim.type = SC_ANIM;
				p_anim.size = sizeof(sc_packet_anim);
				p_anim.id = i;
				p_anim.ani_index = clients[i].playerinfo->m_AniIndex;

				for (int j = 0; j < MAX_USER; ++j)
				{
					if (clients[j].connected == true)
						sendPacket(j, &p_anim);
				}

				clients[i].playerinfo->hitted = false;
			}

			//clients[i].playerinfo->animate(fTime);

			if (clients[i].playerinfo->m_PlayerController != nullptr) {
				PxExtendedVec3 position = clients[i].playerinfo->m_PlayerController->getPosition();
				//cout << int(i) << " Client Pos : " << position.x << "," << position.y - 17.5 << "," << position.z << endl;

				clients[i].playerinfo->m_Pos.x = position.x;
				clients[i].playerinfo->m_Pos.y = position.y - 17.5;
				clients[i].playerinfo->m_Pos.z = position.z;
			}
		}
	}
}

void broadcastPosPacket()
{
	//cout << "BroadCast\n";
	for (int i = 0; i < MAX_USER; ++i)
	{
		if (clients[i].connected == true)
		{
			for (int j = 0; j < MAX_USER; ++j)
			{
				if (clients[j].connected == true)
				{
					send_pos_packet(j, i);
				}
			}
		}
	}
}

void logic()
{
	float PosBroadCastTime = 0.0f;

	gGameTimer.Reset();
	while (true)
	{
		if (start)
		{
			gGameTimer.Tick(60.0f);
			PosBroadCastTime += 1.f / 60.f;

			clientInputProcess();

			gPhysx->m_Scene->simulate(1.f / 60.f);
			gPhysx->m_Scene->fetchResults(true);

			clientUpdateProcess(gGameTimer.GetTimeElapsed());

			if (PosBroadCastTime > 0.1f)
			{
				broadcastPosPacket();
				PosBroadCastTime = 0.0f;
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

	PxTriangleMesh* triMesh = gPhysx->GetTriangleMesh(gMapVertex, gMapIndex);

	PxVec3 scaleTmp = PxVec3(1.0f, 1.0f, 1.0f);

	PxMeshScale PxScale;
	PxScale.scale = scaleTmp;

	PxTriangleMeshGeometry meshGeo(triMesh, PxScale);
	PxTransform location(0, 0, 0);

	PxMaterial* mat = gPhysx->m_Physics->createMaterial(0.2f, 0.2f, 0.2f);

	PxRigidActor* m_Actor = PxCreateStatic(*gPhysx->m_Physics, location, meshGeo, *mat);
	gPhysx->m_Scene->addActor(*m_Actor);
}

void aniLoad()
{
	ifstream in("ani.txt");
	int i;
	float f;

	while (in) {
		in >> i;
		in >> f;

		gAniInfo[i] = f;
	}
	in.close();

	//for (auto d : aniInfo)
	//{
	//   cout << d.first << " : " << d.second << endl;
	//}
}

int main()
{
	gPhysx = new CPhysx;
	gPhysx->initPhysics();

	mapLoad();
	aniLoad();

	for (int i = 0; i < MAX_WEAPON_TYPE; ++i)
		for (int j = 0; j < MAX_WEAPON_NUM; ++j)
			Weapon[i][j] = -1;

	for (int i = 0; i < MAX_USER; ++i)
	{
		clients[i].connected = false;
		clients[i].playerinfo = new CPlayer();
		clients[i].playerinfo->setAniInfo(gAniInfo);
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