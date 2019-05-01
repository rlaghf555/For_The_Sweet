#pragma once
#include "Timer.h"
#include "Shader.h"
#include "Player.h"
#include "Camera.h"
#include "Physx.h"
#include "Protocol.h"

class CScene
{
public:
	CScene();
	~CScene();
	//������ ���콺�� Ű���� �޽����� ó���Ѵ�.
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, CPhysx* physx);
	void BuildRootSignature(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	void ReleaseObjects();
	bool ProcessInput(UCHAR *pKeysBuffer);
	void AnimateObjects(float fTimeElapsed);
	void CollisionProcess();
	void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	void ReleaseUploadBuffers();
	void SetCharacter(Model_Animation *model_anim) { character_anim = model_anim; }
	void SetMap(LoadModel **mapmodel) { Map_Model = mapmodel; }
	void SetWeapon(LoadModel **weaponmodel) { weapon_Model = weaponmodel; }
	CCamera* GetCamera() { return m_Camera.get(); }
	//�׷��� ��Ʈ �ñ׳��ĸ� �����Ѵ�.
	ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	ID3D12RootSignature *GetGraphicsRootSignature();

	CPlayer						*m_pPlayer[MAX_USER];
	CPlayer*					getplayer(char id) { return m_pPlayer[id]; }
	Model_Animation				*character_anim;
	PlayerShader				*m_pPlayerShader[MAX_USER];

	CModelShader				*m_MapShader = NULL;
	LoadModel					**Map_Model;

	WeaponShader				*m_WeaponShader = NULL;
	LoadModel					**weapon_Model;

	MeshShader					*m_BackGroundShader = NULL;

	WaveShader					*m_WavesShader = NULL;

	unique_ptr<CCamera>			m_Camera = nullptr;

protected:
	//���� ���� ��ü���� �����̴�. ���� ��ü�� ���̴��� �����Ѵ�.
	//CGameObject * *m_ppObjects = NULL;
	//int m_nObjects = 0;


	//��ġ(Batch) ó���� �ϱ� ���Ͽ� ���� ���̴����� ����Ʈ�� ǥ���Ѵ�.
	//CObjectsShader *m_pShaders = NULL;
	CInstancingShader *m_pInstancingShaders = NULL;
	int m_nInstancingShaders = 0;

	CObjectsShader *m_pMapShader = NULL;
	int m_nMapShader = 0;

	//CPlayerObjectsShader *m_pPlayerObjectShaders = NULL;
	int m_nPlayerObjectShaders = 0;

	ID3D12RootSignature *m_pd3dGraphicsRootSignature = NULL;
};

