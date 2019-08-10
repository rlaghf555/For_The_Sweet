#include "stdafx.h"
#include "Player.h"
#include "Shader.h"

Jump::Jump() :
	mV0(0.0f),
	mJumpTime(0.0f),
	mJump(false)
{
}

void Jump::startJump(PxF32 v0)
{
	if (mJump)	return;
	mJumpTime = 0.0f;
	mV0 = v0;
	mJump = true;
}

void Jump::stopJump()
{
	if (!mJump)	return;
	mJump = false;
}

PxF32 Jump::getHeight(PxF32 elapsedTime)
{
	if (!mJump)	return 0.0f;

	mJumpTime += elapsedTime;
	const PxF32 h = gJumpGravity * mJumpTime*mJumpTime + mV0 * mJumpTime;
	jump_height = h * elapsedTime;

	return h * elapsedTime;
}

PxControllerBehaviorFlags CPlayer::getBehaviorFlags(const PxShape& shape, const PxActor& actor)
{
	if (actor.userData == (void *)(int)1)
	{
		//cout << "1" << endl;
		//PxExtendedVec3 pos = m_PlayerController->getPosition();
		//pos.y += 0.5f;
		//m_PlayerController->setPosition(pos);

		return PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT;
	}
	return PxControllerBehaviorFlags(0);
}

PxControllerBehaviorFlags CPlayer::getBehaviorFlags(const PxController& controller)
{
	return PxControllerBehaviorFlags(0);
}

PxControllerBehaviorFlags CPlayer::getBehaviorFlags(const PxObstacle& obstacle) 
{
	return PxControllerBehaviorFlags(0);
}

CPlayer::CPlayer(Model_Animation* ma, ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) : ModelObject(ma, pd3dDevice, pd3dCommandList)
{
	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, -1.f);
	
	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fMaxVelocityXZ = 0.0f;
	m_fMaxVelocityY = 0.0f;
	m_fFriction = 0.0f;
	
	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;
	
	m_pPlayerUpdatedContext = NULL;
	m_pCameraUpdatedContext = NULL;

	m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);
	if (m_pCamera) m_pCamera->InitCamera(pd3dDevice, pd3dCommandList);


	//플레이어를 위한 셰이더 변수를 생성한다.
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	//플레이어의 위치를 설정한다.
	XMFLOAT3 pposition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	SetPosition(pposition);
	SetLook(m_xmf3Look);  
}

CPlayer::~CPlayer()
{
	ReleaseShaderVariables();

	if (m_pCamera) delete m_pCamera;
	ModelObject::~ModelObject();
}

void CPlayer::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList	*pd3dCommandList)
{
	CGameObject::CreateShaderVariables(pd3dDevice, pd3dCommandList);
	//if (m_pCamera) m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CPlayer::ReleaseShaderVariables()
{
	CGameObject::ReleaseShaderVariables();
	if (m_pCamera) m_pCamera->ReleaseShaderVariables();
}

void CPlayer::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	CGameObject::UpdateShaderVariables(pd3dCommandList);
}

void CPlayer::SetPhysController(CPhysx* physx, PxUserControllerHitReport* callback, PxExtendedVec3* pos)
{
	m_PlayerController = physx->getCapsuleController(*pos, callback, this);
}

void CPlayer::move()
{
	if (m_PlayerController)
	{
		PxVec3 disp;
		disp.x = m_xmf3Velocity.x;
		disp.y = m_xmf3Velocity.y;
		disp.z = m_xmf3Velocity.z;

		m_PlayerController->move(disp, 0, 1 / 60, m_ControllerFilter);
	}
}

void CPlayer::init()
{
	m_hp = 100;
	m_mp = 0;
	weapon_skill = -1;
	weapon_index = -1;
	weapon_type = -1;

	m_dashed = false;
	weapon_grab = false;
	Scale_flag = false;
	m_status = STATUS::FREE;
	Scale_time = 0.3f;
	for (int i = 0; i < m_NumofAnim; i++)
		m_ani[i]->initspeed();
	m_ani[Anim_Walk]->SetAnimSpeed(1.3);
}

