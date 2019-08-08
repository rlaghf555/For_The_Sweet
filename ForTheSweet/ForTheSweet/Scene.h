#pragma once
#include "Timer.h"
#include "Shader.h"
#include "Player.h"
#include "Camera.h"
#include "Physx.h"
#include "Protocol.h"
#include "UIShader.h"
#define MODE_TEAM 0 
#define MODE_INDIVIDUAL 1
#define MODE_KING 2
class CScene
{
public:
	CScene();
	~CScene();
	//씬에서 마우스와 키보드 메시지를 처리한다.
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, CPhysx* physx);
	void ReBuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, CPhysx* physx);
	void BuildUI(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	void BuildRootSignature(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	void ReleaseObjects();
	bool ProcessInput(UCHAR *pKeysBuffer);
	void AnimateObjects(float fTimeElapsed);
	void AnimateWeapon(int i);
	void CollisionProcess(int index);
	void Collision_telleport(int index);
	void Collision_Cotton();
	void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	void RenderUI(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList);
	void RenderLoading(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList);
	void ReleaseUploadBuffers();
	void SetCharacter(Model_Animation *model_anim) { character_anim = model_anim; }
	void SetMap(LoadModel **mapmodel) { Map_Model = mapmodel; }
	void SetWeapon(LoadModel **weaponmodel) { weapon_Model = weaponmodel; }
	void SetEffect(LoadModel **effectmodel) { effect_Model = effectmodel; }
	void initUI(wchar_t *character_id[]);
	void initObject();
	void SetTeamUI();
	CCamera* GetCamera() { return m_Camera.get(); }
	//그래픽 루트 시그너쳐를 생성한다.
	ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	ID3D12RootSignature *GetGraphicsRootSignature();

	PxRigidActor				*test;

	CPlayer						*m_pPlayer[MAX_USER];
	CPlayer*					getplayer(char id) { return m_pPlayer[id]; }
	Model_Animation				*character_anim;
	PlayerShader				*m_pPlayerShader[MAX_USER];
	PlayerShadowShader			*m_pPlayerShadowShader[MAX_USER];

	CModelShader				*m_MapShader[22] = { NULL, };
	LoadModel					**Map_Model;

	int							animate_flag = 0;

	StairShader					*m_StairShader[2];
	BridgeShader				*m_BridgeShader;

	int							ready_state	= UI_READY;
	float						ready_state_test = 0.3f;

	CottonCloudShader			*m_CottonShader[2];

	WeaponShader				*m_WeaponShader[WEAPON_MAX_NUM];
	LoadModel					**weapon_Model;

	testBox						*bounding_box_test[MAX_USER] = { NULL, };
	testBox						*weapon_box[WEAPON_MAX_NUM][WEAPON_EACH_NUM] = { NULL, };
	testBox						*door[8] = { NULL, };
	BoundingOrientedBox			Cotton_box[2];
	MeshShader					*m_BackGroundShader[2] = { NULL, };
	EffectShader				*m_EffectShader = NULL;
	SkillEffectShader			*m_SkillEffectShader[MAX_USER] = { NULL, };
	SkillParticleShader			*m_SkillParticleShader[MAX_USER][3] = { NULL, };
	WaveShader					*m_WavesShader = NULL;

	unique_ptr<CCamera>			m_Camera = nullptr;
	//UI
	vector<UIShader*>			m_ppUIShaders;
	UINT						m_nUIShaders;
	
	MessageShader				*m_MessageShader =  NULL;

	ShadowDebugShader			*m_ShadowShader[5] = { NULL, };

	ShadowREverseShader			*m_ShadowReverseShader[8] = { NULL, };
	ShadowReverseModelShader	*m_ShadowReverseModelShader[3] = { NULL, };

	StunShader					*m_StunShader[MAX_USER] = { NULL, };
	LoadModel					**effect_Model;

	MagicShader					*m_MagicShader[MAX_USER] = { NULL, };

	ExplosionShader				*m_ExplosionShader[MAX_USER] = { NULL, };
	TeamShader					*m_TeamShader = NULL;
	TeamShader					*m_EnemyShader = NULL;
	DarkShader					*m_DarkShader = NULL;
	int Selected_Map = 0;
	int mode = MODE_TEAM;
	int myid;
	bool Map_1_Build = false;
	bool Map_2_Build = false;
	bool Map_3_Build = false;
protected:
	//씬은 게임 객체들의 집합이다. 게임 객체는 셰이더를 포함한다.
	//CGameObject * *m_ppObjects = NULL;
	//int m_nObjects = 0;


	//배치(Batch) 처리를 하기 위하여 씬을 셰이더들의 리스트로 표현한다.
	//CObjectsShader *m_pShaders = NULL;
	CInstancingShader *m_pInstancingShaders = NULL;
	int m_nInstancingShaders = 0;

	CObjectsShader *m_pMapShader = NULL;
	int m_nMapShader = 0;

	//CPlayerObjectsShader *m_pPlayerObjectShaders = NULL;
	int m_nPlayerObjectShaders = 0;

	ID3D12RootSignature *m_pd3dGraphicsRootSignature = NULL;
};

