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
	animate_flag = Selected_Map;
	for (int i = 0; i < MAX_USER; ++i)
	{
		m_pPlayerShader[i] = new PlayerShader(character_anim);
		m_pPlayerShader[i]->BuildObjects(pd3dDevice, pd3dCommandList);
		m_pPlayer[i] = reinterpret_cast<CPlayer*>(m_pPlayerShader[i]->getPlayer());

		m_pPlayerShadowShader[i] = new PlayerShadowShader(character_anim);
		m_pPlayerShadowShader[i]->BuildObjects(pd3dDevice, pd3dCommandList);
	}

	if (Selected_Map == M_Map_1) {
		m_MapShader[0] = new CModelShader(Map_Model[M_Map_1]);
		m_MapShader[0]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_1);

		m_MapShader[1] = new CModelShader(Map_Model[M_Map_1_wall]);
		m_MapShader[1]->BuildObjects(pd3dDevice, pd3dCommandList, M_Map_1);

		for (int i = 0; i < 2; i++) {
			m_CottonShader[i] = new CottonCloudShader(Map_Model[M_Map_1_cotton]);
			m_CottonShader[i]->BuildObjects(pd3dDevice, pd3dCommandList, i);
		}

		m_MapShader[2] = new CModelShader(Map_Model[M_Map_1_macaron]);
		m_MapShader[2]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_1_macaron_1);
		
		for (int i = 0; i < WEAPON_MAX_NUM; i++) {
			m_WeaponShader[i] = new WeaponShader(weapon_Model[i]);
			m_WeaponShader[i]->BuildObjects(pd3dDevice, pd3dCommandList, i, M_Map_1);
		}

		m_ShadowShader[0] = new ShadowDebugShader(Map_Model[M_Map_1_Shadow_test]);
		m_ShadowShader[0]->BuildObjects(pd3dDevice, pd3dCommandList, M_Map_1);
	}
	if (Selected_Map == M_Map_2) {
		m_MapShader[0] = new CModelShader(Map_Model[M_Map_2]);
		m_MapShader[0]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_2);

		m_MapShader[1] = new CModelShader(Map_Model[M_Map_1_macaron]);
		m_MapShader[1]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_1_macaron);
		m_MapShader[2] = new CModelShader(Map_Model[M_Map_1_macaron]);
		m_MapShader[2]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_1_macaron_2);
		
		m_MapShader[3] = new CModelShader(Map_Model[M_Map_2_chocolate_bar]);
		m_MapShader[3]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_2_chocolate_bar);
		m_MapShader[4] = new CModelShader(Map_Model[M_Map_2_chocolate_bar]);
		m_MapShader[4]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_2_chocolate_bar_2);
		
		for (int i = 0; i < WEAPON_MAX_NUM; i++) {
			m_WeaponShader[i] = new WeaponShader(weapon_Model[i]);
			m_WeaponShader[i]->BuildObjects(pd3dDevice, pd3dCommandList, i, M_Map_2);
		}
	}
	if (Selected_Map == M_Map_3) {
		for (int i = 0; i < 8; i++) {
			door[i] = new testBox();
			door[i]->BuildObjects(pd3dDevice, pd3dCommandList, m_pPlayer[i], 11 + i);
		}

		m_MapShader[0] = new CModelShader(Map_Model[M_Map_3]);
		m_MapShader[0]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3);
		m_MapShader[1] = new CModelShader(Map_Model[M_Map_3]);
		m_MapShader[1]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3_2);

		m_MapShader[2] = new CModelShader(Map_Model[M_Map_3_cake_2]);
		m_MapShader[2]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3_cake_2);
		m_MapShader[3] = new CModelShader(Map_Model[M_Map_3_cake_2]);
		m_MapShader[3]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3_cake_2_2);

		m_MapShader[4] = new CModelShader(Map_Model[M_Map_3_cake_3]);
		m_MapShader[4]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3_cake_3);
		m_MapShader[5] = new CModelShader(Map_Model[M_Map_3_cake_3]);
		m_MapShader[5]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3_cake_3_2);

		m_MapShader[6] = new CModelShader(Map_Model[M_Map_3_in_black]);
		m_MapShader[6]->BuildObjects(pd3dDevice, pd3dCommandList, M_Map_3);

		m_MapShader[7] = new CModelShader(Map_Model[M_Map_1_macaron]);
		m_MapShader[7]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_1_macaron_3);

		m_MapShader[8] = new CModelShader(Map_Model[M_Map_3_in]);
		m_MapShader[8]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3_in);
		m_MapShader[9] = new CModelShader(Map_Model[M_Map_3_in_stair_1]);
		m_MapShader[9]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3_in_stair_1);
		m_MapShader[10] = new CModelShader(Map_Model[M_Map_3_in_stair_2]);
		m_MapShader[10]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3_in_stair_2);

		m_MapShader[11] = new CModelShader(Map_Model[M_Map_3_in]);
		m_MapShader[11]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3_in_2);
		m_MapShader[12] = new CModelShader(Map_Model[M_Map_3_in_stair_1]);
		m_MapShader[12]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3_in_stair_1_2);
		m_MapShader[13] = new CModelShader(Map_Model[M_Map_3_in_stair_2]);
		m_MapShader[13]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3_in_stair_2_2);

		m_BridgeShader = new BridgeShader(Map_Model[M_Map_3_bridge]);
		m_BridgeShader->BuildObjects(pd3dDevice, pd3dCommandList, physx);

		m_ShadowShader[0] = new ShadowDebugShader(Map_Model[M_Map_3_cake_2]);
		m_ShadowShader[0]->BuildObjects(pd3dDevice, pd3dCommandList, M_Map_3_cake_2);
		m_ShadowShader[1] = new ShadowDebugShader(Map_Model[M_Map_3_cake_2]);
		m_ShadowShader[1]->BuildObjects(pd3dDevice, pd3dCommandList, M_Map_3_cake_2_2);

		m_ShadowShader[2] = new ShadowDebugShader(Map_Model[M_Map_3_cake_3]);
		m_ShadowShader[2]->BuildObjects(pd3dDevice, pd3dCommandList, M_Map_3_cake_3);
		m_ShadowShader[3] = new ShadowDebugShader(Map_Model[M_Map_3_cake_3]);
		m_ShadowShader[3]->BuildObjects(pd3dDevice, pd3dCommandList, M_Map_3_cake_3_2);

		m_ShadowReverseShader = new ShadowREverseShader();
		m_ShadowReverseShader->BuildObjects(pd3dDevice, pd3dCommandList);

		for (int i = 0; i < 2; i++) {
			m_StairShader[i] = new StairShader(Map_Model[M_Map_3_stair]);
			m_StairShader[i]->BuildObjects(pd3dDevice, pd3dCommandList, physx, i);
		}

		for (int i = 0; i < WEAPON_MAX_NUM; i++) {
			m_WeaponShader[i] = new WeaponShader(weapon_Model[i]);
			m_WeaponShader[i]->BuildObjects(pd3dDevice, pd3dCommandList, i, M_Map_2);
		}
	}

	m_WavesShader = new WaveShader();
	m_WavesShader->BuildObjects(pd3dDevice, pd3dCommandList, Map_SELECT);

	m_BackGroundShader[0] = new MeshShader();
	m_BackGroundShader[0]->BuildObjects(pd3dDevice, pd3dCommandList, 0);

	if (Selected_Map == M_Map_3) {
		m_BackGroundShader[1] = new MeshShader();
		m_BackGroundShader[1]->BuildObjects(pd3dDevice, pd3dCommandList, 1);
	}
	//m_pPlayer[0]->SetWeapon(true, 0);
	
	for (int i = 0; i < MAX_USER; i++) {
		bounding_box_test[i] = new testBox();
		bounding_box_test[i]->BuildObjects(pd3dDevice, pd3dCommandList, m_pPlayer[i], OBJECT_PLAYER);
	}
	for (int i = 0; i < WEAPON_MAX_NUM; i++) {
		for (int j = 0; j < WEAPON_EACH_NUM; j++) {
			weapon_box[i][j] = new testBox();
			weapon_box[i][j]->BuildObjects(pd3dDevice, pd3dCommandList, m_WeaponShader[i]->getObject(j), i);
			if (i == 4) {
				j = WEAPON_EACH_NUM;
			}
		}
	}
	for (int i = 0; i < PATTERN_LIGHTNING_NUM; i++) {
		m_EffectShader[i] = new EffectShader();
		m_EffectShader[i]->BuildObjects(pd3dDevice, pd3dCommandList, i);
	}
}

