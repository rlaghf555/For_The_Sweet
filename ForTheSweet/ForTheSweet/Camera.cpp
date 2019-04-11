#include "stdafx.h"
#include "Player.h"
#include "Camera.h"

CCamera::CCamera()
{
	m_xmf4x4View = Matrix4x4::Identity();
	m_xmf4x4Projection = Matrix4x4::Identity();
	m_d3dViewport = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT, 0.0f, 1.0f };
	m_d3dScissorRect = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT };
	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;
	m_xmf3Offset = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fTimeLag = 0.0f;
	m_xmf3LookAtWorld = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_nMode = 0x00;
	m_pPlayer = NULL;
}

CCamera::CCamera(CCamera *pCamera)
{
	if (pCamera)
	{
		//카메라가 이미 있으면 기존 카메라의 정보를 새로운 카메라에 복사한다.
		//*this = *pCamera;
	}
	else
	{
		//카메라가 없으면 기본 정보를 설정한다.
		m_xmf4x4View = Matrix4x4::Identity();
		m_xmf4x4Projection = Matrix4x4::Identity();
		m_d3dViewport = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT, 0.0f, 1.0f };
		m_d3dScissorRect = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT };
		m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
		m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
		m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
		m_fPitch = 0.0f;
		m_fRoll = 0.0f;
		m_fYaw = 0.0f;
		m_xmf3Offset = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_fTimeLag = 0.0f;
		m_xmf3LookAtWorld = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_nMode = 0x00;
		m_pPlayer = NULL;
	}
}

CCamera::~CCamera()
{
}

void CCamera::SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ, float fMaxZ)
{
	m_d3dViewport.TopLeftX = float(xTopLeft);
	m_d3dViewport.TopLeftY = float(yTopLeft);
	m_d3dViewport.Width = float(nWidth);
	m_d3dViewport.Height = float(nHeight);
	m_d3dViewport.MinDepth = fMinZ;
	m_d3dViewport.MaxDepth = fMaxZ;
}
void CCamera::SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom)
{
	m_d3dScissorRect.left = xLeft;
	m_d3dScissorRect.top = yTop;
	m_d3dScissorRect.right = xRight;
	m_d3dScissorRect.bottom = yBottom;
}

void CCamera::GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle)
{
	//	m_xmf4x4Projection = Matrix4x4::PerspectiveFovLH(XMConvertToRadians(fFOVAngle), fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
	XMMATRIX xmmtxProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(fFOVAngle), fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
	XMStoreFloat4x4(&m_xmf4x4Projection, xmmtxProjection);

#ifdef _WITH_DIERECTX_MATH_FRUSTUM
	BoundingFrustum::CreateFromMatrix(m_xmFrustumView, xmmtxProjection);
#endif
}


