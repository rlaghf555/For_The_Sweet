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

		m_ShadowReverseShader[0] = new ShadowREverseShader();
		m_ShadowReverseShader[0]->BuildObjects(pd3dDevice, pd3dCommandList, 0);
		m_ShadowReverseShader[1] = new ShadowREverseShader();
		m_ShadowReverseShader[1]->BuildObjects(pd3dDevice, pd3dCommandList, 1);
		m_ShadowReverseShader[2] = new ShadowREverseShader();
		m_ShadowReverseShader[2]->BuildObjects(pd3dDevice, pd3dCommandList, 2);
		m_ShadowReverseShader[3] = new ShadowREverseShader();
		m_ShadowReverseShader[3]->BuildObjects(pd3dDevice, pd3dCommandList, 3);

		m_RoundBackGroundShader[0] = new RoundBackGroundShader(Map_Model[M_Map_round_background_1]);
		m_RoundBackGroundShader[0]->BuildObjects(pd3dDevice, pd3dCommandList, 0, true);
		Map_1_Build = true;
	}
	//m_mapshader 2까지 map 1
	if (Selected_Map == M_Map_2) {
		m_MapShader[3] = new CModelShader(Map_Model[M_Map_2]);
		m_MapShader[3]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_2);

		m_MapShader[4] = new CModelShader(Map_Model[M_Map_1_macaron]);
		m_MapShader[4]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_1_macaron);
		m_MapShader[5] = new CModelShader(Map_Model[M_Map_1_macaron]);
		m_MapShader[5]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_1_macaron_2);
		
		m_MapShader[6] = new CModelShader(Map_Model[M_Map_2_chocolate_bar]);
		m_MapShader[6]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_2_chocolate_bar);
		m_MapShader[7] = new CModelShader(Map_Model[M_Map_2_chocolate_bar]);
		m_MapShader[7]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_2_chocolate_bar_2);
		
		for (int i = 0; i < WEAPON_MAX_NUM; i++) {
			m_WeaponShader[i] = new WeaponShader(weapon_Model[i]);
			m_WeaponShader[i]->BuildObjects(pd3dDevice, pd3dCommandList, i, M_Map_2);
		}

		m_ShadowReverseModelShader[0] = new ShadowReverseModelShader(Map_Model[M_Map_2_shadow_reverse_test]);
		m_ShadowReverseModelShader[0]->BuildObjects(pd3dDevice, pd3dCommandList, 0);
		m_ShadowReverseModelShader[1] = new ShadowReverseModelShader(Map_Model[M_Map_2_shadow_reverse_test_1]);
		m_ShadowReverseModelShader[1]->BuildObjects(pd3dDevice, pd3dCommandList, 1);
		m_ShadowReverseModelShader[2] = new ShadowReverseModelShader(Map_Model[M_Map_2_shadow_reverse_test_2]);
		m_ShadowReverseModelShader[2]->BuildObjects(pd3dDevice, pd3dCommandList, 2);

		m_RoundBackGroundShader[1] = new RoundBackGroundShader(Map_Model[M_Map_round_background_1]);
		m_RoundBackGroundShader[1]->BuildObjects(pd3dDevice, pd3dCommandList, 1, true);
		Map_2_Build = true;
	}
	//m_mapshader 3~8까지 map 2
	if (Selected_Map == M_Map_3) {
		for (int i = 0; i < 8; i++) {
			door[i] = new testBox();
			door[i]->BuildObjects(pd3dDevice, pd3dCommandList, m_pPlayer[i], 11 + i);
		}

		m_MapShader[8] = new CModelShader(Map_Model[M_Map_3]);
		m_MapShader[8]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3);
		m_MapShader[9] = new CModelShader(Map_Model[M_Map_3]);
		m_MapShader[9]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3_2);

		m_MapShader[10] = new CModelShader(Map_Model[M_Map_3_cake_2]);
		m_MapShader[10]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3_cake_2);
		m_MapShader[11] = new CModelShader(Map_Model[M_Map_3_cake_2]);
		m_MapShader[11]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3_cake_2_2);

		m_MapShader[12] = new CModelShader(Map_Model[M_Map_3_cake_3]);
		m_MapShader[12]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3_cake_3);
		m_MapShader[13] = new CModelShader(Map_Model[M_Map_3_cake_3]);
		m_MapShader[13]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3_cake_3_2);

		m_MapShader[14] = new CModelShader(Map_Model[M_Map_3_in_black]);
		m_MapShader[14]->BuildObjects(pd3dDevice, pd3dCommandList, M_Map_3);

		m_MapShader[15] = new CModelShader(Map_Model[M_Map_1_macaron]);
		m_MapShader[15]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_1_macaron_3);

		m_MapShader[16] = new CModelShader(Map_Model[M_Map_3_in]);
		m_MapShader[16]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3_in);
		m_MapShader[17] = new CModelShader(Map_Model[M_Map_3_in_stair_1]);
		m_MapShader[17]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3_in_stair_1);
		m_MapShader[18] = new CModelShader(Map_Model[M_Map_3_in_stair_2]);
		m_MapShader[18]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3_in_stair_2);

		m_MapShader[19] = new CModelShader(Map_Model[M_Map_3_in]);
		m_MapShader[19]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3_in_2);
		m_MapShader[20] = new CModelShader(Map_Model[M_Map_3_in_stair_1]);
		m_MapShader[20]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3_in_stair_1_2);
		m_MapShader[21] = new CModelShader(Map_Model[M_Map_3_in_stair_2]);
		m_MapShader[21]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3_in_stair_2_2);
		m_MapShader[22] = new CModelShader(Map_Model[M_Map_3_in_wall_collision]);
		m_MapShader[22]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3_in_wall_collision);
		m_MapShader[23] = new CModelShader(Map_Model[M_Map_3_in_wall_collision]);
		m_MapShader[23]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3_in_wall_collision_2);

		m_BridgeShader = new BridgeShader(Map_Model[M_Map_3_bridge]);
		m_BridgeShader->BuildObjects(pd3dDevice, pd3dCommandList, physx);

		m_ShadowShader[1] = new ShadowDebugShader(Map_Model[M_Map_3_cake_2]);
		m_ShadowShader[1]->BuildObjects(pd3dDevice, pd3dCommandList, M_Map_3_cake_2);
		m_ShadowShader[2] = new ShadowDebugShader(Map_Model[M_Map_3_cake_2]);
		m_ShadowShader[2]->BuildObjects(pd3dDevice, pd3dCommandList, M_Map_3_cake_2_2);

		m_ShadowShader[3] = new ShadowDebugShader(Map_Model[M_Map_3_cake_3]);
		m_ShadowShader[3]->BuildObjects(pd3dDevice, pd3dCommandList, M_Map_3_cake_3);
		m_ShadowShader[4] = new ShadowDebugShader(Map_Model[M_Map_3_cake_3]);
		m_ShadowShader[4]->BuildObjects(pd3dDevice, pd3dCommandList, M_Map_3_cake_3_2);

		m_ShadowReverseShader[4] = new ShadowREverseShader();
		m_ShadowReverseShader[4]->BuildObjects(pd3dDevice, pd3dCommandList, 4);
		m_ShadowReverseShader[5] = new ShadowREverseShader();
		m_ShadowReverseShader[5]->BuildObjects(pd3dDevice, pd3dCommandList, 5);
		m_ShadowReverseShader[6] = new ShadowREverseShader();
		m_ShadowReverseShader[6]->BuildObjects(pd3dDevice, pd3dCommandList, 6);
		m_ShadowReverseShader[7] = new ShadowREverseShader();
		m_ShadowReverseShader[7]->BuildObjects(pd3dDevice, pd3dCommandList, 7);

		for (int i = 0; i < 2; i++) {
			m_StairShader[i] = new StairShader(Map_Model[M_Map_3_stair]);
			m_StairShader[i]->BuildObjects(pd3dDevice, pd3dCommandList, physx, i);
		}

		for (int i = 0; i < WEAPON_MAX_NUM; i++) {
			m_WeaponShader[i] = new WeaponShader(weapon_Model[i]);
			m_WeaponShader[i]->BuildObjects(pd3dDevice, pd3dCommandList, i, M_Map_2);
		}

		m_RoundBackGroundShader[2] = new RoundBackGroundShader(Map_Model[M_Map_round_background_2]);
		m_RoundBackGroundShader[2]->BuildObjects(pd3dDevice, pd3dCommandList, 2, true);
		Map_3_Build = true;
	}
	//m_mapshader 9~22까지 map3
	Cotton_box[0].Center = XMFLOAT3(0, 10, -120);
	Cotton_box[0].Extents = XMFLOAT3(45, 30, 45);
	Cotton_box[1].Center = XMFLOAT3(0, 10, 120);
	Cotton_box[1].Extents = XMFLOAT3(45, 30, 45);

	m_WavesShader = new WaveShader();
	m_WavesShader->BuildObjects(pd3dDevice, pd3dCommandList, Map_SELECT);

	m_BackGroundShader[0] = new MeshShader();
	m_BackGroundShader[0]->BuildObjects(pd3dDevice, pd3dCommandList, 0);

	if (Selected_Map == M_Map_3) {
		m_BackGroundShader[1] = new MeshShader();
		m_BackGroundShader[1]->BuildObjects(pd3dDevice, pd3dCommandList, 1);
	}
	//m_pPlayer[0]->SetWeapon(true, 0);
	m_BackGroundShader[2] = new MeshShader();
	m_BackGroundShader[2]->BuildObjects(pd3dDevice, pd3dCommandList, 2);
	
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

	for (int i = 0; i < WEAPON_MAX_NUM; i++) {
		for (int j = 0; j < WEAPON_EACH_NUM; j++) {
			m_ExplosionShader[i][j] = new ExplosionModelShader(effect_Model[M_Effect_Explosion_Star]);
			m_ExplosionShader[i][j]->BuildObjects(pd3dDevice, pd3dCommandList, i);
			if (i == 4) {
				j = WEAPON_EACH_NUM;
			}
		}
	}

	m_EffectShader = new EffectShader();
	m_EffectShader->BuildObjects(pd3dDevice, pd3dCommandList, Selected_Map);
	//for (int i = 0; i < 10; i++)m_EffectShader->getObject(i)->visible = true;

	for (int i = 0; i < MAX_USER; i++) {
		m_SkillEffectShader[i] = new SkillEffectShader();
		m_SkillEffectShader[i]->BuildObjects(pd3dDevice, pd3dCommandList);
	}
	for(int j=0;j<MAX_USER;j++)
	for (int i = 0; i < 3; i++) {
		m_SkillParticleShader[j][i] = new SkillParticleShader();
		m_SkillParticleShader[j][i]->BuildObjects(pd3dDevice, pd3dCommandList,i);
	}
	m_pPlayer[0]->selected_skill = SKILL_ATTACK;		//각 플레이어 별로 방에서 고른 스킬이 뭔지 넣어주면 스킬키 눌럿을때 해당 파티클 출력 default SKILL_ATTACK

	for (int i = 0; i < MAX_USER; i++) {
		m_StunShader[i] = new StunShader(effect_Model[M_Effect_Head_Star]);
		m_StunShader[i]->BuildObjects(pd3dDevice, pd3dCommandList);
	}

	m_MagicShader = new MagicShader();
	m_MagicShader->BuildObjects(pd3dDevice, pd3dCommandList);
			
	m_TeamShader = new TeamShader();
	m_TeamShader->BuildObjects(pd3dDevice, pd3dCommandList, 0);
	m_EnemyShader = new TeamShader();
	m_EnemyShader->BuildObjects(pd3dDevice, pd3dCommandList, 1);
}

