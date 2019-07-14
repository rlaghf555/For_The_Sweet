#pragma once

#include "header.h"
#include "Physx.h"
#include "Util.h"

using namespace physx;

static PxF32 gJumpGravity = -140.0f;

enum STATUS {
	FREE,
	WEAK_ATTACK,
	HARD_ATTACK,
	DEFENSE,
	JUMP,
	HITTED
};

enum Anim {
	Idle,
	Walk,
	Run,
	Jump,
	Guard,
	Small_React,
	Pick_Up,
	PowerUp,
	CupCake_Eat,
	Weak_Attack1,
	Weak_Attack2,
	Weak_Attack3,
	Hard_Attack1,
	Hard_Attack2,
	Lollipop_Attack1,
	Lollipop_Attack2,
	Lollipop_Guard,
	Lollipop_Hard_Attack,
	Lollipop_Skill
};

class PlayerHitReport : public PxUserControllerHitReport {
public:
	void	onShapeHit(const PxControllerShapeHit &hit) {
		//cout << "ShapedHit!!!\n";
	}
	void 	onControllerHit(const PxControllersHit &hit) {
		//cout << "ControllerHit!!!\n";
	}
	void 	onObstacleHit(const PxControllerObstacleHit &hit) {
		//cout << "ObstacleHit!!!\n";
	}

};

class CJump
{
public:
	CJump();

	PxF32		mV0;
	PxF32		mJumpTime;
	bool			mJump;

	void			startJump(PxF32 v0);
	void			stopJump();
	PxF32		getHeight(PxF32 elapsedTime);
};

class CPlayer
{
public:
	CPlayer();
	~CPlayer();

	void move(int direction, float distance);

	void setPlayerController(CPhysx *physx);
	void setTrigger(CPhysx *physx);
	void setPosition(PxVec3 pos);
	void setVelocity(PxVec3 vel);
	void setDashed(bool dashed);
	void setLook(PxVec3 look);
	void setAniIndex(char index);
	void setAniInfo(float *aniinfo);
	void setStatus(char status);
	void jumpstart() { m_Jump.startJump(120); }

	PxRigidActor* getTrigger() { return m_AttackTrigger; }
	PxRigidDynamic* getControllerActor() { return m_PlayerController->getActor(); }

public:
	PxVec3 m_Pos;
	PxVec3 m_Vel;
	PxVec3 m_Look;

	PxCapsuleController* m_PlayerController;
	PlayerHitReport* m_HitReport;
	PxRigidActor* m_AttackTrigger;
	CJump m_Jump;
	CJump m_Fall;
	volatile bool m_dashed = false;
	volatile bool hitted = false;

	float m_AniInfo[MAX_ANIM];
	bool m_AniLoop;

	char m_AniIndex;

	char weapon_type = -1;
	char weapon_index = -1;
	volatile bool weapon_send = false;

	char m_status;
	high_resolution_clock::time_point attack_time;
	char attack_count = 0;
};