/*플레이어의 위치를 변경하는 함수이다. 플레이어의 위치는 기본적으로 사용자가 플레이어를 이동하기 위한 키보드를
누를 때 변경된다. 플레이어의 이동 방향(dwDirection)에 따라 플레이어를 fDistance 만큼 이동한다.*/
void CPlayer::Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity)
{
	if (dwDirection)
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		XMFLOAT3 xmf3Direction = XMFLOAT3(0, 0, 0);
		XMFLOAT3 Look = XMFLOAT3(0, 0, 0);
		//화살표 키 ‘↑’를 누르면 로컬 z-축 방향으로 이동(전진)한다. ‘↓’를 누르면 반대 방향으로 이동한다.
		if (dwDirection & DIR_FORWARD) {
			Look.z = -1.f;
			xmf3Direction = Look;
			xmf3Direction.z = 1.f;
			xmf3Shift = Vector3::Add(xmf3Shift, xmf3Direction, fDistance);
		}
		if (dwDirection & DIR_BACKWARD) {
			Look.z = 1.f;
			xmf3Direction = Look;
			xmf3Direction.z = -1.f;
			xmf3Shift = Vector3::Add(xmf3Shift, xmf3Direction, fDistance);
		}
		//화살표 키 ‘→’를 누르면 로컬 x-축 방향으로 이동한다. ‘←’를 누르면 반대 방향으로 이동한다.
		if (dwDirection & DIR_RIGHT) {
			Look.x = -1.f;
			xmf3Direction = Look;
			xmf3Direction.x = 1.f;
			xmf3Shift = Vector3::Add(xmf3Shift, xmf3Direction, fDistance);
		}
		if (dwDirection & DIR_LEFT) {
			Look.x = 1.f;
			xmf3Direction = Look;
			xmf3Direction.x = -1.f;
			xmf3Shift = Vector3::Add(xmf3Shift, xmf3Direction, fDistance);
		}
		m_xmf3Look = Look;
		//‘Page Up’을 누르면 로컬 y-축 방향으로 이동한다. ‘Page Down’을 누르면 반대 방향으로 이동한다.
		if (dwDirection & DIR_UP) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, fDistance);
		if (dwDirection & DIR_DOWN) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, -fDistance);

		//플레이어를 현재 위치 벡터에서 xmf3Shift 벡터만큼 이동한다.
		Move(xmf3Shift, bUpdateVelocity);
	}
}



void CPlayer::Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity)
{
	//bUpdateVelocity가 참이면 플레이어를 이동하지 않고 속도 벡터를 변경한다.
	if (bUpdateVelocity)
	{
		//플레이어의 속도 벡터를 xmf3Shift 벡터만큼 변경한다.
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	}
	else
	{
		//플레이어를 현재 위치 벡터에서 xmf3Shift 벡터만큼 이동한다.
		m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);
		//플레이어의 위치가 변경되었으므로 카메라의 위치도 xmf3Shift 벡터만큼 이동한다.
		//m_pCamera->Move(const_cast<XMFLOAT3&>(xmf3Shift));

		XMFLOAT3 bb = xmf3Shift;
		XMFLOAT3 pos = m_xmf3Position;
		m_pCamera->Move(bb);

	}
}

