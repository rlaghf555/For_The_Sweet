#include "stdafx.h"
#include "Object.h"
#include "Shader.h"

void CGameObject::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
}
void CGameObject::ReleaseShaderVariables()
{
}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
	//객체의 월드 변환 행렬을 루트 상수(32-비트 값)를 통하여 셰이더 변수(상수 버퍼)로 복사한다.
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
}

void CGameObject::SetWorld(XMFLOAT4X4 & xmf4x4World)
{
	m_xmf4x4World = xmf4x4World;
}

void CGameObject::SetWorld(XMFLOAT3 & xmf3Look, XMFLOAT3 & xmf3Up, XMFLOAT3 & xmf3Right)
{
	m_xmf4x4World._11 = xmf3Right.x;
	m_xmf4x4World._12 = xmf3Right.y;
	m_xmf4x4World._13 = xmf3Right.z;

	m_xmf4x4World._21 = xmf3Up.x;
	m_xmf4x4World._22 = xmf3Up.y;
	m_xmf4x4World._23 = xmf3Up.z;

	m_xmf4x4World._31 = xmf3Look.x;
	m_xmf4x4World._32 = xmf3Look.y;
	m_xmf4x4World._33 = xmf3Look.z;
}

void CGameObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}
void CGameObject::SetPosition(XMFLOAT3& xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}
void CGameObject::SetEffectLook(XMFLOAT3& xmf3Look)
{
	if (xmf3Look.x == 0 && xmf3Look.y == 0 && xmf3Look.z == 0)
		return;
	XMFLOAT3 m_xmf3Look = xmf3Look;
	XMFLOAT3 m_xmf3Up(0,1,0);
	m_xmf3Look.y = 0.0f;
	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	XMFLOAT3 m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
	SetWorld(m_xmf3Look, m_xmf3Up, m_xmf3Right);

}
void CGameObject::SetScale(float value)
{
	m_xmf4x4World._11 *= value;
	m_xmf4x4World._22 *= value;
	m_xmf4x4World._33 *= value;
}
XMFLOAT3 CGameObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}
//게임 객체의 로컬 z-축 벡터를 반환한다.
XMFLOAT3 CGameObject::GetLook()
{
	XMFLOAT3 a = XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33);
	return(Vector3::Normalize(a));
}
//게임 객체의 로컬 y-축 벡터를 반환한다.
XMFLOAT3 CGameObject::GetUp()
{
	XMFLOAT3 a = XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23);
	return(Vector3::Normalize(a));
}
//게임 객체의 로컬 x-축 벡터를 반환한다.
XMFLOAT3 CGameObject::GetRight()
{
	XMFLOAT3 a = XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13);
	return(Vector3::Normalize(a));
}
//게임 객체를 로컬 x-축 방향으로 이동한다.
void CGameObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CGameObject::SetPosition(xmf3Position);
}
//게임 객체를 로컬 y-축 방향으로 이동한다.
void CGameObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CGameObject::SetPosition(xmf3Position);
}
//게임 객체를 로컬 z-축 방향으로 이동한다.
void CGameObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CGameObject::SetPosition(xmf3Position);
}
//게임 객체를 주어진 각도로 회전한다.
void CGameObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch),
		XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CGameObject::init()
{
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
}

void CGameObject::init(XMFLOAT3 pos)
{
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());

	SetPosition(pos);
}

void CGameObject::Rotate(XMFLOAT3 *pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

CGameObject::CGameObject()
{
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
}

CGameObject::~CGameObject()
{
	if (m_pMesh) m_pMesh->Release();
	if (m_pShader)
	{
		m_pShader->ReleaseShaderVariables();
		m_pShader->Release();
	}
}

void CGameObject::SetShader(CShader *pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}

void CGameObject::SetMesh(CMesh *pMesh)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}

void CGameObject::SetMesh(int nIndex, shared_ptr<MMesh> pMesh)
{
	if (!m_ppMeshes.empty())
	{
		if (m_ppMeshes[nIndex]) m_ppMeshes[nIndex].release();
		m_ppMeshes[nIndex] = make_unique<MMesh>(*pMesh);
	}
}

