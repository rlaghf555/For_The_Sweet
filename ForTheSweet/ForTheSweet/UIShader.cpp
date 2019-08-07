#include "stdafx.h"
#include "UIShader.h"
#include "Object.h"

UIShader::UIShader() { }
UIShader::~UIShader() { }

D3D12_BLEND_DESC UIShader::CreateBlendState(int index)
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));

	d3dBlendDesc.AlphaToCoverageEnable = false;
	d3dBlendDesc.IndependentBlendEnable = false;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = false;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return d3dBlendDesc;
}

D3D12_DEPTH_STENCIL_DESC UIShader::CreateDepthStencilState(int index)
{
	D3D12_DEPTH_STENCIL_DESC desc;
	::ZeroMemory(&desc, sizeof(D3D12_DEPTH_STENCIL_DESC));

	desc.DepthEnable = false;
	desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	desc.StencilEnable = false;
	desc.StencilReadMask = 0x00;
	desc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	desc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	desc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	desc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_LESS;
	desc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	desc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	desc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	desc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_LESS;

	return desc;
}

void UIShader::Animate(float fTimeElapsed)
{
	for (UINT i = 0; i < m_nObjects; ++i) {
		m_pUIObjects[i]->Update(fTimeElapsed);
	}
}

void UIShader::UpdateState(int ui_state)
{
	for (UINT i = 0; i < m_nObjects; ++i) {
		m_pUIObjects[i]->uistate = ui_state;
	}
}

void UIShader::RefreshTimer(float fTimeElapsed, UINT index)
{
	if (!m_pUIObjects[index]->m_bEnabled) return;

	m_ElapsedTime += fTimeElapsed;

	if (m_ElapsedTime >= 0.0f && m_ElapsedTime <= m_ApearTime)
		m_pUIObjects[index]->SetAlpha(D3DMath::Lerp(0.0f, 1.0f, m_ElapsedTime / m_ApearTime));

	else if (m_ElapsedTime >= m_ApearTime && m_ElapsedTime <= m_ApearTime + m_ExistTime)
		m_pUIObjects[index]->SetAlpha(1.0f);

	else if (m_ElapsedTime >= m_ApearTime + m_ExistTime && m_ElapsedTime <= m_ApearTime + m_ExistTime + m_RetreatTime)
		m_pUIObjects[index]->SetAlpha(D3DMath::Lerp(1.0f, 0.0f, ((m_ElapsedTime - (m_ApearTime + m_ExistTime)) / m_RetreatTime)));

	else
		m_pUIObjects[index]->m_bEnabled = false;

	return;
}

void UIShader::SetNowSprite(XMUINT2 & nowSprite, UINT index)
{
	m_pUIObjects[index]->m_nNowSprite = nowSprite;
	if (m_bTimeUI) {
		m_pUIObjects[index]->m_bEnabled = true;
		m_ElapsedTime = 0.0f;
	}
}

void UIShader::SetPosScreenRatio(XMFLOAT2& ratio, UINT index)
{
	m_pUIObjects[index]->m_xmf2ScreenPos = XMFLOAT2(
		static_cast<float>(FRAME_BUFFER_WIDTH) * ratio.x,
		static_cast<float>(FRAME_BUFFER_HEIGHT) * ratio.y
	);

	m_pUIObjects[index]->CreateCollisionBox();
}

void UIShader::SetPosScreenRatio(XMFLOAT2& ratio)
{
	for (UINT i = 0; i < m_nObjects; ++i) {
		m_pUIObjects[i]->m_xmf2ScreenPos = XMFLOAT2(
			static_cast<float>(FRAME_BUFFER_WIDTH) * ratio.x,
			static_cast<float>(FRAME_BUFFER_HEIGHT) * ratio.y
		);

		m_pUIObjects[i]->CreateCollisionBox();
	}
}

void UIShader::SetAlpha(float alpha, UINT index)
{
	m_pUIObjects[index]->m_fAlpha = alpha;
}

void UIShader::SetScale(XMFLOAT2 * scale)
{
	for (UINT i = 0; i < m_nObjects; ++i) {
		m_pUIObjects[i]->m_xmf2Scale = *scale;
	}

	CreateCollisionBox();
}


void UIShader::MovePos(XMFLOAT2 & pos, UINT index)
{
	XMFLOAT2 originPos = m_pUIObjects[index]->m_xmf2ScreenPos;
	XMFLOAT2 originScale = m_pUIObjects[index]->m_xmf2Scale;
	m_pUIObjects[index]->SetPosition(XMFLOAT2(originPos.x + (pos.x * originScale.x), originPos.y + (pos.y * originScale.y)));
}

