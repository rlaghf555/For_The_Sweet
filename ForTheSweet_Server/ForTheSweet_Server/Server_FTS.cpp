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

HANDLE g_iocp;

CPhysx *gPhysx;

vector<PxVec3> gMapVertex;
vector<int> gMapIndex;
vector<pair<int, float>> aniInfo;

CGameTimer gGameTimer;
volatile bool start = false;

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
	bool connected;

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
			cout << "Error1 - IO pending Failure\n";
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
			cout << "Error1 - IO pending Failure\n";
		}
	}
	else {
		//cout << "Non Overlapped Send~~~~~~~~~~~~.\n";
	}
}

void process_packet(char key, char *buffer)
{
	switch (buffer[1]) {
	case CS_CONNECT:
		cout << "[" << int(key) << "] Clients Login\n";
		sc_packet_login p_login;
		p_login.id = key;
		p_login.x = PlayerInitPosition[key].x;
		p_login.y = PlayerInitPosition[key].y;
		p_login.z = PlayerInitPosition[key].z;
		p_login.vx = 0.f;
		p_login.vy = 0.f;
		p_login.vz = 0.f;
		p_login.type = SC_LOGIN;
		p_login.size = sizeof(sc_packet_login);

		sc_packet_put_player p_put;
		p_put.id = key;
		p_put.x = PlayerInitPosition[key].x;
		p_put.y = PlayerInitPosition[key].y;
		p_put.z = PlayerInitPosition[key].z;
		p_put.vx = 0.f;
		p_put.vy = 0.f;
		p_put.vz = 0.f;
		p_put.ani_index = Anim::Idle;
		p_put.ani_frame = 0.0f;
		p_put.type = SC_PUT_PLAYER;
		p_put.size = sizeof(sc_packet_put_player);

		clients[key].playerinfo->setPosition(PlayerInitPosition[key]);
		clients[key].playerinfo->setVelocity(PxVec3(0, 0, 0));
		clients[key].playerinfo->setLook(PxVec3(0, 0, 1));
		clients[key].playerinfo->setPlayerController(gPhysx);
		clients[key].playerinfo->setTrigger(gPhysx);
		gPhysx->registerPlayer(clients[key].playerinfo, key);
		clients[key].connected = true;

		cout << "Login Packet Send\n";
		sendPacket(key, &p_login);

		// 자신(key)과 다른 클라에게 자기 위치 정보 Send
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (clients[i].connected)
			{
				if (i != key)
				{
					cout << "Put Packet Send\n";
					sendPacket(i, &p_put);
				}
			}
		}
		// 로그인 시, 접속 중인 다른 클라 위치 정보를 자신에게 Send
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (clients[i].connected)
			{
				if (i != key)
				{
					p_put.id = i;
					p_put.x = clients[i].playerinfo->m_Pos.x;
					p_put.y = clients[i].playerinfo->m_Pos.y;
					p_put.z = clients[i].playerinfo->m_Pos.z;
					p_put.vx = clients[i].playerinfo->m_Vel.x;
					p_put.vy = clients[i].playerinfo->m_Vel.y;
					p_put.vz = clients[i].playerinfo->m_Vel.z;
					p_put.ani_index = clients[i].playerinfo->m_AniIndex;
					p_put.ani_frame = clients[i].playerinfo->m_AniFrame;
					p_put.type = SC_PUT_PLAYER;
					p_put.size = sizeof(sc_packet_put_player);
					sendPacket(key, &p_put);
				}
			}
		}
		break;

	case CS_DISCONNECT:
		cout << "[" << int(key) << "] Clients Disconnect\n";
		break;

	case CS_MOVE:
		cs_packet_move *p_move;
		p_move = reinterpret_cast<cs_packet_move*>(buffer);


		if (p_move->key == CS_UP && p_move->state == 0) {  // forward
			clients[key].playerinfo->m_Vel.z = 0.f;
		}
		if (p_move->key == CS_DOWN && p_move->state == 0) {   // backward
			clients[key].playerinfo->m_Vel.z = 0.f;
		}
		if (p_move->key == CS_LEFT && p_move->state == 0) {   // left
			clients[key].playerinfo->m_Vel.x = 0.f;
		}
		if (p_move->key == CS_RIGHT && p_move->state == 0) {   // right
			clients[key].playerinfo->m_Vel.x = 0.f;
		}
		if (p_move->key == CS_UP && p_move->state == 1) {  // forward
			clients[key].playerinfo->m_Vel.z = 1.f;
		}
		if (p_move->key == CS_DOWN && p_move->state == 1) {   // backward
			clients[key].playerinfo->m_Vel.z = -1.f;
		}
		if (p_move->key == CS_LEFT && p_move->state == 1) {   // left
			clients[key].playerinfo->m_Vel.x = -1.f;
		}
		if (p_move->key == CS_RIGHT && p_move->state == 1) {   // right
			clients[key].playerinfo->m_Vel.x = 1.f;
		}

		clients[key].playerinfo->setVelocity(Normalize(clients[key].playerinfo->m_Vel));
		if (clients[key].playerinfo->m_Vel.x == 0.0f && clients[key].playerinfo->m_Vel.z == 0.0f) {
			clients[key].playerinfo->m_AniIndex = Anim::Idle;
		}
		else {
			clients[key].playerinfo->m_AniIndex = Anim::Walk;
			clients[key].playerinfo->setLook(Normalize(clients[key].playerinfo->m_Vel));
		}
		clients[key].playerinfo->m_AniFrame = 0.0f;

		cout << "[" << int(key) << "] Clients Move => " << clients[key].playerinfo->m_Vel.x << ","
			<< clients[key].playerinfo->m_Vel.y << "," << clients[key].playerinfo->m_Vel.z << "\n";

		sc_packet_pos p_pos;
		p_pos.id = key;
		p_pos.x = clients[key].playerinfo->m_Pos.x;
		p_pos.y = clients[key].playerinfo->m_Pos.y;
		p_pos.z = clients[key].playerinfo->m_Pos.z;
		p_pos.vx = clients[key].playerinfo->m_Vel.x;
		p_pos.vy = clients[key].playerinfo->m_Vel.y;
		p_pos.vz = clients[key].playerinfo->m_Vel.z;
		p_pos.ani_index = clients[key].playerinfo->m_AniIndex;
		p_pos.ani_frame = clients[key].playerinfo->m_AniFrame;
		p_pos.type = SC_POS;
		p_pos.size = sizeof(sc_packet_pos);

		//cout << p_pos.vx << "," << p_pos.vy << "," << p_pos.vz << endl;

		// Move한 정보를 브로드캐스팅
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (clients[i].connected)
				sendPacket(i, &p_pos);
		}
		break;

	case CS_ATTACK:
		cs_packet_anim *p_anim;
		p_anim = reinterpret_cast<cs_packet_anim*>(buffer);

		char Anim_Index = clients[key].playerinfo->m_AniIndex;
		float Anim_Time = clients[key].playerinfo->m_AniFrame;

		if (p_anim->key == CS_JUMP) {
			clients[key].playerinfo->setAniIndex(Anim::Jump);
			clients[key].playerinfo->setAniFrame(0.0f);
			clients[key].playerinfo->setAniLoop(false);
			clients[key].playerinfo->jumpstart();
		}

		if (p_anim->key == CS_GUARD) {                        // 예외처리 필수!
			clients[key].playerinfo->setAniIndex(Anim::Guard);
			clients[key].playerinfo->setAniFrame(10.0f);
			clients[key].playerinfo->setAniLoop(false);
		}
		if (p_anim->key == CS_WEAK) {
			if (Anim_Index == Anim::Idle || Anim_Index == Anim::Walk) {
				clients[key].playerinfo->setAniIndex(Anim::Weak_Attack1);
				clients[key].playerinfo->setAniFrame(0.0f);
				clients[key].playerinfo->setAniLoop(false);
			}
			if (Anim_Index == Anim::Weak_Attack1 && (Anim_Time > 10 && Anim_Time < 15)) {
				clients[key].playerinfo->setAniIndex(Anim::Weak_Attack2);
				clients[key].playerinfo->setAniFrame(Anim_Time);
				clients[key].playerinfo->setAniLoop(false);
			}
			if (Anim_Index == Anim::Weak_Attack2 && (Anim_Time > 20 && Anim_Time < 25)) {
				clients[key].playerinfo->setAniIndex(Anim::Weak_Attack3);
				clients[key].playerinfo->setAniFrame(Anim_Time);
				clients[key].playerinfo->setAniLoop(false);
			}
		}
		if (p_anim->key == CS_HARD) {
			if (Anim_Index == Anim::Idle || Anim_Index == Anim::Walk) {
				clients[key].playerinfo->setAniIndex(Anim::Hard_Attack1);
				clients[key].playerinfo->setAniFrame(0.0f);
				clients[key].playerinfo->setAniLoop(false);
			}
			if (Anim_Index == Anim::Hard_Attack1 && (Anim_Time > 10 && Anim_Time < 20)) {
				clients[key].playerinfo->setAniIndex(Anim::Hard_Attack2);
				clients[key].playerinfo->setAniFrame(Anim_Time);
				clients[key].playerinfo->setAniLoop(false);
			}
		}

		sc_packet_anim p_anim2;
		p_anim2.type = SC_ANIM;
		p_anim2.size = sizeof(sc_packet_anim);
		p_anim2.id = key;
		p_anim2.ani_index = clients[key].playerinfo->m_AniIndex;
		p_anim2.ani_frame = clients[key].playerinfo->m_AniFrame;

		for (int i = 0; i < MAX_USER; ++i)
		{
			if (clients[i].connected)
				sendPacket(i, &p_anim2);
		}
		break;
	}
}

