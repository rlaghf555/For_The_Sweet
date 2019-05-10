#include "Player.h"



CPlayer::CPlayer()
{
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

void CPlayer::setAniIndex(char index)
{
	m_AniIndex = index;
}

void CPlayer::setAniFrame(float frame)
{
	m_AniFrame = frame;
}

void CPlayer::setAniInfo(vector<pair<int, float>> aniInfo)
{
	m_AniInfo.resize((int)(aniInfo.size()));

	std::copy(aniInfo.begin(), aniInfo.end(), m_AniInfo.begin());
}

void CPlayer::setAniLoop(bool loop)
{
	m_AniLoop = loop;
}

void CPlayer::animate(float fTime)
{
	//cout << int(m_AniIndex) << endl;

	m_AniFrame += 30 * fTime;
	if (m_AniFrame > m_AniInfo[m_AniIndex].second) {
		m_AniFrame = 0.0f;
		if (!m_AniLoop) {
			m_AniIndex = Anim_Idle;
			m_AniLoop = true;
		}
	}
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
		//ȭ��ǥ Ű ���桯�� ������ ���� x-�� �������� �̵��Ѵ�. ���硯�� ������ �ݴ� �������� �̵��Ѵ�.
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