void UIShader::CreateGraphicsRootSignature(ID3D12Device * pd3dDevice)
{
	//ComPtr<ID3D12RootSignature> pd3dGraphicsRootSignature = nullptr;

	CD3DX12_DESCRIPTOR_RANGE pd3dDescriptorRanges[1 + NUM_MAX_UITEXTURE];
	pd3dDescriptorRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, CBVUIInfo, 0, 0); // GameObject
	for (int i = 0; i < NUM_MAX_UITEXTURE; ++i)
		pd3dDescriptorRanges[1 + i].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, SRVUITextureMap + i, 0, 0); // Texture

	CD3DX12_ROOT_PARAMETER pd3dRootParameters[1 + NUM_MAX_UITEXTURE];
	pd3dRootParameters[0].InitAsDescriptorTable(1, &pd3dDescriptorRanges[0], D3D12_SHADER_VISIBILITY_ALL);
	for (int i = 0; i < NUM_MAX_UITEXTURE; ++i)
		pd3dRootParameters[1 + i].InitAsDescriptorTable(1, &pd3dDescriptorRanges[1 + i], D3D12_SHADER_VISIBILITY_ALL);

	D3D12_STATIC_SAMPLER_DESC d3dSamplerDesc[2];
	::ZeroMemory(&d3dSamplerDesc, sizeof(D3D12_STATIC_SAMPLER_DESC));

	d3dSamplerDesc[0] = CD3DX12_STATIC_SAMPLER_DESC(
		0,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		0.0f,
		1,
		D3D12_COMPARISON_FUNC_ALWAYS,
		D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
		0.0f,
		D3D12_FLOAT32_MAX,
		D3D12_SHADER_VISIBILITY_PIXEL
	);

	d3dSamplerDesc[1] = CD3DX12_STATIC_SAMPLER_DESC(
		1, // shaderRegister
		D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressW
		0.0f,                               // mipLODBias
		16,                                 // maxAnisotropy
		D3D12_COMPARISON_FUNC_LESS_EQUAL,
		D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK
	);


	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 2;
	d3dRootSignatureDesc.pStaticSamplers = d3dSamplerDesc;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ComPtr<ID3DBlob> pd3dSignatureBlob = NULL;
	ComPtr<ID3DBlob> pd3dErrorBlob = NULL;
	HRESULT hr = D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, pd3dSignatureBlob.GetAddressOf(), pd3dErrorBlob.GetAddressOf());

	if (pd3dErrorBlob != nullptr)
	{
		::OutputDebugStringA((char*)pd3dErrorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(pd3dDevice->CreateRootSignature(0,
		pd3dSignatureBlob->GetBufferPointer(),
		pd3dSignatureBlob->GetBufferSize(),
		IID_PPV_ARGS(m_RootSignature[PSO_OBJECT].GetAddressOf()))
	);
}

void UIShader::CreateCollisionBox()
{
	for (UINT i = 0; i < m_nObjects; ++i)
		m_pUIObjects[i]->CreateCollisionBox();
}

void UIShader::CreateShaderVariables(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	m_ObjectCB = make_unique<UploadBuffer<CB_UI_INFO>>(pd3dDevice, m_nObjects, true);
}

void UIShader::UpdateShaderVariables(ID3D12GraphicsCommandList * pd3dCommandList)
{
	CB_UI_INFO cBuffer;

	for (unsigned int i = 0; i < m_nObjects; ++i) {
		cBuffer.m_fData = m_pUIObjects[i]->m_fData;
		cBuffer.m_fData2 = m_pUIObjects[i]->m_fData2;
		cBuffer.m_nNowSprite = m_pUIObjects[i]->m_nNowSprite;
		cBuffer.m_nNumSprite = m_pUIObjects[i]->m_nNumSprite;
		cBuffer.m_nSize = m_pUIObjects[i]->m_nSize;
		cBuffer.m_xmf2Scale = m_pUIObjects[i]->m_xmf2Scale;
		cBuffer.m_nTexType = m_pUIObjects[i]->m_nTexType;
		cBuffer.m_xmf2ScreenPos = m_pUIObjects[i]->m_xmf2ScreenPos;
		cBuffer.m_xmf2ScreenSize = m_pUIObjects[i]->m_xmf2ScreenSize;
		cBuffer.m_fAlpha = m_pUIObjects[i]->m_fAlpha;
		m_ObjectCB->CopyData(i, cBuffer);
	}
}

void UIShader::CreateCbvAndSrvDescriptorHeaps(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nConstantBufferViews, int nShaderResourceViews, bool bIsGraphics)
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = nConstantBufferViews + nShaderResourceViews; //CBVs + SRVs 
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask = 0;

	if (bIsGraphics) {
		HRESULT hResult = pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_CbvSrvDescriptorHeap);

		ThrowIfFailed(pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc,
			IID_PPV_ARGS(&m_CbvSrvDescriptorHeap)));

		m_d3dCbvCPUDescriptorStartHandle = m_CbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		m_d3dCbvGPUDescriptorStartHandle = m_CbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		m_d3dSrvCPUDescriptorStartHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);
		m_d3dSrvGPUDescriptorStartHandle.ptr = m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);
	}
	else {
		HRESULT hResult = pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_ComputeCbvSrvDescriptorHeap);

		ThrowIfFailed(pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc,
			IID_PPV_ARGS(&m_ComputeCbvSrvDescriptorHeap)));

		m_d3dComputeCbvCPUDescriptorStartHandle = m_ComputeCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		m_d3dComputeCbvGPUDescriptorStartHandle = m_ComputeCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		m_d3dComputeSrvCPUDescriptorStartHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);
		m_d3dComputeSrvGPUDescriptorStartHandle.ptr = m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);
	}
}

void UIShader::CreateConstantBufferViews(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nConstantBufferViews, ID3D12Resource * pd3dConstantBuffers, UINT nStride)
{
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = pd3dConstantBuffers->GetGPUVirtualAddress();
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	d3dCBVDesc.SizeInBytes = nStride;
	for (int j = 0; j < nConstantBufferViews; ++j)
	{
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (nStride * j);
		D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
		d3dCbvCPUDescriptorHandle.ptr = m_CbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart().ptr + (::gnCbvSrvDescriptorIncrementSize * j);
		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
	}
}

void UIShader::CreateShaderResourceViews(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, CTexture * pTexture, UINT nRootParameterStartIndex, bool bAutoIncrement, bool bIsGraphics)
{
	D3D12_CPU_DESCRIPTOR_HANDLE d3dSrvCPUDescriptorHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGPUDescriptorHandle;
	if (bIsGraphics) {
		d3dSrvCPUDescriptorHandle = m_d3dSrvCPUDescriptorStartHandle;
		d3dSrvGPUDescriptorHandle = m_d3dSrvGPUDescriptorStartHandle;
	}

	else {
		d3dSrvCPUDescriptorHandle = m_d3dComputeSrvCPUDescriptorStartHandle;
		d3dSrvGPUDescriptorHandle = m_d3dComputeSrvGPUDescriptorStartHandle;
	}

	int nTextures = pTexture->GetTextureCount();

	for (int i = 0; i < nTextures; i++)
	{
		int nTextureType = pTexture->GetTextureType(i);
		ComPtr<ID3D12Resource> pShaderResource = pTexture->GetTexture(i);
		D3D12_RESOURCE_DESC d3dResourceDesc = pShaderResource->GetDesc();
		D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = GetShaderResourceViewDesc(d3dResourceDesc, nTextureType);
		pd3dDevice->CreateShaderResourceView(pShaderResource.Get(), &d3dShaderResourceViewDesc, d3dSrvCPUDescriptorHandle);
		d3dSrvCPUDescriptorHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

		pTexture->SetRootArgument(i, (bAutoIncrement) ? (nRootParameterStartIndex + i) : nRootParameterStartIndex, d3dSrvGPUDescriptorHandle);
		d3dSrvGPUDescriptorHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	}
}

D3D12_SHADER_BYTECODE UIShader::CreateVertexShader(int index)
{
	//wchar_t filename[100] = L"UIShader.hlsl";
	//return(CShader::CompileShaderFromFile(filename, "VSUITextured", "vs_5_1", ppd3dShaderBlob));
	D3D12_SHADER_BYTECODE byteCode;
	::ZeroMemory(&byteCode, sizeof(D3D12_SHADER_BYTECODE));

	if (m_VSByteCode[index] != nullptr) {
		byteCode.pShaderBytecode = m_VSByteCode[index]->GetBufferPointer();
		byteCode.BytecodeLength = m_VSByteCode[index]->GetBufferSize();
	}
	return byteCode;
}

D3D12_SHADER_BYTECODE UIShader::CreatePixelShader(int index)
{
	//wchar_t filename[100] = L"UIShader.hlsl";
	//return(CShader::CompileShaderFromFile(filename, "PSDefaultUI", "ps_5_1", ppd3dShaderBlob));
	D3D12_SHADER_BYTECODE byteCode;
	::ZeroMemory(&byteCode, sizeof(D3D12_SHADER_BYTECODE));

	if (m_PSByteCode[index] != nullptr) {
		byteCode.pShaderBytecode = m_PSByteCode[index]->GetBufferPointer();
		byteCode.BytecodeLength = m_PSByteCode[index]->GetBufferSize();
	}

	return byteCode;
}

