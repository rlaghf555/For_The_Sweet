#include "stdafx.h"
#include "CGameFramework.h"

char * ConvertWCtoC(wchar_t* str)
{
	//반환할 char* 변수 선언
	char* pStr;

	//입력받은 wchar_t 변수의 길이를 구함
	int strSize = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	//char* 메모리 할당
	pStr = new char[strSize];

	//형 변환 
	WideCharToMultiByte(CP_ACP, 0, str, -1, pStr, strSize, 0, 0);
	return pStr;
}
CGameFramework::CGameFramework()
{
	_tcscpy_s(m_pszFrameRate, _T("ForTheSweet("));

	m_pdxgiFactory = NULL;
	m_pdxgiSwapChain = NULL;
	m_pd3dDevice = NULL;

	m_pd3dCommandAllocator = NULL;
	m_pd3dCommandQueue = NULL;
	m_pd3dCommandList = NULL;

	m_pd3dPipelineState = NULL;

	for (int i = 0; i < m_nSwapChainBuffers; i++) m_nFenceValues[i] = 0;

	m_pScene = NULL;
	m_pPlayer = NULL;

	m_pCamera = NULL;

	for (int i = 0; i < m_nSwapChainBuffers; i++) m_ppd3dRenderTargetBuffers[i] = NULL;
	m_nSwapChainBufferIndex = 0;

	m_pd3dDepthStencilBuffer = NULL;

	m_pd3dRtvDescriptorHeap = NULL;
	m_pd3dDsvDescriptorHeap = NULL;

	m_nRtvDescriptorIncrementSize = 0;
	m_nDsvDescriptorIncrementSize = 0;

	m_hFenceEvent = NULL;
	m_pd3dFence = NULL;

	//m_nFenceValue = 0;
	m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;
	
	Character_Model = NULL;

	m_pPhysx = NULL;
}

//다음 함수는 응용 프로그램이 실행되어 주 윈도우가 생성되면 호출된다는 것에 유의하라.
bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;
	//Direct3D 디바이스, 명령 큐와 명령 리스트, 스왑 체인 등을 생성하는 함수를 호출한다.

	CreateDirect3DDevice();
	CreateCommandQueueAndList();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();
	OnResizeBackBuffers();
	LoadModels();
	//CreateRenderTargetView();
	//CreateDepthStencilView();

	BuildObjects();		//렌더링할 객체(게임 월드 객체)를 생성한다.

	if (SERVER_ON) {
		if (UI_ON) {
			m_pSocket = new CSocket(m_pid, m_pip);
			if (m_pSocket) {
				if (m_pSocket->init())
				{
					m_pSocket->sendPacket(CS_CONNECT, 0, 0, 0);
					cout << "CONNECT 패킷 보냄\n";
				}
				else
					return false;
			}
		}
		else if (UI_ON==false) { 
			char tmpip[15];
			cout << "ip:";
			cin >> tmpip;
			m_pSocket = new CSocket(m_pid, tmpip);
			if (m_pSocket) {
				if (m_pSocket->init())
				{
					m_pSocket->sendPacket(CS_CONNECT, 0, 0, 0);
					cout << "CONNECT 패킷 보냄\n";
				}
				else
					return false;
			}
		}
	}

	return(true);
}

void CGameFramework::OnDestroy()
{
	if (m_pSocket) m_pSocket->Release();

	ReleaseObjects();
	::CloseHandle(m_hFenceEvent);
#if defined(_DEBUG)
	if (m_pd3dDebugController) m_pd3dDebugController->Release();
#endif
	for (int i = 0; i < m_nSwapChainBuffers; i++) if (m_ppd3dRenderTargetBuffers[i])
		m_ppd3dRenderTargetBuffers[i]->Release();
	if (m_pd3dRtvDescriptorHeap) m_pd3dRtvDescriptorHeap->Release();
	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
	if (m_pd3dDsvDescriptorHeap) m_pd3dDsvDescriptorHeap->Release();
	if (m_pd3dCommandAllocator) m_pd3dCommandAllocator->Release();
	if (m_pd3dCommandQueue) m_pd3dCommandQueue->Release();
	if (m_pd3dCommandList) m_pd3dCommandList->Release();
	if (m_pd3dFence) m_pd3dFence->Release();
	m_pdxgiSwapChain->SetFullscreenState(FALSE, NULL);
	if (m_pdxgiSwapChain) m_pdxgiSwapChain->Release();
	if (m_pd3dDevice) m_pd3dDevice->Release();
	if (m_pdxgiFactory) m_pdxgiFactory->Release();
}

void CGameFramework::CreateSwapChain()
{
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	m_nWndClientWidth = rcClient.right - rcClient.left;
	m_nWndClientHeight = rcClient.bottom - rcClient.top;

//#ifdef _WITH_CREATE_SWAPCHAIN_FOR_HWND
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers;
	dxgiSwapChainDesc.BufferDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.BufferDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.OutputWindow = m_hWnd;
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	dxgiSwapChainDesc.Windowed = TRUE;
#ifdef _WITH_ONLY_RESIZE_BACKBUFFERS
	//전체화면 모드에서 바탕화면의 해상도를 바꾸지 않고 후면버퍼의 크기를 바탕화면 크기로 변경한다.
	dxgiSwapChainDesc.Flags = 0;
#else
	//전체화면 모드에서 바탕화면의 해상도를 스왑체인(후면버퍼)의 크기에 맞게 변경한다.
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
#endif
	HRESULT hResult = m_pdxgiFactory->CreateSwapChain(m_pd3dCommandQueue, &dxgiSwapChainDesc, (IDXGISwapChain **)&m_pdxgiSwapChain);
	
	m_pdxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);
	//“Alt+Enter” 키의 동작을 비활성화한다.

	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
	//스왑체인의 현재 후면버퍼 인덱스를 저장한다.
}

void CGameFramework::CreateDirect3DDevice()
{
#if defined(_DEBUG)
	D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void **)&m_pd3dDebugController);
	m_pd3dDebugController->EnableDebugLayer();
#endif
	::CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void **)&m_pdxgiFactory);
	//DXGI 팩토리를 생성한다.
	IDXGIAdapter1 *pd3dAdapter = NULL;
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_pdxgiFactory->EnumAdapters1(i, &pd3dAdapter); i++)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
		pd3dAdapter->GetDesc1(&dxgiAdapterDesc);
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
		if (SUCCEEDED(D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_12_0,
			_uuidof(ID3D12Device), (void **)&m_pd3dDevice))) break;
	}
	//모든 하드웨어 어댑터 대하여 특성 레벨 12.0을 지원하는 하드웨어 디바이스를 생성한다.
	if (!pd3dAdapter)
	{
		m_pdxgiFactory->EnumWarpAdapter(_uuidof(IDXGIFactory4), (void **)&pd3dAdapter);
		D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), (void**)&m_pd3dDevice);
	}
	//특성 레벨 12.0을 지원하는 하드웨어 디바이스를 생성할 수 없으면 WARP 디바이스를 생성한다.
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount = 4; //Msaa4x 다중 샘플링
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;
	m_pd3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;
	//디바이스가 지원하는 다중 샘플의 품질 수준을 확인한다.
	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false;
	//다중 샘플의 품질 수준이 1보다 크면 다중 샘플링을 활성화한다.
	m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&m_pd3dFence);
	//펜스를 생성하고 펜스 값을 0으로 설정한다.
	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	/*펜스와 동기화를 위한 이벤트 객체를 생성한다(이벤트 객체의 초기값을 FALSE이다). 이벤트가 실행되면(Signal) 이
	벤트의 값을 자동적으로 FALSE가 되도록 생성한다.*/
	//m_d3dViewport.TopLeftX = 0;
	//m_d3dViewport.TopLeftY = 0;
	//m_d3dViewport.Width = static_cast<float>(m_nWndClientWidth);
	//m_d3dViewport.Height = static_cast<float>(m_nWndClientHeight);
	//m_d3dViewport.MinDepth = 0.0f;
	//m_d3dViewport.MaxDepth = 1.0f;
	////뷰포트를 주 윈도우의 클라이언트 영역 전체로 설정한다.
	//m_d3dScissorRect = { 0, 0, m_nWndClientWidth, m_nWndClientHeight };
	////씨저 사각형을 주 윈도우의 클라이언트 영역 전체로 설정한다.
	if (pd3dAdapter) pd3dAdapter->Release();
}

void CGameFramework::CreateCommandQueueAndList()
{
	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	HRESULT hResult = m_pd3dDevice->CreateCommandQueue(&d3dCommandQueueDesc, _uuidof(ID3D12CommandQueue), (void **)&m_pd3dCommandQueue);
	//직접(Direct) 명령 큐를 생성한다.

	hResult = m_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void **)&m_pd3dCommandAllocator);
	//직접(Direct) 명령 할당자를 생성한다.

	hResult = m_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pd3dCommandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&m_pd3dCommandList);
	//직접(Direct) 명령 리스트를 생성한다.

	hResult = m_pd3dCommandList->Close();
	//명령 리스트는 생성되면 열린(Open) 상태이므로 닫힌(Closed) 상태로 만든다.
}

