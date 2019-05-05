#pragma once
#include "Mesh.h"
#include "Camera.h"
#include "Model.h"
#include "Texture.h"
#include "Physx.h"

class CShader;

class CGameObject
{
public:
	//상수 버퍼를 생성한다.
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	//상수 버퍼의 내용을 갱신한다.
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	//게임 객체의 월드 변환 행렬에서 위치 벡터와 방향(x-축, y-축, z-축) 벡터를 반환한다.
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();
	//게임 객체의 위치를 설정한다.
	virtual void SetPosition(float x, float y, float z);
	virtual void SetPosition(XMFLOAT3& xmf3Position);
	void SetScale(float value);
	//게임 객체를 로컬 x-축, y-축, z-축 방향으로 이동한다.
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);
	//게임 객체를 회전(x-축, y-축, z-축)한다.
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	//virtual void SetAnimations(UINT num, LoadAnimation** tmp) {}
	virtual void Move(ULONG nDirection, float fDistance, bool bVelocity = false) {}
	virtual void Move(const XMFLOAT3& xmf3Shift, bool bVelocity = false) {}
	virtual void Move(float fxOffset = 0.0f, float fyOffset = 0.0f, float fzOffset = 0.0f) {}
	virtual CCamera* GetCamera() { return nullptr; }
	virtual void Update(float fTimeElapsed) {}
	virtual XMFLOAT4X4* GetBoneData() { return nullptr; }
	virtual int GetBoneNum() const { return 0; }
public:
	void Rotate(XMFLOAT3 *pxmf3Axis, float fAngle);
public:
	CGameObject();
	virtual ~CGameObject();
private:
	int m_nReferences = 0;
public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
public:
	XMFLOAT4X4					m_xmf4x4World;
	CMesh						*m_pMesh = NULL;
	CShader						*m_pShader = NULL;
	vector<unique_ptr<MMesh>>	m_ppMeshes;
	UINT						m_nMeshes;
	BoundingOrientedBox			m_xmOOBB;
	D3D12_GPU_DESCRIPTOR_HANDLE					m_d3dCbvGPUDescriptorHandle;
	CMaterial					*m_pMaterial;
	int m_nRootIndex;

public:
	void ReleaseUploadBuffers();
	virtual void SetWorld(XMFLOAT4X4& xmf4x4World);
	virtual void SetWorld(XMFLOAT3& xmf3Look, XMFLOAT3& xmf3Up, XMFLOAT3& xmf3Right);
	virtual void SetMesh(CMesh *pMesh);
	virtual void SetMesh(int nIndex, MMesh *pMesh);
	virtual void SetShader(CShader *pShader);
	virtual void Animate(float fTimeElapsed);
	virtual void OnPrepareRender();
	virtual void OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	virtual void SetRootParameter(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, UINT nObject, CCamera *pCamera);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, UINT nInstances);
	void SetCbvGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle) { m_d3dCbvGPUDescriptorHandle = d3dCbvGPUDescriptorHandle; }
	void SetCbvGPUDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr) { m_d3dCbvGPUDescriptorHandle.ptr = nCbvGPUDescriptorHandlePtr; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetCbvGPUDescriptorHandle() { return(m_d3dCbvGPUDescriptorHandle); }

};

class CPlayerObject : public CGameObject {
public:
	CPlayerObject();
	virtual ~CPlayerObject();

};

class CRotatingObject : public CGameObject
{
public:
	CRotatingObject();
	virtual ~CRotatingObject();
private:
	XMFLOAT3 m_xmf3RotationAxis;
	float m_fRotationSpeed;
public:
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_xmf3RotationAxis = xmf3RotationAxis; }
	virtual void Animate(float fTimeElapsed);
};