void UIShader::BuildPSO(ID3D12Device * pd3dDevice, UINT nRenderTargets, int index)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	::ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	psoDesc.InputLayout = CreateInputLayout();
	psoDesc.pRootSignature = m_RootSignature[index].Get();
	psoDesc.VS = CreateVertexShader(index);
	psoDesc.PS = CreatePixelShader(index);
	psoDesc.RasterizerState = CreateRasterizerState();
	psoDesc.BlendState = CreateBlendState(index);
	psoDesc.DepthStencilState = CreateDepthStencilState(index);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = nRenderTargets;
	for (UINT i = 0; i < nRenderTargets; i++)
		psoDesc.RTVFormats[i] = m_Format;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	ThrowIfFailed(pd3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(m_pPSO[index].GetAddressOf())));
}



void UIShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nRenderTargets, void * pContext) {
	//TextureDataForm* textures = reinterpret_cast<TextureDataForm*>(pContext);
	UINT nTextures = 1;

	m_nObjects = nTextures;
	m_nPSO = 1;

	CreatePipelineParts();
	m_VSByteCode[0] = D3DUtil::CompileShader(L"UIShader.hlsl", nullptr, "VSUITextured", "vs_5_1");
	m_PSByteCode[0] = D3DUtil::CompileShader(L"UIShader.hlsl", nullptr, "PSDefaultUI", "ps_5_1");

	CTexture *pTexture = new CTexture(nTextures, RESOURCE_TEXTURE2D, 0);
	//for (int i = 0; i < nTextures; ++i) {
	//	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\image\\UIsample2.dds", i);
	//}
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\image\\UIsample1.dds", 0);
	//pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\image\\UIsample2.dds", 1);

	UINT ncbElementBytes = D3DUtil::CalcConstantBufferByteSize(sizeof(CB_UI_INFO));

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, pTexture->GetTextureCount());
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_ObjectCB->Resource(), ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 1, true);

	CreateGraphicsRootSignature(pd3dDevice);
	BuildPSO(pd3dDevice, nRenderTargets);

	m_pUIObjects = vector<UIObject*>(m_nObjects);

	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
	m_pMaterial->SetReflection(1);

	XMFLOAT2 scale = XMFLOAT2(0.7f, 0.7f);

	for (UINT i = 0; i < m_nObjects; ++i) {
		UIObject* ui;
		ui = new UIObject();
		XMFLOAT2 tmp = XMFLOAT2(static_cast<float>(FRAME_BUFFER_WIDTH) / 2, static_cast<float>(FRAME_BUFFER_HEIGHT) * (3.0f - 1.5f * i) / 9.0);
		ui->SetPosition(tmp);
		ui->SetScale(scale);
		m_pUIObjects[i] = ui;
	}

	for (UINT i = 0; i < m_nObjects; ++i) {
		m_pUIObjects[i]->SetScreenSize(XMFLOAT2(static_cast<float>(FRAME_BUFFER_WIDTH), static_cast<float>(FRAME_BUFFER_HEIGHT)));
		XMUINT2 tmpsize(1, 1);
		m_pUIObjects[i]->SetNumSprite(tmpsize, tmpsize);
		XMUINT2 sizetmp(1, 1);
		sizetmp = GetSpriteSize(i, pTexture, sizetmp);
		m_pUIObjects[i]->SetSize(sizetmp);
		m_pUIObjects[i]->SetType(i);
		m_pUIObjects[i]->CreateCollisionBox();
		m_pUIObjects[i]->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
	}
}

void UIShader::ReleaseObjects()
{
	for (UINT i = 0; i < m_nObjects; i++) {
		if (m_pUIObjects[i])
			delete m_pUIObjects[i];
	}
	//m_pUIObjects.clear();
}

void UIShader::ReleaseShaderVariables()
{
	if (m_ObjectCB)
		m_ObjectCB->~UploadBuffer();
}

void UIShader::OnPrepareRender(ID3D12GraphicsCommandList * pd3dCommandList, int index)
{
	if (m_RootSignature[index])
		pd3dCommandList->SetGraphicsRootSignature(m_RootSignature[index].Get());

	if (m_pPSO[index])
		pd3dCommandList->SetPipelineState(m_pPSO[index].Get());

	pd3dCommandList->SetDescriptorHeaps(1, m_CbvSrvDescriptorHeap.GetAddressOf());

	UpdateShaderVariables(pd3dCommandList);
}

void UIShader::Render(ID3D12GraphicsCommandList * pd3dCommandList)
{
	OnPrepareRender(pd3dCommandList, 0);

	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);

	for (unsigned int j = 0; j < m_nObjects; j++)
	{
		if (m_pUIObjects[j] && m_pUIObjects[j]->m_bEnabled)
			m_pUIObjects[j]->Render(pd3dCommandList);
	}
}

void UIShader::SetTimer(float apearTime, float existTime, float retreatTime)
{
	m_bTimeUI = true;
	m_ApearTime = apearTime;
	m_RetreatTime = retreatTime;
	m_ExistTime = existTime;
	m_ElapsedTime = 0.0f;
}

XMUINT2 UIShader::GetSpriteSize(int texIndex, CTexture* pTexture, XMUINT2 numSprite)
{
	D3D12_RESOURCE_DESC desc = pTexture->GetTexture(texIndex)->GetDesc();
	return XMUINT2(desc.Width / numSprite.x, desc.Height / numSprite.y);
}

void UIShader::CreatePipelineParts() {
	if (m_nPSO > 0) {
		m_pPSO = new ComPtr<ID3D12PipelineState>[m_nPSO];
		m_RootSignature = new ComPtr<ID3D12RootSignature>[m_nPSO];

		m_VSByteCode = new ComPtr<ID3DBlob>[m_nPSO];
		m_PSByteCode = new ComPtr<ID3DBlob>[m_nPSO];
	}

	if (m_nComputePSO > 0) {
		m_ComputeRootSignature = new ComPtr<ID3D12RootSignature>();
		m_pComputePSO = new ComPtr<ID3D12PipelineState>[m_nComputePSO];

		m_pComputeSRVUAVCPUHandles = vector<D3D12_CPU_DESCRIPTOR_HANDLE>(m_nComputeBuffers * 2);
		m_pComputeSRVUAVGPUHandles = vector<D3D12_GPU_DESCRIPTOR_HANDLE>(m_nComputeBuffers * 2);

		m_pComputeUAVBuffers = vector<ComPtr<ID3D12Resource>>(m_nComputeBuffers);
		m_pComputeOutputBuffers = vector<ComPtr<ID3D12Resource>>(m_nComputeBuffers);

		m_CSByteCode = new ComPtr<ID3DBlob>[m_nComputePSO];
	}
}

void FogShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nRenderTargets, void * pContext)
{
	UINT nTextures = 1;

	m_nObjects = 8;
	m_nPSO = 1;

	CreatePipelineParts();
	m_VSByteCode[0] = D3DUtil::CompileShader(L"UIShader.hlsl", nullptr, "VSUITextured", "vs_5_1");
	m_PSByteCode[0] = D3DUtil::CompileShader(L"UIShader.hlsl", nullptr, "PSDefaultUI", "ps_5_1");

	CTexture *pTexture = new CTexture(nTextures, RESOURCE_TEXTURE2D, 0);

	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\image\\Fog.dds", 0);

	UINT ncbElementBytes = D3DUtil::CalcConstantBufferByteSize(sizeof(CB_UI_INFO));

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, pTexture->GetTextureCount());
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_ObjectCB->Resource(), ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 1, true);

	CreateGraphicsRootSignature(pd3dDevice);
	BuildPSO(pd3dDevice, nRenderTargets);

	m_pUIObjects = vector<UIObject*>(m_nObjects);

	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
	m_pMaterial->SetReflection(1);

	XMFLOAT2 scale = XMFLOAT2(5, 5);

	for (UINT i = 0; i < m_nObjects; ++i) {
		UIObject* fog;
		fog = new UIObject();
		fog->SetScale(scale);
		m_pUIObjects[i] = fog;
	}

	for (UINT i = 0; i < m_nObjects; ++i) {
		m_pUIObjects[i]->SetScreenSize(XMFLOAT2(static_cast<float>(FRAME_BUFFER_WIDTH), static_cast<float>(FRAME_BUFFER_HEIGHT)));
		XMUINT2 sizetmp(1, 1);
		sizetmp = GetSpriteSize(0, pTexture, sizetmp);
		m_pUIObjects[i]->SetSize(sizetmp);
		m_pUIObjects[i]->SetType(i);
		m_pUIObjects[i]->CreateCollisionBox();
		m_pUIObjects[i]->m_bEnabled=false;
		// m_pUIObjects[i]->m_fAlpha = 0.8f;
		m_pUIObjects[i]->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
	}
	XMUINT2 numsprite(2, 2);
	XMUINT2 nowsprite(0, 0);
	m_pUIObjects[0]->SetNumSprite(numsprite, nowsprite);
	nowsprite= XMUINT2(0, 1);
	m_pUIObjects[1]->SetNumSprite(numsprite, nowsprite);
	nowsprite = XMUINT2(1, 0);
	m_pUIObjects[2]->SetNumSprite(numsprite, nowsprite);
	nowsprite = XMUINT2(1, 1);
	m_pUIObjects[3]->SetNumSprite(numsprite, nowsprite);
	nowsprite = XMUINT2(0, 0);
	m_pUIObjects[4]->SetNumSprite(numsprite, nowsprite);
	nowsprite = XMUINT2(0, 1);
	m_pUIObjects[5]->SetNumSprite(numsprite, nowsprite);
	nowsprite = XMUINT2(1, 0);
	m_pUIObjects[6]->SetNumSprite(numsprite, nowsprite);
	nowsprite = XMUINT2(1, 1);
	m_pUIObjects[7]->SetNumSprite(numsprite, nowsprite);
}

void FogShader::SetFog()
{
	for (UINT i = 0; i < m_nObjects; ++i) {
		m_pUIObjects[i]->m_bEnabled = true;
		m_pUIObjects[i]->m_fAlpha = 0.8f;
	}
	
	m_pUIObjects[0]->SetPosition(0,-720);
	m_pUIObjects[1]->SetPosition(1000, -720);
	m_pUIObjects[2]->SetPosition(0,-1720);
	m_pUIObjects[3]->SetPosition(1000,-1720);
	m_pUIObjects[4]->SetPosition(500,-720);
	m_pUIObjects[5]->SetPosition(0,-1320);
	m_pUIObjects[6]->SetPosition(500,-1720);
	m_pUIObjects[7]->SetPosition(1000, -1320);

	Fog_Flag = true;
	Fog_Off_Flag = false;
	ftime = 0.f;
	SoundManager::GetInstance()->StopBackGroundSounds();
	SoundManager::GetInstance()->PlayBackGroundSounds(FOG);
}

void FogShader::FogOff()
{
	Fog_Off_Flag = true;
	
}

void FogShader::Animate(float fTimeElapsed)
{
	if (Fog_Off_Flag) {
		for (int i = 0; i < m_nObjects; ++i) {
			m_pUIObjects[i]->m_fAlpha -= 0.01f;
		}
		if (m_pUIObjects[0]->m_fAlpha < 0.f) {
			for (int i = 0; i < m_nObjects; ++i) {
				m_pUIObjects[i]->m_bEnabled = false;
			}
			Fog_Flag = false;
			Fog_Off_Flag = false;
			SoundManager::GetInstance()->StopBackGroundSounds();
			SoundManager::GetInstance()->PlayBackGroundSounds(BACKGROUND);
		}
	}
	if (!Fog_Flag)
		return;
	if (IsZero(fTimeElapsed))
		return;
	ftime += fTimeElapsed;
	if (ftime > 20)	// 지속시간
		FogOff();
	for (int i = 0; i < m_nObjects; ++i) {
		auto pos = m_pUIObjects[i]->GetPos();
		pos.x -=0.3f; pos.y += 2;
		m_pUIObjects[i]->SetPosition(pos);
		if (pos.y > 1700) {
			switch (i)
			{
			case 0:
				m_pUIObjects[0]->SetPosition(0, -720);
				break;
			case 1:
				m_pUIObjects[1]->SetPosition(1500, -720);
				break;
			case 2:
				m_pUIObjects[2]->SetPosition(0, -1720);
				break;
			case 3:
				m_pUIObjects[3]->SetPosition(1500, -1720);
				break;
			case 4:
				m_pUIObjects[4]->SetPosition(1000, -720);
				break;
			case 5:
				m_pUIObjects[5]->SetPosition(500, -1320);
				break;
			case 6:
				m_pUIObjects[6]->SetPosition(1000, -1720);
				break;
			case 7:
				m_pUIObjects[7]->SetPosition(1500, -1320);
				break;
			}
		}
	}
}

void UIHPBarShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nRenderTargets, void * pContext)
{
	UINT nTextures = 3;
	m_nObjects = 3;
	m_nPSO = 1;

	CreatePipelineParts();

	m_VSByteCode[0] = D3DUtil::CompileShader(L"UIShader.hlsl", nullptr, "VSUITextured", "vs_5_1");
	m_PSByteCode[0] = D3DUtil::CompileShader(L"UIShader.hlsl", nullptr, "PSUIHPBar", "ps_5_1");

	CTexture *pTexture = new CTexture(nTextures, RESOURCE_TEXTURE2D, 0);

	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\image\\HPedge.dds", 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\image\\HPbar.dds", 1);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\image\\MPbar.dds", 2);

	UINT ncbElementBytes = D3DUtil::CalcConstantBufferByteSize(sizeof(CB_UI_INFO));

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, pTexture->GetTextureCount());
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_ObjectCB->Resource(), ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 1, true);

	CreateGraphicsRootSignature(pd3dDevice);

	BuildPSO(pd3dDevice, nRenderTargets);

	m_pUIObjects = vector<UIObject*>(m_nObjects);
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
	m_pMaterial->SetReflection(1);

	XMFLOAT2 pos = XMFLOAT2(0, 0);
	XMFLOAT2 scale = XMFLOAT2(0.8f, 0.8f);

	UIObject* HPedge;
	HPedge = new UIObject();
	HPedge->SetPosition(pos);
	HPedge->SetScale(scale);
	m_pUIObjects[0] = HPedge;

	HPBarObject* hpBar;
	hpBar = new HPBarObject();
	hpBar->SetPosition(XMFLOAT2(pos));
	hpBar->SetScale(scale);
	m_pUIObjects[1] = hpBar;

	HPBarObject* mpBar;
	mpBar = new HPBarObject();
	mpBar->SetPosition(pos);
	mpBar->SetScale(scale);
	m_pUIObjects[2] = mpBar;

	for (UINT i = 0; i < m_nObjects; ++i) {
		m_pUIObjects[i]->SetScreenSize(XMFLOAT2(static_cast<float>(FRAME_BUFFER_WIDTH), static_cast<float>(FRAME_BUFFER_HEIGHT)));
		XMUINT2 sizetmp(1, 1);
		sizetmp = GetSpriteSize(i, pTexture, sizetmp);
		m_pUIObjects[i]->SetSize(sizetmp);
		m_pUIObjects[i]->SetType(i);
		m_pUIObjects[i]->CreateCollisionBox();
		m_pUIObjects[i]->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
	}

	delete pTexture;
}

