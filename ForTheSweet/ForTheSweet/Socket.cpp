#include "stdafx.h"
#include "Socket.h"


CSocket::CSocket()
{
	memset(buf, 0, MAX_PACKET_SIZE);
}


CSocket::~CSocket()
{
	closesocket(clientSocket);
}

void CSocket::Release()
{
	if (clientSocket) closesocket(clientSocket);
	WSACleanup();
}

bool CSocket::init()
{
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return false;
	}

	clientSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (clientSocket == INVALID_SOCKET)
	{
		return false;
	}

	int option = TRUE;
	setsockopt(clientSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&option, sizeof(option));

	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	//serveraddr.sin_addr.s_addr = inet_addr(serverip);
	//serverAddr.sin_addr.s_addr = inet_addr("218.37.39.194");
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serverAddr.sin_port = htons(SERVER_PORT);
	if (connect(clientSocket, (SOCKADDR *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		cout << "Fail Connect\n";
		return false;
	}

	return true;
}

void CSocket::sendPacket(char type, char key, char state, char id)
{
	switch (type)
	{
	case CS_CONNECT:
		cs_packet_connect p_connect;
		p_connect.type = CS_CONNECT;
		p_connect.size = sizeof(cs_packet_connect);
		send(clientSocket, (char *)&p_connect, sizeof(cs_packet_connect), 0);
		break;

	case CS_MOVE:
		//cout << "MOVE~~~~~~~~~~~~~~~~~~\n";
		cs_packet_move p_move;
		p_move.type = CS_MOVE;
		p_move.key = key;
		p_move.state = state;
		p_move.size = sizeof(cs_packet_move);
		send(clientSocket, (char *)&p_move, sizeof(cs_packet_move), 0);
		break;

	case CS_DISCONNECT:
		cs_packet_disconnect p_disconnect;
		p_disconnect.type = CS_DISCONNECT;
		p_disconnect.size = sizeof(cs_packet_disconnect);
		//send(clientSocket, (char *)moveflag, sizeof(cs_packet_disconnect), 0);
		break;

	case CS_ATTACK:
		cs_packet_anim p_anim;
		p_anim.type = CS_ATTACK;
		p_anim.size = sizeof(cs_packet_anim);
		p_anim.key = key;
		send(clientSocket, (char *)&p_anim, sizeof(cs_packet_anim), 0);
		break;

	case CS_WEAPON:
		cs_packet_weapon p_weapon;
		p_weapon.type = CS_WEAPON;
		p_weapon.size = sizeof(cs_packet_weapon);
		p_weapon.weapon_type = key;
		p_weapon.weapon_index = state;
		send(clientSocket, (char *)&p_weapon, sizeof(cs_packet_weapon), 0);
		break;
	}
}

void CSocket::recvCallBack()
{
	sc_packet_login p_login;
	sc_packet_pos p_pos;
	sc_packet_put_player p_put;
	sc_packet_remove p_remove;
	int retval;

	while (true)
	{
		retval = recv(clientSocket, buf, MAX_PACKET_SIZE, 0);

		if (retval == 0 || retval == SOCKET_ERROR)
		{
			closesocket(clientSocket);
			exit(1);
			break;
		}

		while (retval > 0)
		{
			BYTE type = buf[0];

			if (type == SC_LOGIN)
			{
				memset(&p_login, 0, sizeof(p_login));
				memcpy(&p_login, buf, sizeof(p_login));

				memmove(buf, buf + sizeof(p_login), retval - sizeof(p_login));
				retval -= sizeof(p_login);
			}
			if (type == SC_PUT_PLAYER)
			{
				memset(&p_put, 0, sizeof(p_put));
				memcpy(&p_put, buf, sizeof(p_put));

				memmove(buf, buf + sizeof(p_put), retval - sizeof(p_put));
				retval -= sizeof(p_put);
			}
			if (type == SC_POS) {
				memset(&p_pos, 0, sizeof(p_pos));
				memcpy(&p_pos, buf, sizeof(p_pos));



				// 여러 Client Position 정보가 버퍼에 누적되어있을 수도 있으니 땡겨주자.
				memmove(buf, buf + sizeof(p_pos), retval - sizeof(p_pos));
				retval -= sizeof(p_pos);
			}
			if (type == SC_REMOVE) {
				memset(&p_remove, 0, sizeof(p_remove));
				memcpy(&p_remove, buf, sizeof(p_remove));

				// 여러 Client Position 정보가 버퍼에 누적되어있을 수도 있으니 땡겨주자.
				memmove(buf, buf + sizeof(p_remove), retval - sizeof(p_remove));
				retval -= sizeof(p_remove);
			}

		}
		memset(buf, 0, sizeof(MAX_PACKET_SIZE));
	}
}