void CGameFramework::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.NumDescriptors = m_nSwapChainBuffers;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	HRESULT hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dRtvDescriptorHeap);
	//렌더 타겟 서술자 힙(서술자의 개수는 스왑체인 버퍼의 개수)을 생성한다.
	m_nRtvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//렌더 타겟 서술자 힙의 원소의 크기를 저장한다.

	d3dDescriptorHeapDesc.NumDescriptors = 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dDsvDescriptorHeap);
	//깊이-스텐실 서술자 힙(서술자의 개수는 1)을 생성한다.
	m_nDsvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	//깊이-스텐실 서술자 힙의 원소의 크기를 저장한다.

	::gnCbvSrvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void CGameFramework::recvCallBack()
{
	sc_packet_login p_login;
	sc_packet_pos p_pos;
	sc_packet_put_player p_put;
	sc_packet_remove p_remove;
	sc_packet_anim p_anim;
	sc_packet_weapon p_weapon;

	int retval;

	//while(m_pSocket == NULL);

	retval = recv(m_pSocket->clientSocket, m_pSocket->buf, MAX_PACKET_SIZE, 0);

	if (retval == 0 || retval == SOCKET_ERROR)
	{
		closesocket(m_pSocket->clientSocket);
	}

	char *ptr = m_pSocket->buf;
	//cout << "Packet Len : " << retval << endl;

	while (retval > 0)
	{
		char size = ptr[0];
		char type = ptr[1];

		//cout << "size : " << int(size) << "type : " << int(type) << endl;

		if (type == SC_LOGIN)
		{
			memcpy(&p_login, ptr, sizeof(p_login));

			My_ID = p_login.id;
			XMFLOAT3 pos(p_login.x, p_login.y, p_login.z);
			XMFLOAT3 vel(p_login.vx, p_login.vy, p_login.vz);
			//XMFLOAT3 look(0, 0, 1);

			m_pPlayer = m_pScene->getplayer(My_ID);//pPlayer;
			cout << "ID : " << My_ID << endl;
			m_pCamera = m_pScene->m_pPlayer[My_ID]->GetCamera();
			m_pScene->getplayer(My_ID)->SetPosition(pos);
			m_pScene->getplayer(My_ID)->SetVelocity(vel);
			//m_pScene->getplayer(My_ID)->SetLook(look);
			PxExtendedVec3 ControllerPos = PxExtendedVec3(pos.x, pos.y + 17.5, pos.z);
			m_pScene->getplayer(My_ID)->SetPhysController(m_pPhysx, m_pScene->getplayer(My_ID)->getCollisionCallback(), &ControllerPos);
			m_pScene->getplayer(My_ID)->ChangeAnimation(Anim_Idle);
			//m_pScene->getplayer(My_ID)->SetAnimFrame(0.0f);
			m_pScene->getplayer(My_ID)->EnableLoop();

			m_pScene->m_pPlayer[My_ID]->SetConnected(true);
			//m_pScene->getplayer(My_ID)->SetLook(XMFLOAT3(0, 0, 1));
			//m_pScene->m_pPlayer[My_ID]->SetLook(look);
			//cout << "캐릭터 위치 : " << pos.x << ", " << pos.y << ", " << pos.z << endl;
			//m_pScene->m_pPlayer[My_ID]->SetLookVector(look);
			//m_pScene->m_pPlayer[My_ID]->SetConnected(true);

			
			cout << "Client Login sucess\n";

			ptr += sizeof(p_login);
			retval -= sizeof(p_login);
		}
		if (type == SC_PUT_PLAYER)
		{
			memcpy(&p_put, ptr, sizeof(p_put));

			XMFLOAT3 pos(p_put.x, p_put.y, p_put.z);
			XMFLOAT3 vel(p_put.vx, p_put.vy, p_put.vz);

			cout << "Put Player : " << int(p_put.id) << endl;

			m_pScene->getplayer(p_put.id)->SetPosition(pos);
			m_pScene->getplayer(p_put.id)->SetVelocity(vel);
			PxExtendedVec3 ControllerPos = PxExtendedVec3(pos.x, pos.y + 17.5, pos.z);
			m_pScene->getplayer(p_put.id)->SetPhysController(m_pPhysx, m_pScene->getplayer(p_put.id)->getCollisionCallback(), &ControllerPos);
			m_pScene->getplayer(p_put.id)->ChangeAnimation(p_put.ani_index);
			m_pScene->getplayer(p_put.id)->SetAnimFrame(p_put.ani_frame);
			if (p_put.ani_index == Anim_Idle || p_put.ani_index == Anim_Walk || p_put.ani_index == Anim_Run) {
				m_pScene->getplayer(p_put.id)->EnableLoop();
			}
			else {
				m_pScene->getplayer(p_put.id)->DisableLoop();
			}
			m_pScene->m_pPlayer[p_put.id]->SetConnected(true);

			ptr += sizeof(p_put);
			retval -= sizeof(p_put);
		}
		if (type == SC_POS)
		{
			memcpy(&p_pos, m_pSocket->buf, sizeof(p_pos));

			XMFLOAT3 pos(p_pos.x, p_pos.y, p_pos.z);
			XMFLOAT3 vel(p_pos.vx, p_pos.vy, p_pos.vz);

			if (p_pos.size == sizeof(sc_packet_pos))
			{

				//m_pScene->m_pPlayer[p_pos.id]->SetPosition(position);
				m_pScene->getplayer(p_pos.id)->SetPosition(pos);
				m_pScene->getplayer(p_pos.id)->SetVelocity(vel);
				m_pScene->getplayer(p_pos.id)->SetLook(vel);

				m_pScene->getplayer(p_pos.id)->ChangeAnimation(p_pos.ani_index);
				m_pScene->getplayer(p_pos.id)->SetAnimFrame(p_pos.ani_frame);
				if (p_pos.ani_index == Anim_Idle || p_pos.ani_index == Anim_Walk || p_pos.ani_index == Anim_Run) {
					if (p_pos.ani_index == Anim_Run) cout << p_pos.ani_frame << endl;
					m_pScene->getplayer(p_pos.id)->EnableLoop();
				}
				else {
					m_pScene->getplayer(p_pos.id)->DisableLoop();
				}

				//cout << int(p_pos.id) << "Player SC_POS : " << pos.x << "," << pos.z << endl;
			}

			ptr += sizeof(p_pos);
			retval -= sizeof(p_pos);
		}
		if (type == SC_REMOVE) {
			memcpy(&p_remove, m_pSocket->buf, sizeof(p_remove));

			m_pScene->m_pPlayer[p_remove.id]->SetConnected(false);
			m_pScene->m_pPlayer[p_remove.id]->m_PlayerController->release(); m_pScene->m_pPlayer[p_remove.id]->m_PlayerController = NULL;
			//m_pScene->m_pPlayer[p_remove.id]->m_AttackTrigger->release();

			// 여러 Client Position 정보가 버퍼에 누적되어있을 수도 있으니 땡겨주자.
			ptr += sizeof(p_remove);
			retval -= sizeof(p_remove);
		}
		if (type == SC_ANIM) {
			memcpy(&p_anim, m_pSocket->buf, sizeof(p_anim));

			cout << int(p_anim.id) << "Player SC_ANIM : " << int(p_anim.ani_index) << ", " << int(p_anim.ani_frame) << endl;

			if (p_anim.ani_index >= Anim_Idle && p_anim.ani_index <= Anim_Run) {
				m_pScene->getplayer(p_anim.id)->ChangeAnimation(p_anim.ani_index);
				m_pScene->getplayer(p_anim.id)->SetAnimFrame(p_anim.ani_frame);
				m_pScene->getplayer(p_anim.id)->DisableLoop();

				if (m_pScene->getplayer(p_anim.id)->getAnimIndex() == Anim_Jump) {
					m_pScene->getplayer(p_anim.id)->jumpstart();
				}
			}
			//if (p_anim.ani_index == Anim_Idle)	m_pScene->getplayer(p_anim.id)->EnableLoop();
			//else m_pScene->getplayer(p_anim.id)->DisableLoop();
			ptr += sizeof(p_anim);
			retval -= sizeof(p_anim);
		}

		if(type == SC_WEAPON)
		{
			memcpy(&p_weapon, m_pSocket->buf, sizeof(p_weapon));

			m_pScene->m_pPlayer[p_weapon.id]->SetWeapon(true, p_weapon.weapon_type ,p_weapon.weapon_index);

			cout << p_weapon.id << " Player Weapon Success\n";
			// 여러 Client Position 정보가 버퍼에 누적되어있을 수도 있으니 땡겨주자.
			ptr += sizeof(p_weapon);
			retval -= sizeof(p_weapon);
		}
	}
	memset(m_pSocket->buf, 0, sizeof(MAX_PACKET_SIZE));
}

