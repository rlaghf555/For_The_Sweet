#pragma once

#include "header.h"
#include "Physx.h"
#include "Util.h"

using namespace physx;

static PxF32 gJumpGravity = -80.0f;

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
	Small_React
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
	void setAniInfo(vector<pair<int, float>> aniInfo);
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
	bool hitted = false;

	vector<pair<int, float>> m_AniInfo;
	bool m_AniLoop;

	char m_AniIndex;
	float m_AniFrame;
};

