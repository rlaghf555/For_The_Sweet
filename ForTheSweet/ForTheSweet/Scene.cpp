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

void CScene::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, CPhysx* physx)
{
	//서버에서 오브젝트, 캐릭터 위치를 받는다.

	//m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);
	//m_nInstancingShaders = 1;
	//m_pInstancingShaders = new CInstancingShader[m_nInstancingShaders];
	//m_pInstancingShaders[0].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	//m_pInstancingShaders[0].BuildObjects(pd3dDevice, pd3dCommandList);

	//m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);
	/*
	m_nMapShader = 1;
	m_pMapShader = new CObjectsShader[m_nMapShader];
	m_pMapShader[0].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	m_pMapShader[0].BuildObjects(pd3dDevice, pd3dCommandList, XMFLOAT3(0, -10, 0), OBJECT_MAP);
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);
	*/
	
	//m_nPlayerObjectShaders = 1;
	//m_pPlayerObjectShaders = new CPlayerObjectsShader[m_nPlayerObjectShaders];
	//for (int i = 0; i < m_nPlayerObjectShaders; i++) {
	//	m_pPlayerObjectShaders[i].CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	//	m_pPlayerObjectShaders[i].BuildObjects(character_anim, pd3dDevice, pd3dCommandList,XMFLOAT3(50,17.5,0));
	//}

	//m_pMapShader = new CObjectsShader();
	//m_pMapShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	//Character_Model = new LoadModel("main_character.FBX", false);

	BuildRootSignature(pd3dDevice, pd3dCommandList);

	for (int i = 0; i < MAX_USER; ++i)
	{
		m_pPlayerShader[i] = new PlayerShader(character_anim);
		m_pPlayerShader[i]->BuildObjects(pd3dDevice, pd3dCommandList);
		m_pPlayer[i] = reinterpret_cast<CPlayer*>(m_pPlayerShader[i]->getPlayer());
	}

	if (Map_SELECT == M_Map_1) {
		m_MapShader[0] = new CModelShader(Map_Model[M_Map_1]);
		m_MapShader[0]->BuildObjects(pd3dDevice, pd3dCommandList, physx);

		m_MapShader[1] = new CModelShader(Map_Model[M_Map_1_wall]);
		m_MapShader[1]->BuildObjects(pd3dDevice, pd3dCommandList);

		for (int i = 0; i < 2; i++) {
			m_CottonShader[i] = new CottonCloudShader(Map_Model[M_Map_1_cotton]);
			m_CottonShader[i]->BuildObjects(pd3dDevice, pd3dCommandList, i);
		}
		
		m_Map_ObjectShader[0] = new Map_Macaron_Shader(Map_Model[M_Map_1_macaron]);
		m_Map_ObjectShader[0]->BuildObjects(pd3dDevice, pd3dCommandList);

		m_WavesShader = new WaveShader();
		m_WavesShader->BuildObjects(pd3dDevice, pd3dCommandList);
	}

	for (int i = 0; i < WEAPON_MAX_NUM; i++) {
		m_WeaponShader[i] = new WeaponShader(weapon_Model[i]);
		m_WeaponShader[i]->BuildObjects(pd3dDevice, pd3dCommandList, i);
	}
	
	m_BackGroundShader = new MeshShader();
	m_BackGroundShader->BuildObjects(pd3dDevice, pd3dCommandList);

	//m_pPlayer[0]->SetWeapon(true, 0);
	
	for (int i = 0; i < MAX_USER; i++) {
		bounding_box_test[i] = new testBox();
		bounding_box_test[i]->BuildObjects(pd3dDevice, pd3dCommandList, m_pPlayer[i], OBJECT_PLAYER);
	}
	for (int i = 0; i < WEAPON_MAX_NUM; i++) {
		for (int j = 0; j < WEAPON_EACH_NUM; j++) {
			weapon_box[i][j] = new testBox();
			weapon_box[i][j]->BuildObjects(pd3dDevice, pd3dCommandList, m_WeaponShader[i]->getObject(j), i);
		}
	}

}