void UIHPBarShader::Animate(float fTimeElapsed)
{
	if (IsZero(fTimeElapsed))
		return;
	ftime += fTimeElapsed;
	if (ftime > 0.01f) {
		ftime = 0.f;
		if (Fog_Flag) {
			if (ypos >= -250) {
				for (int i = 0; i < m_nObjects; i++) {
					XMFLOAT2 tmp = m_pUIObjects[i]->GetPos();
					tmp.y -= 2.5;
					m_pUIObjects[i]->SetPosition(tmp);
				}
				ypos -= 2.5;
			}
		}
		else {
			if (ypos <= 0) {
				for (int i = 0; i < m_nObjects; i++) {
					XMFLOAT2 tmp = m_pUIObjects[i]->GetPos();
					tmp.y += 2.5;
					m_pUIObjects[i]->SetPosition(tmp);
				}
				ypos += 2.5;
			}
		}
	}
}

void UIHPBarShader::FogOn(bool flag)
{
	Fog_Flag = flag;
}


void UITimeShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nRenderTargets, void * pContext)
{
	UINT nTextures = 1;
	m_nObjects = 3;
	m_nPSO = 1;

	CreatePipelineParts();

	m_VSByteCode[0] = D3DUtil::CompileShader(L"UIShader.hlsl", nullptr, "VSUITextured", "vs_5_1");
	m_PSByteCode[0] = D3DUtil::CompileShader(L"UIShader.hlsl", nullptr, "PSDefaultUI", "ps_5_1");

	CTexture *pTexture = new CTexture(nTextures, RESOURCE_TEXTURE2D, 0);

	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\image\\sprite_num.dds", 0);

	UINT ncbElementBytes = D3DUtil::CalcConstantBufferByteSize(sizeof(CB_UI_INFO));

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, pTexture->GetTextureCount());
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_ObjectCB->Resource(), ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 1, true);

	CreateGraphicsRootSignature(pd3dDevice);

	BuildPSO(pd3dDevice, nRenderTargets);

	m_pUIObjects = vector<UIObject*>(m_nObjects);
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
	m_pMaterial->SetReflection(1);

	XMFLOAT2 pos;
	XMFLOAT2 scale = XMFLOAT2(0.2f, 0.4f);

	pos = XMFLOAT2(570, 650);
	UIObject* Time;
	Time = new UIObject();
	Time->SetPosition(pos);
	Time->SetScale(scale);
	m_pUIObjects[0] = Time;

	pos = XMFLOAT2(680, 650);
	UIObject* min1;
	min1 = new UIObject();
	min1->SetPosition(pos);
	min1->SetScale(scale);
	m_pUIObjects[1] = min1;

	pos = XMFLOAT2(750, 650);
	UIObject* min2;
	min2 = new UIObject();
	min2->SetPosition(pos);
	min2->SetScale(scale);
	m_pUIObjects[2] = min2;

	for (UINT i = 0; i < m_nObjects; ++i) {
		m_pUIObjects[i]->SetScreenSize(XMFLOAT2(static_cast<float>(FRAME_BUFFER_WIDTH), static_cast<float>(FRAME_BUFFER_HEIGHT)));
		XMUINT2 sizetmp(1, 1);
		sizetmp = GetSpriteSize(0, pTexture, sizetmp);
		m_pUIObjects[i]->SetSize(sizetmp);
		m_pUIObjects[i]->SetType(i);
		XMUINT2 numsprite(5, 2);
		XMUINT2 nowsprite(0, 0);
		m_pUIObjects[i]->SetNumSprite(numsprite, nowsprite);
		m_pUIObjects[i]->CreateCollisionBox();
		m_pUIObjects[i]->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
	}
	delete pTexture;
}

void UITimeShader::SetTime(int t)
{
	int time = t / 60;
	int min_1 = t % 60 / 10;
	int min_2 = t % 60 % 10;
	XMUINT2 numsprite(5, 2);

	XMUINT2 nowsprite(0, 0);
	nowsprite.x = (time) % 5;
	nowsprite.y = (time) / 5;
	m_pUIObjects[0]->SetNumSprite(numsprite, nowsprite);
	nowsprite.x = (min_1) % 5;
	nowsprite.y = (min_1) / 5;
	m_pUIObjects[1]->SetNumSprite(numsprite, nowsprite);
	nowsprite.x = (min_2) % 5;
	nowsprite.y = (min_2) / 5;
	m_pUIObjects[2]->SetNumSprite(numsprite, nowsprite);

}

void UITimeShader::Animate(float fTimeElapsed)
{
	//Time -= fTimeElapsed;
	//SetTime(Time);
}

void UIDotShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nRenderTargets, void * pContext)
{

	UINT nTextures = 1;
	m_nObjects = 1;
	m_nPSO = 1;

	CreatePipelineParts();

	m_VSByteCode[0] = D3DUtil::CompileShader(L"UIShader.hlsl", nullptr, "VSUITextured", "vs_5_1");
	m_PSByteCode[0] = D3DUtil::CompileShader(L"UIShader.hlsl", nullptr, "PSUIHPBar", "ps_5_1");

	CTexture *pTexture = new CTexture(nTextures, RESOURCE_TEXTURE2D, 0);

	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\image\\dot.dds", 0);

	UINT ncbElementBytes = D3DUtil::CalcConstantBufferByteSize(sizeof(CB_UI_INFO));

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, pTexture->GetTextureCount());
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_ObjectCB->Resource(), ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 1, true);

	CreateGraphicsRootSignature(pd3dDevice);

	BuildPSO(pd3dDevice, nRenderTargets);

	m_pUIObjects = vector<UIObject*>(m_nObjects);
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
	m_pMaterial->SetReflection(1);

	XMFLOAT2 pos = XMFLOAT2(620, 650);	//570,680;
	XMFLOAT2 scale = XMFLOAT2(0.8f, 0.8f);

	UIObject* Time;
	Time = new UIObject();
	Time->SetPosition(pos);
	Time->SetScale(scale);
	m_pUIObjects[0] = Time;

	for (UINT i = 0; i < m_nObjects; ++i) {
		m_pUIObjects[i]->SetScreenSize(XMFLOAT2(static_cast<float>(FRAME_BUFFER_WIDTH), static_cast<float>(FRAME_BUFFER_HEIGHT)));
		XMUINT2 sizetmp(1, 1);
		sizetmp = GetSpriteSize(i, pTexture, sizetmp);
		m_pUIObjects[i]->SetSize(sizetmp);
		m_pUIObjects[i]->SetType(i);
		m_pUIObjects[i]->CreateCollisionBox();
		m_pUIObjects[i]->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
	}
	delete pTexture;
}

void UIReadyShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nRenderTargets, void * pContext)
{
	UINT nTextures = 1;
	m_nObjects = nTextures;
	m_nPSO = 1;

	CreatePipelineParts();

	m_VSByteCode[0] = D3DUtil::CompileShader(L"UIShader.hlsl", nullptr, "VSUITextured", "vs_5_1");
	m_PSByteCode[0] = D3DUtil::CompileShader(L"UIShader.hlsl", nullptr, "PSUIHPBar", "ps_5_1");

	CTexture *pTexture = new CTexture(nTextures, RESOURCE_TEXTURE2D, 0);

	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\image\\ready.dds", 0);

	UINT ncbElementBytes = D3DUtil::CalcConstantBufferByteSize(sizeof(CB_UI_INFO));

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, pTexture->GetTextureCount());
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_ObjectCB->Resource(), ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 1, true);

	CreateGraphicsRootSignature(pd3dDevice);

	BuildPSO(pd3dDevice, nRenderTargets);

	m_pUIObjects = vector<UIObject*>(m_nObjects);
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
	m_pMaterial->SetReflection(1);

	XMFLOAT2 pos = XMFLOAT2(0, 0);	//570,680;
	XMFLOAT2 scale = XMFLOAT2(0.8f, 0.8f);

	MessageObject* Ready;
	Ready = new MessageObject();
	Ready->SetPosition(pos);
	Ready->SetScale(scale);
	Ready->mystate = UI_READY;
	m_pUIObjects[0] = Ready;

	for (UINT i = 0; i < m_nObjects; ++i) {
		m_pUIObjects[i]->SetScreenSize(XMFLOAT2(static_cast<float>(FRAME_BUFFER_WIDTH), static_cast<float>(FRAME_BUFFER_HEIGHT)));
		XMUINT2 sizetmp(1, 1);
		sizetmp = GetSpriteSize(i, pTexture, sizetmp);
		m_pUIObjects[i]->SetSize(sizetmp);
		m_pUIObjects[i]->SetType(i);
		m_pUIObjects[i]->CreateCollisionBox();
		m_pUIObjects[i]->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
	}
	delete pTexture;
}

void UIFightShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nRenderTargets, void * pContext)
{
	UINT nTextures = 1;
	m_nObjects = nTextures;
	m_nPSO = 1;

	CreatePipelineParts();

	m_VSByteCode[0] = D3DUtil::CompileShader(L"UIShader.hlsl", nullptr, "VSUITextured", "vs_5_1");
	m_PSByteCode[0] = D3DUtil::CompileShader(L"UIShader.hlsl", nullptr, "PSUIHPBar", "ps_5_1");

	CTexture *pTexture = new CTexture(nTextures, RESOURCE_TEXTURE2D, 0);

	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\image\\fight.dds", 0);

	UINT ncbElementBytes = D3DUtil::CalcConstantBufferByteSize(sizeof(CB_UI_INFO));

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, pTexture->GetTextureCount());
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_ObjectCB->Resource(), ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 1, true);

	CreateGraphicsRootSignature(pd3dDevice);

	BuildPSO(pd3dDevice, nRenderTargets);

	m_pUIObjects = vector<UIObject*>(m_nObjects);
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
	m_pMaterial->SetReflection(1);

	XMFLOAT2 pos = XMFLOAT2(0, 0);	//570,680;
	XMFLOAT2 scale = XMFLOAT2(0.8f, 0.8f);

	MessageObject* Fight;
	Fight = new MessageObject();
	Fight->SetPosition(pos);
	Fight->SetScale(scale);
	Fight->mystate = UI_FIGHT;
	m_pUIObjects[0] = Fight;

	for (UINT i = 0; i < m_nObjects; ++i) {
		m_pUIObjects[i]->SetScreenSize(XMFLOAT2(static_cast<float>(FRAME_BUFFER_WIDTH), static_cast<float>(FRAME_BUFFER_HEIGHT)));
		XMUINT2 sizetmp(1, 1);
		sizetmp = GetSpriteSize(i, pTexture, sizetmp);
		m_pUIObjects[i]->SetSize(sizetmp);
		m_pUIObjects[i]->SetType(i);
		m_pUIObjects[i]->CreateCollisionBox();
		m_pUIObjects[i]->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
	}
	delete pTexture;
}

void MessageShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nRenderTargets, void * pContext)
{
	UINT nTextures = 5;
	m_nObjects = nTextures;
	m_nPSO = 1;

	CreatePipelineParts();

	m_VSByteCode[0] = D3DUtil::CompileShader(L"UIShader.hlsl", nullptr, "VSUITextured", "vs_5_1");
	m_PSByteCode[0] = D3DUtil::CompileShader(L"UIShader.hlsl", nullptr, "PSUIHPBar", "ps_5_1");

	CTexture *pTexture = new CTexture(nTextures, RESOURCE_TEXTURE2D, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\image\\message_weapon.dds", 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\image\\message_fever.dds", 1);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\image\\message_fog.dds", 2);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\image\\message_cupcake.dds", 3);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\image\\message_lightning.dds", 4); 

	UINT ncbElementBytes = D3DUtil::CalcConstantBufferByteSize(sizeof(CB_UI_INFO));

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, pTexture->GetTextureCount());
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_ObjectCB->Resource(), ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 1, true);

	CreateGraphicsRootSignature(pd3dDevice);

	BuildPSO(pd3dDevice, nRenderTargets);

	m_pUIObjects = vector<UIObject*>(m_nObjects);
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
	m_pMaterial->SetReflection(1);

	XMFLOAT2 pos = XMFLOAT2(640, 590);	//570,680;
	XMFLOAT2 scale = XMFLOAT2(0.6f, 0.6f);

	UIObject* weapon;
	weapon = new UIObject();
	weapon->SetPosition(pos);
	weapon->SetScale(scale);
	weapon->m_bEnabled = false;
	m_pUIObjects[0] = weapon;

	UIObject* fever;
	fever = new UIObject();
	fever->SetPosition(pos);
	fever->SetScale(scale);
	fever->m_bEnabled = false;
	m_pUIObjects[1] = fever;

	UIObject* fog;
	fog = new UIObject();
	fog->SetPosition(pos);
	fog->SetScale(scale);
	fog->m_bEnabled = false;
	m_pUIObjects[2] = fog;

	UIObject* cupcake;
	cupcake = new UIObject();
	cupcake->SetPosition(pos);
	cupcake->SetScale(scale);
	cupcake->m_bEnabled = false;
	m_pUIObjects[3] = cupcake;

	UIObject* lightning;
	lightning = new UIObject();
	lightning->SetPosition(pos);
	lightning->SetScale(scale);
	lightning->m_bEnabled = false;
	m_pUIObjects[4] = lightning;

	for (UINT i = 0; i < MESSAGE_NUM; ++i) {
		m_pUIObjects[i]->SetScreenSize(XMFLOAT2(static_cast<float>(FRAME_BUFFER_WIDTH), static_cast<float>(FRAME_BUFFER_HEIGHT)));
		XMUINT2 sizetmp(1, 1);
		sizetmp = GetSpriteSize(i, pTexture, sizetmp);
		m_pUIObjects[i]->SetSize(sizetmp);
		m_pUIObjects[i]->SetType(i);
		m_pUIObjects[i]->CreateCollisionBox();
		m_pUIObjects[i]->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
	}
	delete pTexture;
}

