#pragma once
#include "Physx.h"

struct CWeapon_Respawn {
	bool respawn_able = true;
	char type = 0;
	char index = 0;
};

class CWeapon
{
public:
	char owner;
	bool empty;
	PxVec3 pos;

public:
	CWeapon();
	~CWeapon();

	void SetOwner(char own) { owner = own; }
	void SetEmpty(bool emp) { empty = emp; }
};