void CGameObject::ReleaseUploadBuffers()
{
	if (!m_ppMeshes.empty())
	{
		for (UINT i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i])
				m_ppMeshes[i]->ReleaseUploadBuffers();
		}
	}

	//정점 버퍼를 위한 업로드 버퍼를 소멸시킨다.
	if (m_pMesh) m_pMesh->ReleaseUploadBuffers();
}

void CGameObject::Animate(float fTimeElapsed)
{
	/*
	if (m_pMesh)
	{
		m_pMesh->m_xmOOBB.Transform(m_xmOOBB, XMLoadFloat4x4(&m_xmf4x4World));
		XMStoreFloat4(&m_xmOOBB.Orientation, XMQuaternionNormalize(XMLoadFloat4(&m_xmOOBB.Orientation)));
		//m_pMesh->A
	}*/
}

void CGameObject::OnPrepareRender()
{
}

void CGameObject::OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
}

void CGameObject::SetRootParameter(ID3D12GraphicsCommandList *pd3dCommandList)
{
	//pd3dCommandList->SetGraphicsRootConstantBufferView(1, m_d3dCbvGPUDescriptorHandle.ptr);
	pd3dCommandList->SetGraphicsRootDescriptorTable(m_nRootIndex, m_d3dCbvGPUDescriptorHandle);
}

void CGameObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	OnPrepareRender(pd3dCommandList, pCamera);
	//객체의 정보를 셰이더 변수(상수 버퍼)로 복사한다.
	//UpdateShaderVariables(pd3dCommandList);

	if (m_pMaterial)
	{
		if (m_pMaterial->m_pShader)
		{
			m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
			m_pMaterial->m_pShader->UpdateShaderVariables(pd3dCommandList);

			UpdateShaderVariables(pd3dCommandList);
		}
		if (m_pMaterial->m_pTexture)
		{
			m_pMaterial->m_pTexture->UpdateShaderVariables(pd3dCommandList);
		}
	}

	//pd3dCommandList->SetGraphicsRootDescriptorTable(m_nRootIndex, m_d3dCbvGPUDescriptorHandle);
	// SetRootParameter(pd3dCommandList);

	pd3dCommandList->SetGraphicsRootDescriptorTable(1, m_d3dCbvGPUDescriptorHandle);

	//if (m_pShader) m_pShader->Render(pd3dCommandList, pCamera);
	if (m_pMesh) m_pMesh->Render(pd3dCommandList);

	if (!m_ppMeshes.empty())
	{
		for (UINT i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i])
				m_ppMeshes[i]->Render(pd3dCommandList);
		}
	}
}

void CGameObject::Render(ID3D12GraphicsCommandList * pd3dCommandList, UINT nObject, CCamera * pCamera)
{
	OnPrepareRender(pd3dCommandList, pCamera);

	if (m_pMaterial)
	{
		if (m_pMaterial->m_pShader)
		{
			m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
			m_pMaterial->m_pShader->UpdateShaderVariables(pd3dCommandList);

			UpdateShaderVariables(pd3dCommandList);
		}
		if (m_pMaterial->m_pTexture)
		{
			m_pMaterial->m_pTexture->UpdateShaderVariables(pd3dCommandList);
		}
	}

	SetRootParameter(pd3dCommandList);

	if (!m_ppMeshes.empty())
	{
		for (UINT i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) {
				m_ppMeshes[i]->Render(pd3dCommandList, nObject);
			}
		}
	}
}


void CGameObject::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, UINT nInstances)
{
	OnPrepareRender();
	if (m_pMesh) m_pMesh->Render(pd3dCommandList, nInstances);
}

CRotatingObject::CRotatingObject()
{
	m_xmf3RotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotationSpeed = 90.0f;
}

CRotatingObject::~CRotatingObject()
{
}

void CRotatingObject::Animate(float fTimeElapsed)
{
	CGameObject::Rotate(&m_xmf3RotationAxis, m_fRotationSpeed * fTimeElapsed);
}

CPlayerObject::CPlayerObject()
{
}

CPlayerObject::~CPlayerObject()
{
}
