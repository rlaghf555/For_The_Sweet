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
	/*
	m_nMapShader = 1;
	m_pMapShader = new CObjectsShader[m_nMapShader];
	m_pMapShader[0].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	m_pMapShader[0].BuildObjects(pd3dDevice, pd3dCommandList, XMFLOAT3(0, -10, 0), OBJECT_MAP);
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);
	*/
	
	m_nPlayerObjectShaders = 1;
	m_pPlayerObjectShaders = new CPlayerObjectsShader[m_nPlayerObjectShaders];
	for (int i = 0; i < m_nPlayerObjectShaders; i++) {
		m_pPlayerObjectShaders[i].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
		m_pPlayerObjectShaders[i].BuildObjects(character_anim, pd3dDevice, pd3dCommandList,XMFLOAT3(50,17.5,0));
	}

	//m_pMapShader = new CObjectsShader();
	//m_pMapShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	//Character_Model = new LoadModel("main_character.FBX", false);
	m_pPlayer = new CPlayer(character_anim, pd3dDevice, pd3dCommandList);

	Map_1_Model = new LoadModel("map_1.FBX", false);
	Map = new CMapObject(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature,Map_1_Model,XMFLOAT3(0, 0, 0));
	XMFLOAT3 a = XMFLOAT3(1.f, 0.f, 0.f);
	Map->Rotate(&a, 90.f);
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
	for (int i = 0; i < m_nInstancingShaders; i++) m_pInstancingShaders[i].AnimateObjects(fTimeElapsed);
	for (int i = 0; i < m_nPlayerObjectShaders; i++) m_pPlayerObjectShaders[i].AnimateObjects(fTimeElapsed);
}

void CScene::CollisionProcess()
{
	/*
	XMFLOAT4 a = XMFLOAT4(m_pMapShader[0].getPos().x, m_pMapShader[0].getPos().y, m_pMapShader[0].getPos().z, 1.f);
	XMVECTOR k = XMLoadFloat4(&a);
	//	PlaneIntersectionType intersectType = m_pPlayer->m_xmOOBB.Intersects(k);

	for (int i = 0; i < m_nInstancingShaders; i++) {
		m_pInstancingShaders[i].getPos();
	}
	for (int i = 0; i < m_nPlayerObjectShaders; i++) {
		m_pPlayerObjectShaders[i].getPos();
	}*/
}

void CScene::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	pCamera->UpdateShaderVariables(pd3dCommandList);
//	for (int i = 0; i < m_nMapShader; i++) {
//		m_pMapShader[i].Render(pd3dCommandList, pCamera);
//	}
	for (int i = 0; i < m_nInstancingShaders; i++)
	{
		m_pInstancingShaders[i].Render(pd3dCommandList, pCamera);
	}
	for (int i = 0; i < m_nPlayerObjectShaders; i++)
	{
		m_pPlayerObjectShaders[i].Render(pd3dCommandList, pCamera);
	}
	if(m_pMapShader)
		m_pMapShader->Render(pd3dCommandList, pCamera);
	if(Map)
		Map->Render(pd3dCommandList, pCamera);
}