void CScene::ReBuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, CPhysx * physx)
{
	animate_flag = Selected_Map;
	if (Selected_Map == M_Map_1 && Map_1_Build == false) {
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

		//for (int i = 0; i < WEAPON_MAX_NUM; i++) {
		//	m_WeaponShader[i] = new WeaponShader(weapon_Model[i]);
		//	m_WeaponShader[i]->BuildObjects(pd3dDevice, pd3dCommandList, i, M_Map_1);
		//}

		m_ShadowShader[0] = new ShadowDebugShader(Map_Model[M_Map_1_Shadow_test]);
		m_ShadowShader[0]->BuildObjects(pd3dDevice, pd3dCommandList, M_Map_1);

		m_ShadowReverseShader[0] = new ShadowREverseShader();
		m_ShadowReverseShader[0]->BuildObjects(pd3dDevice, pd3dCommandList, 0);
		m_ShadowReverseShader[1] = new ShadowREverseShader();
		m_ShadowReverseShader[1]->BuildObjects(pd3dDevice, pd3dCommandList, 1);
		m_ShadowReverseShader[2] = new ShadowREverseShader();
		m_ShadowReverseShader[2]->BuildObjects(pd3dDevice, pd3dCommandList, 2);
		m_ShadowReverseShader[3] = new ShadowREverseShader();
		m_ShadowReverseShader[3]->BuildObjects(pd3dDevice, pd3dCommandList, 3);

		m_RoundBackGroundShader[0] = new RoundBackGroundShader(Map_Model[M_Map_round_background_1]);
		m_RoundBackGroundShader[0]->BuildObjects(pd3dDevice, pd3dCommandList, 0, true);
		Map_1_Build = true;
	}
	//m_mapshader 2까지 map 1
	if (Selected_Map == M_Map_2 && Map_2_Build == false) {
		m_MapShader[3] = new CModelShader(Map_Model[M_Map_2]);
		m_MapShader[3]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_2);

		m_MapShader[4] = new CModelShader(Map_Model[M_Map_1_macaron]);
		m_MapShader[4]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_1_macaron);
		m_MapShader[5] = new CModelShader(Map_Model[M_Map_1_macaron]);
		m_MapShader[5]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_1_macaron_2);

		m_MapShader[6] = new CModelShader(Map_Model[M_Map_2_chocolate_bar]);
		m_MapShader[6]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_2_chocolate_bar);
		m_MapShader[7] = new CModelShader(Map_Model[M_Map_2_chocolate_bar]);
		m_MapShader[7]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_2_chocolate_bar_2);

		//for (int i = 0; i < WEAPON_MAX_NUM; i++) {
		//	m_WeaponShader[i] = new WeaponShader(weapon_Model[i]);
		//	m_WeaponShader[i]->BuildObjects(pd3dDevice, pd3dCommandList, i, M_Map_2);
		//}

		m_ShadowReverseModelShader[0] = new ShadowReverseModelShader(Map_Model[M_Map_2_shadow_reverse_test]);
		m_ShadowReverseModelShader[0]->BuildObjects(pd3dDevice, pd3dCommandList, 0);
		m_ShadowReverseModelShader[1] = new ShadowReverseModelShader(Map_Model[M_Map_2_shadow_reverse_test_1]);
		m_ShadowReverseModelShader[1]->BuildObjects(pd3dDevice, pd3dCommandList, 1);
		m_ShadowReverseModelShader[2] = new ShadowReverseModelShader(Map_Model[M_Map_2_shadow_reverse_test_2]);
		m_ShadowReverseModelShader[2]->BuildObjects(pd3dDevice, pd3dCommandList, 2);

		m_RoundBackGroundShader[1] = new RoundBackGroundShader(Map_Model[M_Map_round_background_1]);
		m_RoundBackGroundShader[1]->BuildObjects(pd3dDevice, pd3dCommandList, 1, true);
		Map_2_Build = true;
	}
	//m_mapshader 3~8까지 map 2
	if (Selected_Map == M_Map_3 && Map_3_Build == false) {
		for (int i = 0; i < 8; i++) {
			door[i] = new testBox();
			door[i]->BuildObjects(pd3dDevice, pd3dCommandList, m_pPlayer[i], 11 + i);
		}

		m_MapShader[8] = new CModelShader(Map_Model[M_Map_3]);
		m_MapShader[8]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3);
		m_MapShader[9] = new CModelShader(Map_Model[M_Map_3]);
		m_MapShader[9]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3_2);

		m_MapShader[10] = new CModelShader(Map_Model[M_Map_3_cake_2]);
		m_MapShader[10]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3_cake_2);
		m_MapShader[11] = new CModelShader(Map_Model[M_Map_3_cake_2]);
		m_MapShader[11]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3_cake_2_2);

		m_MapShader[12] = new CModelShader(Map_Model[M_Map_3_cake_3]);
		m_MapShader[12]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3_cake_3);
		m_MapShader[13] = new CModelShader(Map_Model[M_Map_3_cake_3]);
		m_MapShader[13]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3_cake_3_2);

		m_MapShader[14] = new CModelShader(Map_Model[M_Map_3_in_black]);
		m_MapShader[14]->BuildObjects(pd3dDevice, pd3dCommandList, M_Map_3);

		m_MapShader[15] = new CModelShader(Map_Model[M_Map_1_macaron]);
		m_MapShader[15]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_1_macaron_3);

		m_MapShader[16] = new CModelShader(Map_Model[M_Map_3_in]);
		m_MapShader[16]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3_in);
		m_MapShader[17] = new CModelShader(Map_Model[M_Map_3_in_stair_1]);
		m_MapShader[17]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3_in_stair_1);
		m_MapShader[18] = new CModelShader(Map_Model[M_Map_3_in_stair_2]);
		m_MapShader[18]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3_in_stair_2);

		m_MapShader[19] = new CModelShader(Map_Model[M_Map_3_in]);
		m_MapShader[19]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3_in_2);
		m_MapShader[20] = new CModelShader(Map_Model[M_Map_3_in_stair_1]);
		m_MapShader[20]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3_in_stair_1_2);
		m_MapShader[21] = new CModelShader(Map_Model[M_Map_3_in_stair_2]);
		m_MapShader[21]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3_in_stair_2_2);
		m_MapShader[22] = new CModelShader(Map_Model[M_Map_3_in_wall_collision]);
		m_MapShader[22]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3_in_wall_collision);
		m_MapShader[23] = new CModelShader(Map_Model[M_Map_3_in_wall_collision]);
		m_MapShader[23]->BuildObjects(pd3dDevice, pd3dCommandList, physx, M_Map_3_in_wall_collision_2);

		m_BridgeShader = new BridgeShader(Map_Model[M_Map_3_bridge]);
		m_BridgeShader->BuildObjects(pd3dDevice, pd3dCommandList, physx);

		m_ShadowShader[1] = new ShadowDebugShader(Map_Model[M_Map_3_cake_2]);
		m_ShadowShader[1]->BuildObjects(pd3dDevice, pd3dCommandList, M_Map_3_cake_2);
		m_ShadowShader[2] = new ShadowDebugShader(Map_Model[M_Map_3_cake_2]);
		m_ShadowShader[2]->BuildObjects(pd3dDevice, pd3dCommandList, M_Map_3_cake_2_2);

		m_ShadowShader[3] = new ShadowDebugShader(Map_Model[M_Map_3_cake_3]);
		m_ShadowShader[3]->BuildObjects(pd3dDevice, pd3dCommandList, M_Map_3_cake_3);
		m_ShadowShader[4] = new ShadowDebugShader(Map_Model[M_Map_3_cake_3]);
		m_ShadowShader[4]->BuildObjects(pd3dDevice, pd3dCommandList, M_Map_3_cake_3_2);

		m_ShadowReverseShader[4] = new ShadowREverseShader();
		m_ShadowReverseShader[4]->BuildObjects(pd3dDevice, pd3dCommandList, 4);
		m_ShadowReverseShader[5] = new ShadowREverseShader();
		m_ShadowReverseShader[5]->BuildObjects(pd3dDevice, pd3dCommandList, 5);
		m_ShadowReverseShader[6] = new ShadowREverseShader();
		m_ShadowReverseShader[6]->BuildObjects(pd3dDevice, pd3dCommandList, 6);
		m_ShadowReverseShader[7] = new ShadowREverseShader();
		m_ShadowReverseShader[7]->BuildObjects(pd3dDevice, pd3dCommandList, 7);

		for (int i = 0; i < 2; i++) {
			m_StairShader[i] = new StairShader(Map_Model[M_Map_3_stair]);
			m_StairShader[i]->BuildObjects(pd3dDevice, pd3dCommandList, physx, i);
		}

		//for (int i = 0; i < WEAPON_MAX_NUM; i++) {
		//	m_WeaponShader[i] = new WeaponShader(weapon_Model[i]);
		//	m_WeaponShader[i]->BuildObjects(pd3dDevice, pd3dCommandList, i, M_Map_2);
		//}
		m_BackGroundShader[1] = new MeshShader();					// 맵 3 내부 감싸는 검은바탕
		m_BackGroundShader[1]->BuildObjects(pd3dDevice, pd3dCommandList, 1);

		m_RoundBackGroundShader[2] = new RoundBackGroundShader(Map_Model[M_Map_round_background_2]);
		m_RoundBackGroundShader[2]->BuildObjects(pd3dDevice, pd3dCommandList, 2, true);
		Map_3_Build = true;
	}
}

