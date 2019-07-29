#include "Player.h"

CJump::CJump() :
	mV0(0.0f),
	mJumpTime(0.0f),
	mJump(false)
{
}

void CJump::startJump(PxF32 v0)
{
	if (mJump)	return;
	mJumpTime = 0.0f;
	mV0 = v0;
	mJump = true;
}

void CJump::stopJump()
{
	if (!mJump)	return;
	mJump = false;
}

PxF32 CJump::getHeight(PxF32 elapsedTime)
{
	if (!mJump)	return 0.0f;

	mJumpTime += elapsedTime;
	const PxF32 h = gJumpGravity * mJumpTime*mJumpTime + mV0 * mJumpTime;
	return h * elapsedTime;
	//return -1.0f;
}

CPlayer::CPlayer()
{
	m_Pos = PxVec3(0, 0, 0);
	m_Vel = PxVec3(0, 0, 0);
	m_Look = PxVec3(0, 0, 1);
	m_dashed = false;

	weapon_type = -1;
	weapon_index = -1;
	weapon_send = false;

	m_status = STATUS::FREE;
	attack_count = 0;

	m_PlayerController = nullptr;
	//m_HitReport = nullptr;
	m_AttackTrigger = nullptr;
}

CPlayer::~CPlayer()
{
}

void CPlayer::setPosition(PxVec3 pos)
{
	m_Pos = pos;
}

void CPlayer::setVelocity(PxVec3 vel)
{
	m_Vel = vel;
}

void CPlayer::setJumpVelocity(PxVec3 jumpVel)
{
	m_JumpVel = jumpVel;
}

void CPlayer::setDashed(bool dashed)
{
	m_dashed = dashed;
}

void CPlayer::setLook(PxVec3 look)
{
	m_Look = look;
}

void CPlayer::setAniIndex(char index)
{
	m_AniIndex = index;
}

void CPlayer::setStatus(char status)
{
	m_status = status;
}

void CPlayer::setClientNum(int num)
{
	client_num = num;
}

void CPlayer::move(int direction, float distance)
{
	if (m_PlayerController)
	{
		PxVec3 Shift = PxVec3(0, 0, 0);
		PxVec3 Direction = PxVec3(0, 0, 0);
		PxVec3 Look = PxVec3(0, 0, 0);

		if (direction & 0x01) {
			Look.z = -1.f;
			Direction = Look;
			Direction.z = 1.f;
			Shift = Direction * distance;
		}
		if (direction & 0x02) {
			Look.z = 1.f;
			Direction = Look;
			Direction.z = -1.f;
			Shift = Direction * distance;
		}
		//화살표 키 ‘→’를 누르면 로컬 x-축 방향으로 이동한다. ‘←’를 누르면 반대 방향으로 이동한다.
		if (direction & 0x04) {
			Look.x = -1.f;
			Direction = Look;
			Direction.x = 1.f;
			Shift = Direction * distance;
		}
		if (direction & 0x08) {
			Look.x = 1.f;
			Direction = Look;
			Direction.x = -1.f;
			Shift = Direction * distance;
		}

		PxControllerFilters filters;
		m_PlayerController->move(Shift, 0, 1 / 60, filters);
	}
}


void CPlayer::setPlayerController(CPhysx *physx)
{
	m_PlayerController = physx->getCapsuleController(m_Pos, CH_CAPSULE_HEIGHT, CH_CAPSULE_RADIUS, m_HitReport);
}

void CPlayer::setTrigger(CPhysx *physx)
{
	PxVec3 pos(100, 100, 100);
	m_AttackTrigger = physx->getTrigger(pos, PxVec3(5, 5, 5));
}