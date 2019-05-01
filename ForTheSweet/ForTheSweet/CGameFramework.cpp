#include "stdafx.h"
#include "CGameFramework.h"


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

//���� �Լ��� ���� ���α׷��� ����Ǿ� �� �����찡 �����Ǹ� ȣ��ȴٴ� �Ϳ� �����϶�.
bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;
	//Direct3D ����̽�, ��� ť�� ��� ����Ʈ, ���� ü�� ���� �����ϴ� �Լ��� ȣ���Ѵ�.

	CreateDirect3DDevice();
	CreateCommandQueueAndList();
	CreateRtvAndDsvDescriptorHeaps();
	CreateSwapChain();
	LoadModels();
	//CreateRenderTargetView();
	//CreateDepthStencilView();

	BuildObjects();		//�������� ��ü(���� ���� ��ü)�� �����Ѵ�.

	m_pSocket = new CSocket();
	if (m_pSocket) {
		m_pSocket->init();
		if (m_pSocket->init())
		{
			m_pSocket->sendPacket(CS_CONNECT, 0, 0);
			cout << "CONNECT ��Ŷ ����\n";
		}
		else
			return false;
	}
	return(true);
}

void CGameFramework::OnDestroy()
{
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
	//��üȭ�� ��忡�� ����ȭ���� �ػ󵵸� �ٲ��� �ʰ� �ĸ������ ũ�⸦ ����ȭ�� ũ��� �����Ѵ�.
	dxgiSwapChainDesc.Flags = 0;
#else
	//��üȭ�� ��忡�� ����ȭ���� �ػ󵵸� ����ü��(�ĸ����)�� ũ�⿡ �°� �����Ѵ�.
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
#endif
	HRESULT hResult = m_pdxgiFactory->CreateSwapChain(m_pd3dCommandQueue, &dxgiSwapChainDesc, (IDXGISwapChain **)&m_pdxgiSwapChain);
	
	m_pdxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);
	//��Alt+Enter�� Ű�� ������ ��Ȱ��ȭ�Ѵ�.

	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
	//����ü���� ���� �ĸ���� �ε����� �����Ѵ�.
}

void CGameFramework::CreateDirect3DDevice()
{
#if defined(_DEBUG)
	D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void **)&m_pd3dDebugController);
	m_pd3dDebugController->EnableDebugLayer();
#endif
	::CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void **)&m_pdxgiFactory);
	//DXGI ���丮�� �����Ѵ�.
	IDXGIAdapter1 *pd3dAdapter = NULL;
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_pdxgiFactory->EnumAdapters1(i, &pd3dAdapter); i++)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
		pd3dAdapter->GetDesc1(&dxgiAdapterDesc);
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
		if (SUCCEEDED(D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_12_0,
			_uuidof(ID3D12Device), (void **)&m_pd3dDevice))) break;
	}
	//��� �ϵ���� ����� ���Ͽ� Ư�� ���� 12.0�� �����ϴ� �ϵ���� ����̽��� �����Ѵ�.
	if (!pd3dAdapter)
	{
		m_pdxgiFactory->EnumWarpAdapter(_uuidof(IDXGIFactory4), (void **)&pd3dAdapter);
		D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), (void**)&m_pd3dDevice);
	}
	//Ư�� ���� 12.0�� �����ϴ� �ϵ���� ����̽��� ������ �� ������ WARP ����̽��� �����Ѵ�.
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount = 4; //Msaa4x ���� ���ø�
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;
	m_pd3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;
	//����̽��� �����ϴ� ���� ������ ǰ�� ������ Ȯ���Ѵ�.
	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false;
	//���� ������ ǰ�� ������ 1���� ũ�� ���� ���ø��� Ȱ��ȭ�Ѵ�.
	m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&m_pd3dFence);
	//�潺�� �����ϰ� �潺 ���� 0���� �����Ѵ�.
	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	/*�潺�� ����ȭ�� ���� �̺�Ʈ ��ü�� �����Ѵ�(�̺�Ʈ ��ü�� �ʱⰪ�� FALSE�̴�). �̺�Ʈ�� ����Ǹ�(Signal) ��
	��Ʈ�� ���� �ڵ������� FALSE�� �ǵ��� �����Ѵ�.*/
	//m_d3dViewport.TopLeftX = 0;
	//m_d3dViewport.TopLeftY = 0;
	//m_d3dViewport.Width = static_cast<float>(m_nWndClientWidth);
	//m_d3dViewport.Height = static_cast<float>(m_nWndClientHeight);
	//m_d3dViewport.MinDepth = 0.0f;
	//m_d3dViewport.MaxDepth = 1.0f;
	////����Ʈ�� �� �������� Ŭ���̾�Ʈ ���� ��ü�� �����Ѵ�.
	//m_d3dScissorRect = { 0, 0, m_nWndClientWidth, m_nWndClientHeight };
	////���� �簢���� �� �������� Ŭ���̾�Ʈ ���� ��ü�� �����Ѵ�.
	if (pd3dAdapter) pd3dAdapter->Release();
}