void CScene::BuildUI(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList)
{
	m_ppUIShaders.clear();
	m_nUIShaders = 22;
	m_ppUIShaders.resize(m_nUIShaders);

	//UIShader* pSample = new UIShader();
	//pSample->BuildObjects(pDevice, pCommandList, 1);
	//XMFLOAT2 pos = XMFLOAT2(0.5f, 0.045f);
	//pSample->SetPosScreenRatio(pos, 0);
	//m_ppUIShaders[0] = pSample;

	for (int i = 0; i < 4; i++) {
		UIHPBarShader* pHPBar = new UIHPBarShader();
		pHPBar->BuildObjects(pDevice, pCommandList, 1);
		XMFLOAT2 pos = XMFLOAT2(172 + i * 313, 150);
		pHPBar->SetPos(&pos, 0);
		pos = XMFLOAT2(172 + i * 313, 156);
		pHPBar->SetPos(&pos, 1);
		pos = XMFLOAT2(172 + i * 313, 136);
		pHPBar->SetPos(&pos, 2);
		m_ppUIShaders[i] = pHPBar;

	}
	for (int i = 0; i < 4; i++) {
		UIHPBarShader* pHPBar = new UIHPBarShader();
		pHPBar->BuildObjects(pDevice, pCommandList, 1);
		XMFLOAT2 pos = XMFLOAT2(172 + i * 313, 40);
		pHPBar->SetPos(&pos, 0);
		pos = XMFLOAT2(172 + i * 313, 46);
		pHPBar->SetPos(&pos, 1);
		pos = XMFLOAT2(172 + i * 313, 26);
		pHPBar->SetPos(&pos, 2);
		m_ppUIShaders[i + 4] = pHPBar;
	}
	m_ppUIShaders[0]->getObejct(1)->SetHP(m_pPlayer[0]->hp);	
	m_ppUIShaders[0]->getObejct(2)->SetHP(m_pPlayer[0]->mp);
	UITimeShader* pTime = new UITimeShader();
	pTime->BuildObjects(pDevice, pCommandList, 1);
	m_ppUIShaders[8] = pTime;
	m_ppUIShaders[8]->SetTime(300);

	UIDotShader* pDot = new UIDotShader();
	pDot->BuildObjects(pDevice, pCommandList);
	m_ppUIShaders[9] = pDot;

	UIReadyShader* pReady = new UIReadyShader();
	pReady->BuildObjects(pDevice, pCommandList);
	XMFLOAT2 pos = XMFLOAT2(1300, 400);
	pReady->SetPos(&pos, 0);
	m_ppUIShaders[10] = pReady;

	UIFightShader* pFight = new UIFightShader();
	pos = XMFLOAT2(1280, 400);
	pFight->BuildObjects(pDevice, pCommandList);
	pFight->SetPos(&pos, 0);
	m_ppUIShaders[11] = pFight;
	for (int i = 0; i < 4; i++) {
		UIIDShader* pID = new UIIDShader();
		pID->BuildObjects(pDevice, pCommandList);
		for (int j = 0; j < 10; j++) {
			pos = XMFLOAT2(50 + j * 10 + i * 313, 200);
			pID->SetPos(&pos, j);
		}
		wchar_t tmp[10] = L"ABCD4";			//플레이어 아이디 입력하자
		pID->SetID(tmp);
		m_ppUIShaders[12+i] = pID;
	}
	for (int i = 0; i < 4; i++) {
		UIIDShader* pID = new UIIDShader();
		pID->BuildObjects(pDevice, pCommandList);
		for (int j = 0; j < 10; j++) {
			pos = XMFLOAT2(50 + j * 10 + i * 313, 90);
			pID->SetPos(&pos, j);
		}
		wchar_t tmp[10] = L"01234ABCD";
		pID->SetID(tmp);
		m_ppUIShaders[12 + i + 4] = pID;
	}
	WinLoseShader *winlose = new WinLoseShader();
	winlose->BuildObjects(pDevice, pCommandList);
	m_ppUIShaders[20] = winlose;
	//m_ppUIShaders[20]->ShowMessage(false);   //true면 win false면 lose

	//안개는 UISHADER 마지막에 
	FogShader *fog = new FogShader();
	fog->BuildObjects(pDevice, pCommandList);
	m_ppUIShaders[21] = fog;
	m_ppUIShaders[21]->SetFog();

	m_MessageShader = new MessageShader();
	m_MessageShader->BuildObjects(pDevice, pCommandList);
	m_MessageShader->ShowMessage(MESSAGE_LIGHTING);

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
	for (int i = 0; i < 14; i++) {
		if (m_MapShader[i]) {
			m_MapShader[i]->ReleaseShaderVariables();
			m_MapShader[i]->ReleaseObjects();
			delete m_MapShader[i];
		}
	}
	for (int i = 0; i < 5; ++i) {
		if (m_ShadowShader[i]) {
			m_ShadowShader[i]->ReleaseShaderVariables();
			m_ShadowShader[i]->ReleaseObjects();
			delete m_ShadowShader[i];
		}
	}
	if (m_ShadowReverseShader) {
		m_ShadowReverseShader->ReleaseShaderVariables();
		m_ShadowReverseShader->ReleaseObjects();
		delete m_ShadowReverseShader;
	}
	if (m_BridgeShader) {
		m_BridgeShader->ReleaseShaderVariables();
		m_BridgeShader->ReleaseObjects();
		delete m_BridgeShader;
	}
	for (int i = 0; i < 2; ++i) {
		if (m_CottonShader[i]) {
			m_CottonShader[i]->ReleaseShaderVariables();
			m_CottonShader[i]->ReleaseObjects();
			delete m_CottonShader[i];
		}
	}
	for (int i = 0; i < 2; ++i) {
		if (m_StairShader[i]) {
			m_StairShader[i]->ReleaseShaderVariables();
			m_StairShader[i]->ReleaseObjects();
			delete m_StairShader[i];
		}
	}
	for (int i = 0; i < 2; ++i) {
		if (m_BackGroundShader[i]) {
			m_BackGroundShader[i]->ReleaseShaderVariables();
			m_BackGroundShader[i]->ReleaseObjects();
			delete m_BackGroundShader[i];
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
	for (int i = 0; i < 8; ++i) {
		if (door[i]) {
			door[i]->ReleaseShaderVariables();
			door[i]->ReleaseObjects();
			delete door[i];
		}
	}
	for (UINT i = 0; i < m_nUIShaders; ++i) {
		if (m_ppUIShaders[i]) {
			m_ppUIShaders[i]->ReleaseShaderVariables();
			m_ppUIShaders[i]->ReleaseObjects();
			delete m_ppUIShaders[i];
		}
	}
	if (m_MessageShader) {
		m_MessageShader->ReleaseShaderVariables();
		m_MessageShader->ReleaseObjects();
		delete m_MessageShader;
	}
	for (int i = 0; i < PATTERN_LIGHTNING_NUM; ++i) {
		if (m_EffectShader[i]) {
			m_EffectShader[i]->ReleaseShaderVariables();
			m_EffectShader[i]->ReleaseObjects();
			delete m_EffectShader[i];
		}
	}
}

void CScene::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nInstancingShaders; i++) m_pInstancingShaders[i].ReleaseUploadBuffers();

	for (int i = 0; i < MAX_USER; ++i)
	{
		if (m_pPlayerShader[i]) {
			m_pPlayerShader[i]->ReleaseUploadBuffers();
		}
	}
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
		for (int j = 0; j < WEAPON_EACH_NUM; j++) {
			if (i == 4 && j == 1)//컵케익 1개
				break;
			weapon_box[i][j]->getObjects()->m_xmf4x4World = m_WeaponShader[i]->getObject(j)->m_xmf4x4World;
		}
	}

	if (Selected_Map == M_Map_1) {
		if (m_WeaponShader[M_Weapon_cupcake]->get_cupcake_up_flag() == false)
			m_WeaponShader[M_Weapon_cupcake]->Animate(fTimeElapsed, animate_flag, M_Weapon_cupcake);

		m_MapShader[2]->Animate(fTimeElapsed, animate_flag);
	}
	if (Selected_Map == M_Map_3) {
		m_MapShader[7]->Animate(fTimeElapsed, animate_flag);
	}

	m_WavesShader->Animate(fTimeElapsed, Selected_Map);

	for (UINT i = 0; i < m_nUIShaders; i++) {
		if (m_ppUIShaders[i]) {
			m_ppUIShaders[i]->UpdateState(ready_state);
			m_ppUIShaders[i]->Animate(fTimeElapsed);
		}
	}
	m_MessageShader->Animate(fTimeElapsed);
	for (int i = 0; i < PATTERN_LIGHTNING_NUM; i++)m_EffectShader[i]->Animate(fTimeElapsed);

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
	else {
		player._11 = player_right.x; player._12 = player_right.y; player._13 = player_right.z;
	}

	player._21 = player_up.x; player._22 = player_up.y; player._23 = player_up.z;

	player._31 = player_look.x; player._32 = player_look.y; player._33 = player_look.z;

	bone = Matrix4x4::Multiply(bone, player);

	XMFLOAT3 pos = XMFLOAT3(hand_pos.x, hand_pos.y, hand_pos.z);
	int weapon_type = m_pPlayer[i]->Get_Weapon_type();
	int weapon_index = m_pPlayer[i]->Get_Weapon_index();
	 
	m_WeaponShader[weapon_type]->getObject(weapon_index)->SetWorld(bone);
	m_WeaponShader[weapon_type]->getObject(weapon_index)->SetPosition(pos);