//스왑체인의 각 후면 버퍼에 대한 렌더 타겟 뷰를 생성한다.
void CGameFramework::CreateRenderTargetView()
{
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle =	m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	for (UINT i = 0; i < m_nSwapChainBuffers; i++)
	{
		HRESULT hResult = m_pdxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&m_ppd3dRenderTargetBuffers[i]);
		m_pd3dDevice->CreateRenderTargetView(m_ppd3dRenderTargetBuffers[i], NULL, d3dRtvCPUDescriptorHandle);
		d3dRtvCPUDescriptorHandle.ptr += m_nRtvDescriptorIncrementSize;
	}
}

void CGameFramework::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = m_nWndClientWidth;
	d3dResourceDesc.Height = m_nWndClientHeight;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dResourceDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	D3D12_HEAP_PROPERTIES d3dHeapProperties;
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));

	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapProperties.CreationNodeMask = 1;
	d3dHeapProperties.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE d3dClearValue;
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;
	m_pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue, __uuidof(ID3D12Resource), (void **)&m_pd3dDepthStencilBuffer);
	//깊이-스텐실 버퍼를 생성한다.

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, NULL, d3dDsvCPUDescriptorHandle);
	//깊이-스텐실 버퍼 뷰를 생성한다.
}

void CGameFramework::LoadModels()
{
	vector<pair<string, float>> character_animation;
	character_animation.emplace_back(make_pair("./resource/character/stay.FBX", 0));			// Anim_Idle
	character_animation.emplace_back(make_pair("./resource/character/walk.FBX", 0));			// Anim_Walk

	character_animation.emplace_back(make_pair("./resource/character/weak_attack_1.FBX", 0));	// Anim_Weak_Attack1
	character_animation.emplace_back(make_pair("./resource/character/weak_attack_2.FBX", 0));	// Anim_Weak_Attack2
	character_animation.emplace_back(make_pair("./resource/character/weak_attack_3.FBX", 0));	// Anim_Weak_Attack3

	character_animation.emplace_back(make_pair("./resource/character/hard_attack_1.FBX", 0));	// Anim_Hard_Attack1
	character_animation.emplace_back(make_pair("./resource/character/hard_attack_2.FBX", 0));	// Anim_Hard_Attack2
	
	character_animation.emplace_back(make_pair("./resource/character/guard.FBX", 0));			// Anim_Guard
	character_animation.emplace_back(make_pair("./resource/character/power_up.FBX", 0));		// Anim_PowerUp
	character_animation.emplace_back(make_pair("./resource/character/jump.FBX", 0));			// Anim_Jump

	character_animation.emplace_back(make_pair("./resource/character/lollipop_attack_1.FBX", 0));	// Anim_Lollipop_Attack1
	character_animation.emplace_back(make_pair("./resource/character/lollipop_attack_2.FBX", 0));	// Anim_Lollipop_Attack2
	character_animation.emplace_back(make_pair("./resource/character/lollipop_guard.FBX", 0));		// Anim_Lollipop_Guard
	character_animation.emplace_back(make_pair("./resource/character/lollipop_hard_attack.FBX", 0));// Anim_Lollipop_HardAttack
	character_animation.emplace_back(make_pair("./resource/character/lollipop_skill.FBX", 0));		// Anim_Lollipop_skill

	character_animation.emplace_back(make_pair("./resource/character/small_react.FBX", 0));		// Anim_Small_React
	character_animation.emplace_back(make_pair("./resource/character/pick_up.FBX", 0));			// Anim_Pick_up
	character_animation.emplace_back(make_pair("./resource/character/run.FBX", 0));				// Anim_Run

	character_animation.emplace_back(make_pair("./resource/character/cupcake_eat.FBX", 0));		// Anim_Cupckae_Eat
	   
	Character_Model = new Model_Animation("./resource/character/main_character.FBX", &character_animation);
	
	Map_Model[0] = new LoadModel("./resource/map/map_1.FBX", true);				// M_Map_1
	Map_Model[1] = new LoadModel("./resource/map/cotton_1.FBX", true);			// cotton_candy_cloud
	Map_Model[2] = new LoadModel("./resource/map/111.FBX", true);				// map_1_wall
	Map_Model[3] = new LoadModel("./resource/map/map_macaron.FBX", true);		// map_1_macaron
	
	Map_Model[4] = new LoadModel("./resource/map/map_oreo.FBX", true);			// map_2
	Map_Model[5] = new LoadModel("./resource/map/map_chocolate_bar.FBX", true);	// map_2_chocolate_bar

	Map_Model[6] = new LoadModel("./resource/map/map_3_cake_1.FBX", true);		// map_3
	Map_Model[7] = new LoadModel("./resource/map/map_3_cake_2.FBX", true);		// map_3_cake_2
	Map_Model[8] = new LoadModel("./resource/map/map_3_cake_3.FBX", true);		// map_3_cake_3
	Map_Model[9] = new LoadModel("./resource/map/map_3_stair.FBX", true);		// map_3_stair
	Map_Model[10] = new LoadModel("./resource/map/map_3_bridge.FBX", true);		// map_3_bridge
	Map_Model[11] = new LoadModel("./resource/map/map_3_cake_black.FBX", true); // map_3_cake_in_black

	weapon[0] = new LoadModel("./resource/weapon/lollipop.FBX", true);			// M_Weapon_Lollipop
	weapon[1] = new LoadModel("./resource/weapon/candy.FBX", true);				// M_Weapon_chupachupse
	weapon[2] = new LoadModel("./resource/weapon/pepero.FBX", true);			// M_Weapon_pepero
	weapon[3] = new LoadModel("./resource/weapon/chocolate.FBX", true);			// M_Weapon_chocolate
	weapon[4] = new LoadModel("./resource/weapon/cupcake.FBX", true);			// M_Weapon_cupcake
}

void CGameFramework::BuildObjects()
{
	m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);
	
	m_pScene = new CScene();

	m_pPhysx = new CPhysx();
	m_pPhysx->initPhysics();

	if (m_pScene) {
		//if (My_ID != -1)
		//	m_pScene->SetMyID(My_ID);
		m_pScene->SetCharacter(Character_Model);
		m_pScene->SetMap(Map_Model);
		m_pScene->SetWeapon(weapon);

		m_pScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList, m_pPhysx);
		m_pScene->BuildUI(m_pd3dDevice, m_pd3dCommandList);
	}
	if (SERVER_ON) {
		if (m_pCamera == nullptr) {
			m_pCamera = new CCamera();
			m_pCamera->InitCamera(m_pd3dDevice, m_pd3dCommandList);
		}
	}
	if (!SERVER_ON) {
		My_ID = 0;
		m_pPlayer = m_pScene->getplayer(My_ID);//pPlayer;
		cout << "ID : " << My_ID << endl;
		m_pCamera = m_pPlayer->GetCamera();
		XMFLOAT3 pos = XMFLOAT3(0, 0, 0);
		m_pPlayer->SetPosition(pos);
		PxVec3 triggerPos = PxVec3(100, 100, 100);
		m_pPlayer->m_AttackTrigger = m_pPhysx->getTrigger(triggerPos, XMFLOAT3(5, 5, 5));
		m_pPhysx->registerPlayer(m_pPlayer, 0);
		m_pPlayer->SetConnected(true);

		pos = XMFLOAT3(100, 10, 0);
		m_pScene->getplayer(1)->SetPosition(pos);
		m_pPhysx->registerPlayer(m_pScene->getplayer(1), 1);
		m_pScene->getplayer(1)->SetConnected(true);

		if (m_pPhysx)
		{
			PxRigidStatic* groundPlane = PxCreatePlane(*m_pPhysx->m_Physics, PxPlane(0, 1, 0, 0), *m_pPhysx->m_Material);
			m_pPhysx->m_Scene->addActor(*groundPlane);

			//PxBoxControllerDesc bdesc;
			//bdesc.position = PxExtendedVec3(0, 0, 0);
			//bdesc.material = m_pPhysx->m_Material;
			//bdesc.halfHeight = 10.f;
			//bdesc.halfSideExtent = 10.f;
			//bdesc.halfForwardExtent = 10.f;

			//PxCapsuleControllerDesc desc;
			//desc.height = 15.f;
			//desc.radius = 10.f;
			//desc.position = PxExtendedVec3(0, 17.5, 0);
			//desc.material = m_pPhysx->m_Material;
			PxExtendedVec3 position = PxExtendedVec3(0, 17.5, 0);

			m_pPlayer->m_PlayerController = m_pPhysx->getCapsuleController(position, m_pPlayer->getCollisionCallback());

			position = PxExtendedVec3(100, 27.5, 0);

			m_pScene->m_pPlayer[1]->m_PlayerController = m_pPhysx->getCapsuleController(position, m_pScene->m_pPlayer[1]->getCollisionCallback());

			//m_pPhysx->m_PlayerController = m_pPhysx->m_PlayerManager->createController(desc);

			//PxShape* boxshape = m_pPhysx->m_Physics->createShape(PxBoxGeometry(17.5, 17.5, 17.5), *(m_pPhysx->m_Material));
			//boxshape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
			//boxshape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
			//
			//PxRigidStatic* box = m_pPhysx->m_Physics->createRigidStatic(PxTransform(PxVec3(50, 17.5, 0)));
			//box->attachShape(*boxshape);
			//m_pPhysx->m_Scene->addActor(*box);

			//PxRigidStatic* box = PxCreateRigidStatic(*(m_pPhysx->m_Physics), PxTransform(PxVec3(50, 17.5, 0)), PxBoxGeometry(17.5, 17.5, 17.5), *(m_pPhysx->m_Material), 1.0f);
			//dynamic->setAngularDamping(0.5f);
			//dynamic->setLinearVelocity(PxVec3(0, 0, 0));

		}
	}

	m_pd3dCommandList->Close();
	ID3D12CommandList *ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);
	WaitForGpuComplete();
	if (m_pScene) m_pScene->ReleaseUploadBuffers();
	
	m_GameTimer.Reset();
}