void CCamera::CalculateFrustumPlanes()
{
#ifdef _WITH_DIERECTX_MATH_FRUSTUM
	m_xmFrustumView.Transform(m_xmFrustumWorld, XMMatrixInverse(NULL, XMLoadFloat4x4(&m_xmf4x4View)));
#else
	XMFLOAT4X4 mtxViewProjection = Matrix4x4::Multiply(m_xmf4x4View, m_xmf4x4Projection);

	m_pxmf4FrustumPlanes[0].x = -(mtxViewProjection._14 + mtxViewProjection._11);
	m_pxmf4FrustumPlanes[0].y = -(mtxViewProjection._24 + mtxViewProjection._21);
	m_pxmf4FrustumPlanes[0].z = -(mtxViewProjection._34 + mtxViewProjection._31);
	m_pxmf4FrustumPlanes[0].w = -(mtxViewProjection._44 + mtxViewProjection._41);

	m_pxmf4FrustumPlanes[1].x = -(mtxViewProjection._14 - mtxViewProjection._11);
	m_pxmf4FrustumPlanes[1].y = -(mtxViewProjection._24 - mtxViewProjection._21);
	m_pxmf4FrustumPlanes[1].z = -(mtxViewProjection._34 - mtxViewProjection._31);
	m_pxmf4FrustumPlanes[1].w = -(mtxViewProjection._44 - mtxViewProjection._41);

	m_pxmf4FrustumPlanes[2].x = -(mtxViewProjection._14 - mtxViewProjection._12);
	m_pxmf4FrustumPlanes[2].y = -(mtxViewProjection._24 - mtxViewProjection._22);
	m_pxmf4FrustumPlanes[2].z = -(mtxViewProjection._34 - mtxViewProjection._32);
	m_pxmf4FrustumPlanes[2].w = -(mtxViewProjection._44 - mtxViewProjection._42);

	m_pxmf4FrustumPlanes[3].x = -(mtxViewProjection._14 + mtxViewProjection._12);
	m_pxmf4FrustumPlanes[3].y = -(mtxViewProjection._24 + mtxViewProjection._22);
	m_pxmf4FrustumPlanes[3].z = -(mtxViewProjection._34 + mtxViewProjection._32);
	m_pxmf4FrustumPlanes[3].w = -(mtxViewProjection._44 + mtxViewProjection._42);

	m_pxmf4FrustumPlanes[4].x = -(mtxViewProjection._13);
	m_pxmf4FrustumPlanes[4].y = -(mtxViewProjection._23);
	m_pxmf4FrustumPlanes[4].z = -(mtxViewProjection._33);
	m_pxmf4FrustumPlanes[4].w = -(mtxViewProjection._43);

	m_pxmf4FrustumPlanes[5].x = -(mtxViewProjection._14 - mtxViewProjection._13);
	m_pxmf4FrustumPlanes[5].y = -(mtxViewProjection._24 - mtxViewProjection._23);
	m_pxmf4FrustumPlanes[5].z = -(mtxViewProjection._34 - mtxViewProjection._33);
	m_pxmf4FrustumPlanes[5].w = -(mtxViewProjection._44 - mtxViewProjection._43);

	for (int i = 0; i < 6; i++) m_pxmf4FrustumPlanes[i] = Plane::Normalize(m_pxmf4FrustumPlanes[i]);
#endif
}

bool CCamera::IsInFrustum(BoundingOrientedBox& xmBoundingBox)
{
#ifdef _WITH_DIERECTX_MATH_FRUSTUM
	return(m_xmFrustumWorld.Intersects(xmBoundingBox));
#else
#endif
}

bool CCamera::IsInFrustum(BoundingBox& xmBoundingBox)
{
#ifdef _WITH_DIERECTX_MATH_FRUSTUM
	return(m_xmFrustumWorld.Intersects(xmBoundingBox));
#else
	XMFLOAT3 xmf3NearPoint, xmf3Normal;
	XMFLOAT3 xmf3Minimum = Vector3::Subtract(xmBoundingBox.Center, xmBoundingBox.Extents);
	XMFLOAT3 xmf3Maximum = Vector3::Add(xmBoundingBox.Center, xmBoundingBox.Extents);
	for (int i = 0; i < 6; i++)
	{
		xmf3Normal = XMFLOAT3(m_pxmf4FrustumPlanes[i].x, m_pxmf4FrustumPlanes[i].y, m_pxmf4FrustumPlanes[i].z);
		if (xmf3Normal.x >= 0.0f)
		{
			if (xmf3Normal.y >= 0.0f)
			{
				if (xmf3Normal.z >= 0.0f)
				{
					xmf3NearPoint = XMFLOAT3(xmf3Minimum.x, xmf3Minimum.y, xmf3Minimum.z);
				}
				else
				{
					xmf3NearPoint = XMFLOAT3(xmf3Minimum.x, xmf3Minimum.y, xmf3Maximum.z);
				}
			}
			else
			{
				if (xmf3Normal.z >= 0.0f)
				{
					xmf3NearPoint = XMFLOAT3(xmf3Minimum.x, xmf3Maximum.y, xmf3Minimum.z);
				}
				else
				{
					xmf3NearPoint = XMFLOAT3(xmf3Minimum.x, xmf3Maximum.y, xmf3Maximum.z);
				}
			}
		}
		else
		{
			if (xmf3Normal.y >= 0.0f)
			{
				if (xmf3Normal.z >= 0.0f)
				{
					xmf3NearPoint = XMFLOAT3(xmf3Maximum.x, xmf3Minimum.y, xmf3Minimum.z);
				}
				else
				{
					xmf3NearPoint = XMFLOAT3(xmf3Maximum.x, xmf3Minimum.y, xmf3Maximum.z);
				}
			}
			else
			{
				if (xmf3Normal.z >= 0.0f)
				{
					xmf3NearPoint = XMFLOAT3(xmf3Maximum.x, xmf3Maximum.y, xmf3Minimum.z);
				}
				else
				{
					xmf3NearPoint = XMFLOAT3(xmf3Maximum.x, xmf3Maximum.y, xmf3Maximum.z);
				}
			}
		}
		if ((Vector3::DotProduct(xmf3Normal, xmf3NearPoint) + m_pxmf4FrustumPlanes[i].w) > 0.0f) return(false);
	}

	return(true);
#endif
}


