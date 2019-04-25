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

	
	XMFLOAT3 m_xmf3Velocity;		//�÷��̾��� �̵� �ӵ��� ��Ÿ���� �����̴�.
	XMFLOAT3 m_xmf3Gravity;			//�÷��̾ �ۿ��ϴ� �߷��� ��Ÿ���� �����̴�.
	float m_fMaxVelocityXZ;			//xz-��鿡�� (�� ������ ����) �÷��̾��� �̵� �ӷ��� �ִ밪�� ��Ÿ����.
	float m_fMaxVelocityY;			//y-�� �������� (�� ������ ����) �÷��̾��� �̵� �ӷ��� �ִ밪�� ��Ÿ����.
	float m_fFriction;				//�÷��̾ �ۿ��ϴ� �������� ��Ÿ����.
	LPVOID m_pPlayerUpdatedContext;	//�÷��̾��� ��ġ�� �ٲ� ������ ȣ��Ǵ� OnPlayerUpdateCallback() �Լ����� ����ϴ� �������̴�.
	LPVOID m_pCameraUpdatedContext;	//ī�޶��� ��ġ�� �ٲ� ������ ȣ��Ǵ� OnCameraUpdateCallback() �Լ����� ����ϴ� �������̴�.
	CCamera *m_pCamera = NULL;		//�÷��̾ ���� ������ ī�޶��̴�.
	Model_Animation *character;
public:
	CPlayer(Model_Animation* ma, ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual ~CPlayer();

	XMFLOAT3 GetPosition() { return(m_xmf3Position); }
	XMFLOAT3 GetLookVector() { return(m_xmf3Look); }
	XMFLOAT3 GetUpVector() { return(m_xmf3Up); }
	XMFLOAT3 GetRightVector() { return(m_xmf3Right); }

	void SetFriction(float fFriction) { m_fFriction = fFriction; }
	void SetGravity(XMFLOAT3& xmf3Gravity) { m_xmf3Gravity = xmf3Gravity; }
	void SetMaxVelocityXZ(float fMaxVelocity) { m_fMaxVelocityXZ = fMaxVelocity; }
	void SetMaxVelocityY(float fMaxVelocity) { m_fMaxVelocityY = fMaxVelocity; }
	void SetVelocity(XMFLOAT3& xmf3Velocity) { m_xmf3Velocity = xmf3Velocity; }
	
	/*�÷��̾��� ��ġ�� xmf3Position ��ġ�� �����Ѵ�. xmf3Position ���Ϳ��� ���� �÷��̾��� ��ġ ���͸� ���� ��
	�� �÷��̾��� ��ġ���� xmf3Position ���������� ���Ͱ� �ȴ�. ���� �÷��̾��� ��ġ���� �� ���� ��ŭ �̵��Ѵ�.*/
	//virtual void SetPosition(XMFLOAT3& xmf3Position) {
	//	Move(XMFLOAT3(xmf3Position.x - m_xmf3Position.x, xmf3Position.y - m_xmf3Position.y, xmf3Position.z - m_xmf3Position.z), false);
//	}

	XMFLOAT3& GetVelocity() { return(m_xmf3Velocity); }
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