void CGameFramework::CreateCommandQueueAndList()
{
	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	HRESULT hResult = m_pd3dDevice->CreateCommandQueue(&d3dCommandQueueDesc, _uuidof(ID3D12CommandQueue), (void **)&m_pd3dCommandQueue);
	//����(Direct) ��� ť�� �����Ѵ�.

	hResult = m_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void **)&m_pd3dCommandAllocator);
	//����(Direct) ��� �Ҵ��ڸ� �����Ѵ�.

	hResult = m_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pd3dCommandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&m_pd3dCommandList);
	//����(Direct) ��� ����Ʈ�� �����Ѵ�.

	hResult = m_pd3dCommandList->Close();
	//��� ����Ʈ�� �����Ǹ� ����(Open) �����̹Ƿ� ����(Closed) ���·� �����.
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
	//���� Ÿ�� ������ ��(�������� ������ ����ü�� ������ ����)�� �����Ѵ�.
	m_nRtvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//���� Ÿ�� ������ ���� ������ ũ�⸦ �����Ѵ�.

	d3dDescriptorHeapDesc.NumDescriptors = 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dDsvDescriptorHeap);
	//����-���ٽ� ������ ��(�������� ������ 1)�� �����Ѵ�.
	m_nDsvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	//����-���ٽ� ������ ���� ������ ũ�⸦ �����Ѵ�.

	::gnCbvSrvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void CGameFramework::recvCallBack()
{
	sc_packet_login p_login;
	sc_packet_pos p_pos;
	sc_packet_put_player p_put;
	sc_packet_remove p_remove;

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
			XMFLOAT3 position(p_login.x, p_login.y, p_login.z);
			XMFLOAT3 look(p_login.vx, p_login.vy, p_login.vz);

			m_pPlayer = m_pScene->getplayer(My_ID);//pPlayer;
			cout << "ID : " << My_ID << endl;
			m_pCamera = m_pPlayer->GetCamera();

			m_pScene->m_pPlayer[My_ID]->SetPosition(position);
			//m_pScene->m_pPlayer[My_ID]->SetLook(look);
			cout << "ĳ���� ��ġ : " << position.x << ", " << position.y << ", " << position.z << endl;
			//m_pScene->m_pPlayer[My_ID]->SetLookVector(look);
			m_pScene->m_pPlayer[My_ID]->SetConnected(true);

			m_pCamera = m_pScene->m_pPlayer[My_ID]->GetCamera();
			cout << "Client Login sucess\n";

			ptr += sizeof(p_login);
			retval -= sizeof(p_login);
		}
		if (type == SC_PUT_PLAYER)
		{
			memcpy(&p_put, ptr, sizeof(p_put));

			XMFLOAT3 position(p_put.x, p_put.y, p_put.z);
			XMFLOAT3 vel(p_put.vx, p_put.vy, p_put.vz);

			cout << "ĳ���� ��ġ : " << position.x << ", " << position.y << ", " << position.z << endl;

			m_pScene->m_pPlayer[p_put.id]->SetPosition(position);
			m_pScene->m_pPlayer[p_put.id]->SetLook(vel);
			//m_pScene->getplayer(p_put.id)->SetPosition(position);
			//m_pScene->m_pPlayer[p_put.id]->SetLookVector(look);
			m_pScene->m_pPlayer[p_put.id]->SetConnected(true);

			ptr += sizeof(p_put);
			retval -= sizeof(p_put);
		}
		if (type == SC_POS) {
			memcpy(&p_pos, m_pSocket->buf, sizeof(p_pos));

			XMFLOAT3 position(p_pos.x, p_pos.y, p_pos.z);
			XMFLOAT3 look(p_pos.vx, p_pos.vy, p_pos.vz);

			//m_pScene->m_pPlayer[p_pos.id]->SetPosition(position);
			m_pScene->getplayer(p_pos.id)->SetPosition(position);
			m_pScene->getplayer(p_pos.id)->SetLook(look);

			//cout << "1[ID : " << p_pos.id << "]Pos ĳ���� ��ġ : " << position.x << ", " << position.y << ", " << position.z << endl;
			//
			//position = m_pScene->getplayer(p_pos.id)->GetPosition();
			////m_pScene->m_pPlayer[p_pos.id]->SetLookVector(look);
			////cout << position.x << "," << position.y << "," << position.z << "\n";
			////m_pCamera = m_pScene->m_pPlayer[p_pos.id]->GetCamera();
			//
			//cout << "2[ID : " << p_pos.id << "]Pos ĳ���� ��ġ : " << position.x << ", " << position.y << ", " << position.z << endl;

			// ���� Client Position ������ ���ۿ� �����Ǿ����� ���� ������ ��������.
			ptr += sizeof(p_pos);
			retval -= sizeof(p_pos);
		}
		if (type == SC_REMOVE) {
			memcpy(&p_remove, m_pSocket->buf, sizeof(p_remove));

			m_pScene->m_pPlayer[p_remove.id]->SetConnected(false);

			// ���� Client Position ������ ���ۿ� �����Ǿ����� ���� ������ ��������.
			ptr += sizeof(p_remove);
			retval -= sizeof(p_remove);
		}

	}
	memset(m_pSocket->buf, 0, sizeof(MAX_PACKET_SIZE));
}

