#pragma once
#include "Timer.h"
#include "Scene.h"
#include "Player.h"
#include "Model_Animation.h"
#include "Physx.h"
#include "Socket.h"

struct KEY {
	bool key;
	high_resolution_clock::time_point time;
};

class CGameFramework
{
public:
	// Socket
	CSocket *m_pSocket;

	int current_player_num;
	int current_weapon_num;

	int max_player_num;
	int max_weapon_num;

	bool setting_player;
	bool setting_weapon;

	bool setting_send;

	bool input_able = false;
	bool fever = false;
	bool move_actor_flag = false;
	bool isKing = false;

	wchar_t *Characters_ID[MAX_USER];
	// �̵����� 
	bool move_state[5] = { false, false, false, false, false }; // ��, ��, ��, ��, shift

	// ����� ����
	bool weak_attack_state = false;
	char weak_attack_count = 0;
	high_resolution_clock::time_point weak_attack_time;

	// ������ ����
	bool hard_attack_state = false;
	char hard_attack_count = 0;
	high_resolution_clock::time_point hard_attack_time;

	// ���� ����
	bool jump_state = false;

	// ��� ����
	bool defense_state = false;
	bool defense_check = false;
	bool defense_key;
	high_resolution_clock::time_point defense_time;

	// ��ų ����
	bool skill_state = false;

	PxRigidActor *Choco_Actor[WEAPON_EACH_NUM];

	vector<KEY> key_buffer;

	char *m_pid, *m_pip;	//id, ip
	// Physx
	CPhysx *m_pPhysx;

	//�÷��̾� ��ü�� ���� �������̴�.
	CPlayer * m_pPlayer;
	CCamera* m_pCamera;
	int My_ID = -1;
	int Camera_ID = 0;
	int time = 0;
	int selected_map = 0;
	int mode;
	int CameraShakeX = 0;
	float CameraShake_Time = 0.f;
	bool camerashake = false;
	//���������� ���콺 ��ư�� Ŭ���� ���� ���콺 Ŀ���� ��ġ�̴�.
	POINT m_ptOldCursorPos;

private:
	CGameTimer m_GameTimer;		// ���� �����ӿ�ũ���� ����� Ÿ�̸��̴�.
	_TCHAR m_pszFrameRate[50];	// ������ ����Ʈ�� �� �������� ĸ�ǿ� ����ϱ� ���� ���ڿ��̴�.

	HINSTANCE m_hInstance;
	HWND m_hWnd;
	int m_nWndClientWidth;
	int m_nWndClientHeight;
	IDXGIFactory4 *m_pdxgiFactory;			//DXGI ���丮 �������̽��� ���� �������̴�.
	IDXGISwapChain3 *m_pdxgiSwapChain;		//���� ü�� �������̽��� ���� �������̴�. �ַ� ���÷��̸� �����ϱ� ���Ͽ� �ʿ��ϴ�.
	ID3D12Device *m_pd3dDevice;				//Direct3D ����̽� �������̽��� ���� �������̴�. �ַ� ���ҽ��� �����ϱ� ���Ͽ� �ʿ��ϴ�.

	bool m_bMsaa4xEnable = false;
	UINT m_nMsaa4xQualityLevels = 0;
	//MSAA ���� ���ø��� Ȱ��ȭ�ϰ� ���� ���ø� ������ �����Ѵ�.
	static const UINT m_nSwapChainBuffers = 2;
	//���� ü���� �ĸ� ������ �����̴�.
	UINT m_nSwapChainBufferIndex;
	//���� ���� ü���� �ĸ� ���� �ε����̴�.
	ID3D12Resource *m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap *m_pd3dRtvDescriptorHeap;
	UINT m_nRtvDescriptorIncrementSize;
	//���� Ÿ�� ����, ������ �� �������̽� ������, ���� Ÿ�� ������ ������ ũ���̴�.
	ID3D12Resource *m_pd3dDepthStencilBuffer;
	ID3D12DescriptorHeap *m_pd3dDsvDescriptorHeap;
	UINT m_nDsvDescriptorIncrementSize;
	//����-���ٽ� ����, ������ �� �������̽� ������, ����-���ٽ� ������ ������ ũ���̴�.
	ID3D12CommandQueue *m_pd3dCommandQueue;
	ID3D12PipelineState *m_pd3dPipelineState;
	ID3D12CommandAllocator *m_pd3dCommandAllocator;
	ID3D12GraphicsCommandList *m_pd3dCommandList;
	//��� ť, ��� �Ҵ���, ��� ����Ʈ �������̽� �������̴�.
	ID3D12Fence *m_pd3dFence;
	UINT64 m_nFenceValues[m_nSwapChainBuffers];
	CScene *m_pScene;

	HANDLE m_hFenceEvent;
	//�潺 �������̽� ������, �潺�� ��, �̺�Ʈ �ڵ��̴�.

	Model_Animation *Character_Model;
	LoadModel		*Map_Model[22];
	LoadModel		*weapon[7];
	LoadModel		*effect[2];
#if defined(_DEBUG)
	ID3D12Debug *m_pd3dDebugController;

#endif
	//	D3D12_VIEWPORT m_d3dViewport;
	//	D3D12_RECT m_d3dScissorRect;		//����Ʈ�� ���� �簢���̴�.

public:
	CGameFramework();
	~CGameFramework();
	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	//�����ӿ�ũ�� �ʱ�ȭ�ϴ� �Լ��̴�(�� �����찡 �����Ǹ� ȣ��ȴ�).
	void SetIDIP(wchar_t *id, wchar_t *ip);
	void MoveToNextFrame();

	void OnResizeBackBuffers();

	void OnDestroy();
	bool Restart();

	void CreateSwapChain();
	void CreateDirect3DDevice();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateCommandQueueAndList();
	//���� ü��, ����̽�, ������ ��, ��� ť/�Ҵ���/����Ʈ�� �����ϴ� �Լ��̴�.

	void LoadModels();

	void BuildObjects();
	void ReleaseObjects();
	//�������� �޽��� ���� ��ü�� �����ϰ� �Ҹ��ϴ� �Լ��̴�.

	//�����ӿ�ũ�� �ٽ�(����� �Է�, �ִϸ��̼�, ������)�� �����ϴ� �Լ��̴�.
	void ProcessInput();
	void AnimateObjects();
	void UpdateProcess();
	void CollisionProcess();
	void Map2Camera();
	void FrameAdvance();

	void WaitForGpuComplete();

	void CameraShake();
	void GameOver();

	//CPU�� GPU�� ����ȭ�ϴ� �Լ��̴�.

	// Socket
	void processPacket(char *ptr);

	void CreateRenderTargetView();
	void CreateDepthStencilView();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM	lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	//�������� �޽���(Ű����, ���콺 �Է�)�� ó���ϴ� �Լ��̴�.
	bool playing = false;
};
