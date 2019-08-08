#pragma once

#define DIR_FORWARD 0x01
#define DIR_BACKWARD 0x02
#define DIR_LEFT 0x04
#define DIR_RIGHT 0x08
#define DIR_UP 0x10
#define DIR_DOWN 0x20

#include "Object.h"
#include "Camera.h"
#include "Model.h"
#include "ModelObject.h"
#include "PxPhysicsAPI.h"

using namespace physx;

enum STATUS {
	FREE,
	WEAK_ATTACK,
	HARD_ATTACK,
	DEFENSE,
	JUMP,
	HITTED,
	CRI_HITTED,
	SKILL_WEAPON_MOVE,
	SKILL_WEAPON_NO_MOVE,
};

static PxF32 gJumpGravity = -140.0f;

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

class Jump
{
public:
	Jump();

	PxF32		mV0;
	PxF32		mJumpTime;
	bool			mJump;

	void			startJump(PxF32 v0);
	void			stopJump();
	PxF32		getHeight(PxF32 elapsedTime);
};

class CPlayer : public ModelObject
{
protected:
	//�÷��̾��� ��ġ ����, x-��(Right), y-��(Up), z-��(Look) �����̴�.
	XMFLOAT3 m_xmf3Position;
	XMFLOAT3 m_xmf3Right;
	XMFLOAT3 m_xmf3Up;
	XMFLOAT3 m_xmf3Look;

	//�÷��̾ ���� x-��(Right), y-��(Up), z-��(Look)���� �󸶸�ŭ ȸ���ߴ°��� ��Ÿ����.
	float m_fPitch;
	float m_fYaw;
	float m_fRoll;

	bool m_connected;
	bool m_dashed = false;
	bool weapon_grab = false;
	bool Scale_flag = false;
	char m_status = STATUS::FREE;

	int weapon_skill = -1;
	int weapon_index = -1;
	int weapon_type = -1;
	XMFLOAT3 m_xmf3Velocity;		//�÷��̾��� �̵� �ӵ��� ��Ÿ���� �����̴�.
	PxVec3 m_JumpVelocity;
	XMFLOAT3 m_xmf3Gravity;			//�÷��̾ �ۿ��ϴ� �߷��� ��Ÿ���� �����̴�.
	float m_fMaxVelocityXZ;			//xz-��鿡�� (�� ������ ����) �÷��̾��� �̵� �ӷ��� �ִ밪�� ��Ÿ����.
	float m_fMaxVelocityY;			//y-�� �������� (�� ������ ����) �÷��̾��� �̵� �ӷ��� �ִ밪�� ��Ÿ����.
	float m_fFriction;				//�÷��̾ �ۿ��ϴ� �������� ��Ÿ����.
	LPVOID m_pPlayerUpdatedContext;	//�÷��̾��� ��ġ�� �ٲ� ������ ȣ��Ǵ� OnPlayerUpdateCallback() �Լ����� ����ϴ� �������̴�.
	LPVOID m_pCameraUpdatedContext;	//ī�޶��� ��ġ�� �ٲ� ������ ȣ��Ǵ� OnCameraUpdateCallback() �Լ����� ����ϴ� �������̴�.
	CCamera *m_pCamera = NULL;		//�÷��̾ ���� ������ ī�޶��̴�.
	Model_Animation *character;

	int m_hp = 100;
	int m_mp = 0;
public:
	int selected_skill = 0;
	PxCapsuleController *m_PlayerController;
	PxControllerFilters	 m_ControllerFilter;
	PlayerHitReport m_HitReport;
	PxRigidActor* m_AttackTrigger;

	Jump m_Jump;
	Jump m_Fall;
	
	float Scale_time = 0.3f;