void CScene::BuildRootSignature(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)
{
	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[2];

	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	pd3dDescriptorRanges[0].NumDescriptors = 1;
	pd3dDescriptorRanges[0].BaseShaderRegister = 2; //Game Objects
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = 0;

	pd3dDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[1].NumDescriptors = 1;
	pd3dDescriptorRanges[1].BaseShaderRegister = 0; //Texture2DArray
	pd3dDescriptorRanges[1].RegisterSpace = 0;
	pd3dDescriptorRanges[1].OffsetInDescriptorsFromTableStart = 0;

	D3D12_ROOT_PARAMETER pd3dRootParameters[5];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = CBVCameraInfo; //Camera
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[1].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[1].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[0]; //Game Objects
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = CBVMaterial; //Materials
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[3].Descriptor.ShaderRegister = CBVLights; //Lights
	pd3dRootParameters[3].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[4].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[4].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[1]; //Texture2DArray
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_STATIC_SAMPLER_DESC d3dSamplerDesc;
	::ZeroMemory(&d3dSamplerDesc, sizeof(D3D12_STATIC_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	d3dSamplerDesc.MipLODBias = 0;
	d3dSamplerDesc.MaxAnisotropy = 1;
	d3dSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dSamplerDesc.MinLOD = 0;
	d3dSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	d3dSamplerDesc.ShaderRegister = 0;
	d3dSamplerDesc.RegisterSpace = 0;
	d3dSamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 1;
	d3dRootSignatureDesc.pStaticSamplers = &d3dSamplerDesc;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob *pd3dSignatureBlob = NULL;
	ID3DBlob *pd3dErrorBlob = NULL;
	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);

	ThrowIfFailed(pDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_pd3dGraphicsRootSignature)));
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();
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
	for (int i = 0; i < MAX_USER; ++i)
	{
		if (m_pPlayerShader[i]) {
			m_pPlayerShader[i]->ReleaseShaderVariables();
			m_pPlayerShader[i]->ReleaseObjects();
			delete m_pPlayerShader[i];
		}
	}
	if (m_MapShader[0]) {
		m_MapShader[0]->ReleaseShaderVariables();
		m_MapShader[0]->ReleaseObjects();
		delete m_MapShader[0];
	}
	if (m_MapShader[1]) {
		m_MapShader[1]->ReleaseShaderVariables();
		m_MapShader[1]->ReleaseObjects();
		delete m_MapShader[1];
	}
	if (m_Map_ObjectShader[0]) {
		m_Map_ObjectShader[0]->ReleaseShaderVariables();
		m_Map_ObjectShader[0]->ReleaseObjects();
		delete m_Map_ObjectShader[0];
	}
	for (int i = 0; i < 2; ++i) {
		if (m_CottonShader[i]) {
			m_CottonShader[i]->ReleaseShaderVariables();
			m_CottonShader[i]->ReleaseObjects();
			delete m_CottonShader[i];
		}
	}
	for (int i = 0; i < WEAPON_MAX_NUM; ++i) {
		if (m_WeaponShader[i]) {
			m_WeaponShader[i]->ReleaseShaderVariables();
			m_WeaponShader[i]->ReleaseObjects();
			delete m_WeaponShader[i];
		}
	}
	for (int i = 0; i < WEAPON_MAX_NUM; ++i) {
		for (int j = 0; j < WEAPON_EACH_NUM; ++j) {
			if (weapon_box[i][j]) {
				weapon_box[i][j]->ReleaseShaderVariables();
				weapon_box[i][j]->ReleaseObjects();
				delete weapon_box[i][j];
			}
		}
	}
	for (int i = 0; i < MAX_USER; ++i) {
		if (bounding_box_test[i]) {
			bounding_box_test[i]->ReleaseShaderVariables();
			bounding_box_test[i]->ReleaseObjects();
			delete bounding_box_test[i];
		}
	}
}

void CScene::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nInstancingShaders; i++) m_pInstancingShaders[i].ReleaseUploadBuffers();
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
	//for (int i = 0; i < m_nInstancingShaders; i++) m_pInstancingShaders[i].AnimateObjects(fTimeElapsed);


	for (int i = 0; i < MAX_USER; ++i)
	{
		if (m_pPlayer[i]) {
			if (m_pPlayer[i]->GetConnected()) {
				m_pPlayerShader[i]->Animate(fTimeElapsed);
				if (m_pPlayer[i]->Get_Weapon_grab()) {
					AnimateWeapon(i);
				}
				XMFLOAT3 tmp = m_pPlayer[i]->GetPosition();

				bounding_box_test[i]->getObjects()->m_xmf4x4World = m_pPlayer[i]->m_xmf4x4World;
			}
		}
	}
	for (int i = 0; i < WEAPON_MAX_NUM; i++) {	
		for (int j = 0; j < WEAPON_EACH_NUM; j++)
			weapon_box[i][j]->getObjects()->m_xmf4x4World = m_WeaponShader[i]->getObject(j)->m_xmf4x4World;
	}
	
	if (Map_SELECT == M_Map_1) {
		if (m_WeaponShader[M_Weapon_cupcake]->get_cupcake_up_flag() == false)
			m_WeaponShader[M_Weapon_cupcake]->Animate(fTimeElapsed, macaron_flag, M_Weapon_cupcake);

		m_Map_ObjectShader[0]->Animate(fTimeElapsed, macaron_flag);

		m_WavesShader->Animate(fTimeElapsed);
	}
}