void CGameFramework::ReleaseObjects()
{
	if (m_pScene) m_pScene->ReleaseObjects();
	if (m_pScene) delete m_pScene;
}

void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		//마우스 캡쳐를 하고 현재 마우스 위치를 가져온다.
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		//마우스 캡쳐를 해제한다.
		::ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
}

void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM	wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
			
		case VK_F1:
		case VK_F2:
		case VK_F3:
			//m_pCamera[My_ID] = m_pPlayer[My_ID]->ChangeCamera((DWORD)(wParam - VK_F1 + 1), m_GameTimer.GetTimeElapsed());
			//if (m_pPlayer) m_pPlayer->ChangeCamera((wParam - VK_F1 + 1), m_GameTimer.GetTimeElapsed());
			break;
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		case VK_RETURN:
			break;
		case VK_F8:
			break;
		case VK_F9:	// 윈도우 모드와 전체화면 모드의 전환을 처리한다.
		{
			BOOL bFullScreenState = FALSE;
			m_pdxgiSwapChain->GetFullscreenState(&bFullScreenState, NULL);
			m_pdxgiSwapChain->SetFullscreenState(!bFullScreenState, NULL);
			DXGI_MODE_DESC dxgiTargetParameters;
			dxgiTargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			dxgiTargetParameters.Width = m_nWndClientWidth;
			dxgiTargetParameters.Height = m_nWndClientHeight;
			dxgiTargetParameters.RefreshRate.Numerator = 60;
			dxgiTargetParameters.RefreshRate.Denominator = 1;
			dxgiTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			dxgiTargetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			m_pdxgiSwapChain->ResizeTarget(&dxgiTargetParameters);
			OnResizeBackBuffers();
			break;
		}
		default:
			break;
		}
		break;
	default:
		break;
	}
}

LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_ACTIVATE:
	{
		/*if (LOWORD(wParam) == WA_INACTIVE)
			m_GameTimer.Stop();
		else
			m_GameTimer.Start();*/
		break;
	}
	case WM_SIZE:
	{
		m_nWndClientWidth = LOWORD(lParam);
		m_nWndClientHeight = HIWORD(lParam);
		//윈도우의 크기가 변경되면 후면버퍼의 크기를 변경한다.
		OnResizeBackBuffers();
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return(0);
}

