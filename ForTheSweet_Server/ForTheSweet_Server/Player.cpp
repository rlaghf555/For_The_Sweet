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
	hp = 100;

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


void CPlayer::setPlayerController(CPhysx *physx)
{
	m_PlayerController = physx->getCapsuleController(m_Pos, CH_CAPSULE_HEIGHT, CH_CAPSULE_RADIUS, m_HitReport);
}

void CPlayer::setTrigger(CPhysx *physx)
{
	PxVec3 pos(100, 100, 100);
	m_AttackTrigger = physx->getTrigger(pos, PxVec3(5, 5, 5));
}