void CScene::BuildUI(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList)
{
	m_ppUIShaders.clear();
	m_nUIShaders = 23;
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
	
	UITimeShader* pTime = new UITimeShader();
	pTime->BuildObjects(pDevice, pCommandList, 1);
	m_ppUIShaders[8] = pTime;
	

	UIDotShader* pDot = new UIDotShader();
	pDot->BuildObjects(pDevice, pCommandList);
	m_ppUIShaders[9] = pDot;

	UIReadyShader* pReady = new UIReadyShader();
	pReady->BuildObjects(pDevice, pCommandList);
	
	m_ppUIShaders[10] = pReady;

	UIFightShader* pFight = new UIFightShader();
	
	pFight->BuildObjects(pDevice, pCommandList);
	XMFLOAT2 pos;
	m_ppUIShaders[11] = pFight;
	for (int i = 0; i < 4; i++) {
		UIIDShader* pID = new UIIDShader();
		pID->BuildObjects(pDevice, pCommandList);
		for (int j = 0; j < 10; j++) {
			pos = XMFLOAT2(50 + j * 10 + i * 313, 200);
			pID->SetPos(&pos, j);
		}	
		m_ppUIShaders[12+i] = pID;
	}
	for (int i = 0; i < 4; i++) {
		UIIDShader* pID = new UIIDShader();
		pID->BuildObjects(pDevice, pCommandList);
		for (int j = 0; j < 10; j++) {
			pos = XMFLOAT2(50 + j * 10 + i * 313, 90);
			pID->SetPos(&pos, j);
		}	
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
	//m_ppUIShaders[21]->SetFog();

	UILoadingShader *load = new UILoadingShader();
	load->BuildObjects(pDevice, pCommandList);
	m_ppUIShaders[22] = load;

	m_MessageShader = new MessageShader();
	m_MessageShader->BuildObjects(pDevice, pCommandList);
	
	m_DarkShader = new DarkShader();
	m_DarkShader->BuildObjects(pDevice, pCommandList);
	//m_MessageShader->ShowMessage(MESSAGE_LIGHTING);

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
	for (int i = 0; i < 24; i++) {
		if (m_MapShader[i]) {
			m_MapShader[i]->ReleaseShaderVariables();
			m_MapShader[i]->ReleaseObjects();
			delete m_MapShader[i];
		}
	}
	for (int i = 0; i < 3; i++){
		if (m_RoundBackGroundShader[i]) {
			m_RoundBackGroundShader[i]->ReleaseShaderVariables();
			m_RoundBackGroundShader[i]->ReleaseObjects();
			delete m_RoundBackGroundShader[i];
		}
	}
	for (int i = 0; i < 5; ++i) {
		if (m_ShadowShader[i]) {
			m_ShadowShader[i]->ReleaseShaderVariables();
			m_ShadowShader[i]->ReleaseObjects();
			delete m_ShadowShader[i];
		}
	}
	for (int i = 0; i < 8; ++i) {
		if (m_ShadowReverseShader[i]) {
			m_ShadowReverseShader[i]->ReleaseShaderVariables();
			m_ShadowReverseShader[i]->ReleaseObjects();
			delete m_ShadowReverseShader[i];
		}
	}
	for (int i = 0; i < 3; ++i) {
		if (m_ShadowReverseModelShader[i]) {
			m_ShadowReverseModelShader[i]->ReleaseShaderVariables();
			m_ShadowReverseModelShader[i]->ReleaseObjects();
			delete m_ShadowReverseModelShader[i];
		}
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
	for (int i = 0; i < 3; ++i) {
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
	for (int i = 0; i < WEAPON_MAX_NUM; ++i) {
		for (int j = 0; j < WEAPON_EACH_NUM; ++j) {
			if (m_ExplosionShader[i][j]) {
				m_ExplosionShader[i][j]->ReleaseShaderVariables();
				m_ExplosionShader[i][j]->ReleaseObjects();
				delete m_ExplosionShader[i][j];
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
	if (m_MagicShader) {
		m_MagicShader->ReleaseShaderVariables();
		m_MagicShader->ReleaseObjects();
		delete m_MagicShader;
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
	if (m_EffectShader) {
		m_EffectShader->ReleaseShaderVariables();
		m_EffectShader->ReleaseObjects();
		delete m_EffectShader;
	}
	for (int i = 0; i < MAX_USER; ++i) {
		if (m_SkillEffectShader[i]) {
			m_SkillEffectShader[i]->ReleaseShaderVariables();
			m_SkillEffectShader[i]->ReleaseObjects();
			delete m_SkillEffectShader[i];
		}
	}
	for(int j=0;j<MAX_USER;++j)
	for (int i = 0; i < 3; ++i) {
		if (m_SkillParticleShader[j][i]) {
			m_SkillParticleShader[j][i]->ReleaseShaderVariables();
			m_SkillParticleShader[j][i]->ReleaseObjects();
			delete m_SkillParticleShader[j][i];
		}
	}
	for (int i = 0; i < MAX_USER; ++i) {
		if (m_StunShader[i]) {
			m_StunShader[i]->ReleaseShaderVariables();
			m_StunShader[i]->ReleaseObjects();
			delete m_StunShader[i];
		}
	}
	if (m_TeamShader) {
		m_TeamShader->ReleaseShaderVariables();
		m_TeamShader->ReleaseObjects();
		delete m_TeamShader;
	}
	if (m_EnemyShader) {
		m_EnemyShader->ReleaseShaderVariables();
		m_EnemyShader->ReleaseObjects();
		delete m_EnemyShader;
	}
	if (m_DarkShader) {
		m_DarkShader->ReleaseShaderVariables();
		m_DarkShader->ReleaseObjects();
		delete m_DarkShader;
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

void CScene::initUI(wchar_t *character_id[])
{
	for (int i = 0; i < 8; i++) {
		m_ppUIShaders[i]->getObejct(1)->SetHP(m_pPlayer[i]->Get_HP());	//hp
		m_ppUIShaders[i]->getObejct(2)->SetHP(m_pPlayer[i]->Get_MP());	//mp
	}
	for (int i = 0; i < 4; i++) {
		XMFLOAT2 pos = XMFLOAT2(172 + i * 313, 150);
		m_ppUIShaders[i]->SetPos(&pos, 0);
		pos = XMFLOAT2(172 + i * 313, 156);
		m_ppUIShaders[i]->SetPos(&pos, 1);
		pos = XMFLOAT2(172 + i * 313, 136);
		m_ppUIShaders[i]->SetPos(&pos, 2);
	}
	for (int i = 0; i < 4; i++) {
		XMFLOAT2 pos = XMFLOAT2(172 + i * 313, 40);
		m_ppUIShaders[i + 4]->SetPos(&pos, 0);
		pos = XMFLOAT2(172 + i * 313, 46);
		m_ppUIShaders[i + 4]->SetPos(&pos, 1);
		pos = XMFLOAT2(172 + i * 313, 26);
		m_ppUIShaders[i + 4]->SetPos(&pos, 2);
	}

	//time
	m_ppUIShaders[8]->SetTime(150);
	//m_ppUIShaders[9]   --- :
	//Ready
	XMFLOAT2 pos = XMFLOAT2(1300, 400);
	m_ppUIShaders[10]->SetPos(&pos, 0);
	//Fight
	pos = XMFLOAT2(1280, 400);
	m_ppUIShaders[11]->SetPos(&pos, 0);

	//id 초기화
	for (int i = 12; i < 20; i++) {
		//wchar_t tmp[10] = L"ABCD4";			//플레이어 아이디 입력하자			
		m_ppUIShaders[i]->SetID(character_id[i - 12]);
		if (character_id[i - 12] == nullptr) {
			m_ppUIShaders[i - 12]->SetEnable(false,0);
			m_ppUIShaders[i - 12]->SetEnable(false,1);
			m_ppUIShaders[i - 12]->SetEnable(false,2);
		}
	}
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 10; j++) {
			pos = XMFLOAT2(50 + j * 10 + i * 313, 200);
			m_ppUIShaders[12 + i]->SetPos(&pos, j);
		}
	}
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 10; j++) {
			pos = XMFLOAT2(50 + j * 10 + i * 313, 90);
			m_ppUIShaders[12 + i + 4]->SetPos(&pos, j);
		}
	}
	m_DarkShader->getObejct(0)->SetAlpha(0.f);
	m_DarkShader->ftime = 0.f;
	m_DarkShader->is_dark = false;
//	m_ppUIShaders[20]->ShowMessage(false);   //true면 win false면 lose

	//-------------------------------안개생성하기
	//m_ppUIShaders[21]->SetFog();
	//for (int i = 0; i < 8; i++) {
	//	m_ppUIShaders[i]->FogOn(true);
	//}
	//for (int i = 12; i < 20; i++) {
	//	m_ppUIShaders[i]->FogOn(true);
	//}
	//--------------------------------여기까지

	//m_MessageShader->ShowMessage(MESSAGE_LIGHTING);

}

void CScene::initObject()
{
	//무기 매트릭스 초기화
	for (int i = 0; i < WEAPON_MAX_NUM; i++) {
		for (int j = 0; j < WEAPON_EACH_NUM; j++) {
			//m_WeaponShader[i]->getObject(j)->init();
			m_WeaponShader[i]->getObject(j)->visible = true;

			if (SERVER_ON) {
				m_WeaponShader[i]->getObject(j)->init();
				m_WeaponShader[i]->getObject(j)->visible = false;
				m_WeaponShader[i]->getObject(j)->SetPosition(1000.f, 1000.f, 1000.f);

			}
			if (i == 4) {
				j = WEAPON_EACH_NUM;
			}
		}
	}

	if (Selected_Map == M_Map_1) {
		//캐릭터 좌표 및 스킬 초기화
		for (int i = 0; i < MAX_USER; i++) {
			if (m_pPlayer[i]->GetConnected()) {
				m_pPlayerShader[i]->render = true;
				m_pPlayerShadowShader[i]->render = true;
			}
			//else {
			//	m_pPlayerShader[i]->render = false;
			//	m_pPlayerShadowShader[i]->render = false;
			//}
			XMFLOAT3 pos(0, 0, 0);
			m_pPlayer[i]->init();
			m_pPlayerShadowShader[i]->init();
			m_pPlayer[i]->SetPosition(pos);
			//m_pPlayer[i]->selected_skill = ;
		}
		//번개 위치
		for (int i = 0; i < 10; i++) {
			m_EffectShader->getObject(i)->visible = false;
			//m_EffectShader->setPosition();
		}
		m_EffectShader->getObject(0)->SetPosition(-240.f, 100.f, 110.f);
		m_EffectShader->getObject(1)->SetPosition(-120.f, 100.f, 110.f);
		m_EffectShader->getObject(2)->SetPosition(60.f, 100.f, 110.f);
		m_EffectShader->getObject(3)->SetPosition(180.f, 100.f, 110.f);
		m_EffectShader->getObject(4)->SetPosition(-150.f, 100.f, 0.f);
		m_EffectShader->getObject(5)->SetPosition(90.f, 100.f, 0.f);
		m_EffectShader->getObject(6)->SetPosition(-240.f, 100.f, -110.f);
		m_EffectShader->getObject(7)->SetPosition(-120.f, 100.f, -110.f);
		m_EffectShader->getObject(8)->SetPosition(60.f, 100.f, -110.f);
		m_EffectShader->getObject(9)->SetPosition(180.f, 100.f, -110.f);
		//무기 좌표
		for (int i = 0; i < WEAPON_MAX_NUM; i++) {
			for (int j = 0; j < WEAPON_EACH_NUM; j++) {
				//m_WeaponShader[i]->getObject(j)->SetPosition();
				if (i == 4) {
					j = WEAPON_EACH_NUM;
				}
			}
		}
	}
	else if (Selected_Map == M_Map_2) {
		//캐릭터 좌표 및 스킬 초기화
		for (int i = 0; i < MAX_USER; i++) {
			if (m_pPlayer[i]->GetConnected()) {
				m_pPlayerShader[i]->render = true;
				m_pPlayerShadowShader[i]->render = true;
			}
			//else {
			//	m_pPlayerShader[i]->render = false;
			//	m_pPlayerShadowShader[i]->render = false;
			//}
			XMFLOAT3 pos(0, 0, 0);
			m_pPlayer[i]->init();
			m_pPlayerShadowShader[i]->init();
			m_pPlayer[i]->SetPosition(pos);

			//m_pPlayer[i]->selected_skill = ;
		}
		//번개 위치
		for (int i = 0; i < 10; i++) {
			m_EffectShader->getObject(i)->visible = false;
			//m_EffectShader->setPosition();
		}
		m_EffectShader->getObject(0)->SetPosition(-240.f, 100.f, 110.f);
		m_EffectShader->getObject(1)->SetPosition(-120.f, 100.f, 110.f);
		m_EffectShader->getObject(2)->SetPosition(60.f, 100.f, 110.f);
		m_EffectShader->getObject(3)->SetPosition(180.f, 100.f, 110.f);
		m_EffectShader->getObject(4)->SetPosition(-150.f, 100.f, 0.f);
		m_EffectShader->getObject(5)->SetPosition(90.f, 100.f, 0.f);
		m_EffectShader->getObject(6)->SetPosition(-240.f, 100.f, -110.f);
		m_EffectShader->getObject(7)->SetPosition(-120.f, 100.f, -110.f);
		m_EffectShader->getObject(8)->SetPosition(60.f, 100.f, -110.f);
		m_EffectShader->getObject(9)->SetPosition(180.f, 100.f, -110.f);

		//무기 좌표
		for (int i = 0; i < WEAPON_MAX_NUM; i++) {
			for (int j = 0; j < WEAPON_EACH_NUM; j++) {
				//m_WeaponShader[i]->getObject(j)->SetPosition();
				if (i == 4) {
					j = WEAPON_EACH_NUM;
				}
			}
		}
	}
	else if (Selected_Map == M_Map_3) {
		//캐릭터 좌표 및 스킬 초기화
		for (int i = 0; i < MAX_USER; i++) {
			if (m_pPlayer[i]->GetConnected()) {
				m_pPlayerShader[i]->render = true;
				m_pPlayerShadowShader[i]->render = true;
			}
			//else {
			//	m_pPlayerShader[i]->render = false;
			//	m_pPlayerShadowShader[i]->render = false;
			//}
			XMFLOAT3 pos(0, 0, 0);
			m_pPlayer[i]->init();
			m_pPlayerShadowShader[i]->init();
			m_pPlayer[i]->SetPosition(pos);
			//m_pPlayer[i]->selected_skill = ;
		}

		//번개 위치
		for (int i = 0; i < 10; i++) {
			m_EffectShader->getObject(i)->visible = false;
			//m_EffectShader->setPosition();
		}
		m_EffectShader->getObject(0)->SetPosition(-240.f, 100.f, 110.f);
		m_EffectShader->getObject(1)->SetPosition(-120.f, 100.f, 110.f);
		m_EffectShader->getObject(2)->SetPosition(60.f, 100.f, 110.f);
		m_EffectShader->getObject(3)->SetPosition(180.f, 100.f, 110.f);
		m_EffectShader->getObject(4)->SetPosition(-150.f, 100.f, 0.f);
		m_EffectShader->getObject(5)->SetPosition(90.f, 100.f, 0.f);
		m_EffectShader->getObject(6)->SetPosition(-240.f, 100.f, -110.f);
		m_EffectShader->getObject(7)->SetPosition(-120.f, 100.f, -110.f);
		m_EffectShader->getObject(8)->SetPosition(60.f, 100.f, -110.f);
		m_EffectShader->getObject(9)->SetPosition(180.f, 100.f, -110.f);

		//무기 좌표
		for (int i = 0; i < WEAPON_MAX_NUM; i++) {
			for (int j = 0; j < WEAPON_EACH_NUM; j++) {
				//m_WeaponShader[i]->getObject(j)->SetPosition();
				if (i == 4) {
					j = WEAPON_EACH_NUM;
				}
			}
		}
	}
	SetTeamUI();
}

void CScene::SetTeamUI()
{
	for (int i = 0; i < MAX_USER; i++) {
		m_TeamShader->getObject(i)->visible = false;
		m_EnemyShader->getObject(i)->visible = false;
		m_TeamShader->getObject(i)->SetPosition(0, -5000, 0);
		m_EnemyShader->getObject(i)->SetPosition(0, -5000, 0);
	}
	if (mode == MODE_TEAM || mode == MODE_KING) {
		if (myid < 4) {
			for (int i = 0; i < 4; i++)
				m_TeamShader->getObject(i)->visible = true;

			for (int i = 4; i < MAX_USER; i++)
				m_EnemyShader->getObject(i)->visible = true;
		}
	}
	else {
		for (int i = 0; i < MAX_USER; i++) {
			if (myid != i)
				m_EnemyShader->getObject(i)->visible = true;
			else m_TeamShader->getObject(i)->visible = true;
		}
	}

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
				int animindex = m_pPlayer[i]->getAnimIndex();
				m_pPlayerShader[i]->Animate(fTimeElapsed);
				if (animindex != m_pPlayerShadowShader[i]->getAnimIndex()) {
					m_pPlayerShadowShader[i]->ChangeAnimation(animindex);
				}
				XMFLOAT3 Look = m_pPlayer[i]->GetLook();
				XMFLOAT3 Up = m_pPlayer[i]->GetUp();
				XMFLOAT3 Right = m_pPlayer[i]->GetRight();
				m_pPlayerShadowShader[i]->getPlayer()->SetWorld(Look, Up, Right);

				if (animindex == Anim_Jump) m_pPlayerShadowShader[i]->Animate(fTimeElapsed, m_pPlayer[i]->GetPosition(), m_pPlayer[i]->GetJumpPos());
				else m_pPlayerShadowShader[i]->Animate(fTimeElapsed, m_pPlayer[i]->GetPosition(), m_pPlayer[i]->GetPosition().y);
				
				if (animindex == Anim_Stun) {	//Anim_Jump
					m_StunShader[i]->visible = true;
					m_StunShader[i]->Animate(fTimeElapsed, m_pPlayer[i]->GetPosition()); //Anim_Stun
				}
				else m_StunShader[i]->visible = false;

				if (animindex == Anim_PowerUp && m_pPlayer[i]->getAnimtime() <= 1) {
					m_SkillEffectShader[i]->getObject(0)->visible = true;
					m_SkillEffectShader[i]->Animate(fTimeElapsed, m_pPlayer[i]->GetPosition());

					m_SkillParticleShader[i][m_pPlayer[i]->selected_skill]->ShowParticle(true, m_pPlayer[i]->GetPosition());
				}

				if (m_pPlayer[i]->GetScaleflag()) {	// 서버에서 받아오는 컵케이크 플래크
					if (animindex == Anim_Cupckae_Eat) {
						if (m_pPlayer[i]->getAnimtime() >= 0 && m_pPlayer[i]->getAnimtime() < 20) m_pPlayer[i]->SetScale(1.0f, true);
						if (m_pPlayer[i]->getAnimtime() >= 20 && m_pPlayer[i]->getAnimtime() < 30) m_pPlayer[i]->SetScale(1.3f, true);
						if (m_pPlayer[i]->getAnimtime() >= 30 && m_pPlayer[i]->getAnimtime() < 38) m_pPlayer[i]->SetScale(1.7f, true);
						if (m_pPlayer[i]->getAnimtime() >= 38 && m_pPlayer[i]->getAnimtime() < 40) m_pPlayer[i]->SetScale(2.0f, true);
					}
					else {
						m_pPlayer[i]->SetScale(2.0f, true);

						if (m_pPlayer[i]->Scale_time < 10) m_pPlayer[i]->Scale_time += fTimeElapsed;
						else {
							m_pPlayer[i]->SetScaleflag(false);
							m_pPlayer[i]->Scale_time = 0.f;
						}
					}
				}
				else m_pPlayer[i]->SetScale(1.f, true);

				if (m_pPlayer[i]->Get_Weapon_grab()) {
					AnimateWeapon(i);
				}
				XMFLOAT3 tmp = m_pPlayer[i]->GetPosition();

				bounding_box_test[i]->getObjects()->m_xmf4x4World = m_pPlayer[i]->m_xmf4x4World;
				tmp.y += 40;
				tmp.x -= 2.5;
				if (m_ppUIShaders[21]->Get_Fog_Flag()) {
					tmp.y -= 10000;
				}
				m_TeamShader->getObject(i)->SetPosition(tmp);
				m_EnemyShader->getObject(i)->SetPosition(tmp);
			}
		}
	}
	for (int i = 0; i < MAX_USER; i++) {
		for(int j=0;j<3;j++)
		m_SkillParticleShader[i][j]->Animate(fTimeElapsed);
	}

	for (int i = 0; i < WEAPON_MAX_NUM; i++) {
		for (int j = 0; j < WEAPON_EACH_NUM; j++) {
			if (i == 4 && j == 1)//컵케익 1개
				break;
			weapon_box[i][j]->getObjects()->m_xmf4x4World = m_WeaponShader[i]->getObject(j)->m_xmf4x4World;
		}
	}

	for (int i = 0; i < WEAPON_MAX_NUM; i++) {
		for (int j = 0; j < WEAPON_EACH_NUM; j++) {
			if (i == 4 && j == 1)//컵케익 1개
				break;
			m_ExplosionShader[i][j]->Animate(fTimeElapsed);
		}
	}

	//if (m_ExplosionShader[0][0]->m_bBlowingUp == false) m_ExplosionShader[0][0]->m_bBlowingUp = true;	//test
	//m_ExplosionShader[0][0]->Setposition(-100, 30, 20);
	//m_ExplosionShader[0][0]->Animate(fTimeElapsed);

	if (Selected_Map == M_Map_1) {
		if (m_WeaponShader[M_Weapon_cupcake]->get_cupcake_up_flag() == false)
			m_WeaponShader[M_Weapon_cupcake]->Animate(fTimeElapsed, animate_flag, M_Weapon_cupcake);

		//m_MapShader[2]->Animate(fTimeElapsed, animate_flag);
	}
	if (Selected_Map == M_Map_3) {
		m_MapShader[15]->Animate(fTimeElapsed, animate_flag);
	}

	m_WavesShader->Animate(fTimeElapsed, Selected_Map);
	if (m_DarkShader) m_DarkShader->Animate(fTimeElapsed);
	for (UINT i = 0; i < m_nUIShaders; i++) {
		if (m_ppUIShaders[i]) {
			m_ppUIShaders[i]->UpdateState(ready_state);
			m_ppUIShaders[i]->Animate(fTimeElapsed);
		}
	}
	if (m_ppUIShaders[21]->Get_Fog_Flag()&&m_ppUIShaders[21]->Get_Fog_Off_Flag()) {
		for (int i = 0; i < 8; i++) {
			m_ppUIShaders[i]->FogOn(false);
		}
		for (int i = 12; i < 20; i++) {
			m_ppUIShaders[i]->FogOn(false);
		}
	}
	m_MessageShader->Animate(fTimeElapsed);
	m_EffectShader->Animate(fTimeElapsed);

	m_MagicShader->Animate(fTimeElapsed);
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
	int animindex = m_pPlayer[i]->getAnimIndex();
	if (animindex == Anim_chocolate_Attack) {
		if (m_pPlayer[i]->getAnimtime() > 15) m_WeaponShader[weapon_type]->getObject(weapon_index)->Rotate(-30, 0, 60);
		else m_WeaponShader[weapon_type]->getObject(weapon_index)->Rotate(-70, 0, 0);
	}
	else if (animindex == Anim_chocolate_HardAttack || animindex == Anim_chocolate_Skill || animindex == Anim_chocolate_Guard) {
		m_WeaponShader[weapon_type]->getObject(weapon_index)->Rotate(-10, 0, 55);
	}
	else if (animindex == Anim_candy_HardAttack) {
		if (m_pPlayer[i]->getAnimtime() > 18) m_WeaponShader[weapon_type]->getObject(weapon_index)->Rotate(-140, 20, 30);
		else m_WeaponShader[weapon_type]->getObject(weapon_index)->Rotate(-70, 20, 30);
	}
	else if (animindex == Anim_pepero_Skill) {
		if (m_pPlayer[i]->getAnimtime() > 19 && m_pPlayer[i]->getAnimtime() < 24) m_WeaponShader[weapon_type]->getObject(weapon_index)->Rotate(-120, 0, 0);
		else m_WeaponShader[weapon_type]->getObject(weapon_index)->Rotate(-70, 0, 0);
	}
	else if (weapon_type == M_Weapon_chocolate) m_WeaponShader[weapon_type]->getObject(weapon_index)->Rotate(-100, 10, 0);
	else m_WeaponShader[weapon_type]->getObject(weapon_index)->Rotate(-70, 0, 0);

	if (SERVER_ON == false) {
		if (animindex == Anim_Lollipop_Skill && m_pPlayer[i]->getAnimtime() >= 23) {
			m_MagicShader->getObjects(weapon_index)->visible = true;//마법진

			XMFLOAT3 pos = m_pPlayer[0]->GetPosition();

			XMFLOAT3 aa = m_WeaponShader[weapon_type]->getObject(weapon_index)->GetPosition();
			aa.y -= 3.f;
			m_WeaponShader[weapon_type]->getObject(weapon_index)->init(aa);
			m_WeaponShader[weapon_type]->getObject(weapon_index)->Rotate(0, 90.f, 0);
			//m_WeaponShader[weapon_type]->getObject(weapon_index)->SetScale(1.5f);

			m_MagicShader->getObjects(weapon_index)->SetPosition(aa);
		}
	}
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
				if (weapon_box[i][j]->pick == false)
				{
					bool result = weapon_box[i][j]->bounding.Intersects(bounding_box_test[index]->bounding);
					if (result) {
						m_pPlayer[index]->SetWeapon(false, i, j);
						//cout << "player "<<index<< "---"<<"무기 종류: " << i << ", 무기 번호: " << j << endl;
						return;
					}
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
		//bounding_box_test[index]->bounding.Center = m_pPlayer[index]->GetPosition();

		bool result = door[i]->bounding.Intersects(bounding_box_test[index]->bounding);
		if (result) {
			//XMFLOAT3 telpos;
			float tmp = 30.f;
			if (i == 0) {
				XMFLOAT3 telpos = door[1]->getObjects()->GetPosition(); telpos.z += tmp;
				physx::PxExtendedVec3 tmp;
				tmp.x=telpos.x;	tmp.y = telpos.y;tmp.z = telpos.z;
				m_pPlayer[index]->m_PlayerController->setPosition(tmp);
			}		// LEFT_DOWN_OUT	
			else if (i == 1) {
				XMFLOAT3 telpos = door[0]->getObjects()->GetPosition(); telpos.z -= tmp;
				physx::PxExtendedVec3 tmp;
				tmp.x = telpos.x;	tmp.y = telpos.y; tmp.z = telpos.z;
				m_pPlayer[index]->m_PlayerController->setPosition(tmp);
			}		// LEFT_DOWN_IN	
			else if (i == 2) {
				XMFLOAT3 telpos = door[3]->getObjects()->GetPosition(); telpos.z += tmp; 
				physx::PxExtendedVec3 tmp;
				tmp.x = telpos.x;	tmp.y = telpos.y; tmp.z = telpos.z;
				m_pPlayer[index]->m_PlayerController->setPosition(tmp);
			}		// LEFT_UP_OUT		
			else if (i == 3) { 
				XMFLOAT3 telpos = door[2]->getObjects()->GetPosition(); telpos.z -= tmp;
				physx::PxExtendedVec3 tmp;
				tmp.x = telpos.x;	tmp.y = telpos.y; tmp.z = telpos.z;
				m_pPlayer[index]->m_PlayerController->setPosition(tmp);
			}		// LEFT_UP_IN		
																						  		
			else if (i == 4) {
				XMFLOAT3 telpos = door[5]->getObjects()->GetPosition(); telpos.z += tmp;
				physx::PxExtendedVec3 tmp;
				tmp.x=telpos.x;	tmp.y = telpos.y;tmp.z = telpos.z;
				m_pPlayer[index]->m_PlayerController->setPosition(tmp);
			}		// RIGHT_DOWN_OUT	
			else if (i == 5) {
				XMFLOAT3 telpos = door[4]->getObjects()->GetPosition(); telpos.z -= tmp;
				physx::PxExtendedVec3 tmp;
				tmp.x=telpos.x;	tmp.y = telpos.y;tmp.z = telpos.z;
				m_pPlayer[index]->m_PlayerController->setPosition(tmp);
			}		// RIGHT_DOWN_IN	
			else if (i == 6) {
				XMFLOAT3 telpos = door[7]->getObjects()->GetPosition(); telpos.z += tmp;
				physx::PxExtendedVec3 tmp;
				tmp.x=telpos.x;	tmp.y = telpos.y;tmp.z = telpos.z;
				m_pPlayer[index]->m_PlayerController->setPosition(tmp);
			}		// RIGHT_UP_OUT	
			else if (i == 7) { 
				XMFLOAT3 telpos = door[6]->getObjects()->GetPosition(); telpos.z -= tmp; physx::PxExtendedVec3 tmp;
				tmp.x=telpos.x;	tmp.y = telpos.y;tmp.z = telpos.z;
				m_pPlayer[index]->m_PlayerController->setPosition(tmp);
			}		// RIGHT_UP_IN		
			//m_pPlayer[index]->SetPosition(telpos);
			//return;
			//bounding_box_test[index]->bounding.Center = m_pPlayer[index]->GetPosition();
			//cout << "door"<<i<<" x:" << door[i]->getObjects()->GetPosition().x << " y:" << door[i]->getObjects()->GetPosition().y << " z:" << door[i]->getObjects()->GetPosition().z << endl;
			//cout << "player x:" << m_pPlayer[index]->GetPosition().x << " y:" << m_pPlayer[index]->GetPosition().y << " z:" << m_pPlayer[index]->GetPosition().z << endl;
			break;
		}
	}
}

void CScene::Collision_Cotton()
{	
	if (Selected_Map == M_Map_1) {
		for (int i = 0; i < MAX_USER; ++i) {
			bounding_box_test[i]->bounding.Center = m_pPlayer[i]->GetPosition();
			m_pPlayerShader[i]->render = true;
			if (m_pPlayer[i]->Get_Weapon_grab())
				m_WeaponShader[m_pPlayer[i]->Get_Weapon_type()]->getObject(m_pPlayer[i]->Get_Weapon_index())->visible = true;
			if (bounding_box_test[i]->bounding.Intersects(Cotton_box[0])) {
				m_pPlayerShader[i]->render = false;		
				if(m_pPlayer[i]->Get_Weapon_grab())
					m_WeaponShader[m_pPlayer[i]->Get_Weapon_type()]->getObject(m_pPlayer[i]->Get_Weapon_index())->visible = false;
				if (myid == i) {
					m_pPlayerShader[i]->render = true;
					if (m_pPlayer[i]->Get_Weapon_grab())
						m_WeaponShader[m_pPlayer[i]->Get_Weapon_type()]->getObject(m_pPlayer[i]->Get_Weapon_index())->visible = true;
				}
				continue;
			}

			if (bounding_box_test[i]->bounding.Intersects(Cotton_box[1])) {
				m_pPlayerShader[i]->render = false;
				if (m_pPlayer[i]->Get_Weapon_grab())
					m_WeaponShader[m_pPlayer[i]->Get_Weapon_type()]->getObject(m_pPlayer[i]->Get_Weapon_index())->visible = false;
				if (myid == i) {
					m_pPlayerShader[i]->render = true;
					if (m_pPlayer[i]->Get_Weapon_grab())
						m_WeaponShader[m_pPlayer[i]->Get_Weapon_type()]->getObject(m_pPlayer[i]->Get_Weapon_index())->visible = true;
				}
				continue;
			}
		}

		if(SERVER_ON == false) if (myid == -1) return;

		if (bounding_box_test[myid]->bounding.Intersects(Cotton_box[0])) {
			for (int i = 0; i < MAX_USER; ++i) {
				if (myid == i) continue;
				if (bounding_box_test[i]->bounding.Intersects(Cotton_box[0])) {
					m_pPlayerShader[i]->render = true;
					if (m_pPlayer[i]->Get_Weapon_grab())
						m_WeaponShader[m_pPlayer[i]->Get_Weapon_type()]->getObject(m_pPlayer[i]->Get_Weapon_index())->visible = true;
				}
			}
		}
		if (bounding_box_test[myid]->bounding.Intersects(Cotton_box[1])) {
			for (int i = 0; i < MAX_USER; ++i) {
				if (myid == i) continue;
				if (bounding_box_test[i]->bounding.Intersects(Cotton_box[1])) {
					m_pPlayerShader[i]->render = true;
					if (m_pPlayer[i]->Get_Weapon_grab())
						m_WeaponShader[m_pPlayer[i]->Get_Weapon_type()]->getObject(m_pPlayer[i]->Get_Weapon_index())->visible = true;
				}
			}
		}
	}
}

void CScene::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	for (int i = 0; i < MAX_USER; ++i) if (m_pPlayer[i]->GetConnected()) m_pPlayerShader[i]->Render(pd3dCommandList, pCamera);
	for (int i = 0; i < MAX_USER; i++) if (m_pPlayer[i]->GetConnected()) if (m_StunShader[i])if(m_StunShader[i]->visible) m_StunShader[i]->Render(pd3dCommandList, pCamera);
	
	if (Selected_Map == M_Map_1) {
		if (m_MapShader[0]) m_MapShader[0]->Render(pd3dCommandList, pCamera);
		if (m_MapShader[1]) m_MapShader[1]->Render(pd3dCommandList, pCamera);
		if (m_MapShader[2]) m_MapShader[2]->Render(pd3dCommandList, pCamera);
	}
	if (Selected_Map == M_Map_2) {
		if (m_MapShader[3]) m_MapShader[3]->Render(pd3dCommandList, pCamera);
		if (m_MapShader[4]) m_MapShader[4]->Render(pd3dCommandList, pCamera);
		if (m_MapShader[5]) m_MapShader[5]->Render(pd3dCommandList, pCamera);
		if (m_MapShader[6]) m_MapShader[6]->Render(pd3dCommandList, pCamera);
		if (m_MapShader[7]) m_MapShader[7]->Render(pd3dCommandList, pCamera);
	}
	if (Selected_Map == M_Map_3) {
	//	for (int i = 0; i < 8; ++i)if (door[i]) door[i]->Render(pd3dCommandList, pCamera);
		
		if (m_MapShader[8]) m_MapShader[8]->Render(pd3dCommandList, pCamera);
		if (m_MapShader[9]) m_MapShader[9]->Render(pd3dCommandList, pCamera);
		if (m_MapShader[10]) m_MapShader[10]->Render(pd3dCommandList, pCamera);
		if (m_MapShader[11]) m_MapShader[11]->Render(pd3dCommandList, pCamera);
		if (m_MapShader[12]) m_MapShader[12]->Render(pd3dCommandList, pCamera);
		if (m_MapShader[13]) m_MapShader[13]->Render(pd3dCommandList, pCamera);
		if (m_MapShader[14]) m_MapShader[14]->Render(pd3dCommandList, pCamera);
		if (m_MapShader[15]) m_MapShader[15]->Render(pd3dCommandList, pCamera);

		if (m_MapShader[16]) m_MapShader[16]->Render(pd3dCommandList, pCamera);
		if (m_MapShader[17]) m_MapShader[17]->Render(pd3dCommandList, pCamera);
		if (m_MapShader[18]) m_MapShader[18]->Render(pd3dCommandList, pCamera);
		if (m_MapShader[19]) m_MapShader[19]->Render(pd3dCommandList, pCamera);
		if (m_MapShader[20]) m_MapShader[20]->Render(pd3dCommandList, pCamera);
		if (m_MapShader[21]) m_MapShader[21]->Render(pd3dCommandList, pCamera);
		//if (m_MapShader[22]) m_MapShader[22]->Render(pd3dCommandList, pCamera);
		//if (m_MapShader[23]) m_MapShader[23]->Render(pd3dCommandList, pCamera);

		m_BridgeShader->Render(pd3dCommandList, pCamera);
		for (int i = 0; i < 2; ++i) if (m_StairShader[i]) m_StairShader[i]->Render(pd3dCommandList, pCamera);
	}
	for (int i = 0; i < WEAPON_MAX_NUM; ++i) if (m_WeaponShader[i]) m_WeaponShader[i]->Render(pd3dCommandList, pCamera);

	if (m_WavesShader) m_WavesShader->Render(pd3dCommandList, pCamera);

	//if (m_BackGroundShader[2]) m_BackGroundShader[2]->Render(pd3dCommandList, pCamera);
	//if (m_BackGroundShader[0]) m_BackGroundShader[0]->Render(pd3dCommandList, pCamera);
	
	if (Selected_Map == M_Map_1) {
		if (m_RoundBackGroundShader[0]) m_RoundBackGroundShader[0]->Render(pd3dCommandList, pCamera);

		if (m_TeamShader) m_TeamShader->Render(pd3dCommandList, pCamera, m_pPlayerShader);
		if (m_EnemyShader) m_EnemyShader->Render(pd3dCommandList, pCamera, m_pPlayerShader);
		for (int i = 0; i < 2; ++i) if (m_CottonShader[i]) m_CottonShader[i]->Render(pd3dCommandList, pCamera);
		if (m_ShadowReverseShader[0]) m_ShadowReverseShader[0]->Render(pd3dCommandList, pCamera);
		if (m_ShadowReverseShader[1]) m_ShadowReverseShader[1]->Render(pd3dCommandList, pCamera);
		if (m_ShadowReverseShader[2]) m_ShadowReverseShader[2]->Render(pd3dCommandList, pCamera);
		if (m_ShadowReverseShader[3]) m_ShadowReverseShader[3]->Render(pd3dCommandList, pCamera);
	}
	if (Selected_Map == M_Map_2) {
		if (m_RoundBackGroundShader[1]) m_RoundBackGroundShader[1]->Render(pd3dCommandList, pCamera);

		if (m_TeamShader) m_TeamShader->Render(pd3dCommandList, pCamera, m_pPlayerShader);
		if (m_EnemyShader) m_EnemyShader->Render(pd3dCommandList, pCamera, m_pPlayerShader);
		if (m_ShadowReverseModelShader[0]) m_ShadowReverseModelShader[0]->Render(pd3dCommandList, pCamera);
		if (m_ShadowReverseModelShader[1]) m_ShadowReverseModelShader[1]->Render(pd3dCommandList, pCamera);
		if (m_ShadowReverseModelShader[2]) m_ShadowReverseModelShader[2]->Render(pd3dCommandList, pCamera);
	}
	if (Selected_Map == M_Map_3) {
		if (m_RoundBackGroundShader[2]) m_RoundBackGroundShader[2]->Render(pd3dCommandList, pCamera);

		if (m_TeamShader) m_TeamShader->Render(pd3dCommandList, pCamera, m_pPlayerShader);
		if (m_EnemyShader) m_EnemyShader->Render(pd3dCommandList, pCamera, m_pPlayerShader);
		if (m_ShadowReverseShader[4]) m_ShadowReverseShader[4]->Render(pd3dCommandList, pCamera);
		if (m_ShadowReverseShader[5]) m_ShadowReverseShader[5]->Render(pd3dCommandList, pCamera);
		if (m_ShadowReverseShader[6]) m_ShadowReverseShader[6]->Render(pd3dCommandList, pCamera);
		if (m_ShadowReverseShader[7]) m_ShadowReverseShader[7]->Render(pd3dCommandList, pCamera);

		if (m_ShadowShader[1]) m_ShadowShader[1]->Render(pd3dCommandList, pCamera);
		if (m_ShadowShader[2]) m_ShadowShader[2]->Render(pd3dCommandList, pCamera);
		if (m_ShadowShader[3]) m_ShadowShader[3]->Render(pd3dCommandList, pCamera);
		if (m_ShadowShader[4]) m_ShadowShader[4]->Render(pd3dCommandList, pCamera);

		if (m_BackGroundShader[1]) m_BackGroundShader[1]->Render(pd3dCommandList, pCamera);
	}
	
	//for (int i = 0; i < WEAPON_MAX_NUM; i++) {
	//	for (int j = 0; j < WEAPON_EACH_NUM; j++) {
	//		if (weapon_box[i][j])
	//			weapon_box[i][j]->Render(pd3dCommandList, pCamera);
	//	}
	//}

	for (int i = 0; i < WEAPON_MAX_NUM; i++) {
		for (int j = 0; j < WEAPON_EACH_NUM; j++) {
			if (m_ExplosionShader[i][j])
				if (m_ExplosionShader[i][j]->m_bBlowingUp)
					m_ExplosionShader[i][j]->Render(pd3dCommandList, pCamera);
		}
	}

	for (int i = 0; i < MAX_USER; ++i) if (m_pPlayer[i]->GetConnected() && m_pPlayerShader[i]->render) m_pPlayerShadowShader[i]->Render(pd3dCommandList, pCamera);

	if (m_MagicShader) m_MagicShader->Render(pd3dCommandList, pCamera);

	if (Selected_Map == M_Map_1) if (m_ShadowShader[0]) m_ShadowShader[0]->Render(pd3dCommandList, pCamera);
	
	for (int i = 0; i < MAX_USER; i++)if (m_SkillEffectShader[i]) /*if (m_SkillEffectShader[i]->visible == true)*/ m_SkillEffectShader[i]->Render(pd3dCommandList, pCamera);

	for (int i = 0; i < 10; i++) if (m_EffectShader) m_EffectShader->Render(pd3dCommandList, pCamera);

	//for (int i = 0; i < MAX_USER;i++)if (bounding_box_test[i]) bounding_box_test[i]->Render(pd3dCommandList, pCamera);
	
	for (int i = 0; i < MAX_USER; i++)
		for(int j=0;j<3;j++)
		if (m_SkillParticleShader[i][j])m_SkillParticleShader[i][j]->Render(pd3dCommandList, pCamera);

}

void CScene::RenderUI(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList)
{
	//for (UINT i = 0; i < m_nUIShaders - 2; ++i)
	//	m_ppUIShaders[i]->Render(pCommandList);
	if (m_DarkShader)m_DarkShader->Render(pCommandList);
	for (int i = m_nUIShaders-2; i >= 0; i--) {	//22번 제외
		m_ppUIShaders[i]->Render(pCommandList);
	}
	if(m_MessageShader) m_MessageShader->Render(pCommandList);
}

void CScene::RenderLoading(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList)
{
	m_ppUIShaders[22]->Render(pCommandList);
}