void CGameFramework::ProcessInput()
{
	static UCHAR pKeyBuffer[256];
	DWORD dwDirection = 0;
	/*키보드의 상태 정보를 반환한다. 화살표 키(‘→’, ‘←’, ‘↑’, ‘↓’)를 누르면 플레이어를 오른쪽/왼쪽(로컬 x-축), 앞/
	뒤(로컬 z-축)로 이동한다. ‘Page Up’과 ‘Page Down’ 키를 누르면 플레이어를 위/아래(로컬 y-축)로 이동한다.*/
	BOOL Key_A = FALSE;
	BOOL Key_S = FALSE;
	BOOL Key_D = FALSE;
	BOOL Key_F = FALSE;
	BOOL Key_LShift = FALSE;
	BOOL Key_Space = FALSE;

	// 0x0000 : 이전에 누른적이 없고, 호출 시점에도 눌려있지 않은 상태
	// 0x0001 : 이전에 누른적이 있고, 호출 시점에도 눌려있지 않은 상태
	// 0x8000 : 이전에 누른적이 없고, 호출 시점에는 눌려있는 상태
	// 0x8001 : 이전에 누른적이 있고, 호출 시점에도 눌려있는 상태

	if (SERVER_ON)
	{
		if (::GetFocus())
		{
			if (::GetAsyncKeyState(VK_SHIFT) & 0x8000 && !state[5]) {
				state[5] = true;
			}
			if (::GetAsyncKeyState(VK_SHIFT) == 0 && state[5]) {
				state[5] = false;
			}

			if (::GetAsyncKeyState(VK_UP) & 0x8000 && !state[0]) {
				//cout << "UP DOWN\n";
				if (state[5]) {
					cout << "UP Run DOWN\n";
					m_pSocket->sendPacket(CS_MOVE, CS_UP, 2, 0);
				}
				else {
					cout << "UP Walk DOWN\n";
					m_pSocket->sendPacket(CS_MOVE, CS_UP, 1, 0);
				}
				state[0] = true;
			}
			if (::GetAsyncKeyState(VK_DOWN) & 0x8000 && !state[1]) {
				//cout << "DOWN DOWN\n";
				if (state[5])
					m_pSocket->sendPacket(CS_MOVE, CS_DOWN, 2, 0);
				else
					m_pSocket->sendPacket(CS_MOVE, CS_DOWN, 1, 0);
				state[1] = true;
			}
			if (::GetAsyncKeyState(VK_LEFT) & 0x8000 && !state[2]) {
				//cout << "LEFT DOWN\n";
				if (state[5])
					m_pSocket->sendPacket(CS_MOVE, CS_LEFT, 2, 0);
				else
					m_pSocket->sendPacket(CS_MOVE, CS_LEFT, 1, 0);
				state[2] = true;
			}
			if (::GetAsyncKeyState(VK_RIGHT) & 0x8000 && !state[3]) {
				cout << "RIGHT DOWN\n";
				if (state[5])
					m_pSocket->sendPacket(CS_MOVE, CS_RIGHT, 2, 0);
				else
					m_pSocket->sendPacket(CS_MOVE, CS_RIGHT, 1, 0);
				state[3] = true;
			}

			if (::GetAsyncKeyState(VK_UP) == 0 && state[0]) {
				//cout << "UP UP\n";
				m_pSocket->sendPacket(CS_MOVE, CS_UP, 0, 0);
				state[0] = false;
			}
			if (::GetAsyncKeyState(VK_DOWN) == 0 && state[1]) {
				//cout << "DOWN UP\n";
				m_pSocket->sendPacket(CS_MOVE, CS_DOWN, 0, 0);
				state[1] = false;
			}
			if (::GetAsyncKeyState(VK_LEFT) == 0 && state[2]) {
				//cout << "LEFT UP\n";
				m_pSocket->sendPacket(CS_MOVE, CS_LEFT, 0, 0);
				state[2] = false;
			}
			if (::GetAsyncKeyState(VK_RIGHT) == 0 && state[3]) {
				cout << "RIGHT UP\n";
				m_pSocket->sendPacket(CS_MOVE, CS_RIGHT, 0, 0);
				state[3] = false;
			}

			if (::GetAsyncKeyState(0x41) & 0x8000 && !attackstate) {
				int type = m_pPlayer->Get_Weapon_type();	//무기 종류
				int index = m_pPlayer->Get_Weapon_index();  //무기 번호

				//cout << type << ", " << index << endl;

				if (!m_pPlayer->Get_Weapon_grab())
				{
					if (type != -1 && index != -1) {
						cout << "무기줍기 Send\n";
						m_pSocket->sendPacket(CS_WEAPON, type, index, 0);
					}
					else
					{
						cout << "무기 X 약공격 Send\n";
						m_pSocket->sendPacket(CS_ATTACK, CS_WEAK, 0, 0);
					}
				}
				else {
					cout << "무기 O 약공격 Send\n";
					m_pSocket->sendPacket(CS_ATTACK, CS_WEAK, 0, 0);
				}
				attackstate = true;
			}
			if (::GetAsyncKeyState(0x41) == 0 && attackstate) {
				//cout << "RIGHT UP\n";
				//m_pSocket->sendPacket(CS_MOVE, CS_RIGHT, 0, 0);
				attackstate = false;
			}

			if (::GetAsyncKeyState(0x53) & 0x8000 && !attackstate2) {
				cout << "강공격 Send\n";
				m_pSocket->sendPacket(CS_ATTACK, CS_HARD, 0, 0);
				attackstate2 = true;
			}
			if (::GetAsyncKeyState(0x53) == 0 && attackstate2) {
				//cout << "RIGHT UP\n";
				//m_pSocket->sendPacket(CS_MOVE, CS_RIGHT, 0, 0);
				attackstate2 = false;
			}

			if (::GetAsyncKeyState(VK_SPACE) & 0x8000 && !jumpstate) {
				cout << "점프 Send\n";
				if(m_pPlayer->m_Jump.mJump == false)
					m_pSocket->sendPacket(CS_ATTACK, CS_JUMP, 0, 0);
				jumpstate = true;
			}
			if (::GetAsyncKeyState(VK_SPACE) == 0 && jumpstate) {
				//cout << "RIGHT UP\n";
				//m_pSocket->sendPacket(CS_MOVE, CS_RIGHT, 0, 0);
				jumpstate = false;
			}

			//if (::GetKeyboardState(pKeyBuffer))
			//{
			//	if (pKeyBuffer['A'] & 0xF0) Key_A = TRUE;
			//	if (pKeyBuffer['S'] & 0xF0) Key_S = TRUE;
			//	if (pKeyBuffer['D'] & 0xF0) Key_D = TRUE;
			//	if (pKeyBuffer['F'] & 0xF0) Key_F = TRUE;
			//	if (pKeyBuffer[VK_LSHIFT] & 0xF0) Key_LShift = TRUE;
			//	if (pKeyBuffer[VK_SPACE] & 0xF0) Key_Space = TRUE;
			//
			//	if (Key_A || Key_S) {
			//
			//		if (Key_A&&Key_S) {
			//			cout << "막기 Send\n";
			//			m_pSocket->sendPacket(CS_ATTACK, CS_GUARD, 0, 0);
			//		}
			//		else if (Key_A && !Key_S) { //약공격 or 줍기
			//			cout << "약공격 Send\n";
			//			m_pSocket->sendPacket(CS_ATTACK, CS_WEAK, 0, 0);
			//		}
			//		else if (!Key_A && Key_S) { //강공격 or 줍기
			//			cout << "강공격 Send\n";
			//			m_pSocket->sendPacket(CS_ATTACK, CS_HARD, 0, 0);
			//		}
			//	}
			//}
		}
	}

	else
	{
	int type = m_pPlayer->Get_Weapon_type();
	int index = m_pPlayer->Get_Weapon_index();

		if (::GetKeyboardState(pKeyBuffer))
		{
			if (pKeyBuffer[VK_UP] & 0xF0) dwDirection |= DIR_FORWARD;
			if (pKeyBuffer[VK_DOWN] & 0xF0) dwDirection |= DIR_BACKWARD;
			if (pKeyBuffer[VK_LEFT] & 0xF0) dwDirection |= DIR_LEFT;
			if (pKeyBuffer[VK_RIGHT] & 0xF0) dwDirection |= DIR_RIGHT;
			if (pKeyBuffer[VK_PRIOR] & 0xF0) dwDirection |= DIR_UP;
			if (pKeyBuffer[VK_NEXT] & 0xF0) dwDirection |= DIR_DOWN;
			if (pKeyBuffer['A'] & 0xF0) Key_A = TRUE;
			if (pKeyBuffer['S'] & 0xF0) Key_S = TRUE;
			if (pKeyBuffer['D'] & 0xF0) Key_D = TRUE;
			if (pKeyBuffer['F'] & 0xF0) Key_F = TRUE;
			if (pKeyBuffer[VK_LSHIFT] & 0xF0) Key_LShift = TRUE;
			if (pKeyBuffer[VK_SPACE] & 0xF0) Key_Space = TRUE;


		}

		float rotation = 0.0f;
		POINT ptCursorPos;
		/*마우스를 캡쳐했으면 마우스가 얼마만큼 이동하였는 가를 계산한다. 마우스 왼쪽 또는 오른쪽 버튼이 눌러질 때의
		메시지(WM_LBUTTONDOWN, WM_RBUTTONDOWN)를 처리할 때 마우스를 캡쳐하였다. 그러므로 마우스가 캡쳐된
		것은 마우스 버튼이 눌려진 상태를 의미한다. 마우스 버튼이 눌려진 상태에서 마우스를 좌우 또는 상하로 움직이면 플
		레이어를 x-축 또는 y-축으로 회전한다.*/

		int Anim_Index = m_pPlayer->getAnimIndex();
		float Anim_Time = m_pPlayer->getAnimtime();

		//cout << Anim_Index << " , " << Anim_Time << endl;

		m_pPlayer->m_AttackTrigger->setGlobalPose(PxTransform(100, 100, 100));

		if (m_pPlayer->GetScaleflag()) {
			if (Anim_Index == Anim_Cupckae_Eat && (Anim_Time >= 0 && Anim_Time < 20))
				m_pPlayer->SetScale(1.0f);
			else if (Anim_Index == Anim_Cupckae_Eat && (Anim_Time >= 20 && Anim_Time < 30)) {
				m_pPlayer->SetWeapon(false, -1, -1);
				m_pPlayer->SetScale(1.3f);
			}
			else if (Anim_Index == Anim_Cupckae_Eat && (Anim_Time >= 30 && Anim_Time < 38))
				m_pPlayer->SetScale(1.7f);
			else if (Anim_Index == Anim_Cupckae_Eat && (Anim_Time >= 38 && Anim_Time < 40))
				m_pPlayer->SetScale(2.0f);
			else if (Anim_Index != Anim_Cupckae_Eat) {
				m_pPlayer->SetScale(2.0f);

				if (m_pPlayer->Scale_time < 100)
					m_pPlayer->Scale_time += m_pPlayer->Scale_time * m_GameTimer.GetTimeElapsed();
				else {
					m_pPlayer->SetScaleflag(false);
					m_pPlayer->Scale_time = 0.3f;
				}
			}
		}
		else m_pPlayer->SetScale(1.0f);
		
		if (m_pScene->ready_state != UI_NONE) {
			if (m_pScene->ready_state == UI_READY) {
				if (m_pScene->ready_state_test < 10)
					m_pScene->ready_state_test += m_pScene->ready_state_test * m_GameTimer.GetTimeElapsed();
				else {
					m_pScene->ready_state = UI_FIGHT;
					m_pScene->ready_state_test = 0.3f;
				}
			}
			if (m_pScene->ready_state == UI_FIGHT) {
				if (m_pScene->ready_state_test < 2)
					m_pScene->ready_state_test += m_pScene->ready_state_test * m_GameTimer.GetTimeElapsed();
				else {
					m_pScene->ready_state = UI_NONE;
					m_pScene->ready_state_test = 0.3f;
				}
			}
		}

		if (m_pPlayer->Get_Weapon_grab()) {
			if (type == M_Weapon_Lollipop) {
				if (Key_A || Key_S) {

					if (Key_A&&Key_S) {//둘다 눌리면 막기
						m_pPlayer->ChangeAnimation(Anim_Lollipop_Guard);
						m_pPlayer->SetAnimFrame(10);
						m_pPlayer->DisableLoop();
					}
					else if (Key_A && !Key_S) { //약공격 or 줍기
					   //충돌체크 (무기 오브젝트랑) 충돌이면 줍기      

					   //아니면 약공격
						if (Anim_Index == Anim_Idle || Anim_Index == Anim_Walk) {
							m_pPlayer->ChangeAnimation(Anim_Lollipop_Attack1);
							m_pPlayer->DisableLoop();
						}
						if (Anim_Index == Anim_Lollipop_Attack1 && (Anim_Time > 10 && Anim_Time < 20)) {
							m_pPlayer->ChangeAnimation(Anim_Lollipop_Attack2);
							m_pPlayer->SetAnimFrame(Anim_Time);
							m_pPlayer->DisableLoop();
						}
					}
					else if (!Key_A && Key_S) { //강공격 or 줍기
					   //충돌체크 (무기 오브젝트랑) 충돌이면 줍기

					   //아니면 강공격
						if (Anim_Index == Anim_Idle || Anim_Index == Anim_Walk) {
							m_pPlayer->ChangeAnimation(Anim_Lollipop_Hard_Attack);
							m_pPlayer->DisableLoop();
						}

					}
				}

				if (Key_D) { //무기 스킬
				//무기 번호가 WEAPON_EMPTY 가 아니면 스킬사용
					if (Anim_Index == Anim_Idle || Anim_Index == Anim_Walk) {
						m_pPlayer->ChangeAnimation(Anim_Lollipop_Skill);
						m_pPlayer->DisableLoop();
					}
				}
			}
			else if (type == M_Weapon_chupachupse) {
				if (Key_A || Key_S) {

					if (Key_A&&Key_S) {//둘다 눌리면 막기
						m_pPlayer->ChangeAnimation(Anim_Lollipop_Guard);
						m_pPlayer->SetAnimFrame(10);
						m_pPlayer->DisableLoop();
					}
					else if (Key_A && !Key_S) { //약공격 or 줍기
					   //충돌체크 (무기 오브젝트랑) 충돌이면 줍기      

					   //아니면 약공격
						if (Anim_Index == Anim_Idle || Anim_Index == Anim_Walk) {
							m_pPlayer->ChangeAnimation(Anim_Lollipop_Attack1);
							m_pPlayer->DisableLoop();
						}
						if (Anim_Index == Anim_Lollipop_Attack1 && (Anim_Time > 10 && Anim_Time < 20)) {
							m_pPlayer->ChangeAnimation(Anim_Lollipop_Attack2);
							m_pPlayer->SetAnimFrame(Anim_Time);
							m_pPlayer->DisableLoop();
						}
					}
					else if (!Key_A && Key_S) { //강공격 or 줍기
					   //충돌체크 (무기 오브젝트랑) 충돌이면 줍기

					   //아니면 강공격
						if (Anim_Index == Anim_Idle || Anim_Index == Anim_Walk) {
							m_pPlayer->ChangeAnimation(Anim_Lollipop_Hard_Attack);
							m_pPlayer->DisableLoop();
						}

					}
				}

				if (Key_D) { //무기 스킬
				//무기 번호가 WEAPON_EMPTY 가 아니면 스킬사용
					if (Anim_Index == Anim_Idle || Anim_Index == Anim_Walk) {
						m_pPlayer->ChangeAnimation(Anim_Lollipop_Skill);
						m_pPlayer->DisableLoop();
					}
				}
			}
			else if (type == M_Weapon_pepero) {
				if (Key_A || Key_S) {

					if (Key_A&&Key_S) {//둘다 눌리면 막기
						m_pPlayer->ChangeAnimation(Anim_Lollipop_Guard);
						m_pPlayer->SetAnimFrame(10);
						m_pPlayer->DisableLoop();
					}
					else if (Key_A && !Key_S) { //약공격 or 줍기
					   //충돌체크 (무기 오브젝트랑) 충돌이면 줍기      

					   //아니면 약공격
						if (Anim_Index == Anim_Idle || Anim_Index == Anim_Walk) {
							m_pPlayer->ChangeAnimation(Anim_Lollipop_Attack1);
							m_pPlayer->DisableLoop();
						}
						if (Anim_Index == Anim_Lollipop_Attack1 && (Anim_Time > 10 && Anim_Time < 20)) {
							m_pPlayer->ChangeAnimation(Anim_Lollipop_Attack2);
							m_pPlayer->SetAnimFrame(Anim_Time);
							m_pPlayer->DisableLoop();
						}
					}
					else if (!Key_A && Key_S) { //강공격 or 줍기
					   //충돌체크 (무기 오브젝트랑) 충돌이면 줍기

					   //아니면 강공격
						if (Anim_Index == Anim_Idle || Anim_Index == Anim_Walk) {
							m_pPlayer->ChangeAnimation(Anim_Lollipop_Hard_Attack);
							m_pPlayer->DisableLoop();
						}

					}
				}

				if (Key_D) { //무기 스킬
				//무기 번호가 WEAPON_EMPTY 가 아니면 스킬사용
					if (Anim_Index == Anim_Idle || Anim_Index == Anim_Walk) {
						m_pPlayer->ChangeAnimation(Anim_Lollipop_Skill);
						m_pPlayer->DisableLoop();
					}
				}
			}
			else if (type == M_Weapon_chocolate) {
				if (Key_A || Key_S) {

					if (Key_A&&Key_S) {//둘다 눌리면 막기
						m_pPlayer->ChangeAnimation(Anim_Lollipop_Guard);
						m_pPlayer->SetAnimFrame(10);
						m_pPlayer->DisableLoop();
					}
					else if (Key_A && !Key_S) { //약공격 or 줍기
					   //충돌체크 (무기 오브젝트랑) 충돌이면 줍기      

					   //아니면 약공격
						if (Anim_Index == Anim_Idle || Anim_Index == Anim_Walk) {
							m_pPlayer->ChangeAnimation(Anim_Lollipop_Attack1);
							m_pPlayer->DisableLoop();
						}
						if (Anim_Index == Anim_Lollipop_Attack1 && (Anim_Time > 10 && Anim_Time < 20)) {
							m_pPlayer->ChangeAnimation(Anim_Lollipop_Attack2);
							m_pPlayer->SetAnimFrame(Anim_Time);
							m_pPlayer->DisableLoop();
						}
					}
					else if (!Key_A && Key_S) { //강공격 or 줍기
					   //충돌체크 (무기 오브젝트랑) 충돌이면 줍기

					   //아니면 강공격
						if (Anim_Index == Anim_Idle || Anim_Index == Anim_Walk) {
							m_pPlayer->ChangeAnimation(Anim_Lollipop_Hard_Attack);
							m_pPlayer->DisableLoop();
						}

					}
				}

				if (Key_D) { //무기 스킬
				//무기 번호가 WEAPON_EMPTY 가 아니면 스킬사용
					if (Anim_Index == Anim_Idle || Anim_Index == Anim_Walk) {
						m_pPlayer->ChangeAnimation(Anim_Lollipop_Skill);
						m_pPlayer->DisableLoop();
					}
				}
			}
			else if (type == M_Weapon_cupcake) {
				if (Key_A || Key_S) {

					if (Key_A&&Key_S) {//둘다 눌리면 막기
						m_pPlayer->ChangeAnimation(Anim_Lollipop_Guard);
						m_pPlayer->SetAnimFrame(10);
						m_pPlayer->DisableLoop();
					}
					else if (Key_A && !Key_S) { //약공격 or 줍기
					   //충돌체크 (무기 오브젝트랑) 충돌이면 줍기      

					   //아니면 약공격
						if (Anim_Index == Anim_Idle || Anim_Index == Anim_Walk) {
							m_pPlayer->ChangeAnimation(Anim_Lollipop_Attack1);
							m_pPlayer->DisableLoop();
						}
						if (Anim_Index == Anim_Lollipop_Attack1 && (Anim_Time > 10 && Anim_Time < 20)) {
							m_pPlayer->ChangeAnimation(Anim_Lollipop_Attack2);
							m_pPlayer->SetAnimFrame(Anim_Time);
							m_pPlayer->DisableLoop();
						}
					}
					else if (!Key_A && Key_S) { //강공격 or 줍기
					   //충돌체크 (무기 오브젝트랑) 충돌이면 줍기

					   //아니면 강공격
						if (Anim_Index == Anim_Idle || Anim_Index == Anim_Walk) {
							m_pPlayer->ChangeAnimation(Anim_Lollipop_Hard_Attack);
							m_pPlayer->DisableLoop();
						}

					}
				}

				if (Key_D) { //무기 스킬
				//무기 번호가 WEAPON_EMPTY 가 아니면 스킬사용
					if (Anim_Index == Anim_Idle || Anim_Index == Anim_Walk) {
						m_pPlayer->ChangeAnimation(Anim_Cupckae_Eat);
						m_pPlayer->SetScaleflag(true);
						m_pPlayer->DisableLoop();
					}
				}
			}
		}
		else {
			if (type != -1 && index != -1) {
				if(Anim_Index==Anim_Pick_up)
				if (Anim_Time >= 20 && Anim_Time <= 21)
					m_pPlayer->SetWeapon(true, type, index);
			}

			if (Key_A || Key_S) {

				if (Key_A&&Key_S) {//둘다 눌리면 막기
					m_pPlayer->ChangeAnimation(Anim_Guard);
					m_pPlayer->SetAnimFrame(10);
					m_pPlayer->DisableLoop();
				}
				else if (Key_A && !Key_S) { //약공격 or 줍기
				   //충돌체크 (무기 오브젝트랑) 충돌이면 줍기      

					

					if (type != -1 && index != -1) {
						m_pPlayer->ChangeAnimation(Anim_Pick_up);
						m_pPlayer->DisableLoop();
					//	m_pPlayer->SetWeapon(true, type, index);
						Anim_Index = m_pPlayer->getAnimIndex();
					}

				   //아니면 약공격
					if (Anim_Index == Anim_Idle || Anim_Index == Anim_Walk) {
						m_pPlayer->ChangeAnimation(Anim_Weak_Attack1);
						m_pPlayer->DisableLoop();
					}
					if (Anim_Index == Anim_Weak_Attack1 && (Anim_Time > 10 && Anim_Time < 15)) {
						m_pPlayer->ChangeAnimation(Anim_Weak_Attack2);
						m_pPlayer->SetAnimFrame(Anim_Time);
						m_pPlayer->DisableLoop();
					}
					if (Anim_Index == Anim_Weak_Attack2 && (Anim_Time > 20 && Anim_Time < 25)) {
						m_pPlayer->ChangeAnimation(Anim_Weak_Attack3);
						m_pPlayer->SetAnimFrame(Anim_Time);
						m_pPlayer->DisableLoop();
					}
				}
				else if (!Key_A && Key_S) { //강공격 or 줍기
				   //충돌체크 (무기 오브젝트랑) 충돌이면 줍기

				   //아니면 강공격
					if (Anim_Index == Anim_Idle || Anim_Index == Anim_Walk) {
						m_pPlayer->ChangeAnimation(Anim_Hard_Attack1);
						m_pPlayer->DisableLoop();
					}
					if (Anim_Index == Anim_Hard_Attack1 && (Anim_Time > 10 && Anim_Time < 20)) {
						m_pPlayer->ChangeAnimation(Anim_Hard_Attack2);
						m_pPlayer->SetAnimFrame(Anim_Time);
						m_pPlayer->DisableLoop();
					}
				}
			}

			if (Anim_Index == Anim_Weak_Attack1 || Anim_Index == Anim_Weak_Attack2 || Anim_Index == Anim_Weak_Attack3) {
				if ((Anim_Time > 10 && Anim_Time < 15) || (Anim_Time > 22 && Anim_Time < 27) || (Anim_Time > 32 && Anim_Time < 37))
				{
					PxTransform triggerpos(PxVec3(0, 0, 0));
					PxExtendedVec3 playerpos = m_pPlayer->m_PlayerController->getPosition();
					XMFLOAT3 look = m_pPlayer->GetLookVector();
					triggerpos.p.x = playerpos.x + (look.x * 10);
					triggerpos.p.y = playerpos.y + (look.y * 10);
					triggerpos.p.z = playerpos.z + (look.z * 10);

					//cout << "Ani time : " << Anim_Time << endl;
					//cout << "Trigger Pos : " << triggerpos.p.x << ", " << triggerpos.p.y << ", " << triggerpos.p.z << endl;
					m_pPlayer->m_AttackTrigger->setGlobalPose(triggerpos);
				}
			}
		}

		if (Key_F) { //강화 스킬
			if (Anim_Index == Anim_Idle || Anim_Index == Anim_Walk) {      //강화 게이지(?) 플래그가 있어야함
				m_pPlayer->ChangeAnimation(Anim_PowerUp);
				m_pPlayer->DisableLoop();

			}
		}

		if (Key_Space) { //점프
			if (Anim_Index == Anim_Idle || Anim_Index == Anim_Walk) {
				m_pPlayer->ChangeAnimation(Anim_Jump);
				m_pPlayer->DisableLoop();
				m_pPlayer->jumpstart();
				m_pPlayer->SetWeapon(false, -1,-1);
			}
		}
		if (Key_LShift) { //구르기


		}
		//if (d_move) return;
		if (dwDirection)
		{
			if (Anim_Index == Anim_Idle) {
				//float Player_Yaw = m_pPlayer->GetYaw();
				if (dwDirection & DIR_FORWARD)
					m_pPlayer->ChangeAnimation(Anim_Walk);
				if (dwDirection & DIR_BACKWARD)
					m_pPlayer->ChangeAnimation(Anim_Walk);
				if (dwDirection & DIR_LEFT)
					m_pPlayer->ChangeAnimation(Anim_Walk);
				if (dwDirection & DIR_RIGHT)
					m_pPlayer->ChangeAnimation(Anim_Walk);
			}
		}
		else if (Anim_Index == Anim_Walk) {
			m_pPlayer->ChangeAnimation(Anim_Idle);
			m_pPlayer->EnableLoop();
		}




		if (!SERVER_ON)
		{
			//m_pPlayer->Move(dwDirection, 100.0f * m_GameTimer.GetTimeElapsed(), true);
			//m_pPhysx->move(dwDirection, 20.0f * m_GameTimer.GetTimeElapsed());
			XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
			XMFLOAT3 xmf3Direction = XMFLOAT3(0, 0, 0);
			float distance = 20.0f * m_GameTimer.GetTimeElapsed();
			//XMFLOAT3 Look = XMFLOAT3(0, 0, 0);

			if (dwDirection & DIR_FORWARD) {
				//	Look.z = -1.f;
				//	xmf3Direction = Look;
				xmf3Direction.z = 1.f;
				xmf3Shift = Vector3::Add(xmf3Shift, xmf3Direction, distance);
			}
			if (dwDirection & DIR_BACKWARD) {
				//	Look.z = 1.f;
				//	xmf3Direction = Look;
				xmf3Direction.z = -1.f;
				xmf3Shift = Vector3::Add(xmf3Shift, xmf3Direction, distance);
			}
			//화살표 키 ‘→’를 누르면 로컬 x-축 방향으로 이동한다. ‘←’를 누르면 반대 방향으로 이동한다.
			if (dwDirection & DIR_RIGHT) {
				//	Look.x = -1.f;
				//	xmf3Direction = Look;
				xmf3Direction.x = 1.f;
				xmf3Shift = Vector3::Add(xmf3Shift, xmf3Direction, distance);
			}
			if (dwDirection & DIR_LEFT) {
				//	Look.x = 1.f;
				//	xmf3Direction = Look;
				xmf3Direction.x = -1.f;
				xmf3Shift = Vector3::Add(xmf3Shift, xmf3Direction, distance);
			}

			PxVec3 disp;
			disp.x = xmf3Shift.x;
			disp.y = xmf3Shift.y;
			disp.z = xmf3Shift.z;

			float jumpheight;
			jumpheight = m_pPlayer->m_Jump.getHeight(m_GameTimer.GetTimeElapsed());
			//cout << "점프 : " << jumpheight << endl;
			disp.y += jumpheight;

			PxControllerFilters filters;

			const PxU32 flags = m_pPlayer->m_PlayerController->move(disp, 0, 1 / 60, filters);
			if (flags & PxControllerCollisionFlag::eCOLLISION_DOWN)
			{
				//cout << "충돌\n";
				m_pPlayer->m_Jump.stopJump();
			}

			XMFLOAT3 Look = m_pPlayer->GetLook();
			//XMFLOAT3 Right = m_pPlayer->GetRight();
			if (dwDirection & DIR_FORWARD) {
				Look.z = 1.f;
				//   Right.x = -1.f;
			}
			if (dwDirection & DIR_BACKWARD) {
				Look.z = -1.f;
				//   Right.x = 1.f;
			}
			if (dwDirection & DIR_RIGHT) {
				Look.x = 1.f;
				//   Right.z = -1.f;
			}
			if (dwDirection & DIR_LEFT) {
				Look.x = -1.f;
				//   Right.z = 1.f;
			}
			Look = Vector3::Normalize(Look);
			//Right = Vector3::Normalize(Right);
			m_pPlayer->SetLook(Look);
		}


		//플레이어를 실제로 이동하고 카메라를 갱신한다. 중력과 마찰력의 영향을 속도 벡터에 적용한다.
		//if(m_pCamera)
		//m_pPlayer->Update(m_GameTimer.GetTimeElapsed());
	}
}

