#include "stdafx.h"
#include "Socket.h"


CSocket::CSocket()
{
	memset(buf, 0, MAX_PACKET_SIZE);
}

CSocket::CSocket(char * id, char * ip)
{
	m_pid = id;
	m_pip = ip;
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
	//char addr[30];
	//cout << "ipÁÖ¼Ò:";
	//cin >> addr;
	serverAddr.sin_addr.s_addr = inet_addr(m_pip);
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
	{
		cs_packet_connect p_connect;
		p_connect.type = CS_CONNECT;
		p_connect.size = sizeof(cs_packet_connect);
		send(clientSocket, (char *)&p_connect, sizeof(cs_packet_connect), 0);
		break;
	}
	case CS_MOVE:
	{
		//cout << "MOVE~~~~~~~~~~~~~~~~~~\n";
		cs_packet_move p_move;
		p_move.type = CS_MOVE;
		p_move.key = key;
		p_move.state = state;
		p_move.size = sizeof(cs_packet_move);
		send(clientSocket, (char *)&p_move, sizeof(cs_packet_move), 0);
		break;
	}

	case CS_DISCONNECT:
	{
		cs_packet_disconnect p_disconnect;
		p_disconnect.type = CS_DISCONNECT;
		p_disconnect.size = sizeof(cs_packet_disconnect);
		//send(clientSocket, (char *)moveflag, sizeof(cs_packet_disconnect), 0);
		break;
	}
	case CS_ATTACK:
	{
		cs_packet_anim p_anim;
		p_anim.type = CS_ATTACK;
		p_anim.size = sizeof(cs_packet_anim);
		p_anim.key = key;
		p_anim.count = state;
		send(clientSocket, (char *)&p_anim, sizeof(cs_packet_anim), 0);
		break;
	}
	case CS_WEAPON:
	{
		cs_packet_weapon p_weapon;
		p_weapon.type = CS_WEAPON;
		p_weapon.size = sizeof(cs_packet_weapon);
		p_weapon.weapon_type = key;
		p_weapon.weapon_index = state;
		send(clientSocket, (char *)&p_weapon, sizeof(cs_packet_weapon), 0);
		break;
	}
	case CS_WEAPON_SKILL:
	{
		cs_packet_weapon_skill p_weapon_skill;
		p_weapon_skill.type = CS_WEAPON_SKILL;
		p_weapon_skill.size = sizeof(cs_packet_weapon_skill);
		send(clientSocket, (char *)&p_weapon_skill, sizeof(cs_packet_weapon_skill), 0);
		break;
	}
	}
}