void CCamera::GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up)
{
	m_xmf3Position = xmf3Position;
	m_xmf3LookAtWorld = xmf3LookAt;
	m_xmf3Up = xmf3Up;

	GenerateViewMatrix();
}

/*카메라 변환 행렬을 생성한다. 카메라의 위치 벡터, 카메라가 바라보는 지점, 카메라의 Up 벡터(로컬 y-축 벡터)를
파라메터로 사용하는 XMMatrixLookAtLH() 함수를 사용한다.*/
void CCamera::GenerateViewMatrix()
{
	m_xmf4x4View = Matrix4x4::LookAtLH(m_xmf3Position, m_xmf3LookAtWorld, m_xmf3Up);
}

void CCamera::RegenerateViewMatrix()
{
	//카메라의 z-축을 기준으로 카메라의 좌표축들이 직교하도록 카메라 변환 행렬을 갱신한다.
	//카메라의 z-축 벡터를 정규화한다.
	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	//카메라의 z-축과 y-축에 수직인 벡터를 x-축으로 설정한다.
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	//카메라의 z-축과 x-축에 수직인 벡터를 y-축으로 설정한다.
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
	
	m_xmf4x4View._11 = m_xmf3Right.x; m_xmf4x4View._12 = m_xmf3Up.x; m_xmf4x4View._13 =	m_xmf3Look.x;
	m_xmf4x4View._21 = m_xmf3Right.y; m_xmf4x4View._22 = m_xmf3Up.y; m_xmf4x4View._23 =	m_xmf3Look.y;
	m_xmf4x4View._31 = m_xmf3Right.z; m_xmf4x4View._32 = m_xmf3Up.z; m_xmf4x4View._33 =	m_xmf3Look.z;
	m_xmf4x4View._41 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Right);
	m_xmf4x4View._42 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Up);
	m_xmf4x4View._43 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Look);
	
	//m_xmf4x4InverseView._11 = m_xmf3Right.x; m_xmf4x4InverseView._12 = m_xmf3Right.y; m_xmf4x4InverseView._13 = m_xmf3Right.z;
	//m_xmf4x4InverseView._21 = m_xmf3Up.x; m_xmf4x4InverseView._22 = m_xmf3Up.y; m_xmf4x4InverseView._23 = m_xmf3Up.z;
	//m_xmf4x4InverseView._31 = m_xmf3Look.x; m_xmf4x4InverseView._32 = m_xmf3Look.y; m_xmf4x4InverseView._33 = m_xmf3Look.z;
	//m_xmf4x4InverseView._41 = m_xmf3Position.x; m_xmf4x4InverseView._42 = m_xmf3Position.y; m_xmf4x4InverseView._43 = m_xmf3Position.z;
	//
	//m_xmFrustumView.Transform(m_xmFrustumWorld, XMLoadFloat4x4(&m_xmf4x4InverseView));
}


void CCamera::InitCamera(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)
{
	BuildDescriptorHeaps(pDevice, pCommandList);
	CreateShaderVariables(pDevice, pCommandList);
	BuildRootSignature(pDevice, pCommandList);
}