void CScene::AnimateWeapon(int i)
{
	XMFLOAT4 hand_pos;

	hand_pos = m_pPlayer[i]->GetHandPos();

	XMFLOAT4X4 bone = m_pPlayer[i]->GetBoneData()[14];

	XMFLOAT3 bone_right = XMFLOAT3(bone._11, bone._12, bone._13);
	bone_right = Vector3::Normalize(bone_right);

	XMFLOAT3 bone_up = XMFLOAT3(bone._21, bone._22, bone._23);
	bone_up = Vector3::Normalize(bone_up);

	XMFLOAT3 bone_look = XMFLOAT3(bone._31, bone._32, bone._33);
	bone_look = Vector3::Normalize(bone_look);

	bone._11 = bone_right.x; bone._12 = bone_right.y; bone._13 = bone_right.z;
	bone._21 = bone_up.x; bone._22 = bone_up.y; bone._23 = bone_up.z;
	bone._31 = bone_look.x; bone._32 = bone_look.y; bone._33 = bone_look.z;


	XMFLOAT3 player_look = m_pPlayer[i]->GetLook();
	XMFLOAT3 player_up = m_pPlayer[i]->GetUp();
	XMFLOAT3 player_right = m_pPlayer[i]->GetRight();

	XMFLOAT4X4 player = Matrix4x4::Identity();
	if (m_pPlayer[i]->getAnimIndex() != Anim_Lollipop_Guard) {
		player._11 = -player_right.x; player._12 = -player_right.y; player._13 = -player_right.z;
	}
	else
		player._11 = player_right.x; player._12 = player_right.y; player._13 = player_right.z;

	player._21 = player_up.x; player._22 = player_up.y; player._23 = player_up.z;
	player._31 = player_look.x; player._32 = player_look.y; player._33 = player_look.z;

	bone = Matrix4x4::Multiply(bone, player);

	XMFLOAT3 pos = XMFLOAT3(hand_pos.x, hand_pos.y, hand_pos.z);
	int weapon_type = m_pPlayer[i]->Get_Weapon_type();
	int weapon_index = m_pPlayer[i]->Get_Weapon_index();
	 
	m_WeaponShader[weapon_type]->getObject(weapon_index)->SetWorld(bone);
	m_WeaponShader[weapon_type]->getObject(weapon_index)->SetPosition(pos);
//	m_WeaponShader[weapon_type]->getObject(weapon_index)->Rotate(0, 0, 90);
	m_WeaponShader[weapon_type]->getObject(weapon_index)->Rotate(-70, 0, 0);

}

void CScene::CollisionProcess(int index)
{
	bounding_box_test[index]->bounding.Center = m_pPlayer[index]->GetPosition();
	for (int i = 0; i < WEAPON_MAX_NUM; i++) {
		for (int j = 0; j < WEAPON_EACH_NUM; j++) {
			if (m_WeaponShader[i]->getObject(j)) {
				weapon_box[i][j]->bounding.Center = m_WeaponShader[i]->getObject(j)->GetPosition();
				XMMATRIX tmp = XMLoadFloat4x4(&m_WeaponShader[i]->getObject(j)->m_xmf4x4World);
				bool result = weapon_box[i][j]->bounding.Intersects(bounding_box_test[index]->bounding);
				if (result) {
					m_pPlayer[index]->SetWeapon(false, i, j);
					//cout << "player "<<index<< "---"<<"무기 종류: " << i << ", 무기 번호: " << j << endl;
					return;
				}
			}
		}
	}
	m_pPlayer[index]->SetWeapon(false, -1, -1);
}

void CScene::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	for (int i = 0; i < MAX_USER; ++i) if (m_pPlayer[i]->GetConnected()) m_pPlayerShader[i]->Render(pd3dCommandList, pCamera);

	if (Map_SELECT == M_Map_1) {
		if (m_MapShader[0]) m_MapShader[0]->Render(pd3dCommandList, pCamera);
		if (m_MapShader[1]) m_MapShader[1]->Render(pd3dCommandList, pCamera);
		if (m_Map_ObjectShader[0]) m_Map_ObjectShader[0]->Render(pd3dCommandList, pCamera);
	}
	for (int i = 0; i < WEAPON_MAX_NUM; ++i) if (m_WeaponShader[i]) m_WeaponShader[i]->Render(pd3dCommandList, pCamera);

	if (Map_SELECT == M_Map_1) {
		for (int i = 0; i < 2; ++i) if (m_CottonShader[i]) m_CottonShader[i]->Render(pd3dCommandList, pCamera);
		if (m_WavesShader) m_WavesShader->Render(pd3dCommandList, pCamera);
	}
	if (m_BackGroundShader) m_BackGroundShader->Render(pd3dCommandList, pCamera);
	//for (int i = 0; i < WEAPON_MAX_NUM; i++) {
	//	for (int j = 0; j < 30; j++) {
	//		if (weapon_box[i][j])
	//			weapon_box[i][j]->Render(pd3dCommandList, pCamera);
	//	}
	//}
	//for (int i = 0; i < MAX_USER;i++)if (bounding_box_test[i]) bounding_box_test[i]->Render(pd3dCommandList, pCamera);
}