void CGameFramework::AnimateObjects()
{
	if (m_pScene) m_pScene->AnimateObjects(m_GameTimer.GetTimeElapsed());
}

void CGameFramework::UpdateProcess()
{
	if (SERVER_ON)
	{
		//m_pPlayer->Move(dwDirection, 100.0f * m_GameTimer.GetTimeElapsed(), true);
		//m_pPhysx->move(dwDirection, 20.0f * m_GameTimer.GetTimeElapsed());
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (m_pScene->m_pPlayer[i]) {
				if (m_pScene->m_pPlayer[i]->GetConnected())
				{
					PxExtendedVec3 position;
					XMFLOAT3 pos = m_pScene->m_pPlayer[i]->GetPosition();
					position.x = pos.x;
					position.y = pos.y + 17.5;
					position.z = pos.z;
					m_pScene->m_pPlayer[i]->m_PlayerController->setPosition(position);

					float jumpheight;
					jumpheight = m_pScene->m_pPlayer[i]->m_Jump.getHeight(m_GameTimer.GetTimeElapsed());

					if (jumpheight == 0.0f) {
						jumpheight = -9.81 * m_GameTimer.GetTimeElapsed();
					}

					PxVec3 disp;
					XMFLOAT3 xmf_disp = m_pScene->m_pPlayer[i]->GetVelocity();
					//cout << "Vel : " << xmf_disp.x << ", " << xmf_disp.y << ", " << xmf_disp.z << endl;
					disp.x = 20.0f * m_GameTimer.GetTimeElapsed() * xmf_disp.x;
					disp.y = 20.0f * m_GameTimer.GetTimeElapsed() * xmf_disp.y;
					disp.z = 20.0f * m_GameTimer.GetTimeElapsed() * xmf_disp.z;

					disp.y += jumpheight;

					PxControllerFilters filters;
					const PxU32 flags = m_pScene->m_pPlayer[i]->m_PlayerController->move(disp, 0.001, 1 / 60, filters);
					if (flags & PxControllerCollisionFlag::eCOLLISION_DOWN)
					{
						//cout << "충돌\n";
						m_pScene->m_pPlayer[i]->m_Jump.stopJump();
					}

					//dwDirection, 20.0f * m_GameTimer.GetTimeElapsed()
				}
			}
		}
	}
}