void CCamera::BuildRootSignature(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)
{
	CD3DX12_ROOT_PARAMETER slotRootParameter[1];

	CD3DX12_DESCRIPTOR_RANGE cbvTable;
	cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
	slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(1, slotRootParameter, 0, nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(pDevice->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(m_RootSignature.GetAddressOf())));
}
void CCamera::BuildDescriptorHeaps(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = 1;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	ThrowIfFailed(pDevice->CreateDescriptorHeap(&cbvHeapDesc,
		IID_PPV_ARGS(&m_SrvDescriptorHeap)));
}

void CCamera::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList	*pd3dCommandList)
{
	m_ObjectCB = make_unique<UploadBuffer<VS_CB_CAMERA_INFO>>(pd3dDevice, 1, true);

	UINT objCBByteSize = D3DUtil::CalcConstantBufferByteSize(sizeof(VS_CB_CAMERA_INFO));
}

void CCamera::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	//XMFLOAT4X4 xmf4x4View;
	//XMStoreFloat4x4(&xmf4x4View, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4View)));
	////루트 파라메터 인덱스 1의
	//pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4View, 0);
	//
	//XMFLOAT4X4 xmf4x4Projection;
	//XMStoreFloat4x4(&xmf4x4Projection, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4Projection)));
	//
	//pd3dCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4Projection, 16);

	RegenerateViewMatrix();

	VS_CB_CAMERA_INFO cameraConstant;
	XMStoreFloat4x4(&cameraConstant.m_xmf4x4View, 
		DirectX::XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4View)));
	XMStoreFloat4x4(&cameraConstant.m_xmf4x4Projection,
		DirectX::XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4Projection)));
	XMStoreFloat4x4(&cameraConstant.m_xmf4x4InvProjection, 
		DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_xmf4x4View))));

	//for (int i = 0; i < NUM_DIRECTION_LIGHTS; ++i)
	//	XMStoreFloat4x4(&cameraConstant.m_xmf4x4ShadowProjection[i], XMLoadFloat4x4(&m_xmf4x4ShadowProjection[i]));
	::memcpy(&cameraConstant.m_xmf3Position, &m_xmf3Position, sizeof(XMFLOAT3));

	m_ObjectCB->CopyData(0, cameraConstant);
	pd3dCommandList->SetGraphicsRootConstantBufferView(0, m_ObjectCB->Resource()->GetGPUVirtualAddress());
}

void CCamera::ReleaseShaderVariables()
{
}

void CCamera::SetViewportsAndScissorRects(ID3D12GraphicsCommandList *pd3dCommandList)
{
	pd3dCommandList->RSSetViewports(1, &m_d3dViewport);
	pd3dCommandList->RSSetScissorRects(1, &m_d3dScissorRect);
}



CThirdPersonCamera::CThirdPersonCamera(CCamera *pCamera) : CCamera(pCamera)
{
	m_nMode = THIRD_PERSON_CAMERA;
	if (pCamera)
	{
		/*3인칭 카메라로 변경하기 이전의 카메라가 스페이스-쉽 카메라이면 카메라의 Up 벡터를 월드좌표의 y-축이 되도록
		한다. 이것은 스페이스-쉽 카메라의 로컬 y-축 벡터가 어떤 방향이든지 3인칭 카메라(대부분 사람인 경우)의 로컬 y
		축 벡터가 월드좌표의 y-축이 되도록 즉, 똑바로 서있는 형태로 설정한다는 의미이다. 그리고 로컬 x-축 벡터와 로컬
		z-축 벡터의 y-좌표가 0.0f가 되도록 한다. 이것은 로컬 x-축 벡터와 로컬 z-축 벡터를 xz-평면(지면)으로 투영하는
		것을 의미한다. 즉, 3인칭 카메라의 로컬 x-축 벡터와 로컬 z-축 벡터는 xz-평면에 평행하다.*/
		//if (pCamera->GetMode() == SPACESHIP_CAMERA)
		{
			m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
			m_xmf3Right.y = 0.0f;
			m_xmf3Look.y = 0.0f;
			m_xmf3Right = Vector3::Normalize(m_xmf3Right);
			m_xmf3Look = Vector3::Normalize(m_xmf3Look);
		}
	}
}

