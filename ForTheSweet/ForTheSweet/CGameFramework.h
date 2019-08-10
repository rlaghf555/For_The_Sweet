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
	// 이동관련 
	bool move_state[5] = { false, false, false, false, false }; // 상, 하, 좌, 우, shift

	// 약공격 관련
	bool weak_attack_state = false;
	char weak_attack_count = 0;
	high_resolution_clock::time_point weak_attack_time;

	// 강공격 관련
	bool hard_attack_state = false;
	char hard_attack_count = 0;
	high_resolution_clock::time_point hard_attack_time;

	// 점프 관련
	bool jump_state = false;

	// 방어 관련
	bool defense_state = false;
	bool defense_check = false;
	bool defense_key;
	high_resolution_clock::time_point defense_time;

	// 스킬 관련
	bool skill_state = false;

	PxRigidActor *Choco_Actor[WEAPON_EACH_NUM];

	vector<KEY> key_buffer;

	char *m_pid, *m_pip;	//id, ip
	// Physx
	CPhysx *m_pPhysx;

	//플레이어 객체에 대한 포인터이다.
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
	//마지막으로 마우스 버튼을 클릭할 때의 마우스 커서의 위치이다.
	POINT m_ptOldCursorPos;

private:
	CGameTimer m_GameTimer;		// 게임 프레임워크에서 사용할 타이머이다.
	_TCHAR m_pszFrameRate[50];	// 프레임 레이트를 주 윈도우의 캡션에 출력하기 위한 문자열이다.

	HINSTANCE m_hInstance;
	HWND m_hWnd;
	int m_nWndClientWidth;
	int m_nWndClientHeight;
	IDXGIFactory4 *m_pdxgiFactory;			//DXGI 팩토리 인터페이스에 대한 포인터이다.
	IDXGISwapChain3 *m_pdxgiSwapChain;		//스왑 체인 인터페이스에 대한 포인터이다. 주로 디스플레이를 제어하기 위하여 필요하다.
	ID3D12Device *m_pd3dDevice;				//Direct3D 디바이스 인터페이스에 대한 포인터이다. 주로 리소스를 생성하기 위하여 필요하다.

	bool m_bMsaa4xEnable = false;
	UINT m_nMsaa4xQualityLevels = 0;
	//MSAA 다중 샘플링을 활성화하고 다중 샘플링 레벨을 설정한다.
	static const UINT m_nSwapChainBuffers = 2;
	//스왑 체인의 후면 버퍼의 개수이다.
	UINT m_nSwapChainBufferIndex;
	//현재 스왑 체인의 후면 버퍼 인덱스이다.
	ID3D12Resource *m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap *m_pd3dRtvDescriptorHeap;
	UINT m_nRtvDescriptorIncrementSize;
	//렌더 타겟 버퍼, 서술자 힙 인터페이스 포인터, 렌더 타겟 서술자 원소의 크기이다.
	ID3D12Resource *m_pd3dDepthStencilBuffer;
	ID3D12DescriptorHeap *m_pd3dDsvDescriptorHeap;
	UINT m_nDsvDescriptorIncrementSize;
	//깊이-스텐실 버퍼, 서술자 힙 인터페이스 포인터, 깊이-스텐실 서술자 원소의 크기이다.
	ID3D12CommandQueue *m_pd3dCommandQueue;
	ID3D12PipelineState *m_pd3dPipelineState;
	ID3D12CommandAllocator *m_pd3dCommandAllocator;
	ID3D12GraphicsCommandList *m_pd3dCommandList;
	//명령 큐, 명령 할당자, 명령 리스트 인터페이스 포인터이다.
	ID3D12Fence *m_pd3dFence;
	UINT64 m_nFenceValues[m_nSwapChainBuffers];
	CScene *m_pScene;

	HANDLE m_hFenceEvent;
	//펜스 인터페이스 포인터, 펜스의 값, 이벤트 핸들이다.

	Model_Animation *Character_Model;
	LoadModel		*Map_Model[22];
	LoadModel		*weapon[7];
	LoadModel		*effect[2];
#if defined(_DEBUG)
	ID3D12Debug *m_pd3dDebugController;

#endif
	//	D3D12_VIEWPORT m_d3dViewport;
	//	D3D12_RECT m_d3dScissorRect;		//뷰포트와 씨저 사각형이다.

public:
	CGameFramework();
	~CGameFramework();
	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	//프레임워크를 초기화하는 함수이다(주 윈도우가 생성되면 호출된다).
	void SetIDIP(wchar_t *id, wchar_t *ip);
	void MoveToNextFrame();

	void OnResizeBackBuffers();

	void OnDestroy();
	bool Restart();

	void CreateSwapChain();
	void CreateDirect3DDevice();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateCommandQueueAndList();
	//스왑 체인, 디바이스, 서술자 힙, 명령 큐/할당자/리스트를 생성하는 함수이다.

	void LoadModels();

	void BuildObjects();
	void ReleaseObjects();
	//렌더링할 메쉬와 게임 객체를 생성하고 소멸하는 함수이다.

	//프레임워크의 핵심(사용자 입력, 애니메이션, 렌더링)을 구성하는 함수이다.
	void ProcessInput();
	void AnimateObjects();
	void UpdateProcess();
	void CollisionProcess();
	void Map2Camera();
	void FrameAdvance();

	void WaitForGpuComplete();

	void CameraShake();
	void GameOver();

	//CPU와 GPU를 동기화하는 함수이다.

	// Socket
	void processPacket(char *ptr);

	void CreateRenderTargetView();
	void CreateDepthStencilView();

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM	lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	//윈도우의 메시지(키보드, 마우스 입력)를 처리하는 함수이다.
	bool playing = false;
};