void CGameFramework::CollisionProcess()
{
	if (m_pScene) {
		if (My_ID != -1) {
			if (m_pScene->m_pPlayer[My_ID]) {
				if (m_pScene->m_pPlayer[My_ID]->GetConnected()) {
					if (!m_pScene->m_pPlayer[My_ID]->Get_Weapon_grab())
						m_pScene->CollisionProcess(My_ID);
				}
			}
		}
	}
}

void CGameFramework::WaitForGpuComplete()
{
	UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);
	//GPU가 펜스의 값을 설정하는 명령을 명령 큐에 추가한다.

	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		//펜스의 현재 값이 설정한 값보다 작으면 펜스의 현재 값이 설정한 값이 될 때까지 기다린다.
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void CGameFramework::SetIDIP(wchar_t * id, wchar_t * ip)
{
	m_pid = ConvertWCtoC(id);
	m_pip = ConvertWCtoC(ip);
}

void CGameFramework::MoveToNextFrame()
{
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
	UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);
	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

//#define _WITH_PLAYER_TOP
void CGameFramework::FrameAdvance()
{
	m_GameTimer.Tick(60.0f);
	
	ProcessInput();
	UpdateProcess();
	
	if (SERVER_ON) {
		m_pPhysx->m_Scene->simulate(1.f / 60.f);
		m_pPhysx->m_Scene->fetchResults(true);

		//physx::PxExtendedVec3 playerPosition;
		//playerPosition = m_pPhysx->m_PlayerController->getPosition();
		//XMFLOAT3 position = m_pPlayer->GetPosition();;
		//position.x = playerPosition.x;
		//position.y = playerPosition.y - 17.5f;
		//position.z = playerPosition.z;
		//m_pCamera->SetPosition(Vector3::Add(position, m_pCamera->GetOffset()));
		//m_pCamera->SetLookAt(position);
		//m_pPlayer->SetPosition(position);
		for (int i = 0; i < MAX_USER; i++)
		{
			if (m_pScene->m_pPlayer[i]) {
				if (m_pScene->m_pPlayer[i]->GetConnected())
				{
					PxExtendedVec3 playerPosition = m_pScene->m_pPlayer[i]->m_PlayerController->getPosition();
					XMFLOAT3 position;
					position.x = playerPosition.x;
					position.y = playerPosition.y - 17.5f;
					position.z = playerPosition.z;
					//m_pCamera->SetPosition(Vector3::Add(position, m_pCamera->GetOffset()));
					//m_pCamera->SetLookAt(position);
					m_pScene->m_pPlayer[i]->SetPosition(position);
				}
			}
		}
		if (m_pPlayer) {
			if (m_pPlayer->GetConnected())
			{
				XMFLOAT3 position;
				position.x = m_pPlayer->GetPosition().x;
				position.y = m_pPlayer->GetPosition().y;
				position.z = m_pPlayer->GetPosition().z;
				if (position.y < 0) position.y = 0;
				m_pCamera->SetPosition(Vector3::Add(position, m_pCamera->GetOffset()));
				m_pCamera->SetLookAt(position);
			}
		}
		//cout << "캐릭터 위치 : " << position.x << ", " << position.y << ", " << position.z << endl;

	}
	else {
		m_pPhysx->m_Scene->simulate(1.f / 60.f);
		m_pPhysx->m_Scene->fetchResults(1.f / 60.f);

		physx::PxExtendedVec3 playerPosition;
		playerPosition = m_pPlayer->m_PlayerController->getPosition();
		XMFLOAT3 position;
		position.x = playerPosition.x;
		position.y = playerPosition.y - 17.5f;
		position.z = playerPosition.z;
		m_pCamera->SetPosition(Vector3::Add(position, m_pCamera->GetOffset()));
		m_pCamera->SetLookAt(position);
		m_pPlayer->SetPosition(position);

		//cout << "0 캐릭터 위치 : " << position.x << ", " << position.y << ", " << position.z << endl;

		playerPosition = m_pScene->m_pPlayer[1]->m_PlayerController->getPosition();
		position.x = playerPosition.x;
		position.y = playerPosition.y - 17.5f;
		position.z = playerPosition.z;
		//cout << "1 캐릭터 위치 : " << position.x << ", " << position.y << ", " << position.z << endl;
		//position = m_pCamera->GetPosition();
		//cout << "카메라 위치 : " << position.x << ", " << position.y << ", " << position.z << endl;;
	}
	
	
	CollisionProcess();

	AnimateObjects();
	
	HRESULT hResult = m_pd3dCommandAllocator->Reset();
	hResult = m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);
	
	D3D12_RESOURCE_BARRIER d3dResourceBarrier;
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier.Transition.pResource = m_ppd3dRenderTargetBuffers[m_nSwapChainBufferIndex];
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
	
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle =	m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	d3dRtvCPUDescriptorHandle.ptr += (m_nSwapChainBufferIndex *	m_nRtvDescriptorIncrementSize);
	
	float pfClearColor[4] = { 1.0f, 0.8f, 0.8f, 1.0f };//{ 0.0f, 0.125f, 0.3f, 1.0f };
	//원하는 색상으로 렌더 타겟(뷰)을 지운다.
	m_pd3dCommandList->ClearRenderTargetView(d3dRtvCPUDescriptorHandle,	pfClearColor/*Colors::Azure*/, 0, NULL);
	
	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle =	m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle,	D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
	
	m_pd3dCommandList->OMSetRenderTargets(1, &d3dRtvCPUDescriptorHandle, TRUE, &d3dDsvCPUDescriptorHandle);
	
	if (m_pScene) {
		m_pScene->Render(m_pd3dCommandList, m_pCamera);
		m_pScene->RenderUI(m_pd3dDevice, m_pd3dCommandList);
	}
	//3인칭 카메라일 때 플레이어가 항상 보이도록 렌더링한다.
