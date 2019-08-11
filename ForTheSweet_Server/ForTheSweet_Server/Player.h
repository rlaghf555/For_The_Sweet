#pragma once

#include "header.h"
#include "Physx.h"
#include "Room.h"
#include "Util.h"

using namespace physx;

static PxF32 gJumpGravity = -140.0f;

enum STATUS {
	FREE,
	WEAK_ATTACK,
	HARD_ATTACK,
	DEFENSE,
	JUMP,
	HITTED,
	CRI_HITTED,
	STUN,
	SKILL_WEAPON_MOVE,
	SKILL_WEAPON_NO_MOVE,
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
	Lollipop_Skill,
	Pepero_Hard_Attack1,
	Pepero_Hard_Attack2,
	Pepero_Skill,
	Candy_Hard_Attack,
	Candy_Skill,
	Chocolate_Attack,
	Chocolate_Hard_Attack,
	Chocolate_Guard,
	Chocolate_Skill,
	Hard_React,
	Victory,
	Victory_2,
	Death,
	Stun
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

class CPlayer : public PxControllerBehaviorCallback
{
public:
	CPlayer();
	~CPlayer();

	void setPlayerController(CRoom *room, CPhysx *physx);
	void setTrigger(CRoom *room, CPhysx *physx);
	//void setPlayerController(CPhysx *physx);
	//void setTrigger(CPhysx *physx);
	void setPosition(PxVec3 pos);
	void setVelocity(PxVec3 vel);
	void setJumpVelocity(PxVec3 jumpVel);
	void setDashed(bool dashed);
	void setLook(PxVec3 look);
	void setAniIndex(char index);
	void setStatus(char status);
	void setHP(int hp);
	void jumpstart() { m_Jump.startJump(120); }

	// ·Ñ¸®ÆË Heal
	void setLollipopHeal(bool heal);

	PxRigidActor* getTrigger() { return m_AttackTrigger; }
	PxRigidDynamic* getControllerActor() { return m_PlayerController->getActor(); }

	virtual PxControllerBehaviorFlags		getBehaviorFlags(const PxShape& shape, const PxActor& actor);
	virtual PxControllerBehaviorFlags		getBehaviorFlags(const PxController& controller);
	virtual PxControllerBehaviorFlags		getBehaviorFlags(const PxObstacle& obstacle);

public:
	PxVec3 m_Pos;
	PxVec3 m_Vel;
	PxVec3 m_JumpVel;
	PxVec3 m_Look;

	PxVec3 m_Knockback;

	PxCapsuleController* m_PlayerController;
	PlayerHitReport* m_HitReport;
	PxRigidActor* m_AttackTrigger;
	CJump m_Jump;
	CJump m_Fall;
	volatile bool m_dashed = false;
	volatile bool hitted = false;

	char m_AniIndex;

	char weapon_type;
	char weapon_index;
	volatile bool weapon_send = false;

	char m_status;
	high_resolution_clock::time_point attack_time;
	char attack_count;

	int m_hp;
	bool lollipop_heal;
	bool dead = false;
};

