#include "Weapon.h"

CWeapon::CWeapon()
{
	owner = -1;
	empty = true;
	pos = PxVec3(1000.f, 1000.f, 1000.f);
}


CWeapon::~CWeapon()
{
}