void CThirdPersonCamera::Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed)
{
	//플레이어가 있으면 플레이어의 회전에 따라 3인칭 카메라도 회전해야 한다.
	if (m_pPlayer)
	{
		SetLookAt(xmf3LookAt);
		/*
		XMFLOAT4X4 xmf4x4Rotate = Matrix4x4::Identity();
		XMFLOAT3 xmf3Right = m_pPlayer->GetRightVector();
		XMFLOAT3 xmf3Up = m_pPlayer->GetUpVector();
		XMFLOAT3 xmf3Look = m_pPlayer->GetLookVector();
		//플레이어의 로컬 x-축, y-축, z-축 벡터로부터 회전 행렬(플레이어와 같은 방향을 나타내는 행렬)을 생성한다.
		xmf4x4Rotate._11 = xmf3Right.x; xmf4x4Rotate._21 = xmf3Up.x; xmf4x4Rotate._31 =	xmf3Look.x;
		xmf4x4Rotate._12 = xmf3Right.y; xmf4x4Rotate._22 = xmf3Up.y; xmf4x4Rotate._32 =	xmf3Look.y;
		xmf4x4Rotate._13 = xmf3Right.z; xmf4x4Rotate._23 = xmf3Up.z; xmf4x4Rotate._33 =	xmf3Look.z;
		
		//카메라 오프셋 벡터를 회전 행렬로 변환(회전)한다.
		XMFLOAT3 xmf3Offset = Vector3::TransformCoord(m_xmf3Offset, xmf4x4Rotate);
		//회전한 카메라의 위치는 플레이어의 위치에 회전한 카메라 오프셋 벡터를 더한 것이다.
		XMFLOAT3 xmf3Position = Vector3::Add(m_pPlayer->GetPosition(), xmf3Offset);
		//현재의 카메라의 위치에서 회전한 카메라의 위치까지의 방향과 거리를 나타내는 벡터이다.
		XMFLOAT3 xmf3Direction = Vector3::Subtract(xmf3Position, m_xmf3Position,false);

		float fLength = Vector3::Length(xmf3Direction);
		xmf3Direction = Vector3::Normalize(xmf3Direction);

		//3인칭 카메라의 래그(Lag)는 플레이어가 회전하더라도 카메라가 동시에 따라서 회전하지 않고 약간의 시차를 두고 회전하는 효과를 구현하기 위한 것이다. m_fTimeLag가 1보다 크면 fTimeLagScale이 작아지고 실제 회전(이동)이 적
		//게 일어날 것이다. m_fTimeLag가 0이 아닌 경우 fTimeElapsed를 곱하고 있으므로 3인칭 카메라는 1초의 시간동안 (1.0f / m_fTimeLag)의 비율만큼 플레이어의 회전을 따라가게 될 것이다.
		float fTimeLagScale = (m_fTimeLag) ? fTimeElapsed * (1.0f / m_fTimeLag) : 1.0f;
		float fDistance = fLength * fTimeLagScale;
		if (fDistance > fLength) fDistance = fLength;
		if (fLength < 0.01f) fDistance = fLength;
		if (fDistance > 0)
		{
			//실제로 카메라를 회전하지 않고 이동을 한다(회전의 각도가 작은 경우 회전 이동은 선형 이동과 거의 같다).
			m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Direction, fDistance);
			//카메라가 플레이어를 바라보도록 한다.
			SetLookAt(xmf3LookAt);
		}
		*/
	}
}

void CThirdPersonCamera::SetLookAt(XMFLOAT3& xmf3LookAt)
{
	//현재 카메라의 위치에서 플레이어를 바라보기 위한 카메라 변환 행렬을 생성한다.
	XMFLOAT3 e_position = m_pPlayer->GetUpVector();
	XMFLOAT4X4 mtxLookAt = Matrix4x4::LookAtLH(m_xmf3Position, xmf3LookAt, e_position);
	//카메라 변환 행렬에서 카메라의 x-축, y-축, z-축을 구한다.
	m_xmf3Right = XMFLOAT3(mtxLookAt._11, mtxLookAt._21, mtxLookAt._31);
	m_xmf3Up = XMFLOAT3(mtxLookAt._12, mtxLookAt._22, mtxLookAt._32);
	m_xmf3Look = XMFLOAT3(mtxLookAt._13, mtxLookAt._23, mtxLookAt._33);
}