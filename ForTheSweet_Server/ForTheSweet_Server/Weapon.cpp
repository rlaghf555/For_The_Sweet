#include "Weapon.h"

CWeapon::CWeapon()
{
	owner = -1;
	empty = true;
	respawn_index = -1;
	pos = PxVec3(1000.f, 1000.f, 1000.f);
}


CWeapon::~CWeapon()
{
}

void CWeapon::init()
{
	owner = -1;
	empty = true;
	respawn_index = -1;
	pos = PxVec3(1000.f, 1000.f, 1000.f);
}