	PxVec3 m_Knockback;

public:
	CPlayer(Model_Animation* ma, ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual ~CPlayer();
	// Physx
	void SetPhysController(CPhysx* physx, PxUserControllerHitReport* callback, PxExtendedVec3* pos);
	void move();
	void init();
	PlayerHitReport* getCollisionCallback() { return &m_HitReport; }
	PxRigidDynamic* getControllerActor() { return m_PlayerController->getActor(); }
	PxRigidActor* getTrigger() { return m_AttackTrigger; }
	void jumpstart() { m_Jump.startJump(120); }

	void SetScaleflag(bool flag) { Scale_flag = flag; }
	bool GetScaleflag() { return(Scale_flag); }

	XMFLOAT3 GetPosition() { return(m_xmf3Position); }
	XMFLOAT3 GetLookVector() { return(m_xmf3Look); }
	XMFLOAT3 GetUpVector() { return(m_xmf3Up); }
	XMFLOAT3 GetRightVector() { return(m_xmf3Right); }
	bool GetConnected() { return(m_connected); }
	void SetConnected(bool connected) { m_connected = connected; }
	bool GetDashed() { return(m_dashed); }
	void SetDashed(bool dashed) { m_dashed = dashed; }
	char GetStatus() { return (m_status); }
	void SetStatus(char status) { m_status = status; }

	void SetPosition(XMFLOAT3& pos) { m_xmf3Position = pos; }
	void SetFriction(float fFriction) { m_fFriction = fFriction; }
	void SetGravity(XMFLOAT3& xmf3Gravity) { m_xmf3Gravity = xmf3Gravity; }
	void SetMaxVelocityXZ(float fMaxVelocity) { m_fMaxVelocityXZ = fMaxVelocity; }
	void SetMaxVelocityY(float fMaxVelocity) { m_fMaxVelocityY = fMaxVelocity; }
	void SetVelocity(XMFLOAT3& xmf3Velocity) { m_xmf3Velocity = xmf3Velocity; }
	void SetJumpVelocity(PxVec3& JumpVelocity) { m_JumpVelocity = JumpVelocity; }
	void SetWeapon(bool grab, int Weapon_type, int Weapon_Index) { weapon_grab = grab; weapon_type = Weapon_type;  weapon_index = Weapon_Index; }

	int Get_Weapon_Skill() { return weapon_skill; }
	void Set_Weapon_Skill(int weapon_skill_type) { weapon_skill = weapon_skill_type; }

	bool Get_Weapon_grab() { return weapon_grab; }
	int Get_Weapon_type() { return weapon_type; }
	int Get_Weapon_index() { return weapon_index; }

	void Set_HP(int hp) { m_hp = hp; }
	void Set_MP(int mp) { m_mp = mp; }
	int Get_HP() { return m_hp; }
	int Get_MP() { return m_mp; }

	XMFLOAT3& GetVelocity() { return(m_xmf3Velocity); }
	PxVec3& GetJumpVelocity() { return(m_JumpVelocity); }
	float GetYaw() { return(m_fYaw); }
	float GetPitch() { return(m_fPitch); }
	float GetRoll() { return(m_fRoll); }

	CCamera *GetCamera() { return(m_pCamera); }
	void SetCamera(CCamera *pCamera) { m_pCamera = pCamera; }
	
	//�÷��̾ �̵��ϴ� �Լ��̴�.
	virtual void Move(ULONG nDirection, float fDistance, bool bVelocity = false);
	virtual void Move(const XMFLOAT3& xmf3Shift, bool bVelocity = false);
	//virtual void Move(float fxOffset = 0.0f, float fyOffset = 0.0f, float fzOffset = 0.0f);
	
	//�÷��̾ ȸ���ϴ� �Լ��̴�.
	void Rotate(float x, float y, float z);
	
	//�÷��̾��� ��ġ�� ȸ�� ������ ��� �ð��� ���� �����ϴ� �Լ��̴�.
	virtual void Update(float fTimeElapsed);
	virtual void SetLook(XMFLOAT3& xmf3Look);
	virtual void SetRight(XMFLOAT3& xmf3Right){m_xmf3Right=xmf3Right;}
	//�÷��̾��� ��ġ�� �ٲ� ������ ȣ��Ǵ� �Լ��� �� �Լ����� ����ϴ� ������ �����ϴ� �Լ��̴�.
	virtual void OnPlayerUpdateCallback(float fTimeElapsed) { }
	void SetPlayerUpdatedContext(LPVOID pContext) { m_pPlayerUpdatedContext = pContext; }
	//ī�޶��� ��ġ�� �ٲ� ������ ȣ��Ǵ� �Լ��� �� �Լ����� ����ϴ� ������ �����ϴ� �Լ��̴�.
	virtual void OnCameraUpdateCallback(float fTimeElapsed) { }
	void SetCameraUpdatedContext(LPVOID pContext) { m_pCameraUpdatedContext = pContext; }
	
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	
	//ī�޶� �����ϱ� ���Ͽ� ȣ���ϴ� �Լ��̴�.
	CCamera *OnChangeCamera(DWORD nNewCameraMode);
	virtual CCamera *ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);
	
	//�÷��̾��� ��ġ�� ȸ�������κ��� ���� ��ȯ ����� �����ϴ� �Լ��̴�.
	virtual void OnPrepareRender();
	
	//�÷��̾��� ī�޶� 3��Ī ī�޶��� �� �÷��̾�(�޽�)�� �������Ѵ�.
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);
};

/* 
class CGamePlayer : public CPlayer
{
public:
	CGamePlayer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,
		ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual ~CGamePlayer();
	
	virtual CCamera *ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);
	virtual void OnPrepareRender();
};
class ModelPlayer : public CPlayer {
private:
	LoadModel *lm;
public:
	ModelPlayer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, LoadModel *Character_Model);
	virtual CCamera *ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);
	virtual ~ModelPlayer();
};
*/