#ifdef _WITH_PLAYER_TOP
	//렌더 타겟은 그대로 두고 깊이 버퍼를 1.0으로 지우고 플레이어를 렌더링하면 플레이어는 무조건 그려질 것이다.
	//m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle,	D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
#endif
	//3인칭 카메라일 때 플레이어를 렌더링한다.
	//if (m_pPlayer) m_pPlayer->Render(m_pd3dCommandList, m_pCamera);
	
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
	
	hResult = m_pd3dCommandList->Close();
	
	ID3D12CommandList *ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);
	
	WaitForGpuComplete();
	
	m_pdxgiSwapChain->Present(0, 0);
	
	MoveToNextFrame();
	m_GameTimer.GetFrameRate(m_pszFrameRate + 12, 37);
	::SetWindowText(m_hWnd, m_pszFrameRate);
}

void CGameFramework::OnResizeBackBuffers()
{
	WaitForGpuComplete();
	m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);
	for (int i = 0; i < m_nSwapChainBuffers; i++) if (m_ppd3dRenderTargetBuffers[i])
		m_ppd3dRenderTargetBuffers[i]->Release();
	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
#ifdef _WITH_ONLY_RESIZE_BACKBUFFERS
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	m_pdxgiSwapChain->GetDesc(&dxgiSwapChainDesc);
	m_pdxgiSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	m_nSwapChainBufferIndex = 0;
#else
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	m_pdxgiSwapChain->GetDesc(&dxgiSwapChainDesc);
	m_pdxgiSwapChain->ResizeBuffers(m_nSwapChainBuffers, m_nWndClientWidth,
		m_nWndClientHeight, dxgiSwapChainDesc.BufferDesc.Format, dxgiSwapChainDesc.Flags);
	m_nSwapChainBufferIndex = 0;
#endif
	CreateRenderTargetView();
	CreateDepthStencilView();
	m_pd3dCommandList->Close();
	ID3D12CommandList *ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);
	WaitForGpuComplete();
}

CGameFramework::~CGameFramework()
{
}