//����ü���� �� �ĸ� ���ۿ� ���� ���� Ÿ�� �並 �����Ѵ�.
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
	//����-���ٽ� ���۸� �����Ѵ�.

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, NULL, d3dDsvCPUDescriptorHandle);
	//����-���ٽ� ���� �並 �����Ѵ�.
}

void CGameFramework::LoadModels()
{
	vector<pair<string, float>> character_animation;
	character_animation.emplace_back(make_pair("./resource/character/stay.FBX", 0));			//Anim_Idle

	character_animation.emplace_back(make_pair("./resource/character/walk.FBX", 0));			//Anim_Walk

	character_animation.emplace_back(make_pair("./resource/character/weak_attack_1.FBX", 0));	//Anim_Weak_Attack1
	character_animation.emplace_back(make_pair("./resource/character/weak_attack_2.FBX", 0));	//Anim_Weak_Attack2
	character_animation.emplace_back(make_pair("./resource/character/weak_attack_3.FBX", 0));	//Anim_Weak_Attack3

	character_animation.emplace_back(make_pair("./resource/character/hard_attack1.FBX", 0));	//Anim_Hard_Attack1
	character_animation.emplace_back(make_pair("./resource/character/hard_attack2.FBX", 0));	//Anim_Hard_Attack2

	character_animation.emplace_back(make_pair("./resource/character/guard.FBX", 0));	//Anim_Guard

	character_animation.emplace_back(make_pair("./resource/character/powerup.FBX", 0));	//Anim_PowerUp

	character_animation.emplace_back(make_pair("./resource/character/jump.FBX", 0));	//Anim_PowerUp


	Character_Model = new Model_Animation("./resource/character/main_character.FBX", &character_animation);
	
	Map_Model[0] = new LoadModel("./resource/map/map_1.FBX", true);			//M_Map_1

	weapon[0] = new LoadModel("./resource/weapon/lollipop.FBX", true);		//M_Weapon_Lollipop
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
	}
	cout << "ID : " << My_ID << endl;

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
		//���콺 ĸ�ĸ� �ϰ� ���� ���콺 ��ġ�� �����´�.
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		//���콺 ĸ�ĸ� �����Ѵ�.
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
		case VK_F9:	// ������ ���� ��üȭ�� ����� ��ȯ�� ó���Ѵ�.
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
		//�������� ũ�Ⱑ ����Ǹ� �ĸ������ ũ�⸦ �����Ѵ�.
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
	/*Ű������ ���� ������ ��ȯ�Ѵ�. ȭ��ǥ Ű(���桯, ���硯, ���衯, ���顯)�� ������ �÷��̾ ������/����(���� x-��), ��/
	��(���� z-��)�� �̵��Ѵ�. ��Page Up���� ��Page Down�� Ű�� ������ �÷��̾ ��/�Ʒ�(���� y-��)�� �̵��Ѵ�.*/
	BOOL Key_A = FALSE;
	BOOL Key_S = FALSE;
	BOOL Key_D = FALSE;
	BOOL Key_F = FALSE;
	BOOL Key_LShift = FALSE;
	BOOL Key_Space = FALSE;
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

	if (dwDirection != 0) {
		m_pSocket->sendPacket(CS_MOVE, dwDirection, 0);
	}
	count = 0;
	float rotation = 0.0f;
	POINT ptCursorPos;
	/*���콺�� ĸ�������� ���콺�� �󸶸�ŭ �̵��Ͽ��� ���� ����Ѵ�. ���콺 ���� �Ǵ� ������ ��ư�� ������ ����
	�޽���(WM_LBUTTONDOWN, WM_RBUTTONDOWN)�� ó���� �� ���콺�� ĸ���Ͽ���. �׷��Ƿ� ���콺�� ĸ�ĵ�
	���� ���콺 ��ư�� ������ ���¸� �ǹ��Ѵ�. ���콺 ��ư�� ������ ���¿��� ���콺�� �¿� �Ǵ� ���Ϸ� �����̸� ��
	���̾ x-�� �Ǵ� y-������ ȸ���Ѵ�.*/

	int Anim_Index = m_pPlayer->getAnimIndex();
	float Anim_Time = m_pPlayer->getAnimtime();

	if (Key_A||Key_S) {

		if (Key_A&&Key_S) {//�Ѵ� ������ ����
			m_pPlayer->ChangeAnimation(Anim_Guard);
			m_pPlayer->SetAnimFrame(10);
			m_pPlayer->DisableLoop();
		}
		else if (Key_A && !Key_S) { //����� or �ݱ�
			//�浹üũ (���� ������Ʈ��) �浹�̸� �ݱ�		

			//�ƴϸ� �����
			if (Anim_Index == Anim_Idle || Anim_Index == Anim_Walk) {
				m_pPlayer->ChangeAnimation(Anim_Weak_Attack1);
				m_pPlayer->DisableLoop();
			}
			if (Anim_Index == Anim_Weak_Attack1 && (Anim_Time > 10 && Anim_Time < 20)) {
				m_pPlayer->ChangeAnimation(Anim_Weak_Attack2);
				m_pPlayer->SetAnimFrame(Anim_Time);
				m_pPlayer->DisableLoop();
			}
			if (Anim_Index == Anim_Weak_Attack2 && (Anim_Time > 25 && Anim_Time < 34)) {
				m_pPlayer->ChangeAnimation(Anim_Weak_Attack3);
				m_pPlayer->SetAnimFrame(Anim_Time);
				m_pPlayer->DisableLoop();
			}
		}
		else if (!Key_A && Key_S) { //������ or �ݱ�
			//�浹üũ (���� ������Ʈ��) �浹�̸� �ݱ�

			//�ƴϸ� ������
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

	if (Key_D) { //���� ��ų
		//���� ��ȣ�� WEAPON_EMPTY �� �ƴϸ� ��ų���

	}
	if (Key_F) { //��ȭ ��ų
		if (Anim_Index == Anim_Idle || Anim_Index == Anim_Walk) {		//��ȭ ������(?) �÷��װ� �־����
			m_pPlayer->ChangeAnimation(Anim_PowerUp);
			m_pPlayer->DisableLoop();

		}
	}
	
	if (Key_Space) { //����
		if (Anim_Index == Anim_Idle || Anim_Index == Anim_Walk) {
			m_pPlayer->ChangeAnimation(Anim_Jump);
			m_pPlayer->DisableLoop();
		}
	}
	if (Key_LShift) { //������


	}
	//if (d_move) return;
	if (dwDirection)
	{
		if (Anim_Index==Anim_Idle) {
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

	//���콺 �Ǵ� Ű �Է��� ������ �÷��̾ �̵��ϰų�(dwDirection) ȸ���Ѵ�(cxDelta �Ǵ� cyDelta).
	//if ((dwDirection != 0) || (rotation != 0.0f))
	{		
			//m_pPlayer->Rotate(0.0f, rotation, 0.0f);

			/*�÷��̾ dwDirection �������� �̵��Ѵ�(�����δ� �ӵ� ���͸� �����Ѵ�). �̵� �Ÿ��� �ð��� ����ϵ��� �Ѵ�.
			�÷��̾��� �̵� �ӷ��� (50/��)�� �����Ѵ�.*/
		/*
		if (dwDirection)
		{
			//m_pPlayer->Move(dwDirection, 100.0f * m_GameTimer.GetTimeElapsed(), true);
			m_pPhysx->move(dwDirection, 20.0f * m_GameTimer.GetTimeElapsed());
			
			XMFLOAT3 Look = m_pPlayer->GetLook();
			//XMFLOAT3 Right = m_pPlayer->GetRight();
			if (dwDirection & DIR_FORWARD) {
				Look.z = -1.f;
			//	Right.x = -1.f;
			}
			if (dwDirection & DIR_BACKWARD) {
				Look.z = 1.f;
			//	Right.x = 1.f;
			}
			if (dwDirection & DIR_RIGHT) {
				Look.x = -1.f;
			//	Right.z = -1.f;
			}
			if (dwDirection & DIR_LEFT) {
				Look.x = 1.f;
			//	Right.z = 1.f;
			}
			Look = Vector3::Normalize(Look);
			//Right = Vector3::Normalize(Right);
			m_pPlayer->SetLook(Look);		
		}
		*/
	}
	//�÷��̾ ������ �̵��ϰ� ī�޶� �����Ѵ�. �߷°� �������� ������ �ӵ� ���Ϳ� �����Ѵ�.
	//if(m_pCamera)
	//m_pPlayer->Update(m_GameTimer.GetTimeElapsed());
}

void CGameFramework::AnimateObjects()
{
	if (m_pScene) m_pScene->AnimateObjects(m_GameTimer.GetTimeElapsed());
}

void CGameFramework::CollisionProcess()
{
	if (m_pScene) m_pScene->CollisionProcess();
}

void CGameFramework::WaitForGpuComplete()
{
	UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);
	//GPU�� �潺�� ���� �����ϴ� ����� ��� ť�� �߰��Ѵ�.

	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		//�潺�� ���� ���� ������ ������ ������ �潺�� ���� ���� ������ ���� �� ������ ��ٸ���.
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
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

	//m_pPhysx->m_Scene->simulate(1.f / 60.f);
	//m_pPhysx->m_Scene->fetchResults(1.f / 60.f);

	//physx::PxExtendedVec3 playerPosition;
	//playerPosition = m_pPhysx->m_PlayerController->getPosition();
	XMFLOAT3 position = m_pPlayer->GetPosition();;
	//position.x = playerPosition.x;
	//position.y = playerPosition.y - 17.5f;
	//position.z = playerPosition.z;
	m_pCamera->SetPosition(Vector3::Add(position, m_pCamera->GetOffset()));
	m_pCamera->SetLookAt(position);
	m_pPlayer->SetPosition(position);
	
	//cout << "ĳ���� ��ġ : " << position.x << ", " << position.y << ", " << position.z << endl;
	//position = m_pCamera->GetPosition();
	//cout << "ī�޶� ��ġ : " << position.x << ", " << position.y << ", " << position.z << endl;;
	
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
	//���ϴ� �������� ���� Ÿ��(��)�� �����.
	m_pd3dCommandList->ClearRenderTargetView(d3dRtvCPUDescriptorHandle,	pfClearColor/*Colors::Azure*/, 0, NULL);
	
	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle =	m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle,	D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
	
	m_pd3dCommandList->OMSetRenderTargets(1, &d3dRtvCPUDescriptorHandle, TRUE, &d3dDsvCPUDescriptorHandle);
	
	if (m_pScene) m_pScene->Render(m_pd3dCommandList, m_pCamera);

	//3��Ī ī�޶��� �� �÷��̾ �׻� ���̵��� �������Ѵ�.
#ifdef _WITH_PLAYER_TOP
	//���� Ÿ���� �״�� �ΰ� ���� ���۸� 1.0���� ����� �÷��̾ �������ϸ� �÷��̾�� ������ �׷��� ���̴�.
	//m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle,	D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
#endif
	//3��Ī ī�޶��� �� �÷��̾ �������Ѵ�.
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
