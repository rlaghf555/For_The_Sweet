#pragma once

// INCLUDES ///////////////////////////////////////////////
#define WIN32_LEAN_AND_MEAN  
#define INITGUID
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "Protocol.h"

class CSocket
{
public:
	WSADATA wsa;
	SOCKET clientSocket;
	SOCKADDR_IN serverAddr;
	char buf[MAX_PACKET_SIZE];
	//char id = -1;

public:
	CSocket();
	~CSocket();
	
	void Release();

	bool init();
	void sendPacket(char type, char key, char state, char id);
	void recvCallBack();
	
};