void MessageShader::Animate(float fTimeElapsed)
{
	for(UINT i=0;i<MESSAGE_NUM;++i)
	if (m_pUIObjects[i]->m_bEnabled) {
		ShowTime += fTimeElapsed;
		if (ShowTime > 5)		//보이는 시간 조정 (sec)
			m_pUIObjects[i]->m_bEnabled = false;
	}
}

void MessageShader::ShowMessage(int message_type)
{
	m_pUIObjects[message_type]->m_bEnabled = true;
	ShowTime = 0.f;
}


void UIIDShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nRenderTargets, void * pContext)
{
	UINT nTextures = 1;
	m_nObjects = 10;
	m_nPSO = 1;

	CreatePipelineParts();

	m_VSByteCode[0] = D3DUtil::CompileShader(L"UIShader.hlsl", nullptr, "VSUITextured", "vs_5_1");
	m_PSByteCode[0] = D3DUtil::CompileShader(L"UIShader.hlsl", nullptr, "PSDefaultUI", "ps_5_1");

	CTexture *pTexture = new CTexture(nTextures, RESOURCE_TEXTURE2D, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\image\\text.dds", 0);

	UINT ncbElementBytes = D3DUtil::CalcConstantBufferByteSize(sizeof(CB_UI_INFO));

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, pTexture->GetTextureCount());
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_ObjectCB->Resource(), ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 1, true);

	CreateGraphicsRootSignature(pd3dDevice);

	BuildPSO(pd3dDevice, nRenderTargets);

	m_pUIObjects = vector<UIObject*>(m_nObjects);
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
	m_pMaterial->SetReflection(1);

	XMFLOAT2 pos = XMFLOAT2(0, 0);
	XMFLOAT2 scale = XMFLOAT2(1, 0.08f);
	
	//ID
	UIObject* text[10];
	for (int i = 0; i < 10; ++i) {
		text[i] = new UIObject();
		text[i]->SetScale(scale);
		m_pUIObjects[i] = text[i];
	}
	for (int i = 0; i < 10; ++i) {
		m_pUIObjects[i]->SetScreenSize(XMFLOAT2(static_cast<float>(FRAME_BUFFER_WIDTH), static_cast<float>(FRAME_BUFFER_HEIGHT)));
		XMUINT2 sizetmp(1, 1);
		sizetmp = GetSpriteSize(0, pTexture, sizetmp);
		m_pUIObjects[i]->SetSize(sizetmp);
		m_pUIObjects[i]->SetType(0);
		XMUINT2 numsprite(1, 36);
		XMUINT2 nowsprite(0, 0);
		m_pUIObjects[i]->SetNumSprite(numsprite, nowsprite);
		m_pUIObjects[i]->CreateCollisionBox();
		m_pUIObjects[i]->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
	}


	delete pTexture;
}

void UIIDShader::SetID(wchar_t * str)
{
	int ID_Len = lstrlen(str);	
	//cout << "idlen:" << ID_Len << endl;
	XMUINT2 numsprite(1, 36);
	XMUINT2 nowsprite(0, 0);
	for (int i = 0; i < m_nObjects; ++i) {
		m_pUIObjects[i]->m_bEnabled = false;
	}
	if (ID_Len == 0)
		return;
	for (int i = 0; i <= ID_Len; ++i) {

		if (str[i] >= L'0'&&str[i] <= L'9') {
			m_pUIObjects[i]->m_bEnabled = true;
			int tmp = str[i] - L'0';
			nowsprite = XMUINT2(1, tmp);
			m_pUIObjects[i]->SetNumSprite(numsprite, nowsprite);

		}
		else if (str[i] >= L'A'&&str[i] <= L'Z') {
			m_pUIObjects[i]->m_bEnabled = true;
			int tmp = str[i] - L'A'+ 10;
			nowsprite = XMUINT2(1, tmp);
			m_pUIObjects[i]->SetNumSprite(numsprite, nowsprite);
		}

	}
}

void UIIDShader::Animate(float fTimeElapsed)
{
	if (IsZero(fTimeElapsed))
		return;
	ftime += fTimeElapsed;
	if (ftime > 0.01f) {
		ftime = 0.f;
		if (Fog_Flag) {
			if (ypos >= -250) {
				for (int i = 0; i < m_nObjects; i++) {
					XMFLOAT2 tmp = m_pUIObjects[i]->GetPos();
					tmp.y -= 2.5;
					m_pUIObjects[i]->SetPosition(tmp);
				}
				ypos -= 2.5;
			}
		}
		else {
			if (ypos <= 0) {
				for (int i = 0; i < m_nObjects; i++) {
					XMFLOAT2 tmp = m_pUIObjects[i]->GetPos();
					tmp.y += 2.5;
					m_pUIObjects[i]->SetPosition(tmp);
				}
				ypos += 2.5;
			}
		}
	}
}

void UIIDShader::FogOn(bool flag)
{
	Fog_Flag = flag;
}

void WinLoseShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nRenderTargets, void * pContext)
{
	UINT nTextures = 2;
	m_nObjects = nTextures;
	m_nPSO = 1;

	CreatePipelineParts();

	m_VSByteCode[0] = D3DUtil::CompileShader(L"UIShader.hlsl", nullptr, "VSUITextured", "vs_5_1");
	m_PSByteCode[0] = D3DUtil::CompileShader(L"UIShader.hlsl", nullptr, "PSUIHPBar", "ps_5_1");

	CTexture *pTexture = new CTexture(nTextures, RESOURCE_TEXTURE2D, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\image\\win.dds", 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\image\\lose.dds", 1);


	UINT ncbElementBytes = D3DUtil::CalcConstantBufferByteSize(sizeof(CB_UI_INFO));

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, pTexture->GetTextureCount());
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_ObjectCB->Resource(), ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 1, true);

	CreateGraphicsRootSignature(pd3dDevice);

	BuildPSO(pd3dDevice, nRenderTargets);

	m_pUIObjects = vector<UIObject*>(m_nObjects);
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
	m_pMaterial->SetReflection(1);

	XMFLOAT2 pos = XMFLOAT2(-100, 0);	//570,680;
	XMFLOAT2 scale = XMFLOAT2(0.8f, 0.8f);

	UIObject* win;
	win = new UIObject();
	pos = XMFLOAT2(650, 400);
	win->SetPosition(pos);
	win->SetScale(scale);
	win->m_bEnabled = false;
	m_pUIObjects[0] = win;

	UIObject* lose;
	lose = new UIObject();
	lose->SetPosition(pos);
	lose->SetScale(scale);
	lose->m_bEnabled = false;
	m_pUIObjects[1] = lose;



	for (UINT i = 0; i < 2; ++i) {
		m_pUIObjects[i]->SetScreenSize(XMFLOAT2(static_cast<float>(FRAME_BUFFER_WIDTH), static_cast<float>(FRAME_BUFFER_HEIGHT)));
		XMUINT2 sizetmp(1, 1);
		sizetmp = GetSpriteSize(i, pTexture, sizetmp);
		m_pUIObjects[i]->SetSize(sizetmp);
		m_pUIObjects[i]->SetType(i);
		m_pUIObjects[i]->CreateCollisionBox();
		m_pUIObjects[i]->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
	}
	delete pTexture;
}

void WinLoseShader::ShowMessage(bool win)
{
	if (win) {
		m_pUIObjects[0]->m_bEnabled = true;
	}
	else {
		m_pUIObjects[1]->m_bEnabled = true;
	}
}

void UILoadingShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nRenderTargets, void * pContext)
{
	UINT nTextures = 1;
	m_nObjects = 1;
	m_nPSO = 1;

	CreatePipelineParts();

	m_VSByteCode[0] = D3DUtil::CompileShader(L"UIShader.hlsl", nullptr, "VSUITextured", "vs_5_1");
	m_PSByteCode[0] = D3DUtil::CompileShader(L"UIShader.hlsl", nullptr, "PSDefaultUI", "ps_5_1");

	CTexture *pTexture = new CTexture(nTextures, RESOURCE_TEXTURE2D, 0);

	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\image\\Loading.dds", 0);

	UINT ncbElementBytes = D3DUtil::CalcConstantBufferByteSize(sizeof(CB_UI_INFO));

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, pTexture->GetTextureCount());
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_ObjectCB->Resource(), ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 1, true);

	CreateGraphicsRootSignature(pd3dDevice);

	BuildPSO(pd3dDevice, nRenderTargets);

	m_pUIObjects = vector<UIObject*>(m_nObjects);
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
	m_pMaterial->SetReflection(1);

	XMFLOAT2 pos = XMFLOAT2(FRAME_BUFFER_WIDTH/2, FRAME_BUFFER_HEIGHT/2);	//570,680;
	XMFLOAT2 scale = XMFLOAT2(1.f, 1.f);

	UIObject* image;
	image = new UIObject();
	image->SetPosition(pos);
	image->SetScale(scale);
	image->SetAlpha(1.f);
	m_pUIObjects[0] = image;

	for (UINT i = 0; i < m_nObjects; ++i) {
		m_pUIObjects[i]->SetScreenSize(XMFLOAT2(static_cast<float>(FRAME_BUFFER_WIDTH), static_cast<float>(FRAME_BUFFER_HEIGHT)));
		XMUINT2 sizetmp(1, 1);
		sizetmp = GetSpriteSize(i, pTexture, sizetmp);
		m_pUIObjects[i]->SetSize(sizetmp);
		m_pUIObjects[i]->SetType(i);
		XMUINT2 numsprite(1, 1);
		XMUINT2 nowsprite(0, 0);
		m_pUIObjects[i]->SetNumSprite(numsprite, nowsprite);
		m_pUIObjects[i]->CreateCollisionBox();
		m_pUIObjects[i]->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
	}
	//delete pTexture;
}

void DarkShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nRenderTargets, void * pContext)
{
	UINT nTextures = 1;
	m_nObjects = 1;
	m_nPSO = 1;

	CreatePipelineParts();

	m_VSByteCode[0] = D3DUtil::CompileShader(L"UIShader.hlsl", nullptr, "VSUITextured", "vs_5_1");
	m_PSByteCode[0] = D3DUtil::CompileShader(L"UIShader.hlsl", nullptr, "PSDark", "ps_5_1");

	CTexture *pTexture = new CTexture(nTextures, RESOURCE_TEXTURE2D, 0);

	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\image\\Loading.dds", 0);

	UINT ncbElementBytes = D3DUtil::CalcConstantBufferByteSize(sizeof(CB_UI_INFO));

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, pTexture->GetTextureCount());
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_ObjectCB->Resource(), ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 1, true);

	CreateGraphicsRootSignature(pd3dDevice);

	BuildPSO(pd3dDevice, nRenderTargets);

	m_pUIObjects = vector<UIObject*>(m_nObjects);
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
	m_pMaterial->SetReflection(1);

	XMFLOAT2 pos = XMFLOAT2(FRAME_BUFFER_WIDTH / 2, FRAME_BUFFER_HEIGHT / 2);	//570,680;
	XMFLOAT2 scale = XMFLOAT2(1.f, 1.f);

	UIObject* image;
	image = new UIObject();
	image->SetPosition(pos);
	image->SetScale(scale);
	image->SetAlpha(1.f);
	image->m_bEnabled = true;
	m_pUIObjects[0] = image;

	for (UINT i = 0; i < m_nObjects; ++i) {
		m_pUIObjects[i]->SetScreenSize(XMFLOAT2(static_cast<float>(FRAME_BUFFER_WIDTH), static_cast<float>(FRAME_BUFFER_HEIGHT)));
		XMUINT2 sizetmp(1, 1);
		sizetmp = GetSpriteSize(i, pTexture, sizetmp);
		m_pUIObjects[i]->SetSize(sizetmp);
		m_pUIObjects[i]->SetType(i);
		XMUINT2 numsprite(1, 1);
		XMUINT2 nowsprite(0, 0);
		m_pUIObjects[i]->SetNumSprite(numsprite, nowsprite);
		m_pUIObjects[i]->CreateCollisionBox();
		m_pUIObjects[i]->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
	}
	delete pTexture;
}

void DarkShader::Animate(float fTimeElapsed)
{
	if (IsZero(fTimeElapsed))
		return;
	ftime += fTimeElapsed;
	if (ftime > 0.1f) {
		if (!is_dark) {
			if (m_pUIObjects[0]->m_fAlpha >= 0.f) {
				m_pUIObjects[0]->m_fAlpha -= 0.05f;
				//cout << "밝아져" << endl;
			}
		}
		else {
			if (m_pUIObjects[0]->m_fAlpha < 0.5f) {
				m_pUIObjects[0]->m_fAlpha += 0.05f;
				//cout << "어두워져" << endl;
			}
		}
		ftime = 0.f;
	}
}