//	m_WeaponShader[weapon_type]->getObject(weapon_index)->Rotate(0, 0, 90);
	
	//if (weapon_type != M_Weapon_chocolate) m_WeaponShader[weapon_type]->getObject(weapon_index)->Rotate(-70, 0, 0);
	m_WeaponShader[weapon_type]->getObject(weapon_index)->Rotate(-70, 0, 0);

}

void CScene::CollisionProcess(int index)
{
	bounding_box_test[index]->bounding.Center = m_pPlayer[index]->GetPosition();
	for (int i = 0; i < WEAPON_MAX_NUM; i++) {
		for (int j = 0; j < WEAPON_EACH_NUM; j++) {
			if (i == 4 && j == 1)	//컵케익
				break;
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

void CScene::Collision_telleport(int index)
{
	bounding_box_test[index]->bounding.Center = m_pPlayer[index]->GetPosition();
	
	for (int i = 0; i < 8; i++) {
		door[i]->bounding.Center = door[i]->getObjects()->GetPosition();
		bool result = door[i]->bounding.Intersects(bounding_box_test[index]->bounding);
		if (result) {
			//XMFLOAT3 telpos;
			if (i == 0) { XMFLOAT3 telpos = door[1]->getObjects()->GetPosition(); telpos.z += 15.f;      m_pPlayer[index]->SetPosition(telpos);}		// LEFT_DOWN_OUT	
			else if (i == 1) { XMFLOAT3 telpos = door[0]->getObjects()->GetPosition(); telpos.z -= 15.f; m_pPlayer[index]->SetPosition(telpos);}		// LEFT_DOWN_IN	
			else if (i == 2) { XMFLOAT3 telpos = door[3]->getObjects()->GetPosition(); telpos.z += 15.f; m_pPlayer[index]->SetPosition(telpos);}		// LEFT_UP_OUT		
			else if (i == 3) { XMFLOAT3 telpos = door[2]->getObjects()->GetPosition(); telpos.z -= 15.f; m_pPlayer[index]->SetPosition(telpos);}		// LEFT_UP_IN		
																						  		
			else if (i == 4) { XMFLOAT3 telpos = door[5]->getObjects()->GetPosition(); telpos.z += 15.f; m_pPlayer[index]->SetPosition(telpos);}		// RIGHT_DOWN_OUT	
			else if (i == 5) { XMFLOAT3 telpos = door[4]->getObjects()->GetPosition(); telpos.z -= 15.f; m_pPlayer[index]->SetPosition(telpos);}		// RIGHT_DOWN_IN	
			else if (i == 6) { XMFLOAT3 telpos = door[7]->getObjects()->GetPosition(); telpos.z += 15.f; m_pPlayer[index]->SetPosition(telpos);}		// RIGHT_UP_OUT	
			else if (i == 7) { XMFLOAT3 telpos = door[6]->getObjects()->GetPosition(); telpos.z -= 15.f; m_pPlayer[index]->SetPosition(telpos);}		// RIGHT_UP_IN		
			//m_pPlayer[index]->SetPosition(telpos);
			//return;
		}
	}
}

void CScene::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	for (int i = 0; i < MAX_USER; ++i) if (m_pPlayer[i]->GetConnected()) m_pPlayerShader[i]->Render(pd3dCommandList, pCamera);
	for (int i = 0; i < MAX_USER; ++i) if (m_pPlayer[i]->GetConnected()) m_pPlayerShadowShader[i]->Render(pd3dCommandList, pCamera);
	
	if (Selected_Map == M_Map_1) {
		if (m_MapShader[0]) m_MapShader[0]->Render(pd3dCommandList, pCamera);
		if (m_MapShader[1]) m_MapShader[1]->Render(pd3dCommandList, pCamera);
		if (m_MapShader[2]) m_MapShader[2]->Render(pd3dCommandList, pCamera);
		if (m_ShadowShader[0]) m_ShadowShader[0]->Render(pd3dCommandList, pCamera);
	}
	if (Selected_Map == M_Map_2) {
		if (m_MapShader[0]) m_MapShader[0]->Render(pd3dCommandList, pCamera);
		if (m_MapShader[1]) m_MapShader[1]->Render(pd3dCommandList, pCamera);
		if (m_MapShader[2]) m_MapShader[2]->Render(pd3dCommandList, pCamera);
		if (m_MapShader[3]) m_MapShader[3]->Render(pd3dCommandList, pCamera);
		if (m_MapShader[4]) m_MapShader[4]->Render(pd3dCommandList, pCamera);
	}
	if (Selected_Map == M_Map_3) {
		for (int i = 0; i < 8; ++i)if (door[i]) door[i]->Render(pd3dCommandList, pCamera);
		
		if (m_MapShader[8]) m_MapShader[8]->Render(pd3dCommandList, pCamera);
		if (m_MapShader[9]) m_MapShader[9]->Render(pd3dCommandList, pCamera);
		if (m_MapShader[10]) m_MapShader[10]->Render(pd3dCommandList, pCamera);
		if (m_MapShader[11]) m_MapShader[11]->Render(pd3dCommandList, pCamera);
		if (m_MapShader[12]) m_MapShader[12]->Render(pd3dCommandList, pCamera);
		if (m_MapShader[13]) m_MapShader[13]->Render(pd3dCommandList, pCamera);

		if (m_MapShader[0]) m_MapShader[0]->Render(pd3dCommandList, pCamera);
		if (m_MapShader[1]) m_MapShader[1]->Render(pd3dCommandList, pCamera);
		if (m_MapShader[2]) m_MapShader[2]->Render(pd3dCommandList, pCamera);
		if (m_MapShader[3]) m_MapShader[3]->Render(pd3dCommandList, pCamera);
		if (m_MapShader[4]) m_MapShader[4]->Render(pd3dCommandList, pCamera);
		if (m_MapShader[5]) m_MapShader[5]->Render(pd3dCommandList, pCamera);
		if (m_MapShader[6]) m_MapShader[6]->Render(pd3dCommandList, pCamera);
		if (m_MapShader[7]) m_MapShader[7]->Render(pd3dCommandList, pCamera);

		m_BridgeShader->Render(pd3dCommandList, pCamera);
		for (int i = 0; i < 2; ++i) if (m_StairShader[i]) m_StairShader[i]->Render(pd3dCommandList, pCamera);
	}
	for (int i = 0; i < WEAPON_MAX_NUM; ++i) if (m_WeaponShader[i]) m_WeaponShader[i]->Render(pd3dCommandList, pCamera);

	if (m_WavesShader) m_WavesShader->Render(pd3dCommandList, pCamera);
	if (m_BackGroundShader[0]) m_BackGroundShader[0]->Render(pd3dCommandList, pCamera);
	if (Selected_Map == M_Map_1) {
		for (int i = 0; i < 2; ++i) if (m_CottonShader[i]) m_CottonShader[i]->Render(pd3dCommandList, pCamera);
	}
	if (Selected_Map == M_Map_3) {
		if (m_ShadowReverseShader) m_ShadowReverseShader->Render(pd3dCommandList, pCamera);

		if (m_ShadowShader[0]) m_ShadowShader[0]->Render(pd3dCommandList, pCamera);
		if (m_ShadowShader[1]) m_ShadowShader[1]->Render(pd3dCommandList, pCamera);
		if (m_ShadowShader[2]) m_ShadowShader[2]->Render(pd3dCommandList, pCamera);
		if (m_ShadowShader[3]) m_ShadowShader[3]->Render(pd3dCommandList, pCamera);

		if (m_BackGroundShader[1]) m_BackGroundShader[1]->Render(pd3dCommandList, pCamera);
	}
	for (int i = 0; i < WEAPON_MAX_NUM; i++) {
		for (int j = 0; j < WEAPON_EACH_NUM; j++) {
			if (weapon_box[i][j])
				weapon_box[i][j]->Render(pd3dCommandList, pCamera);
		}
	}
	for (int i = 0; i < PATTERN_LIGHTNING_NUM; i++)if (m_EffectShader[i]) m_EffectShader[i]->Render(pd3dCommandList, pCamera);

	for (int i = 0; i < MAX_USER;i++)if (bounding_box_test[i]) bounding_box_test[i]->Render(pd3dCommandList, pCamera);
}

void CScene::RenderUI(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList)
{
	//for (UINT i = 0; i < m_nUIShaders - 2; ++i)
	//	m_ppUIShaders[i]->Render(pCommandList);
	for (int i = m_nUIShaders-1; i >= 0; i--) {
		m_ppUIShaders[i]->Render(pCommandList);
	}
	if(m_MessageShader) m_MessageShader->Render(pCommandList);
}