//플레이어를 로컬 x-축, y-축, z-축을 중심으로 회전한다.
void CPlayer::Rotate(float x, float y, float z)
{
	DWORD nCameraMode = m_pCamera->GetMode();
	//1인칭 카메라 또는 3인칭 카메라의 경우 플레이어의 회전은 약간의 제약이 따른다.
	if (nCameraMode == THIRD_PERSON_CAMERA)
	{
		/*로컬 x-축을 중심으로 회전하는 것은 고개를 앞뒤로 숙이는 동작에 해당한다. 그러므로 x-축을 중심으로 회전하는
		각도는 -89.0~+89.0도 사이로 제한한다. x는 현재의 m_fPitch에서 실제 회전하는 각도이므로 x만큼 회전한 다음
		Pitch가 +89도 보다 크거나 -89도 보다 작으면 m_fPitch가 +89도 또는 -89도가 되도록 회전각도(x)를 수정한다.*/
		if (x != 0.0f)
		{
			m_fPitch += x;
			if (m_fPitch > +89.0f) { x -= (m_fPitch - 89.0f); m_fPitch = +89.0f; }
			if (m_fPitch < -89.0f) { x -= (m_fPitch + 89.0f); m_fPitch = -89.0f; }
		}
		if (y != 0.0f)
		{
			//로컬 y-축을 중심으로 회전하는 것은 몸통을 돌리는 것이므로 회전 각도의 제한이 없다.
			m_fYaw += y;
			if (m_fYaw > 360.0f) m_fYaw -= 360.0f;
			if (m_fYaw < 0.0f) m_fYaw += 360.0f;
		}
		if (z != 0.0f)
		{
			/*로컬 z-축을 중심으로 회전하는 것은 몸통을 좌우로 기울이는 것이므로 회전 각도는 -20.0~+20.0도 사이로 제한된
			다. z는 현재의 m_fRoll에서 실제 회전하는 각도이므로 z만큼 회전한 다음 m_fRoll이 +20도 보다 크거나 -20도보다
			작으면 m_fRoll이 +20도 또는 -20도가 되도록 회전각도(z)를 수정한다.*/
			m_fRoll += z;
			if (m_fRoll > +20.0f) { z -= (m_fRoll - 20.0f); m_fRoll = +20.0f; }
			if (m_fRoll < -20.0f) { z -= (m_fRoll + 20.0f); m_fRoll = -20.0f; }
		}
		//카메라를 x, y, z 만큼 회전한다. 플레이어를 회전하면 카메라가 회전하게 된다.
		//m_pCamera->Rotate(x, y, z);

		/*플레이어를 회전한다. 1인칭 카메라 또는 3인칭 카메라에서 플레이어의 회전은 로컬 y-축에서만 일어난다. 플레이어
		의 로컬 y-축(Up 벡터)을 기준으로 로컬 z-축(Look 벡터)와 로컬 x-축(Right 벡터)을 회전시킨다. 기본적으로 Up 벡
		터를 기준으로 회전하는 것은 플레이어가 똑바로 서있는 것을 가정한다는 의미이다.*/
		if (y != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}

	/*회전으로 인해 플레이어의 로컬 x-축, y-축, z-축이 서로 직교하지 않을 수 있으므로 z-축(LookAt 벡터)을 기준으
	로 하여 서로 직교하고 단위벡터가 되도록 한다.*/
	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
}


//이 함수는 매 프레임마다 호출된다. 플레이어의 속도 벡터에 중력과 마찰력 등을 적용한다.
void CPlayer::Update(float fTimeElapsed)
{
	/*
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Gravity, fTimeElapsed, false));

	float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
	
	float fMaxVelocityXZ = m_fMaxVelocityXZ * fTimeElapsed;
	if (fLength > m_fMaxVelocityXZ)
	{
		m_xmf3Velocity.x *= (fMaxVelocityXZ / fLength);
		m_xmf3Velocity.z *= (fMaxVelocityXZ / fLength);
	}

	float fMaxVelocityY = m_fMaxVelocityY * fTimeElapsed;
	fLength = sqrtf(m_xmf3Velocity.y * m_xmf3Velocity.y);
	if (fLength > m_fMaxVelocityY) m_xmf3Velocity.y *= (fMaxVelocityY / fLength);
	
	//플레이어를 속도 벡터 만큼 실제로 이동한다(카메라도 이동될 것이다).
	Move(m_xmf3Velocity, false);
	
	
	if (m_pPlayerUpdatedContext) OnPlayerUpdateCallback(fTimeElapsed);
	
	
	//플레이어의 위치가 변경되었으므로 3인칭 카메라를 갱신한다.
	m_pCamera->Update(m_xmf3Position,
		fTimeElapsed);
	
	//카메라의 위치가 변경될 때 추가로 수행할 작업을 수행한다.
	if (m_pCameraUpdatedContext) OnCameraUpdateCallback(fTimeElapsed);
	//카메라가 3인칭 카메라이면 카메라가 변경된 플레이어 위치를 바라보도록 한다.
	m_pCamera->SetLookAt(m_xmf3Position);
	//카메라의 카메라 변환 행렬을 다시 생성한다.
	m_pCamera->RegenerateViewMatrix();
	
	fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = (m_fFriction * fTimeElapsed);
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));
	*/
}

void CPlayer::SetLook(XMFLOAT3 & xmf3Look)
{
	if (xmf3Look.x == 0 && xmf3Look.y == 0 && xmf3Look.z == 0)
		return;
	m_xmf3Look = xmf3Look;
	m_xmf3Look.y = 0.0f;
	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
}

/*카메라를 변경할 때 ChangeCamera() 함수에서 호출되는 함수이다. nCurrentCameraMode는 현재 카메라의 모드
이고 nNewCameraMode는 새로 설정할 카메라 모드이다.*/
CCamera *CPlayer::OnChangeCamera(DWORD nNewCameraMode)
{
	//새로운 카메라의 모드에 따라 카메라를 새로 생성한다.
	CCamera *pNewCamera = NULL;
	switch (nNewCameraMode)
	{
	
	case THIRD_PERSON_CAMERA:
		pNewCamera = new CThirdPersonCamera(m_pCamera);
		break;
	
	}
	/*현재 카메라의 모드가 스페이스-쉽 모드의 카메라이고 새로운 카메라가 1인칭 또는 3인칭 카메라이면 플레이어의
	Up 벡터를 월드좌표계의 y-축 방향 벡터(0, 1, 0)이 되도록 한다. 즉, 똑바로 서도록 한다. 그리고 스페이스-쉽 카메
	라의 경우 플레이어의 이동에는 제약이 없다. 특히, y-축 방향의 움직임이 자유롭다. 그러므로 플레이어의 위치는 공
	중(위치 벡터의 y-좌표가 0보다 크다)이 될 수 있다. 이때 새로운 카메라가 1인칭 또는 3인칭 카메라이면 플레이어의
	위치는 지면이 되어야 한다. 그러므로 플레이어의 Right 벡터와 Look 벡터의 y 값을 0으로 만든다. 이제 플레이어의
	Right 벡터와 Look 벡터는 단위벡터가 아니므로 정규화한다.*/

	if (pNewCamera)
	{
		pNewCamera->SetMode(nNewCameraMode);
		//현재 카메라를 사용하는 플레이어 객체를 설정한다.
		pNewCamera->SetPlayer(this);
	}
	if (m_pCamera) delete m_pCamera;
	return(pNewCamera);
}

/*플레이어의 위치와 회전축으로부터 월드 변환 행렬을 생성하는 함수이다. 플레이어의 Right 벡터가 월드 변환 행렬
의 첫 번째 행 벡터, Up 벡터가 두 번째 행 벡터, Look 벡터가 세 번째 행 벡터, 플레이어의 위치 벡터가 네 번째 행
벡터가 된다.*/
void CPlayer::OnPrepareRender()
{
	m_xmf4x4World._11 = m_xmf3Right.x;
	m_xmf4x4World._12 = m_xmf3Right.y;
	m_xmf4x4World._13 = m_xmf3Right.z;
	m_xmf4x4World._21 = m_xmf3Up.x;
	m_xmf4x4World._22 = m_xmf3Up.y;
	m_xmf4x4World._23 = m_xmf3Up.z;
	m_xmf4x4World._31 = m_xmf3Look.x;
	m_xmf4x4World._32 = m_xmf3Look.y;
	m_xmf4x4World._33 = m_xmf3Look.z;
	m_xmf4x4World._41 = m_xmf3Position.x;
	m_xmf4x4World._42 = m_xmf3Position.y;
	m_xmf4x4World._43 = m_xmf3Position.z;
}

void CPlayer::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	OnPrepareRender();
	DWORD nCameraMode = (pCamera) ? pCamera->GetMode() : 0x00;
	//카메라 모드가 3인칭이면 플레이어 객체를 렌더링한다.
	if (nCameraMode == THIRD_PERSON_CAMERA) CGameObject::Render(pd3dCommandList, pCamera);
}
CCamera *CPlayer::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode) return(m_pCamera);

	XMFLOAT3 ggravity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	switch (nNewCameraMode)
	{
	case THIRD_PERSON_CAMERA:
		//플레이어의 특성을 3인칭 카메라 모드에 맞게 변경한다. 지연 효과와 카메라 오프셋을 설정한다.
		SetFriction(250.0f);
		XMFLOAT3 ggravity = XMFLOAT3(0.0f, 0.0f, 0.0f);
		SetGravity(ggravity);
		SetMaxVelocityXZ(125.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA);
		//3인칭 카메라의 지연 효과를 설정한다. 값을 0.25f 대신에 0.0f와 1.0f로 설정한 결과를 비교하기 바란다.
		m_pCamera->SetTimeLag(0.25f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 90.f, -130.0f));
		m_pCamera->SetPosition(Vector3::Add(m_xmf3Position, m_pCamera->GetOffset()));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	default:
		break;
	}
	//플레이어를 시간의 경과에 따라 갱신(위치와 방향을 변경: 속도, 마찰력, 중력 등을 처리)한다.
	Update(fTimeElapsed);
	return(m_pCamera);
}
/*
CGamePlayer::CGamePlayer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{

	CMesh *pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 50, 30, 100);
	//CMesh *pCubeMesh = new CAirplaneMeshDiffused(pd3dDevice, pd3dCommandList);
	SetMesh(pCubeMesh);

	//MeshGeometry *pCubeMesh = new LoadModel("police.fbx");
	
	//플레이어의 카메라를 스페이스-쉽 카메라로 변경(생성)한다.
	m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);
	
	//플레이어를 위한 셰이더 변수를 생성한다.
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	
	//플레이어의 위치를 설정한다.
	XMFLOAT3 pposition = XMFLOAT3(0.0f, 0.0f, -50.0f);
	SetPosition(pposition);
	
	//플레이어(비행기) 메쉬를 렌더링할 때 사용할 셰이더를 생성한다.
	CPlayerShader *pShader = new CPlayerShader();
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	SetShader(pShader);
}

CGamePlayer::~CGamePlayer()
{
}

void CGamePlayer::OnPrepareRender()
{
	CPlayer::OnPrepareRender();
	
	//비행기 모델을 그리기 전에 x-축으로 90도 회전한다.
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(90.f), 0.0f, 0.0f);
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}


//카메라를 변경할 때 호출되는 함수이다. nNewCameraMode는 새로 설정할 카메라 모드이다.
CCamera *CGamePlayer::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode) return(m_pCamera);

	XMFLOAT3 ggravity = XMFLOAT3(0.0f, -250.0f, 0.0f);
	switch (nNewCameraMode)
	{
	case THIRD_PERSON_CAMERA:
		//플레이어의 특성을 3인칭 카메라 모드에 맞게 변경한다. 지연 효과와 카메라 오프셋을 설정한다.
		SetFriction(250.0f);
		//XMFLOAT3 ggravity = XMFLOAT3(0.0f, 0.0f, 0.0f);
		SetGravity(ggravity);
		SetMaxVelocityXZ(125.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
		//3인칭 카메라의 지연 효과를 설정한다. 값을 0.25f 대신에 0.0f와 1.0f로 설정한 결과를 비교하기 바란다.
		m_pCamera->SetTimeLag(0.25f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 200.0f, -500.0f));
		m_pCamera->SetPosition(Vector3::Add(m_xmf3Position, m_pCamera->GetOffset()));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f,	1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	default:
		break;
	}
	//플레이어를 시간의 경과에 따라 갱신(위치와 방향을 변경: 속도, 마찰력, 중력 등을 처리)한다.
	Update(fTimeElapsed);
	return(m_pCamera);
}

ModelPlayer::ModelPlayer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, LoadModel *Character_Model)
{
	m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);
	if (m_pCamera) m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	lm = Character_Model;
	m_nMeshes = lm->getNumMesh();
	if (m_nMeshes > 0)
	{
		m_ppMeshes = vector<unique_ptr<MMesh>>(m_nMeshes);
		for (UINT i = 0; i < m_nMeshes; i++)
			m_ppMeshes[i] = nullptr;
	}
	lm->SetMeshes(pd3dDevice, pd3dCommandList);
	for (UINT i = 0; i < m_nMeshes; i++) {
		if (i > 0)
			lm->SetTextureIndex(i, i);
		SetMesh(i, lm->getMeshes()[i]);
	}

	//플레이어를 위한 셰이더 변수를 생성한다.
	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	//플레이어의 위치를 설정한다.
	XMFLOAT3 pposition = XMFLOAT3(0.0f, 0.0f, -50.0f);
	SetPosition(pposition);
}
CCamera * ModelPlayer::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode) return(m_pCamera);

	XMFLOAT3 ggravity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	switch (nNewCameraMode)
	{
	case THIRD_PERSON_CAMERA:
		//플레이어의 특성을 3인칭 카메라 모드에 맞게 변경한다. 지연 효과와 카메라 오프셋을 설정한다.
		SetFriction(250.0f);
		XMFLOAT3 ggravity = XMFLOAT3(0.0f, 0.0f, 0.0f);
		SetGravity(ggravity);
		SetMaxVelocityXZ(125.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
		//3인칭 카메라의 지연 효과를 설정한다. 값을 0.25f 대신에 0.0f와 1.0f로 설정한 결과를 비교하기 바란다.
		m_pCamera->SetTimeLag(0.25f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 300.f, -500.0f));
		m_pCamera->SetPosition(Vector3::Add(m_xmf3Position, m_pCamera->GetOffset()));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	default:
		break;
	}
	//플레이어를 시간의 경과에 따라 갱신(위치와 방향을 변경: 속도, 마찰력, 중력 등을 처리)한다.
	Update(fTimeElapsed);
	return(m_pCamera);
}

ModelPlayer::~ModelPlayer() {

}
*/