#include "stdafx.h"
#include "Scene.h"

CScene::CScene()
{
	//m_pd3dPipelineState = NULL;
	//m_pd3dGraphicsRootSignature = NULL;
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM	lParam)
{
	return(false);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

CScene::~CScene()
{
}

bool CScene::ProcessInput(UCHAR *pKeysBuffer)
{
	return true;
}

void CScene::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	//서버에서 오브젝트, 캐릭터 위치를 받는다.

	//m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);
	//m_nInstancingShaders = 1;
	//m_pInstancingShaders = new CInstancingShader[m_nInstancingShaders];
	//m_pInstancingShaders[0].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	//m_pInstancingShaders[0].BuildObjects(pd3dDevice, pd3dCommandList);

	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);
	m_nMapShader = 1;
	m_pMapShader = new CObjectsShader[m_nMapShader];
	m_pMapShader[0].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	m_pMapShader[0].BuildObjects(pd3dDevice, pd3dCommandList, XMFLOAT3(0, -10, 0), OBJECT_MAP);

	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);
	m_nPlayerObjectShaders = 7;
	m_pPlayerObjectShaders = new CPlayerObjectsShader[m_nPlayerObjectShaders];
	for (int i = 0; i < m_nPlayerObjectShaders; i++) {
		m_pPlayerObjectShaders[i].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
		m_pPlayerObjectShaders[i].BuildObjects(pd3dDevice, pd3dCommandList,XMFLOAT3((i-3)*200,50,0));
	}
	
	Character_Model = new LoadModel("main_character.FBX", false);
	m_pPlayer = new ModelPlayer(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, Character_Model);
}

void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();
	for (int i = 0; i < m_nInstancingShaders; i++)
	{
		m_pInstancingShaders[i].ReleaseShaderVariables();
		m_pInstancingShaders[i].ReleaseObjects();
	}
	if (m_pInstancingShaders) delete[] m_pInstancingShaders;
	for (int i = 0; i < m_nPlayerObjectShaders; i++)
	{
		m_pPlayerObjectShaders[i].ReleaseShaderVariables();
		m_pPlayerObjectShaders[i].ReleaseObjects();
	}
	if (m_pPlayerObjectShaders) delete[] m_pPlayerObjectShaders;
}

void CScene::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nInstancingShaders; i++) m_pInstancingShaders[i].ReleaseUploadBuffers();
	for (int i = 0; i < m_nPlayerObjectShaders; i++) m_pPlayerObjectShaders[i].ReleaseUploadBuffers();
}

ID3D12RootSignature *CScene::GetGraphicsRootSignature()
{
	return(m_pd3dGraphicsRootSignature);
}

ID3D12RootSignature *CScene::CreateGraphicsRootSignature(ID3D12Device *pd3dDevice)
{
	ID3D12RootSignature *pd3dGraphicsRootSignature = NULL;
	D3D12_ROOT_PARAMETER pd3dRootParameters[3];
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[0].Constants.Num32BitValues = 16;
	pd3dRootParameters[0].Constants.ShaderRegister = 0; //b0: Player
	pd3dRootParameters[0].Constants.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues = 32;
	pd3dRootParameters[1].Constants.ShaderRegister = 1; //b1: Camera
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 0; //t0
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = NULL;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;
	ID3DBlob *pd3dSignatureBlob = NULL;
	ID3DBlob *pd3dErrorBlob = NULL;
	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();
	return(pd3dGraphicsRootSignature);
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	//XMFLOAT4 a = XMFLOAT4(m_pMapShader[0].getPos().x, m_pMapShader[0].getPos().y, m_pMapShader[0].getPos().z, 1.f);
	//if (m_pPlayer->m_xmOOBB.Intersects(XMLoadFloat4(&a))) {
	//	XMFLOAT3 b = m_pPlayer->GetPosition();
	//	b.z += 1;
	//	m_pPlayer->SetPosition(b);
	//}

	for (int i = 0; i < m_nInstancingShaders; i++) m_pInstancingShaders[i].AnimateObjects(fTimeElapsed);
	for (int i = 0; i < m_nPlayerObjectShaders; i++) m_pPlayerObjectShaders[i].AnimateObjects(fTimeElapsed);
}

void CScene::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	pCamera->UpdateShaderVariables(pd3dCommandList);
	for (int i = 0; i < m_nMapShader; i++) {
		m_pMapShader[i].Render(pd3dCommandList, pCamera);
	}
	for (int i = 0; i < m_nInstancingShaders; i++)
	{
		m_pInstancingShaders[i].Render(pd3dCommandList, pCamera);
	}
	for (int i = 0; i < m_nPlayerObjectShaders; i++)
	{
		m_pPlayerObjectShaders[i].Render(pd3dCommandList, pCamera);
	}
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
		XMFLOAT3 ggravity = XMFLOAT3(0.0f, -250.0f, 0.0f);
		SetGravity(ggravity);
		SetMaxVelocityXZ(125.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
		//3인칭 카메라의 지연 효과를 설정한다. 값을 0.25f 대신에 0.0f와 1.0f로 설정한 결과를 비교하기 바란다.
		m_pCamera->SetTimeLag(0.25f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 200.0f, -500.0f));
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