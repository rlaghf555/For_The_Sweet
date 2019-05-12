#pragma once

#include "header.h"
#include "Physx.h"
#include "Util.h"

using namespace physx;

static PxF32 gJumpGravity = -80.0f;

#define Anim_Idle 0
#define Anim_Walk 1
#define Anim_Weak_Attack1 2
#define Anim_Weak_Attack2 3
#define Anim_Weak_Attack3 4
#define Anim_Hard_Attack1 5
#define Anim_Hard_Attack2 6
#define Anim_Guard 7
#define Anim_PowerUp 8
#define Anim_Jump 9

#define Anim_Lollipop_Attack1 10
#define Anim_Lollipop_Attack2 11
#define Anim_Lollipop_Guard 12
#define Anim_Lollipop_Hard_Attack 13
#define Anim_Lollipop_Skill 14

#define Anim_Small_React 15
#define Anim_Pick_up 16

enum Anim {
	Idle,
	Walk,
	Weak_Attack1,
	Weak_Attack2,
	Weak_Attack3,
	Hard_Attack1,
	Hard_Attack2,
	Guard,
	PowerUp,
	Jump,
	Lollipop_Attack1,
	Lollipop_Attack2,
	Lollipop_Guard,
	Lollipop_Hard_Attack,
	Lollipop_Skill,
	Small_React,
	Pick_Up,
	Run
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
	void animate(float eTime);

	void setPlayerController(CPhysx *physx);
	void setTrigger(CPhysx *physx);
	void setPosition(PxVec3 pos);
	void setVelocity(PxVec3 vel);
	void setLook(PxVec3 look);
	void setAniIndex(char index);
	void setAniFrame(float frame);
	void setAniLoop(bool loop);
	void setAniInfo(float *aniinfo);
	void jumpstart() { m_Jump.startJump(70); }

	PxRigidActor* getTrigger() { return m_AttackTrigger; }
	PxRigidDynamic* getControllerActor() { return m_PlayerController->getActor(); }

public:
	PxVec3 m_Pos;
	PxVec3 m_Vel;
	PxVec3 m_Look;

	PxCapsuleController *m_PlayerController;
	PlayerHitReport *m_HitReport;
	PxRigidActor* m_AttackTrigger;
	CJump m_Jump;
	volatile bool hitted = false;

	float m_AniInfo[MAX_ANIM];
	bool m_AniLoop;

	char m_AniIndex;
	float m_AniFrame;

	char weapon_type = -1;
	char weapon_index = -1;
	bool weapon_send = false;
};