void worker_thread()
{
	while (true)
	{
		ULONG io_byte;
		ULONG io_key;
		OVER_EX *over_ex;

		BOOL is_error = GetQueuedCompletionStatus(g_iocp, &io_byte, (PULONG_PTR)&io_key, reinterpret_cast<LPWSAOVERLAPPED *>(&over_ex), INFINITE);

		char key = static_cast<char>(io_key);

		if (is_error == FALSE) {
			//cout << "Error in GQCS\n";
			//cout << "[" << int(key) << "] Clients Disconnect\n";
			sc_packet_remove p_remove;
			p_remove.id = key;
			p_remove.type = SC_REMOVE;
			p_remove.size = sizeof(sc_packet_remove);

			for (int i = 0; i < MAX_USER; ++i)
			{
				if (clients[i].connected)
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
		if (io_byte == 0)
		{
			//cout << "[" << int(key) << "] Clients Disconnect\n";
			sc_packet_remove p_remove;
			p_remove.id = key;
			p_remove.type = SC_REMOVE;
			p_remove.size = sizeof(sc_packet_remove);

			for (int i = 0; i < MAX_USER; ++i)
			{
				if (clients[i].connected)
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

		clients[new_id].connected = true;

		do_recv(new_id);
	}

	// 6-2. 리슨 소켓종료
	closesocket(listenSocket);

	// Winsock End
	WSACleanup();

	return;
}

void clientInputProcess()
{
	for (char i = 0; i < MAX_USER; ++i)
	{
		if (clients[i].connected == true)
		{
			if (clients[i].playerinfo->m_AttackTrigger)
			{
				clients[i].playerinfo->m_AttackTrigger->setGlobalPose(PxTransform(100, 100, 100));

				int Ani_Index = clients[i].playerinfo->m_AniIndex;
				float Anim_Time = clients[i].playerinfo->m_AniFrame;

				if (Ani_Index == Anim::Weak_Attack1 || Ani_Index == Anim::Weak_Attack2 || Ani_Index == Anim::Weak_Attack3) {
					if ((Anim_Time > 10 && Anim_Time < 15) || (Anim_Time > 22 && Anim_Time < 27) || (Anim_Time > 32 && Anim_Time < 37))
					{
						PxTransform triggerpos(PxVec3(0, 0, 0));
						PxExtendedVec3 playerpos = clients[i].playerinfo->m_PlayerController->getPosition();
						PxVec3 look = clients[i].playerinfo->m_Look;
						triggerpos.p.x = playerpos.x + (look.x * 10);
						triggerpos.p.y = playerpos.y + (look.y * 10);
						triggerpos.p.z = playerpos.z + (look.z * 10);

						//cout << "look : " << look.x << "," << look.y << "," << look.z << endl;
						//cout << "Ani time : " << Anim_Time << endl;
						//cout << "Trigger Pos : " << triggerpos.p.x << ", " << triggerpos.p.y << ", " << triggerpos.p.z << endl;
						clients[i].playerinfo->m_AttackTrigger->setGlobalPose(triggerpos);
					}
				}

				float jumpheight;
				jumpheight = clients[i].playerinfo->m_Jump.getHeight(gGameTimer.GetTimeElapsed());

				if (jumpheight == 0.0f) {
					jumpheight = -9.81 * gGameTimer.GetTimeElapsed();
				}

				PxVec3 direction = clients[i].playerinfo->m_Vel;
				//cout << int(i) << " Vel : " << direction.x << ", " << direction.y << ", " << direction.z << endl;
				float elapsedTime = gGameTimer.GetTimeElapsed();

				PxVec3 distance = direction * elapsedTime * 20.f;
				distance.y += jumpheight;

				PxControllerFilters filters;
				if (clients[i].playerinfo->m_PlayerController) {
					const PxU32 flags = clients[i].playerinfo->m_PlayerController->move(distance, 0.001, 1 / 60, filters);

					if (flags & PxControllerCollisionFlag::eCOLLISION_DOWN)
					{
						//cout << "충돌\n";
						clients[i].playerinfo->m_Jump.stopJump();
					}
				}
			}
		}
	}
}

void clientUpdateProcess(float fTime)
{
	for (char i = 0; i < MAX_USER; ++i)
	{
		if (clients[i].connected == true)
		{
			if (clients[i].playerinfo->hitted) {

				sc_packet_anim p_anim;
				p_anim.type = SC_ANIM;
				p_anim.size = sizeof(sc_packet_anim);
				p_anim.id = i;
				p_anim.ani_index = clients[i].playerinfo->m_AniIndex;
				p_anim.ani_frame = clients[i].playerinfo->m_AniFrame;

				for (int j = 0; j < MAX_USER; ++j)
				{
					if (clients[j].connected)
						sendPacket(j, &p_anim);
				}

				clients[i].playerinfo->hitted = false;
			}

			clients[i].playerinfo->animate(fTime);

			if (clients[i].playerinfo->m_PlayerController) {
				PxExtendedVec3 position = clients[i].playerinfo->m_PlayerController->getPosition();
				//cout << int(i) << " Client Pos : " << position.x << "," << position.y << "," << position.z << endl;

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
	for (char i = 0; i < MAX_USER; ++i)
	{
		if (clients[i].connected == true)
		{
			//clients[i].playerinfo->m_Vel = Normalize(clients[i].playerinfo->m_Vel);

			sc_packet_pos p_pos;
			p_pos.id = i;
			p_pos.x = clients[i].playerinfo->m_Pos.x;
			p_pos.y = clients[i].playerinfo->m_Pos.y;
			p_pos.z = clients[i].playerinfo->m_Pos.z;
			p_pos.vx = clients[i].playerinfo->m_Vel.x;
			p_pos.vy = clients[i].playerinfo->m_Vel.y;
			p_pos.vz = clients[i].playerinfo->m_Vel.z;
			p_pos.ani_index = clients[i].playerinfo->m_AniIndex;
			p_pos.ani_frame = clients[i].playerinfo->m_AniFrame;
			p_pos.type = SC_POS;
			p_pos.size = sizeof(sc_packet_pos);

			for (char j = 0; j < MAX_USER; ++j)
			{
				if (clients[j].connected == true)
				{
					//cout << p_pos.x << "," << p_pos.y << "," << p_pos.z << endl;
					sendPacket(j, &p_pos);
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

		aniInfo.push_back(make_pair(i, f));
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

	for (int i = 0; i < MAX_USER; ++i)
	{
		clients[i].connected = false;
		clients[i].playerinfo = new CPlayer();
		clients[i].playerinfo->setAniInfo(aniInfo);
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