#pragma once

#include "Physx.h"

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

class PlayerHitReport : public PxUserControllerHitReport {
public:
	void	onShapeHit(const PxControllerShapeHit &hit) {
		cout << "ShapedHit!!!\n";
	}
	void 	onControllerHit(const PxControllersHit &hit) {
		cout << "ControllerHit!!!\n";
	}
	void 	onObstacleHit(const PxControllerObstacleHit &hit) {
		cout << "ObstacleHit!!!\n";
	}

};

class CPlayer
{
public:
	CPlayer();
	~CPlayer();

	void move(int direction, float distance);
	void animate(float eTime);

	void setPlayerController(CPhysx *physx);
	void setPosition(PxVec3 pos);
	void setVelocity(PxVec3 vel);
	void setAniIndex(char index);
	void setAniFrame(float frame);
	void setAniLoop(bool loop);
	void setAniInfo(vector<pair<int, float>> aniInfo);

public:
	PxVec3 m_Pos;
	PxVec3 m_Vel;

	PxCapsuleController *m_PlayerController;
	PlayerHitReport *m_HitReport;

	vector<pair<int, float>> m_AniInfo;
	bool m_AniLoop;

	char m_AniIndex;
	float m_AniFrame;
};

