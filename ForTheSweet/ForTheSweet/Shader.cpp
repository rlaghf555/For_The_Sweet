#include "stdafx.h"
#include "Shader.h"
#include <math.h>

CompiledShaders::CompiledShaders()
{
}

ComPtr<ID3DBlob> CompiledShaders::GetCompiledShader(const wstring & filename, const D3D_SHADER_MACRO * defines, const string & entrypoint, const string & target)
{
	if (CompiledShader[entrypoint] == nullptr)
		CompiledShader[entrypoint] = D3DUtil::CompileShader(filename, defines, entrypoint, target);

	return CompiledShader[entrypoint];
}

CompiledShaders * CompiledShaders::Instance()
{
	static CompiledShaders instance;

	return &instance;
}

CShader::CShader()
{}

CShader::~CShader()
{
	if (m_ppd3dPipelineStates)
	{
		for (int i = 0; i < m_nPipelineStates; i++)
			if (m_ppd3dPipelineStates[i])
				m_ppd3dPipelineStates[i]->Release();
		delete[] m_ppd3dPipelineStates;
	}
}

//래스터라이저 상태를 설정하기 위한 구조체를 반환한다.
D3D12_RASTERIZER_DESC CShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));

	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	//d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;

	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	//d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_FRONT;

	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;

	d3dRasterizerDesc.DepthClipEnable = TRUE;
	//d3dRasterizerDesc.DepthClipEnable = FALSE;

	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	return(d3dRasterizerDesc);
}

//깊이-스텐실 검사를 위한 상태를 설정하기 위한 구조체를 반환한다.
D3D12_DEPTH_STENCIL_DESC CShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	return(d3dDepthStencilDesc);
}

//블렌딩 상태를 설정하기 위한 구조체를 반환한다.
D3D12_BLEND_DESC CShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;			// FALSE
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = TRUE;	// FALSE
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;	//D3D12_BLEND_ONE
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//D3D12_BLEND_ZERO
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	return(d3dBlendDesc);
}

//입력 조립기에게 정점 버퍼의 구조를 알려주기 위한 구조체를 반환한다.
D3D12_INPUT_LAYOUT_DESC CShader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = NULL;
	d3dInputLayoutDesc.NumElements = 0;
	return(d3dInputLayoutDesc);
}

//정점 셰이더 바이트 코드를 생성(컴파일)한다.
D3D12_SHADER_BYTECODE CShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;
	return(d3dShaderByteCode);
}

//픽셀 셰이더 바이트 코드를 생성(컴파일)한다.
D3D12_SHADER_BYTECODE CShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;
	return(d3dShaderByteCode);
}

//셰이더 소스 코드를 컴파일하여 바이트 코드 구조체를 반환한다.
D3D12_SHADER_BYTECODE CShader::CompileShaderFromFile(WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob **ppd3dShaderBlob)
{
	UINT nCompileFlags = 0;
#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	::D3DCompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderProfile, nCompileFlags, 0, ppd3dShaderBlob, NULL);
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
	d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();
	return(d3dShaderByteCode);
}

//그래픽스 파이프라인 상태 객체를 생성한다.
void CShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	ID3DBlob *pd3dVertexShaderBlob = NULL, *pd3dPixelShaderBlob = NULL;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
	d3dPipelineStateDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob);
	d3dPipelineStateDesc.PS = CreatePixelShader(&pd3dPixelShaderBlob);
	d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	d3dPipelineStateDesc.BlendState = CreateBlendState();
	d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	d3dPipelineStateDesc.SampleMask = UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineStateDesc.NumRenderTargets = 1;
	d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineStateDesc.SampleDesc.Count = 1;
	d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc,
		__uuidof(ID3D12PipelineState), (void **)&m_ppd3dPipelineStates[0]);
	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();
	if (d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[]
		d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

void CShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
}
void CShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
}
void CShader::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4 *pxmf4x4World)
{
	XMFLOAT4X4 xmf4x4World;
	XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);
}
void CShader::ReleaseShaderVariables()
{
}

void CShader::OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList)
{

	//파이프라인에 그래픽스 상태 객체를 설정한다.
	pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[0]);
}
void CShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	OnPrepareRender(pd3dCommandList);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

CObjectsShader::CObjectsShader()
{
}
CObjectsShader::~CObjectsShader()
{
}

D3D12_INPUT_LAYOUT_DESC CObjectsShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;
	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CObjectsShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	wchar_t filename[100] = L"Shaders.hlsl";
	return(CShader::CompileShaderFromFile(filename, "VSDiffused", "vs_5_1", ppd3dShaderBlob));
}
D3D12_SHADER_BYTECODE CObjectsShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	wchar_t filename[100] = L"Shaders.hlsl";
	return(CShader::CompileShaderFromFile(filename, "PSDiffused", "ps_5_1", ppd3dShaderBlob));
}

void CObjectsShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature	*pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];
	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}

void CObjectsShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT3 Position)
{


}

void CObjectsShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, XMFLOAT3 Position, int Object_Kind)
{
	if (Object_Kind == OBJECT_MAP) {
		m_nObjects = 1;
		m_ppObjects = vector<CGameObject*>(m_nObjects);

		CGameObject *Map_Object = NULL;

		Map_Object = new CGameObject();
		setPos(Position);
		Map_Object->SetPosition(Pos_act.x, Pos_act.y, Pos_act.z);
		m_ppObjects[0] = Map_Object;

		CMesh *pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 2000, 20, 1500);
		m_ppObjects[0]->SetMesh(pCubeMesh);

		CreateShaderVariables(pd3dDevice, pd3dCommandList);
	}

}

void CObjectsShader::ReleaseObjects()
{
	if (m_pMaterial) {
		m_pMaterial->ReleaseShaderVariables();
		m_pMaterial->ReleaseUploadBuffers();
		m_pMaterial->Release();
	}
	if (m_ppObjects.size())
	{
		//vector<CGameObject*> a;
		//m_ppObjects.swap(a);
		//a.clear();
		//m_ppObjects.clear();
		//for (int j = 0; j < m_ppObjects.size(); j++)
		//{
		//	if (m_ppObjects[j])
		//		delete m_ppObjects[j];
		//}
		//m_ppObjects[0]->Release();

	}
	//if (m_bbObjects.size()>0)
	//{
	//	vector<ModelObject*> a;
	//	m_bbObjects.swap(a);
	//	a.clear();
	//	m_bbObjects.clear();
	//}
	//
	for (UINT j = 0; j < m_bbObjects.size(); j++)
	{
		if (m_bbObjects[j]) {
			delete m_bbObjects[j];
		}

	}
	m_bbObjects.clear();
}

void CObjectsShader::AnimateObjects(float fTimeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(fTimeElapsed);
	}
}

void CObjectsShader::ReleaseUploadBuffers()
{
	if (m_ppObjects.size() > 0)
	{
		for (int j = 0; j < m_nObjects; j++) m_ppObjects[j]->ReleaseUploadBuffers();
	}
	if (m_bbObjects.size() > 0) {
		for (int j = m_bbObjects.size() - 1; j >= 0; j--) m_bbObjects[j]->ReleaseUploadBuffers();
	}
}

void CObjectsShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);
	for (int j = 0; j < m_nObjects; j++) { if (m_ppObjects[j]) { m_ppObjects[j]->Render(pd3dCommandList, pCamera); } }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////

CInstancingShader::CInstancingShader()
{
}

CInstancingShader::~CInstancingShader()
{
}

D3D12_INPUT_LAYOUT_DESC CInstancingShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	//정점 정보를 위한 입력 원소이다.
	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;
	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CInstancingShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	wchar_t filename[100] = L"Shaders.hlsl";
	return(CShader::CompileShaderFromFile(filename, "VSInstancing", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CInstancingShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	wchar_t filename[100] = L"Shaders.hlsl";
	return(CShader::CompileShaderFromFile(filename, "PSInstancing", "ps_5_1", ppd3dShaderBlob));
}

void CInstancingShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];
	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}

void CInstancingShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	//인스턴스 정보를 저장할 정점 버퍼를 업로드 힙 유형으로 생성한다.
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, sizeof(VS_VB_INSTANCE) * m_nObjects, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, NULL);

	//정점 버퍼(업로드 힙)에 대한 포인터를 저장한다.
	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CInstancingShader::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects) m_pd3dcbGameObjects->Unmap(0, NULL);
	if (m_pd3dcbGameObjects) m_pd3dcbGameObjects->Release();
}

//인스턴싱 정보(객체의 월드 변환 행렬과 색상)를 정점 버퍼에 복사한다.
void CInstancingShader::UpdateShaderVariables(ID3D12GraphicsCommandList	*pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootShaderResourceView(2,
		m_pd3dcbGameObjects->GetGPUVirtualAddress());
	for (int j = 0; j < m_nObjects; j++)
	{
		m_pcbMappedGameObjects[j].m_xmcColor = (j % 2) ? XMFLOAT4(0.5f, 0.0f, 0.0f, 0.0f) : XMFLOAT4(0.0f, 0.0f, 0.5f, 0.0f);
		XMStoreFloat4x4(&m_pcbMappedGameObjects[j].m_xmf4x4Transform,
			XMMatrixTranspose(XMLoadFloat4x4(&m_ppObjects[j]->m_xmf4x4World)));
	}
}

void CInstancingShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	int xObjects = 1, yObjects = 1, zObjects = 1, i = 0;
	m_nObjects = (xObjects * 2 + 1) * (yObjects * 2 + 1) * (zObjects * 2 + 1);
	m_ppObjects = vector<CGameObject*>(m_nObjects);
	float fxPitch = 12.0f * 2.5f;
	float fyPitch = 12.0f * 2.5f;
	float fzPitch = 12.0f * 2.5f;
	CRotatingObject *pRotatingObject = NULL;
	for (int x = -xObjects; x <= xObjects; x++)
	{
		for (int y = -yObjects; y <= yObjects; y++)
		{
			for (int z = -zObjects; z <= zObjects; z++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetPosition(fxPitch*x, fyPitch*y, fzPitch*z);
				pRotatingObject->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
				pRotatingObject->SetRotationSpeed(10.0f*(i % 10));
				m_ppObjects[i++] = pRotatingObject;
			}
		}
	}
	//인스턴싱을 사용하여 렌더링하기 위하여 하나의 게임 객체만 메쉬를 가진다.
	CCubeMeshDiffused *pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 12.0f, 12.0f, 12.0f);
	m_ppObjects[0]->SetMesh(pCubeMesh);
	//인스턴싱을 위한 버퍼(Structured Buffer)를 생성한다.
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CInstancingShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CObjectsShader::Render(pd3dCommandList, pCamera);
	//모든 게임 객체의 인스턴싱 데이터를 버퍼에 저장한다.
	UpdateShaderVariables(pd3dCommandList);
	//하나의 정점 데이터를 사용하여 모든 게임 객체(인스턴스)들을 렌더링한다.
	m_ppObjects[0]->Render(pd3dCommandList, pCamera, m_nObjects);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

CModelShader::CModelShader() : modelIndex(0)
{
}

CModelShader::CModelShader(Model_Animation *ma) : modelIndex(0)
{
	model_anim = ma;
}

CModelShader::CModelShader(LoadModel *ma) : modelIndex(0)
{
	static_model = ma;
}

CModelShader::CModelShader(UINT index) : modelIndex(index)
{
}

CModelShader::~CModelShader()
{
}

void CModelShader::OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList, int index)
{
	if (m_RootSignature[index])
		pd3dCommandList->SetGraphicsRootSignature(m_RootSignature[index].Get());

	if (m_pPSO[index])
		pd3dCommandList->SetPipelineState(m_pPSO[index].Get());

	pd3dCommandList->SetDescriptorHeaps(1, m_CbvSrvDescriptorHeap.GetAddressOf());

	UpdateShaderVariables(pd3dCommandList);
}

void CModelShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature*pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];
	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}

D3D12_INPUT_LAYOUT_DESC CModelShader::CreateInputLayout(int index)
{
	D3D12_INPUT_LAYOUT_DESC inputLayout;

	m_pInputElementDesc =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	0,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0,	36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BORNINDEX",	0, DXGI_FORMAT_R32G32B32A32_UINT,	0,	44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "WEIGHT",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	60, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXINDEX",	0, DXGI_FORMAT_R32_UINT,			0,	72, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	inputLayout = { m_pInputElementDesc.data(), (UINT)m_pInputElementDesc.size() };

	return inputLayout;
}

void CModelShader::CreateCbvAndSrvDescriptorHeaps(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nConstantBufferViews, int nShaderResourceViews, bool bIsGraphics)
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

void CModelShader::CreateConstantBufferViews(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nConstantBufferViews, ID3D12Resource * pd3dConstantBuffers, UINT nStride)
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

void CModelShader::CreateGraphicsRootSignature(ID3D12Device * pd3dDevice)
{
	int i = 0;
	ComPtr<ID3D12RootSignature> pd3dGraphicsRootSignature = nullptr;

	CD3DX12_DESCRIPTOR_RANGE pd3dDescriptorRanges[2];

	pd3dDescriptorRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, CBVObjectInfo, 0, 0); // GameObject
	pd3dDescriptorRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, SRVTexArray, 0, 0); // Texture

	CD3DX12_ROOT_PARAMETER pd3dRootParameters[3];

	pd3dRootParameters[0].InitAsConstantBufferView(1);
	pd3dRootParameters[1].InitAsDescriptorTable(1, &pd3dDescriptorRanges[0], D3D12_SHADER_VISIBILITY_ALL);
	pd3dRootParameters[2].InitAsDescriptorTable(1, &pd3dDescriptorRanges[1], D3D12_SHADER_VISIBILITY_PIXEL);

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

	ThrowIfFailed(pd3dDevice->CreateRootSignature(0,
		pd3dSignatureBlob->GetBufferPointer(),
		pd3dSignatureBlob->GetBufferSize(),
		IID_PPV_ARGS(m_RootSignature[PSO_SHADOWMAP].GetAddressOf()))
	);
}

D3D12_SHADER_BYTECODE CModelShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	wchar_t filename[100] = L"Model.hlsl";
	return(CShader::CompileShaderFromFile(filename, "VSStaticModel", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CModelShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	wchar_t filename[100] = L"Model.hlsl";
	return(CShader::CompileShaderFromFile(filename, "PSDynamicModel", "ps_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_RESOURCE_VIEW_DESC GetShaderResourceViewDesc(D3D12_RESOURCE_DESC d3dResourceDesc, UINT nTextureType)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = {};
	d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
	d3dShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	switch (nTextureType)
	{
	case RESOURCE_TEXTURE2D: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 1)
	case RESOURCE_TEXTURE2D_ARRAY:
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		d3dShaderResourceViewDesc.Texture2D.MipLevels = -1;
		d3dShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
		d3dShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		break;

	case RESOURCE_TEXTURE2DARRAY: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize != 1)
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		d3dShaderResourceViewDesc.Texture2DArray.MipLevels = -1;
		d3dShaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.Texture2DArray.PlaneSlice = 0;
		d3dShaderResourceViewDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
		d3dShaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
		d3dShaderResourceViewDesc.Texture2DArray.ArraySize = d3dResourceDesc.DepthOrArraySize;
		break;

	case RESOURCE_TEXTURE_CUBE: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 6)
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		d3dShaderResourceViewDesc.TextureCube.MipLevels = -1;
		d3dShaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		break;

	case RESOURCE_BUFFER: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		d3dShaderResourceViewDesc.Buffer.FirstElement = 0;
		d3dShaderResourceViewDesc.Buffer.NumElements = 0;
		d3dShaderResourceViewDesc.Buffer.StructureByteStride = 0;
		d3dShaderResourceViewDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		break;

	case RESOURCE_TEXTURE2D_SHADOWMAP:
		d3dShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		d3dShaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		d3dShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.Texture2D.MipLevels = 1;
		d3dShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		d3dShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
		break;
	case RESOURCE_BUFFER_FLOAT32:
		d3dShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		d3dShaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		d3dShaderResourceViewDesc.Buffer.FirstElement = 0;
		d3dShaderResourceViewDesc.Buffer.NumElements = 1;
		break;
	case RESOURCE_TEXTURE2D_HDR:
		d3dShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		d3dShaderResourceViewDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		d3dShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.Texture2D.MipLevels = 1;
		d3dShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		d3dShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
		break;
	}
	return(d3dShaderResourceViewDesc);
}

void CModelShader::CreateShaderResourceViews(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, CTexture * pTexture, UINT nRootParameterStartIndex, bool bAutoIncrement, bool bIsGraphics)
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

void CModelShader::BuildPSO(ID3D12Device * pd3dDevice, UINT nRenderTargets, int index)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	::ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	ID3DBlob *pd3dVertexShaderBlob = NULL, *pd3dPixelShaderBlob = NULL;
	psoDesc.InputLayout = CreateInputLayout(index);
	psoDesc.pRootSignature = m_RootSignature[index].Get();
	psoDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob);
	psoDesc.PS = CreatePixelShader(&pd3dPixelShaderBlob);
	psoDesc.RasterizerState = CreateRasterizerState(index);
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
	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();
}

D3D12_RASTERIZER_DESC CModelShader::CreateRasterizerState(int index)
{
	CD3DX12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	if (index == PSO_SHADOWMAP) {
		rasterizerDesc.DepthBias = 100000;
		rasterizerDesc.DepthBiasClamp = 0.0f;
		rasterizerDesc.SlopeScaledDepthBias = 1.0f;
	}
	return rasterizerDesc;
}

D3D12_BLEND_DESC CModelShader::CreateBlendState(int index)
{
	return CD3DX12_BLEND_DESC(D3D12_DEFAULT);
}

D3D12_DEPTH_STENCIL_DESC CModelShader::CreateDepthStencilState(int index)
{
	return CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
}

void CModelShader::CreateShaderVariables(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	// 오브젝트 수 만큼 메모리 할당 
	m_ObjectCB = make_unique<UploadBuffer<CB_GAMEOBJECT_INFO>>(pd3dDevice, m_nObjects, true);
}

void CModelShader::UpdateShaderVariables(ID3D12GraphicsCommandList * pd3dCommandList)
{
	CB_GAMEOBJECT_INFO cBuffer;
	for (UINT i = 0; i < m_nObjects; ++i) {
		XMStoreFloat4x4(&cBuffer.m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_bbObjects[i]->m_xmf4x4World)));
		cBuffer.m_nMaterial = 0;
		m_ObjectCB->CopyData(i, cBuffer);
	}
}

void CModelShader::ReleaseShaderVariables()
{
	if (m_ObjectCB)
		m_ObjectCB->~UploadBuffer();
}

void CModelShader::CreatePipelineParts()
{
	if (m_nPSO > 0) {
		m_pPSO = new ComPtr<ID3D12PipelineState>[m_nPSO];
		m_RootSignature = new ComPtr<ID3D12RootSignature>[m_nPSO];

		//m_VSByteCode = new ComPtr<ID3DBlob>[m_nPSO];
		//m_PSByteCode = new ComPtr<ID3DBlob>[m_nPSO];
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

void CModelShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, CPhysx* physx, int map_type, int nRenderTargets, void * pContext)
{
	m_nPSO = 1;
	CreatePipelineParts();

	m_nObjects = 1;
	m_bbObjects = vector<ModelObject*>(m_nObjects);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, 1);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_ObjectCB->Resource(), D3DUtil::CalcConstantBufferByteSize(sizeof(CB_GAMEOBJECT_INFO)));

	CreateGraphicsRootSignature(pd3dDevice);
	BuildPSO(pd3dDevice, nRenderTargets);

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	if (map_type == M_Map_1)pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\map\\map_2.dds", 0);
	if (map_type == M_Map_1_macaron_1)pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\map\\map_macaron.dds", 0);

	if (map_type == M_Map_2)pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\map\\map_oreo.dds", 0);
	if (map_type == M_Map_1_macaron || map_type == M_Map_1_macaron_2)pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\map\\map_macaron_2.dds", 0);
	if (map_type == M_Map_2_chocolate_bar || map_type == M_Map_2_chocolate_bar_2) pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\map\\map_chocolate_bar.dds", 0);

	if (map_type == M_Map_3 || map_type == M_Map_3_2) pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\map\\map_3_cake_1.dds", 0);
	if (map_type == M_Map_3_cake_2 || map_type == M_Map_3_cake_2_2) pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\map\\map_3_cake_2.dds", 0);
	if (map_type == M_Map_3_cake_3 || map_type == M_Map_3_cake_3_2) pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\map\\map_3_cake_3.dds", 0);
	if (map_type == M_Map_1_macaron_3) pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\map\\map_macaron.dds", 0);
	if (map_type == M_Map_3_in || map_type == M_Map_3_in_2) pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\map\\map_3_cake_in.dds", 0);
	if (map_type == M_Map_3_in_stair_1 || map_type == M_Map_3_in_stair_1_2) pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\map\\test_1.dds", 0);
	if (map_type == M_Map_3_in_stair_2 || map_type == M_Map_3_in_stair_2_2) pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\map\\test_1.dds", 0);
	if (map_type == M_Map_3_in_wall_collision || map_type == M_Map_3_in_wall_collision_2) pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\map\\test_1.dds", 0);
	
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 2, true);

	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
	m_pMaterial->SetReflection(1);

	float map_3_distance = 250.f;

	for (UINT i = 0; i < m_nObjects; i++) {
		ModelObject* map = new ModelObject(static_model, pd3dDevice, pd3dCommandList);
		if (map_type == M_Map_1_macaron_1) map->SetPosition(0.f, -50.f, 0.f);

		if (map_type == M_Map_2) map->SetPosition(0.f, -20.f, 0.f);
		if (map_type == M_Map_1_macaron) map->SetPosition(185.f, -10.f, 0.f);
		if (map_type == M_Map_1_macaron_2) map->SetPosition(-185.f, -10.f, 0.f);
		if (map_type == M_Map_2_chocolate_bar) map->SetPosition(300.f, -20.f, 0.f);
		if (map_type == M_Map_2_chocolate_bar_2) map->SetPosition(-300.f, -20.f, 0.f);

		if (map_type == M_Map_3) map->SetPosition(map_3_distance, -132.f, 0.f);
		if (map_type == M_Map_3_2) map->SetPosition(-map_3_distance, -132.f, 0.f);
		if (map_type == M_Map_3_cake_2) map->SetPosition(map_3_distance, 0.f, 50.f);
		if (map_type == M_Map_3_cake_2_2) map->SetPosition(-map_3_distance, 0.f, 50.f);
		if (map_type == M_Map_3_cake_3) map->SetPosition(map_3_distance, 80.f, 80.f);
		if (map_type == M_Map_3_cake_3_2) map->SetPosition(-map_3_distance, 80.f, 80.f);
		if (map_type == M_Map_1_macaron_3) map->SetPosition(0, 80.f, 50.f);

		if (map_type == M_Map_3_in) map->SetPosition(map_3_distance, 500.f, 40.f);
		if (map_type == M_Map_3_in_stair_1) map->SetPosition(map_3_distance, 500.f, 40.f);
		if (map_type == M_Map_3_in_stair_2) map->SetPosition(map_3_distance, 500.f, 40.f);
		if (map_type == M_Map_3_in_wall_collision) map->SetPosition(map_3_distance, 500.f, 40.f);
		if (map_type == M_Map_3_in_2) map->SetPosition(-map_3_distance, 500.f, 40.f);
		if (map_type == M_Map_3_in_stair_1_2) map->SetPosition(-map_3_distance, 500.f, 40.f);
		if (map_type == M_Map_3_in_stair_2_2) map->SetPosition(-map_3_distance, 500.f, 40.f);
		if (map_type == M_Map_3_in_wall_collision_2) map->SetPosition(-map_3_distance, 500.f, 40.f);

		map->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
		m_bbObjects[i] = map;

		PxTriangleMesh* triMesh = physx->GetTriangleMesh(static_model->getMesh(0), static_model->getNumVertices());
		PxVec3 scaleTmp = PxVec3(1.0f, 1.0f, 1.0f);

		PxMeshScale PxScale;
		PxScale.scale = scaleTmp;

		PxTriangleMeshGeometry meshGeo(triMesh, PxScale);
		XMFLOAT3 pos = map->GetPosition();
		PxTransform location(pos.x, pos.y, pos.z);

		PxMaterial* mat = physx->m_Physics->createMaterial(0.2f, 0.2f, 0.2f);

		PxRigidActor* m_Actor = PxCreateStatic(*physx->m_Physics, location, meshGeo, *mat);
		physx->m_Scene->addActor(*m_Actor);

		if (map_type == M_Map_1_macaron_1) {
			physx->move_actor = m_Actor;
			physx->move_actor->userData = (void *)(int)1;
		}
	}
	delete pTexture;
}

void CModelShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int map_type, int nRenderTargets, void * pContext)
{
	m_nPSO = 1;
	CreatePipelineParts();

	m_nObjects = 5;
	m_bbObjects = vector<ModelObject*>(m_nObjects);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, 1);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_ObjectCB->Resource(), D3DUtil::CalcConstantBufferByteSize(sizeof(CB_GAMEOBJECT_INFO)));

	CreateGraphicsRootSignature(pd3dDevice);
	BuildPSO(pd3dDevice, nRenderTargets);

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	if (map_type == M_Map_1)pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\map\\map_2.dds", 0);
	if (map_type == M_Map_3)pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\map\\black.dds", 0);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 2, true);

	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
	m_pMaterial->SetReflection(1);

	for (int i = 0; i < m_nObjects; i++) {
		ModelObject* map = new ModelObject(static_model, pd3dDevice, pd3dCommandList);
		map->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
		m_bbObjects[i] = map;
		if (map_type == M_Map_1) m_bbObjects[i]->SetPosition(0, -20 + -20 * i, 0);
		if (map_type == M_Map_3) {
			if (i == 0) m_bbObjects[i]->SetPosition(250.f, 0.f, 50.f);
			if (i == 1) m_bbObjects[i]->SetPosition(-250.f, 0.f, 50.f);
			if (i == 2) m_bbObjects[i]->SetPosition(250.f, 80.f, 130.f);
			if (i == 3) m_bbObjects[i]->SetPosition(-250.f, 80.f, 130.f);
			if (i == 4) m_bbObjects[i]->SetPosition(-250.f, 80.f, 130.f);
		}
	}
	delete pTexture;
}

void CModelShader::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera)
{
	CModelShader::OnPrepareRender(pd3dCommandList, 0);
	//pCamera->SetViewportsAndScissorRects(pd3dCommandList);

	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);

	for (UINT j = 0; j < m_nObjects; j++)
	{
		if (m_bbObjects[j])
		{
			//m_bbObjects[j]->G
			m_bbObjects[j]->Render(pd3dCommandList, pCamera);
		}
	}
}

void CModelShader::Animate(float fTimeElapsed)
{
	if (IsZero(fTimeElapsed))
		return;
	for (UINT i = 0; i < m_nObjects; ++i) {
		if (m_bbObjects[i])
			m_bbObjects[i]->Animate(fTimeElapsed);
	}
}

void CModelShader::Animate(float fTimeElapsed, int flag_up)
{
	if (IsZero(fTimeElapsed)) return;

	for (UINT i = 0; i < m_nObjects; ++i) {
		if (m_bbObjects[i]) {
			m_bbObjects[i]->Animate(fTimeElapsed);
			if (flag_up == M_Map_1) {
				float macaron_y = m_bbObjects[i]->GetPosition().y;
				if (m_bbObjects[i]->GetPosition().y < -3) m_bbObjects[i]->SetPosition(0, macaron_y += 0.2, 0);
				else m_bbObjects[i]->SetPosition(0, -3.0, 0);
			}
			if (flag_up == M_Map_3) {
				if (!map_3_macaron_flag) {
					float m_bbObjects_x = m_bbObjects[i]->GetPosition().x;
					m_bbObjects[i]->SetPosition(m_bbObjects_x += 0.5f, 80.f, 50.f);
					if (m_bbObjects[i]->GetPosition().x > 120) map_3_macaron_flag = true;
				}
				else {
					float m_bbObjects_x = m_bbObjects[i]->GetPosition().x;
					m_bbObjects[i]->SetPosition(m_bbObjects_x -= 0.5f, 80.f, 50.f);
					if (m_bbObjects[i]->GetPosition().x < -120) map_3_macaron_flag = false;
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////

void CModelShader::setScale(float scale)
{
	for (UINT i = 0; i < m_nObjects; ++i) {
		m_bbObjects[i]->SetScale(scale);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

D3D12_SHADER_BYTECODE RoundBackGroundShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	wchar_t filename[100] = L"Model.hlsl";
	return(CShader::CompileShaderFromFile(filename, "PSNoLightModel", "ps_5_1", ppd3dShaderBlob));
}

void RoundBackGroundShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int texture, bool backgroundflag, int nRenderTargets, void * pContext)
{
	m_nPSO = 1;
	CreatePipelineParts();

	m_nObjects = 1;
	m_bbObjects = vector<ModelObject*>(m_nObjects);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, 1);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_ObjectCB->Resource(), D3DUtil::CalcConstantBufferByteSize(sizeof(CB_GAMEOBJECT_INFO)));

	CreateGraphicsRootSignature(pd3dDevice);
	BuildPSO(pd3dDevice, nRenderTargets);

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	if (texture == 0)pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\map\\round_background_1.dds", 0);	// round_background_1
	if (texture == 1)pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\map\\round_background_2.dds", 0);
	if (texture == 2)pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\map\\round_background_3.dds", 0);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 2, true);

	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
	m_pMaterial->SetReflection(1);

	for (int i = 0; i < m_nObjects; i++) {
		ModelObject* map = new ModelObject(static_model, pd3dDevice, pd3dCommandList);
		map->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
		m_bbObjects[i] = map;
		m_bbObjects[i]->SetPosition(0, 0, 0);
	}
	delete pTexture;
}

////////////////////////////////

void StairShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, CPhysx* physx, int type, int nRenderTargets, void * pContext)
{
	m_nPSO = 1;
	CreatePipelineParts();

	m_nObjects = 11;
	m_bbObjects = vector<ModelObject*>(m_nObjects);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, 1);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_ObjectCB->Resource(), D3DUtil::CalcConstantBufferByteSize(sizeof(CB_GAMEOBJECT_INFO)));

	CreateGraphicsRootSignature(pd3dDevice);
	BuildPSO(pd3dDevice, nRenderTargets);

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\map\\map_3_stair.dds", 0);

	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 2, true);

	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
	m_pMaterial->SetReflection(1);

	float map_3_distance = 250.f;

	for (UINT i = 0; i < m_nObjects; i++) {
		ModelObject* map = new ModelObject(static_model, pd3dDevice, pd3dCommandList);
		if (type == 0) {
			if (i == 10) map->SetPosition(128.f + map_3_distance, (i - 1) * 8.f, i * 18.f - 50);
			else map->SetPosition(128.f + map_3_distance, i * 8.f, i * 18.f - 50);
		}
		else if (type == 1) {
			if (i == 10) map->SetPosition(-128.f - map_3_distance, (i - 1) * 8.f, i * 18.f - 50);
			else map->SetPosition(-128.f - map_3_distance, i * 8.f, i * 18.f - 50);
		}
		map->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
		m_bbObjects[i] = map;

		PxTriangleMesh* triMesh = physx->GetTriangleMesh(static_model->getMesh(0), static_model->getNumVertices());
		PxVec3 scaleTmp = PxVec3(1.0f, 1.0f, 1.0f);

		PxMeshScale PxScale;
		PxScale.scale = scaleTmp;

		PxTriangleMeshGeometry meshGeo(triMesh, PxScale);
		XMFLOAT3 pos = map->GetPosition();
		PxTransform location(pos.x, pos.y, pos.z);

		PxMaterial* mat = physx->m_Physics->createMaterial(0.2f, 0.2f, 0.2f);

		PxRigidActor* m_Actor = PxCreateStatic(*physx->m_Physics, location, meshGeo, *mat);
		physx->m_Scene->addActor(*m_Actor);
	}
	delete pTexture;
}

void BridgeShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, CPhysx* physx, int nRenderTargets, void * pContext)
{
	m_nPSO = 1;
	CreatePipelineParts();

	m_nObjects = 4;
	m_bbObjects = vector<ModelObject*>(m_nObjects);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, 1);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_ObjectCB->Resource(), D3DUtil::CalcConstantBufferByteSize(sizeof(CB_GAMEOBJECT_INFO)));

	CreateGraphicsRootSignature(pd3dDevice);
	BuildPSO(pd3dDevice, nRenderTargets);

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\map\\map_3_bridge.dds", 0);

	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 2, true);

	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
	m_pMaterial->SetReflection(1);

	for (UINT i = 0; i < m_nObjects; i++) {
		ModelObject* map = new ModelObject(static_model, pd3dDevice, pd3dCommandList);
		map->SetPosition(0.f, 0.f, i * 20.f + 30.f);
		map->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
		m_bbObjects[i] = map;

		PxTriangleMesh* triMesh = physx->GetTriangleMesh(static_model->getMesh(0), static_model->getNumVertices());
		PxVec3 scaleTmp = PxVec3(1.0f, 1.0f, 1.0f);

		PxMeshScale PxScale;
		PxScale.scale = scaleTmp;

		PxTriangleMeshGeometry meshGeo(triMesh, PxScale);
		XMFLOAT3 pos = map->GetPosition();
		PxTransform location(pos.x, pos.y, pos.z);

		PxMaterial* mat = physx->m_Physics->createMaterial(0.2f, 0.2f, 0.2f);

		PxRigidActor* m_Actor = PxCreateStatic(*physx->m_Physics, location, meshGeo, *mat);
		physx->m_Scene->addActor(*m_Actor);
	}
	delete pTexture;

}


/////////////////////////////////////////////////////////////////////////////////////////////////////

MeshShader::MeshShader()
{

}

MeshShader::~MeshShader()
{

}

void MeshShader::CreateGraphicsRootSignature(ID3D12Device * pd3dDevice)
{
	int i = 0;
	ComPtr<ID3D12RootSignature> pd3dGraphicsRootSignature = nullptr;

	CD3DX12_DESCRIPTOR_RANGE pd3dDescriptorRanges[2];

	pd3dDescriptorRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, CBVObjectInfo, 0, 0); // GameObject
	pd3dDescriptorRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, SRVTexArray, 0, 0); // Texture

	CD3DX12_ROOT_PARAMETER pd3dRootParameters[3];

	pd3dRootParameters[0].InitAsConstantBufferView(1);
	pd3dRootParameters[1].InitAsDescriptorTable(1, &pd3dDescriptorRanges[0], D3D12_SHADER_VISIBILITY_ALL);
	pd3dRootParameters[2].InitAsDescriptorTable(1, &pd3dDescriptorRanges[1], D3D12_SHADER_VISIBILITY_PIXEL);

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

	ThrowIfFailed(pd3dDevice->CreateRootSignature(0,
		pd3dSignatureBlob->GetBufferPointer(),
		pd3dSignatureBlob->GetBufferSize(),
		IID_PPV_ARGS(m_RootSignature[PSO_SHADOWMAP].GetAddressOf()))
	);
}

D3D12_INPUT_LAYOUT_DESC MeshShader::CreateInputLayout(int index)
{
	D3D12_INPUT_LAYOUT_DESC inputLayout;

	m_pInputElementDesc =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	0,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0,	36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXINDEX",	0, DXGI_FORMAT_R32_UINT,			0,	44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	inputLayout = { m_pInputElementDesc.data(), (UINT)m_pInputElementDesc.size() };

	return inputLayout;
}

D3D12_SHADER_BYTECODE MeshShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	wchar_t filename[100] = L"Model.hlsl";
	return(CShader::CompileShaderFromFile(filename, "VSDiffused", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE MeshShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	wchar_t filename[100] = L"Model.hlsl";
	return(CShader::CompileShaderFromFile(filename, "PSDynamicModel", "ps_5_1", ppd3dShaderBlob));
}

void MeshShader::CreateShaderVariables(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	// 오브젝트 수 만큼 메모리 할당 
	m_ObjectCB = make_unique<UploadBuffer<CB_GAMEOBJECT_INFO>>(pd3dDevice, m_nObjects, true);
}

void MeshShader::UpdateShaderVariables(ID3D12GraphicsCommandList * pd3dCommandList)//, XMFLOAT4X4 *pxmf4x4World)
{
	// XMFLOAT4X4 xmf4x4World;
	// XMStoreFloat4x4(&xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(pxmf4x4World)));
	// pd3dCommandList->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4World, 0);

	CB_GAMEOBJECT_INFO cBuffer;
	for (UINT i = 0; i < m_nObjects; ++i) {
		XMStoreFloat4x4(&cBuffer.m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppObjects[i]->m_xmf4x4World)));
		cBuffer.m_nMaterial = 0;
		m_ObjectCB->CopyData(i, cBuffer);
	}
}

D3D12_RASTERIZER_DESC MeshShader::CreateRasterizerState(int index)
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));

	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	//d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;

	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;

	//d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	//d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_FRONT;

	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;

	d3dRasterizerDesc.DepthClipEnable = TRUE;
	//d3dRasterizerDesc.DepthClipEnable = FALSE;

	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	return(d3dRasterizerDesc);
}

void MeshShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int type, int nRenderTargets, void * pContext)
{
	m_nPSO = 1;
	CreatePipelineParts();

	m_nObjects = 1;
	m_ppObjects = vector<CGameObject*>(m_nObjects);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, 1);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_ObjectCB->Resource(), D3DUtil::CalcConstantBufferByteSize(sizeof(CB_GAMEOBJECT_INFO)));

	CreateGraphicsRootSignature(pd3dDevice);
	BuildPSO(pd3dDevice, nRenderTargets);

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	if (type == 0) pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\image\\candyland.dds", 0);	//1400*788
	if (type == 1) pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\map\\black.dds", 0);
	if (type == 2) pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\map\\brick.dds", 0);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 2, true);

	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
	m_pMaterial->SetReflection(1);

	XMFLOAT3 aa = XMFLOAT3(1, 0, 0);
	for (UINT i = 0; i < m_nObjects; i++) {

		CGameObject *Map_Object = NULL;

		Map_Object = new CGameObject();
		if (type == 0)Map_Object->SetPosition(Pos_act.x, Pos_act.y, Pos_act.z);
		if (type == 1)Map_Object->SetPosition(0.f, 400.f, 0.f);
		if (type == 2)Map_Object->SetPosition(0.f, 0.f, 0.f);
		m_ppObjects[i] = Map_Object;

		CMesh *pCubeMesh = NULL;
		if (type == 0) pCubeMesh = new CreateQuad(pd3dDevice, pd3dCommandList, -1400, 1000, 2800, 1500, 700);	// pos(x, y), Width(w, h), depth
		if (type == 1) pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 1000, 500, 500);
		if (type == 2) pCubeMesh = new CreateQuad(pd3dDevice, pd3dCommandList, -1400, 0, 2800, 200, 699);
		m_ppObjects[i]->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * 0));
		m_ppObjects[i]->SetMesh(pCubeMesh);
	}
	delete pTexture;

}

void MeshShader::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera)
{
	CModelShader::OnPrepareRender(pd3dCommandList, 0);

	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);

	for (UINT j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{
			m_ppObjects[j]->Render(pd3dCommandList, pCamera);
		}
	}
}

void MeshShader::Animate(float fTimeElapsed)
{
	if (IsZero(fTimeElapsed))
		return;
	for (UINT i = 0; i < m_nObjects; ++i) {
		if (m_ppObjects[i]) m_ppObjects[i]->Animate(fTimeElapsed);
	}
}

void MeshShader::setScale(float scale)
{
	for (UINT i = 0; i < m_nObjects; ++i) {
		m_ppObjects[i]->SetScale(scale);
	}
}

WaveShader::WaveShader() {}

WaveShader::~WaveShader() {}

D3D12_SHADER_BYTECODE WaveShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	wchar_t filename[100] = L"Model.hlsl";
	return(CShader::CompileShaderFromFile(filename, "VSWave", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE WaveShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	wchar_t filename[100] = L"Model.hlsl";
	return(CShader::CompileShaderFromFile(filename, "PSWaveModel", "ps_5_1", ppd3dShaderBlob));
}

void WaveShader::UpdateShaderVariables(ID3D12GraphicsCommandList * pd3dCommandList)//, XMFLOAT4X4 *pxmf4x4World)
{
	CB_GAMEOBJECT_INFO cBuffer;
	for (UINT i = 0; i < m_nObjects; ++i) {
		XMStoreFloat4x4(&cBuffer.m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppObjects[i]->m_xmf4x4World)));
		cBuffer.m_nMaterial = wave_sin;
		m_ObjectCB->CopyData(i, cBuffer);
	}
}

D3D12_BLEND_DESC WaveShader::CreateBlendState(int index)
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;			// FALSE
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = TRUE;	// FALSE
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;	//D3D12_BLEND_ONE
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//D3D12_BLEND_ZERO
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;

	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	return(d3dBlendDesc);
}

void WaveShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int map_type, int nRenderTargets, void * pContext)
{
	m_nPSO = 1;
	CreatePipelineParts();

	m_nObjects = 1;	//10
	m_ppObjects = vector<CGameObject*>(m_nObjects);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, 1);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_ObjectCB->Resource(), D3DUtil::CalcConstantBufferByteSize(sizeof(CB_GAMEOBJECT_INFO)));

	CreateGraphicsRootSignature(pd3dDevice);
	BuildPSO(pd3dDevice, nRenderTargets);

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	if (map_type == M_Map_1) pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\image\\water3.dds", 0);	//1400*788
	if (map_type == M_Map_2) pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\image\\water3.dds", 0);	//1400*788
	if (map_type == M_Map_3) pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\image\\water3.dds", 0);	//1400*788
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 2, true);

	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
	m_pMaterial->SetReflection(1);

	for (int i = 0; i < m_nObjects; i++) {
		CGameObject *Map_Object = NULL;

		Map_Object = new CGameObject();
		Map_Object->SetPosition(Pos_act.x, Pos_act.y, Pos_act.z);
		m_ppObjects[i] = Map_Object;

		CMesh *pCubeMesh = new CreateGrid(pd3dDevice, pd3dCommandList, 2000, 1000, 200, 200);	// Width(w, h), xycount(m, n)
		m_ppObjects[i]->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
		m_ppObjects[i]->SetMesh(pCubeMesh);
		m_ppObjects[i]->SetPosition(0, -10, 0);//20.f * i - 200,0,200
	}
	delete pTexture;

}

void WaveShader::Animate(float fTimeElapsed, int map_type)
{
	if (IsZero(fTimeElapsed))
		return;
	for (int i = 0; i < m_nObjects; ++i) {
		if (m_ppObjects[i]) {
			m_ppObjects[i]->Animate(fTimeElapsed);

			if (wave_sin >= 10000) wave_sin = 0;
			wave_sin += 1;

			if (map_type == M_Map_1) {
				//float xx = m_ppObjects[i]->GetPosition().x;
				//if (m_ppObjects[i]->GetPosition().x < 1000) m_ppObjects[i]->SetPosition(xx += 0.5, -10, 0);
				//else m_ppObjects[i]->SetPosition(-1000, -10, 0);
				m_ppObjects[i]->SetPosition(0, -10, 0);
			}
			if (map_type == M_Map_2) {
				//float xx = m_ppObjects[i]->GetPosition().x;
				//if (m_ppObjects[i]->GetPosition().x < 1000) m_ppObjects[i]->SetPosition(xx += 0.5, -30, 0);
				//else m_ppObjects[i]->SetPosition(-1000, -30, 0);
				m_ppObjects[i]->SetPosition(0, -30, 0);
			}
			if (map_type == M_Map_3) {
				//float xx = m_ppObjects[i]->GetPosition().x;
				//if (m_ppObjects[i]->GetPosition().x < 1000) m_ppObjects[i]->SetPosition(xx += 0.5, -50, 0);
				//else m_ppObjects[i]->SetPosition(-1000, -50, 0);
				m_ppObjects[i]->SetPosition(0, -50, 0);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////

CottonCloudShader::CottonCloudShader() {}

CottonCloudShader::CottonCloudShader(LoadModel *ma) : CModelShader(ma) { cloud_model = ma; }

CottonCloudShader::~CottonCloudShader() {}

D3D12_SHADER_BYTECODE CottonCloudShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	wchar_t filename[100] = L"Model.hlsl";
	return(CShader::CompileShaderFromFile(filename, "PSCottonModel", "ps_5_1", ppd3dShaderBlob));
}

D3D12_BLEND_DESC CottonCloudShader::CreateBlendState(int index)
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;			// FALSE
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = TRUE;	// FALSE
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;	//D3D12_BLEND_ONE
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//D3D12_BLEND_ZERO
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	return(d3dBlendDesc);
}

void CottonCloudShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int kind, int nRenderTargets, void * pContext)
{
	m_nPSO = 1;
	CreatePipelineParts();

	m_nObjects = 32;	//17
	m_bbObjects = vector<ModelObject*>(m_nObjects);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, 1);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_ObjectCB->Resource(), D3DUtil::CalcConstantBufferByteSize(sizeof(CB_GAMEOBJECT_INFO)));

	CreateGraphicsRootSignature(pd3dDevice);
	BuildPSO(pd3dDevice, nRenderTargets);

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	if (kind == 0)pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\map\\cotton_candy_cloud_2.dds", 0);	//1400*788
	else if (kind == 1)pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\map\\cotton_candy_cloud_1.dds", 0);	//1400*788
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 2, true);

	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
	m_pMaterial->SetReflection(1);
	for (int i = 0; i < m_nObjects; i++) {
		ModelObject* cloud = new ModelObject(cloud_model, pd3dDevice, pd3dCommandList);
		if (i < 16) {
			if (kind == 0) {
				if (i % 2 == 0) cloud->SetPosition(i % 4 * 30 - 45, 10, -75 + (-30 * (int)(i / 4)));
				else cloud->SetPosition(-45, 10, -75);
			}
			else if (kind == 1) {
				if (i % 2 == 1) cloud->SetPosition(i % 4 * 30 - 45, 10, -75 + (-30 * (int)(i / 4)));
				else cloud->SetPosition(30 - 45, 10, -75 + -30);
			}
		}
		else {
			if (kind == 0) {
				if (i % 2 == 0) cloud->SetPosition(i % 4 * 30 - 45, 10, 75 + (30 * (int)(i / 4 - 4)));
				else cloud->SetPosition(-45, 10, 75);
			}
			else if (kind == 1) {
				if (i % 2 == 1) cloud->SetPosition(i % 4 * 30 - 45, 10, 75 + (30 * (int)(i / 4 - 4)));
				else cloud->SetPosition(30 - 45, 10, 75);
			}
		}

		cloud->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
		m_bbObjects[i] = cloud;
	}
	delete pTexture;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////

testBox::testBox() {}
testBox::~testBox() {}


D3D12_RASTERIZER_DESC testBox::CreateRasterizerState(int index)
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));

	//d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;

	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	//d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_FRONT;

	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;

	d3dRasterizerDesc.DepthClipEnable = TRUE;
	//d3dRasterizerDesc.DepthClipEnable = FALSE;

	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	return(d3dRasterizerDesc);
}

D3D12_SHADER_BYTECODE testBox::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{

	wchar_t filename[100] = L"Model.hlsl";
	return(CShader::CompileShaderFromFile(filename, "PSBoundBox", "ps_5_1", ppd3dShaderBlob));
}

void testBox::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ModelObject *mo, int type, int nRenderTargets, void * pContext)
{
	//m_nPSO = 1;
	//CreatePipelineParts();

	m_nObjects = 1;
	m_ppObjects = vector<CGameObject*>(m_nObjects);

	//CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, 1);
	//CreateShaderVariables(pd3dDevice, pd3dCommandList);
	//CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_ObjectCB->Resource(), D3DUtil::CalcConstantBufferByteSize(sizeof(CB_GAMEOBJECT_INFO)));
	//
	//CreateGraphicsRootSignature(pd3dDevice);
	//BuildPSO(pd3dDevice, nRenderTargets);
	for (UINT i = 0; i < m_nObjects; i++) {
		CGameObject *test_Object = NULL;
		test_Object = new CGameObject();
		//BoundingOrientedBox bb = mo->boundingbox;
		XMFLOAT3 pos;
		if (type <= 10)pos = mo->GetPosition();
		else {
			if (type == LEFT_DOWN_OUT) pos = XMFLOAT3(-250.f, 10.f, -40.f);
			if (type == LEFT_DOWN_IN) pos = XMFLOAT3(-250.f, 30.f + 500.f, -80.f);
			if (type == LEFT_UP_OUT) pos = XMFLOAT3(-250.f, 85.f, 40.f);
			if (type == LEFT_UP_IN) pos = XMFLOAT3(-250.f, 85.f + 500.f, 10.f);

			if (type == RIGHT_DOWN_OUT) pos = XMFLOAT3(250.f, 10.f, -40.f);
			if (type == RIGHT_DOWN_IN) pos = XMFLOAT3(250.f, 30.f + 500.f, -80.f);
			if (type == RIGHT_UP_OUT) pos = XMFLOAT3(250.f, 85.f, 40.f);
			if (type == RIGHT_UP_IN) pos = XMFLOAT3(250.f, 85.f + 500.f, 10.f);
		}
		test_Object->SetPosition(pos);
		m_ppObjects[i] = test_Object;

		if (type == OBJECT_PLAYER) {
			//CMesh *pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 15, 35, 15);	// pos(x, y), Width(w, h), depth
			bounding.Extents.x = 15; bounding.Extents.y = 35; bounding.Extents.z = 15;
			//m_ppObjects[i]->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
			//m_ppObjects[i]->SetMesh(pCubeMesh);
			m_ppObjects[i]->Rotate(0, 0, 90);
			m_ppObjects[i]->SetScale(0.7f);
		}
		if (type == M_Weapon_Lollipop) {
			//CMesh *pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 5, 30, 15);	// pos(x, y), Width(w, h), depth
		//	CMesh *pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 10, 10, 10);	// pos(x, y), Width(w, h), depth
			bounding.Extents.x = 5; bounding.Extents.y = 5; bounding.Extents.z = 5;

			//m_ppObjects[i]->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
			//m_ppObjects[i]->SetMesh(pCubeMesh);
		}
		if (type == M_Weapon_chupachupse) {
			//	CMesh *pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 10, 25, 10);	// pos(x, y), Width(w, h), depth
			//	CMesh *pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 10, 10, 10);	// pos(x, y), Width(w, h), depth
			bounding.Extents.x = 5; bounding.Extents.y = 5; bounding.Extents.z = 5;

			//m_ppObjects[i]->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
			//m_ppObjects[i]->SetMesh(pCubeMesh);
		}
		if (type == M_Weapon_pepero) {
			//	CMesh *pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 5, 45, 5);	// pos(x, y), Width(w, h), depth
			//	CMesh *pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 10, 10, 10);	// pos(x, y), Width(w, h), depth
			bounding.Extents.x = 5; bounding.Extents.y = 5; bounding.Extents.z = 5;

			//m_ppObjects[i]->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
			//m_ppObjects[i]->SetMesh(pCubeMesh);
		}
		if (type == M_Weapon_chocolate) {
			//	CMesh *pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 5, 10, 10);	// pos(x, y), Width(w, h), depth
				//CMesh *pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 10, 10, 10);	// pos(x, y), Width(w, h), depth
			bounding.Extents.x = 5; bounding.Extents.y = 5; bounding.Extents.z = 5;

			//m_ppObjects[i]->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
			//m_ppObjects[i]->SetMesh(pCubeMesh);
		}
		if (type == M_Weapon_cupcake) {
			//		CMesh *pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 10, 10, 10);	// pos(x, y), Width(w, h), depth
				//CMesh *pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 10, 10, 10);	// pos(x, y), Width(w, h), depth
			bounding.Extents.x = 5; bounding.Extents.y = 5; bounding.Extents.z = 5;

			//m_ppObjects[i]->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
			//m_ppObjects[i]->SetMesh(pCubeMesh);
		}
		if (type >= LEFT_DOWN_OUT && type <= RIGHT_UP_IN) {
			//CMesh *pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 5, 45, 5);	// pos(x, y), Width(w, h), depth
		//	CMesh *pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 4, 4, 4);	// pos(x, y), Width(w, h), depth
			bounding.Extents.x = 5; bounding.Extents.y = 5; bounding.Extents.z = 5;

			//m_ppObjects[i]->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
			//m_ppObjects[i]->SetMesh(pCubeMesh);
		}
	}
}

void testBox::SetPosition(XMFLOAT3& pos)
{
	m_ppObjects[0]->SetPosition(pos);
}

void testBox::Rotate(float x, float y, float z)
{
	m_ppObjects[0]->Rotate(x, y, z);
}

void testBox::Animate(float fTimeElapsed)
{
	if (IsZero(fTimeElapsed))
		return;
	for (UINT i = 0; i < m_nObjects; ++i) {
		if (m_ppObjects[i])
			m_ppObjects[i]->Animate(fTimeElapsed);
	}
}

//////////////////////////////////////////////////////////////////////////////////

WeaponShader::WeaponShader()
{

}

WeaponShader::WeaponShader(LoadModel *ma) : CModelShader(ma)
{
	weapon_model = ma;
}

WeaponShader::~WeaponShader()
{

}

D3D12_RASTERIZER_DESC WeaponShader::CreateRasterizerState(int index)
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));

	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	//d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;

	//d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	//d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_FRONT;

	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;

	d3dRasterizerDesc.DepthClipEnable = TRUE;
	//d3dRasterizerDesc.DepthClipEnable = FALSE;

	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	return(d3dRasterizerDesc);
}
/*
D3D12_BLEND_DESC WeaponShader::CreateBlendState(int index)
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;			// FALSE
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = TRUE;	// FALSE
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;	//D3D12_BLEND_ONE
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//D3D12_BLEND_ZERO
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	return(d3dBlendDesc);
}*/
void WeaponShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int weapon_num, int mpa_type, int nRenderTargets, void * pContext)
{
	m_nPSO = 1;
	CreatePipelineParts();

	//else 
	if (mpa_type == M_Map_1) m_nObjects = WEAPON_EACH_NUM;
	else if (mpa_type == M_Map_2) m_nObjects = WEAPON_EACH_NUM;

	if (weapon_num == M_Weapon_cupcake) m_nObjects = 1;
	m_bbObjects = vector<ModelObject*>(m_nObjects);

	CreateGraphicsRootSignature(pd3dDevice);
	BuildPSO(pd3dDevice, nRenderTargets);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, 2);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_ObjectCB->Resource(), D3DUtil::CalcConstantBufferByteSize(sizeof(CB_GAMEOBJECT_INFO)));

	CTexture *pTexture;
	pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	if (weapon_num == 0) pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\weapon\\lollipop.dds", 0);
	else if (weapon_num == 1) pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\weapon\\candy.dds", 0);
	else if (weapon_num == 2) pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\weapon\\pepero.dds", 0);
	else if (weapon_num == 3) pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\weapon\\chocolate.dds", 0);
	else if (weapon_num == 4) pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\weapon\\cupcake.dds", 0);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 2, true);

	XMFLOAT3 a = XMFLOAT3(1, 0, 0);

	for (UINT i = 0; i < m_nObjects; i++) {
		m_pMaterial = new CMaterial();
		m_pMaterial->SetTexture(pTexture);
		m_pMaterial->SetReflection(1);

		ModelObject* weapon = new ModelObject(weapon_model, pd3dDevice, pd3dCommandList);
		if (weapon_num == M_Weapon_cupcake) {
			weapon->SetPosition(0, -100 + 15, 0);
		}
		else {
			if (mpa_type == M_Map_1) {
				float b = D3DMath::RandF(-13, 13);
				float c = D3DMath::RandF(-8, 8);
				if (b > -2 && b < 2 || c > -2 && c < 2) { i -= 1; continue; }
				weapon->SetPosition(b * 20, 10, c * 20);
			}
			if (mpa_type == M_Map_2) {
				float b = D3DMath::RandF(-6, 6);
				float c = D3DMath::RandF(-7, 7);
				weapon->SetPosition(b * 20, 10, c * 20);
			}
			if (mpa_type == M_Map_3) {
				float b = D3DMath::RandF(-13, 13);
				float c = D3DMath::RandF(-7, 7);
				if (c > -2 && c < 2) { i -= 1; continue; }
				weapon->SetPosition(b * 20, 0, c * 20);
			}

			//weapon->SetPosition(7.47554874, 8.61560154, -0.784351766);

			if (weapon_num == 3) weapon->Rotate(0, 0, -90);
			else weapon->Rotate(0, 0, 90);
			weapon->Rotate(&a, D3DMath::Rand(0, 4) * 90.f);
		}
		weapon->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
		m_bbObjects[i] = weapon;
	}
}

void WeaponShader::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera)
{
	CModelShader::OnPrepareRender(pd3dCommandList, 0);
	//pCamera->SetViewportsAndScissorRects(pd3dCommandList);

	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);

	for (UINT j = 0; j < m_nObjects; j++)
	{
		if (m_bbObjects[j] && m_bbObjects[j]->visible)
		{
			//m_bbObjects[j]->G
			m_bbObjects[j]->Render(pd3dCommandList, pCamera);
		}
	}
}

void WeaponShader::Animate(float fTimeElapsed, int flag_up, int weapon_num)
{
	if (IsZero(fTimeElapsed)) return;

	for (UINT i = 0; i < m_nObjects; ++i) {
		if (m_bbObjects[i]) {
			m_bbObjects[i]->Animate(fTimeElapsed);
			if (flag_up == M_Map_1 || weapon_num == M_Weapon_cupcake) {
				float macaron_y = m_bbObjects[i]->GetPosition().y;
				if (m_bbObjects[i]->GetPosition().y < 15) m_bbObjects[i]->SetPosition(0, macaron_y += 0.2, 0);
				else {
					m_bbObjects[i]->SetPosition(0, 15, 0);
					cupcake_up_flag = true;
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////

DynamicModelShader::DynamicModelShader(Model_Animation *ma) : CModelShader(ma)
{
}

DynamicModelShader::~DynamicModelShader()
{
}

void DynamicModelShader::CreateShaderVariables(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	// 뼈정보
	m_BoneCB = make_unique<UploadBuffer<CB_DYNAMICOBJECT_INFO>>(pd3dDevice, m_nObjects, true);
}

void DynamicModelShader::CreateGraphicsRootSignature(ID3D12Device * pd3dDevice)
{
	int i = 0;
	ComPtr<ID3D12RootSignature> pd3dGraphicsRootSignature = nullptr;

	CD3DX12_DESCRIPTOR_RANGE pd3dDescriptorRanges[2];

	pd3dDescriptorRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, SRVAnimation, 0, 0); // GameObject
	pd3dDescriptorRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, SRVTexArray, 0, 0); // Texture

	CD3DX12_ROOT_PARAMETER pd3dRootParameters[3];

	pd3dRootParameters[0].InitAsConstantBufferView(1);
	pd3dRootParameters[1].InitAsDescriptorTable(1, &pd3dDescriptorRanges[0], D3D12_SHADER_VISIBILITY_ALL);
	pd3dRootParameters[2].InitAsDescriptorTable(1, &pd3dDescriptorRanges[1], D3D12_SHADER_VISIBILITY_PIXEL);

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

	ThrowIfFailed(pd3dDevice->CreateRootSignature(0,
		pd3dSignatureBlob->GetBufferPointer(),
		pd3dSignatureBlob->GetBufferSize(),
		IID_PPV_ARGS(m_RootSignature[PSO_SHADOWMAP].GetAddressOf()))
	);
}

D3D12_SHADER_BYTECODE DynamicModelShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	wchar_t filename[100] = L"Model.hlsl";
	return(CShader::CompileShaderFromFile(filename, "VSDynamicModel", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE DynamicModelShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{

	wchar_t filename[100] = L"Model.hlsl";
	return(CShader::CompileShaderFromFile(filename, "PSDynamicModel", "ps_5_1", ppd3dShaderBlob));
}

void DynamicModelShader::UpdateShaderVariables(ID3D12GraphicsCommandList * pd3dCommandList)
{
	CB_DYNAMICOBJECT_INFO cBone;
	for (UINT i = 0; i < m_nObjects; ++i) {
		//cBone.m_xmf4x4World = m_ppObjects[i]->m_xmf4x4World;
		XMStoreFloat4x4(&cBone.m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_bbObjects[i]->m_xmf4x4World)));
		cBone.m_nMaterial = 0;
		XMStoreFloat4x4(&cBone.m_xmf4x4ShadowTransform, XMMatrixTranspose(XMLoadFloat4x4(&m_bbObjects[i]->m_xmf4x4World) * shadow_mat));

		memcpy(cBone.m_bone, m_bbObjects[i]->GetBoneData(), sizeof(XMFLOAT4X4) * m_bbObjects[i]->GetBoneNum());

		m_BoneCB->CopyData(i, cBone);
	}
}

void DynamicModelShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nRenderTargets, void * pContext)
{
	m_nPSO = 1;
	CreatePipelineParts();

	m_nObjects = 1;
	m_bbObjects = vector<ModelObject*>(m_nObjects);

	XMVECTOR shadowPlane = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR toMainLight = -XMVectorSet(0.5f, -1.0f, 0.4f, 0.0f);
	XMMATRIX S = XMMatrixShadow(shadowPlane, toMainLight);
	XMMATRIX shadowoffset = XMMatrixTranslation(0.0f, 11.f, 0.0f);
	shadow_mat = S * shadowoffset;//XMMatrixShadow(XMVectorSet(+0.0f, -1.0f, +0.0f, 0.0f), XMVector3Normalize(XMVectorSet(0.f, -1.0f, 0.36f, 0.0f))) * XMMatrixTranslation(+0.0f, +11.0f, 0.f);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_BoneCB->Resource(), D3DUtil::CalcConstantBufferByteSize(sizeof(CB_DYNAMICOBJECT_INFO)));

	CreateGraphicsRootSignature(pd3dDevice);
	BuildPSO(pd3dDevice, nRenderTargets);

	CPlayer* player = new CPlayer(model_anim, pd3dDevice, pd3dCommandList);
	player->SetAnimations(model_anim->getAnimCount(), model_anim->getAnim(Anim_Idle));

	//cout << "애니 갯수 : " << model_anim->getAnimCount() << endl;
	//cout << "뼈 : " << model_anim-> << endl;

	//tmp->SetPosition(XMFLOAT3(0, 0, 0));
	player->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * 0));
	m_bbObjects[0] = player;
	m_player = player;
}

void DynamicModelShader::Animate(float fTimeElapsed)
{
	CModelShader::Animate(fTimeElapsed);
}

PlayerShader::PlayerShader(Model_Animation *ma) : DynamicModelShader(ma)
{

}

PlayerShader::~PlayerShader()
{
}

void PlayerShader::CreateGraphicsRootSignature(ID3D12Device * pd3dDevice)
{
	int i = 0;
	ComPtr<ID3D12RootSignature> pd3dGraphicsRootSignature = nullptr;

	CD3DX12_DESCRIPTOR_RANGE pd3dDescriptorRanges[2];

	pd3dDescriptorRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, SRVAnimation, 0, 0); // GameObject
	pd3dDescriptorRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, SRVTexArray, 0, 0); // Texture

	CD3DX12_ROOT_PARAMETER pd3dRootParameters[3];

	pd3dRootParameters[0].InitAsConstantBufferView(1);
	pd3dRootParameters[1].InitAsDescriptorTable(1, &pd3dDescriptorRanges[0], D3D12_SHADER_VISIBILITY_ALL);
	pd3dRootParameters[2].InitAsDescriptorTable(1, &pd3dDescriptorRanges[1], D3D12_SHADER_VISIBILITY_PIXEL);

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

	ThrowIfFailed(pd3dDevice->CreateRootSignature(0,
		pd3dSignatureBlob->GetBufferPointer(),
		pd3dSignatureBlob->GetBufferSize(),
		IID_PPV_ARGS(m_RootSignature[PSO_SHADOWMAP].GetAddressOf()))
	);
}

void PlayerShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nRenderTargets, void * pContext)
{
	m_nPSO = 1;
	CreatePipelineParts();

	m_nObjects = 1;
	m_bbObjects = vector<ModelObject*>(m_nObjects);

	XMVECTOR shadowPlane = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR toMainLight = -XMVectorSet(0.5f, -1.0f, 0.4f, 0.0f);
	XMMATRIX S = XMMatrixShadow(shadowPlane, toMainLight);
	XMMATRIX shadowoffset = XMMatrixTranslation(0.0f, 11.f, 0.0f);
	shadow_mat = S * shadowoffset;//XMMatrixShadow(XMVectorSet(+0.0f, -1.0f, +0.0f, 0.0f), XMVector3Normalize(XMVectorSet(0.f, -1.0f, 0.36f, 0.0f))) * XMMatrixTranslation(+0.0f, +11.0f, 0.f);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1);
	//CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 3);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_BoneCB->Resource(), D3DUtil::CalcConstantBufferByteSize(sizeof(CB_DYNAMICOBJECT_INFO)));

	CreateGraphicsRootSignature(pd3dDevice);
	BuildPSO(pd3dDevice, nRenderTargets);

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\character\\cloth_1.dds", 0);	//cloth_2
	//pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\character\\body.dds", 0);
	//pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\character\\cloth.dds", 1);
	//pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\character\\eye.dds", 2);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 2, true);

	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
	m_pMaterial->SetReflection(1);

	CPlayer* player = new CPlayer(model_anim, pd3dDevice, pd3dCommandList);
	player->SetAnimations(model_anim->getAnimCount(), model_anim->getAnim(0));
	m_Camera = player->GetCamera();
	//cout << "애니 갯수 : " << model_anim->getAnimCount() << endl;
	//cout << "뼈 : " << model_anim-> << endl;

	//tmp->SetPosition(XMFLOAT3(0, 0, 0));
	player->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * 0));
	m_bbObjects[0] = player;
	m_player = player;
	delete pTexture;
}

void PlayerShader::Animate(float fTimeElapsed)
{
	CModelShader::Animate(fTimeElapsed);
}

void PlayerShader::UpdateShaderVariables(ID3D12GraphicsCommandList * pd3dCommandList)
{
	CB_DYNAMICOBJECT_INFO cBone;
	for (UINT i = 0; i < m_nObjects; ++i) {
		//cBone.m_xmf4x4World = m_ppObjects[i]->m_xmf4x4World;
		XMStoreFloat4x4(&cBone.m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_bbObjects[i]->m_xmf4x4World)));
		cBone.m_nMaterial = 0;
		XMStoreFloat4x4(&cBone.m_xmf4x4ShadowTransform, XMMatrixTranspose(XMLoadFloat4x4(&m_bbObjects[i]->m_xmf4x4World) * shadow_mat));

		memcpy(cBone.m_bone, m_bbObjects[i]->GetBoneData(), sizeof(XMFLOAT4X4) * m_bbObjects[i]->GetBoneNum());

		m_BoneCB->CopyData(i, cBone);
	}
}

void PlayerShader::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera)
{
	CModelShader::OnPrepareRender(pd3dCommandList, 0);
	//pCamera->SetViewportsAndScissorRects(pd3dCommandList);

	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
	if (render)
		for (UINT j = 0; j < m_nObjects; j++)
		{
			if (m_bbObjects[j])
			{
				//m_bbObjects[j]->G
				m_bbObjects[j]->Render(pd3dCommandList, pCamera);
			}
		}
}

////////////////////////////////////////////////////////////////////////

ShadowDebugShader::ShadowDebugShader()
{
}

ShadowDebugShader::ShadowDebugShader(LoadModel *ma) : CModelShader(ma)
{
}

ShadowDebugShader::~ShadowDebugShader()
{
}

void ShadowDebugShader::CreateGraphicsRootSignature(ID3D12Device * pd3dDevice)
{
	int i = 0;
	ComPtr<ID3D12RootSignature> pd3dGraphicsRootSignature = nullptr;

	CD3DX12_DESCRIPTOR_RANGE pd3dDescriptorRanges[2];

	pd3dDescriptorRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 2, 0, 0); // GameObject	//SRVShadowMapp
	pd3dDescriptorRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, SRVTexArray, 0, 0); // Texture

	CD3DX12_ROOT_PARAMETER pd3dRootParameters[3];

	pd3dRootParameters[0].InitAsConstantBufferView(1);
	pd3dRootParameters[1].InitAsDescriptorTable(1, &pd3dDescriptorRanges[0], D3D12_SHADER_VISIBILITY_ALL);
	pd3dRootParameters[2].InitAsDescriptorTable(1, &pd3dDescriptorRanges[1], D3D12_SHADER_VISIBILITY_PIXEL);

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

	ThrowIfFailed(pd3dDevice->CreateRootSignature(0,
		pd3dSignatureBlob->GetBufferPointer(),
		pd3dSignatureBlob->GetBufferSize(),
		IID_PPV_ARGS(m_RootSignature[PSO_SHADOWMAP].GetAddressOf()))
	);
}

D3D12_SHADER_BYTECODE ShadowDebugShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	wchar_t filename[100] = L"Model.hlsl";
	return(CShader::CompileShaderFromFile(filename, "VSShadow", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE ShadowDebugShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{

	wchar_t filename[100] = L"Model.hlsl";
	return(CShader::CompileShaderFromFile(filename, "PSShadow", "ps_5_1", ppd3dShaderBlob));
}

D3D12_BLEND_DESC ShadowDebugShader::CreateBlendState(int index)
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;			// FALSE
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = TRUE;	// FALSE
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;	//D3D12_BLEND_ONE
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//D3D12_BLEND_ZERO
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	return(d3dBlendDesc);
}

//깊이-스텐실 검사를 위한 상태를 설정하기 위한 구조체를 반환한다.
D3D12_DEPTH_STENCIL_DESC ShadowDebugShader::CreateDepthStencilState(int index)
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = true;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;			// D3D12_COMPARISON_FUNC_LESS
	d3dDepthStencilDesc.StencilEnable = true;
	d3dDepthStencilDesc.StencilReadMask = 0xff;
	d3dDepthStencilDesc.StencilWriteMask = 0xff;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_INCR;		// D3D12_STENCIL_OP_KEEP	//D3D12_STENCIL_OP_INVERT	//D3D12_STENCIL_OP_INCR
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;	// D3D12_COMPARISON_FUNC_NEVER
	// 후면 다각형 렌더링
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_INCR;		// D3D12_STENCIL_OP_KEEP	//D3D12_STENCIL_OP_REPLACE
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;	// D3D12_COMPARISON_FUNC_NEVER
	return(d3dDepthStencilDesc);
}

void ShadowDebugShader::CreateShaderVariables(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	m_ShadowCB = make_unique<UploadBuffer<CB_SHADOW_INFO>>(pd3dDevice, m_nObjects, true);
}

void ShadowDebugShader::UpdateShaderVariables(ID3D12GraphicsCommandList * pd3dCommandList)
{
	CB_SHADOW_INFO cBuffer;
	for (UINT i = 0; i < m_nObjects; ++i) {
		//float3 c_Light1 = float3(0.f, -1.0f, 0.4f);
		//auto _Shadow = XMMatrixShadow(XMVectorSet(+0.0f, -1.0f, +0.0f, 0.0f), XMVector3Normalize(XMVectorSet(0.f, -1.0f, 0.4f, 0.0f))) * XMMatrixTranslation(+0.0f, -4.99f, +4.99f);

		XMStoreFloat4x4(&cBuffer.m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_bbObjects[i]->m_xmf4x4World) * shadow_mat));
		cBuffer.m_nMaterial = 0;

		//XMStoreFloat4x4(&cBuffer.m_xmf4x4ShadowTransform, XMMatrixTranspose(shadow_mat));

		m_ShadowCB->CopyData(i, cBuffer);
	}
}

void ShadowDebugShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int type, int nRenderTargets, void * pContext)
{
	m_nPSO = 1;

	CreatePipelineParts();

	m_nObjects = 1;
	m_bbObjects = vector<ModelObject*>(m_nObjects);

	if (type == M_Map_1) {
		XMVECTOR shadowPlane = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		XMVECTOR toMainLight = -XMVectorSet(0.5f, -1.0f, 0.4f, 0.0f);
		XMMATRIX S = XMMatrixShadow(shadowPlane, toMainLight);
		XMMATRIX shadowoffset = XMMatrixTranslation(-4.0f, 11.f, -5.5f);
		shadow_mat = S * shadowoffset;//XMMatrixShadow(XMVectorSet(+0.0f, -1.0f, +0.0f, 0.0f), XMVector3Normalize(XMVectorSet(0.f, -1.0f, 0.36f, 0.0f))) * XMMatrixTranslation(+0.0f, +11.0f, 0.f);
	}
	if (type == M_Map_3_cake_2 || type == M_Map_3_cake_2_2 || type == M_Map_3_cake_3 || type == M_Map_3_cake_3_2) {
		XMVECTOR shadowPlane = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		XMVECTOR toMainLight = -XMVectorSet(0.5f, -1.0f, 0.4f, 0.0f);
		XMMATRIX S = XMMatrixShadow(shadowPlane, toMainLight);
		XMMATRIX shadowoffset;
		if (type == M_Map_3_cake_2) shadowoffset = XMMatrixTranslation(250.f, 1.5f, 50.f);
		if (type == M_Map_3_cake_2_2) shadowoffset = XMMatrixTranslation(-250.f, 1.5f, 50.f);
		if (type == M_Map_3_cake_3) shadowoffset = XMMatrixTranslation(250.f, 80.f, 80.f);
		if (type == M_Map_3_cake_3_2) shadowoffset = XMMatrixTranslation(-250.f, 80.f, 80.f);
		shadow_mat = S * shadowoffset;//XMMatrixShadow(XMVectorSet(+0.0f, -1.0f, +0.0f, 0.0f), XMVector3Normalize(XMVectorSet(0.f, -1.0f, 0.36f, 0.0f))) * XMMatrixTranslation(+0.0f, +11.0f, 0.f);
	}

	//if (map_type == M_Map_3_cake_2) map->SetPosition(map_3_distance, 0.f, 50.f);	250
	//if (map_type == M_Map_3_cake_2_2) map->SetPosition(-map_3_distance, 0.f, 50.f);
	//if (map_type == M_Map_3_cake_3) map->SetPosition(map_3_distance, 80.f, 80.f);
	//if (map_type == M_Map_3_cake_3_2) map->SetPosition(-map_3_distance, 80.f, 80.f);
	//if (map_type == M_Map_1_macaron_3) map->SetPosition(0, 80.f, 50.f);

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\map\\black.dds", 0);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, 1);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_ShadowCB->Resource(), D3DUtil::CalcConstantBufferByteSize(sizeof(CB_SHADOW_INFO)));
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 1, true);

	CreateGraphicsRootSignature(pd3dDevice);

	BuildPSO(pd3dDevice, nRenderTargets);

	for (int i = 0; i < m_nObjects; i++) {
		ModelObject* Shadow_ = new ModelObject(static_model, pd3dDevice, pd3dCommandList);
		Shadow_->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
		m_bbObjects[i] = Shadow_;
	}
}

/////////////////////////////////////////////////////////////////////////////

PlayerShadowShader::PlayerShadowShader(Model_Animation *ma) : PlayerShader(ma)
{
}

PlayerShadowShader::~PlayerShadowShader()
{
}

D3D12_SHADER_BYTECODE PlayerShadowShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	wchar_t filename[100] = L"Model.hlsl";
	return(CShader::CompileShaderFromFile(filename, "VSDynamicShadow", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE PlayerShadowShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{

	wchar_t filename[100] = L"Model.hlsl";
	return(CShader::CompileShaderFromFile(filename, "PSShadow", "ps_5_1", ppd3dShaderBlob));
}

D3D12_BLEND_DESC PlayerShadowShader::CreateBlendState(int index)
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;			// FALSE
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = TRUE;	// FALSE
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;	//D3D12_BLEND_ONE
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//D3D12_BLEND_ZERO
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	return(d3dBlendDesc);
}

//깊이-스텐실 검사를 위한 상태를 설정하기 위한 구조체를 반환한다.
D3D12_DEPTH_STENCIL_DESC PlayerShadowShader::CreateDepthStencilState(int index)
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = true;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;			// D3D12_COMPARISON_FUNC_LESS
	d3dDepthStencilDesc.StencilEnable = true;
	d3dDepthStencilDesc.StencilReadMask = 0xff;
	d3dDepthStencilDesc.StencilWriteMask = 0xff;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_INCR;		// D3D12_STENCIL_OP_KEEP
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;	// D3D12_COMPARISON_FUNC_NEVER
	// 후면 다각형 렌더링
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_INCR;		// D3D12_STENCIL_OP_KEEP	//D3D12_STENCIL_OP_REPLACE
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;	// D3D12_COMPARISON_FUNC_NEVER
	return(d3dDepthStencilDesc);
}

void PlayerShadowShader::UpdateShaderVariables(ID3D12GraphicsCommandList * pd3dCommandList)
{
	CB_DYNAMICOBJECT_INFO cBone;
	for (UINT i = 0; i < m_nObjects; ++i) {
		//cBone.m_xmf4x4World = m_ppObjects[i]->m_xmf4x4World;
		XMStoreFloat4x4(&cBone.m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_bbObjects[i]->m_xmf4x4World) * shadow_mat));
		cBone.m_nMaterial = 0;
		XMStoreFloat4x4(&cBone.m_xmf4x4ShadowTransform, XMMatrixTranspose(XMLoadFloat4x4(&m_bbObjects[i]->m_xmf4x4World) * shadow_mat));

		memcpy(cBone.m_bone, m_bbObjects[i]->GetBoneData(), sizeof(XMFLOAT4X4) * m_bbObjects[i]->GetBoneNum());

		m_BoneCB->CopyData(i, cBone);
	}
}

void PlayerShadowShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nRenderTargets, void * pContext)
{
	m_nPSO = 1;
	CreatePipelineParts();

	m_nObjects = 1;
	m_bbObjects = vector<ModelObject*>(m_nObjects);

	XMVECTOR shadowPlane = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR toMainLight = -XMVectorSet(0.5f, -1.0f, 0.4f, 0.0f);
	XMMATRIX S = XMMatrixShadow(shadowPlane, toMainLight);
	XMMATRIX shadowoffset = XMMatrixTranslation(0.0f, 11.f, 0.0f);
	shadow_mat = S * shadowoffset;//XMMatrixShadow(XMVectorSet(+0.0f, -1.0f, +0.0f, 0.0f), XMVector3Normalize(XMVectorSet(0.f, -1.0f, 0.36f, 0.0f))) * XMMatrixTranslation(+0.0f, +11.0f, 0.f);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1);
	//CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 3);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_BoneCB->Resource(), D3DUtil::CalcConstantBufferByteSize(sizeof(CB_DYNAMICOBJECT_INFO)));

	CreateGraphicsRootSignature(pd3dDevice);
	BuildPSO(pd3dDevice, nRenderTargets);

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\character\\cloth_1.dds", 0);	//cloth_2
	//pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\character\\body.dds", 0);
	//pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\character\\cloth.dds", 1);
	//pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\character\\eye.dds", 2);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 1, true);

	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
	m_pMaterial->SetReflection(1);

	CPlayer* player = new CPlayer(model_anim, pd3dDevice, pd3dCommandList);
	player->SetAnimations(model_anim->getAnimCount(), model_anim->getAnim(0));
	m_Camera = player->GetCamera();
	//cout << "애니 갯수 : " << model_anim->getAnimCount() << endl;
	//cout << "뼈 : " << model_anim-> << endl;

	//tmp->SetPosition(XMFLOAT3(0, 0, 0));
	player->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * 0));
	m_bbObjects[0] = player;
	m_player = player;
}

void PlayerShadowShader::ChangeAnimation(int m_animindex)
{
	if (m_bbObjects[0]) m_bbObjects[0]->ChangeAnimation(m_animindex);
}

void PlayerShadowShader::ChangeAnimationSpeed(UINT AnimIndex, float speed)
{
	if (m_bbObjects[0]) m_bbObjects[0]->ChangeAnimationSpeed(AnimIndex, speed);
}

void PlayerShadowShader::ResetAnimationSpeed(UINT AnimIndex)
{
	if (m_bbObjects[0]) m_bbObjects[0]->ResetAnimationSpeed(AnimIndex);
}

void PlayerShadowShader::init()
{
	for (int i = 0; i < m_bbObjects[0]->GetNumofAnim(); i++) {
		m_bbObjects[0]->m_ani[i]->initspeed();
	}
	m_bbObjects[0]->m_ani[Anim_Walk]->SetAnimSpeed(1.3);
}

int PlayerShadowShader::getAnimIndex()
{
	return m_bbObjects[0]->getAnimIndex();
}

void PlayerShadowShader::Animate(float fTimeElapsed, XMFLOAT3 pos, float jump)
{
	if (IsZero(fTimeElapsed))
		return;
	for (UINT i = 0; i < m_nObjects; ++i) {
		if (m_bbObjects[i]) m_bbObjects[i]->Animate(fTimeElapsed);
		XMVECTOR shadowPlane = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		XMVECTOR toMainLight = -XMVectorSet(0.5f, -1.0f, 0.4f, 0.0f);
		XMMATRIX S = XMMatrixShadow(shadowPlane, toMainLight);
		XMMATRIX shadowoffset = XMMatrixTranslation(pos.x, pos.y - (pos.y - jump), pos.z);

		shadow_mat = S * shadowoffset;//XMMatrixShadow(XMVectorSet(+0.0f, -1.0f, +0.0f, 0.0f), XMVector3Normalize(XMVectorSet(0.f, -1.0f, 0.36f, 0.0f))) * XMMatrixTranslation(+0.0f, +11.0f, 0.f);
	}
}

////////////////////

ShadowReverseModelShader::ShadowReverseModelShader(LoadModel *ma) : CModelShader(ma)
{
	static_model = ma;
}

D3D12_SHADER_BYTECODE ShadowReverseModelShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	wchar_t filename[100] = L"Model.hlsl";
	return(CShader::CompileShaderFromFile(filename, "PSReverseShadow", "ps_5_1", ppd3dShaderBlob));
}

D3D12_BLEND_DESC ShadowReverseModelShader::CreateBlendState(int index)
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;			// FALSE
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = TRUE;	// FALSE
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;	//D3D12_BLEND_ONE
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//D3D12_BLEND_ZERO
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	return(d3dBlendDesc);
}

D3D12_DEPTH_STENCIL_DESC ShadowReverseModelShader::CreateDepthStencilState(int index)
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = true;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;			// D3D12_COMPARISON_FUNC_LESS
	d3dDepthStencilDesc.StencilEnable = true;
	d3dDepthStencilDesc.StencilReadMask = 0xff;
	d3dDepthStencilDesc.StencilWriteMask = 0xff;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_INVERT;		// D3D12_STENCIL_OP_KEEP
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;	// D3D12_COMPARISON_FUNC_NEVER
	// 후면 다각형 렌더링
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;		// D3D12_STENCIL_OP_KEEP	//D3D12_STENCIL_OP_REPLACE
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;	// D3D12_COMPARISON_FUNC_NEVER
	return(d3dDepthStencilDesc);
}

void ShadowReverseModelShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int type, int nRenderTargets, void * pContext)
{
	m_nPSO = 1;
	CreatePipelineParts();

	m_nObjects = 1;
	m_bbObjects = vector<ModelObject*>(m_nObjects);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, 1);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_ObjectCB->Resource(), D3DUtil::CalcConstantBufferByteSize(sizeof(CB_GAMEOBJECT_INFO)));

	CreateGraphicsRootSignature(pd3dDevice);
	BuildPSO(pd3dDevice, nRenderTargets);

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\map\\black.dds", 0);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 1, true);

	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
	m_pMaterial->SetReflection(1);

	XMFLOAT3 a = XMFLOAT3(0, 1, 0);
	for (int i = 0; i < m_nObjects; i++) {
		ModelObject* Shadow_ = new ModelObject(static_model, pd3dDevice, pd3dCommandList);
		Shadow_->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
		m_bbObjects[i] = Shadow_;
		if (type == 0) m_bbObjects[i]->SetPosition(0.f, -20.f, 0.f);
		if (type == 1) { m_bbObjects[i]->SetPosition(185.f, -10.5f, 0.f); m_bbObjects[i]->Rotate(&a, 90.f); }
		if (type == 2) { m_bbObjects[i]->SetPosition(-185.f, -10.5f, 0.f); m_bbObjects[i]->Rotate(&a, 90.f); }
	}
	delete pTexture;
}

//////////////

D3D12_SHADER_BYTECODE ShadowREverseShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	wchar_t filename[100] = L"Model.hlsl";
	return(CShader::CompileShaderFromFile(filename, "PSReverseShadow", "ps_5_1", ppd3dShaderBlob));
}

D3D12_BLEND_DESC ShadowREverseShader::CreateBlendState(int index)
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;			// FALSE
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = TRUE;	// FALSE
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;	//D3D12_BLEND_ONE
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//D3D12_BLEND_ZERO
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	return(d3dBlendDesc);
}

D3D12_DEPTH_STENCIL_DESC ShadowREverseShader::CreateDepthStencilState(int index)
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = true;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;			// D3D12_COMPARISON_FUNC_LESS
	d3dDepthStencilDesc.StencilEnable = true;
	d3dDepthStencilDesc.StencilReadMask = 0xff;
	d3dDepthStencilDesc.StencilWriteMask = 0xff;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_INVERT;		// D3D12_STENCIL_OP_KEEP
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;	// D3D12_COMPARISON_FUNC_NEVER
	// 후면 다각형 렌더링
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;		// D3D12_STENCIL_OP_KEEP	//D3D12_STENCIL_OP_REPLACE
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;	// D3D12_COMPARISON_FUNC_NEVER
	return(d3dDepthStencilDesc);
}

void ShadowREverseShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int type, int nRenderTargets, void * pContext)
{
	m_nPSO = 1;
	CreatePipelineParts();

	m_nObjects = 1;
	m_ppObjects = vector<CGameObject*>(m_nObjects);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, 1);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_ObjectCB->Resource(), D3DUtil::CalcConstantBufferByteSize(sizeof(CB_GAMEOBJECT_INFO)));

	CreateGraphicsRootSignature(pd3dDevice);
	BuildPSO(pd3dDevice, nRenderTargets);

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\map\\black.dds", 0);	//1400*788
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 2, true);

	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
	m_pMaterial->SetReflection(1);

	XMFLOAT3 a = XMFLOAT3(0.f, 1.f, 0.f);

	for (int i = 0; i < m_nObjects; i++) {

		CGameObject *Map_Object = NULL;

		Map_Object = new CGameObject();

		if (type == 0)Map_Object->SetPosition(0, -10, 190.0);	//map_1
		if (type == 1)Map_Object->SetPosition(297.8, -10, 0);	//map_1
		if (type == 2)Map_Object->SetPosition(0, -10, 0);		//map_1
		if (type == 3)Map_Object->SetPosition(-297.5, -10, 0);	//map_1
		if (type == 4)Map_Object->SetPosition(0, 0, 161);		//map_3_뒤
		if (type == 5)Map_Object->SetPosition(0, -20, 150);		//map_3_뒤
		if (type == 6)Map_Object->SetPosition(0, -45, -87);		//map_3_뒤
		if (type == 7)Map_Object->SetPosition(417, -10, 0);		//map_3_뒤

		m_ppObjects[i] = Map_Object;

		CMesh *pCubeMesh = NULL;

		if (type == 0)pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 1000.f, 100.f, 20.f);
		if (type == 1)pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 20.f, 100.f, 1000.f);
		if (type == 2)pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 115.f, 30.f, 115.f);
		if (type == 3)pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 20.f, 100.f, 1000.f);
		if (type == 4)pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 1000.f, 100.f, 20.f);
		if (type == 5)pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 185.f, 50.f, 100.f);
		if (type == 6)pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 185.f, 50.f, 200.f);
		if (type == 7)pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 20.f, 100.f, 500.f);

		m_ppObjects[i]->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
		m_ppObjects[i]->SetMesh(pCubeMesh);

		//m_ppObjects[i]->Rotate(&a, 90.f);
	}
}

////////////////////////////////////////////////////////////////////

EffectShader::EffectShader()
{
}

EffectShader::~EffectShader()
{
}

D3D12_BLEND_DESC EffectShader::CreateBlendState(int index)
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));

	d3dBlendDesc.AlphaToCoverageEnable = true;
	d3dBlendDesc.IndependentBlendEnable = false;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = false;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;	//D3D12_BLEND_INV_SRC_ALPHA
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;

	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;	//D3D12_BLEND_OP_ADD	//D3D12_BLEND_OP_SUBTRACT
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_AND;	// D3D12_LOGIC_OP_NOOP
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return d3dBlendDesc;
}

D3D12_SHADER_BYTECODE EffectShader::CreateVertexShader(ID3DBlob ** ppd3dShaderBlob)
{
	wchar_t filename[100] = L"Model.hlsl";
	return(CShader::CompileShaderFromFile(filename, "VSLightning", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE EffectShader::CreatePixelShader(ID3DBlob ** ppd3dShaderBlob)
{
	wchar_t filename[100] = L"Model.hlsl";
	return(CShader::CompileShaderFromFile(filename, "PSEffect", "ps_5_1", ppd3dShaderBlob));
}

void EffectShader::CreateGraphicsRootSignature(ID3D12Device * pd3dDevice)
{
	int i = 0;
	ComPtr<ID3D12RootSignature> pd3dGraphicsRootSignature = nullptr;

	CD3DX12_DESCRIPTOR_RANGE pd3dDescriptorRanges[2];

	pd3dDescriptorRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, CBVObjectInfo, 0, 0); // GameObject
	pd3dDescriptorRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, SRVAnimation, 0, 0); // Texture

	CD3DX12_ROOT_PARAMETER pd3dRootParameters[3];

	pd3dRootParameters[0].InitAsConstantBufferView(1);
	pd3dRootParameters[1].InitAsDescriptorTable(1, &pd3dDescriptorRanges[0], D3D12_SHADER_VISIBILITY_ALL);
	pd3dRootParameters[2].InitAsDescriptorTable(1, &pd3dDescriptorRanges[1], D3D12_SHADER_VISIBILITY_PIXEL);

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

	ThrowIfFailed(pd3dDevice->CreateRootSignature(0,
		pd3dSignatureBlob->GetBufferPointer(),
		pd3dSignatureBlob->GetBufferSize(),
		IID_PPV_ARGS(m_RootSignature[PSO_SHADOWMAP].GetAddressOf()))
	);
}

void EffectShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int type, int nRenderTargets, void * pContext)
{
	m_nPSO = 1;
	CreatePipelineParts();

	m_nObjects = 10;
	m_ppObjects = vector<CGameObject*>(m_nObjects);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, 1);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_ObjectCB->Resource(), D3DUtil::CalcConstantBufferByteSize(sizeof(CB_GAMEOBJECT_INFO)));

	CreateGraphicsRootSignature(pd3dDevice);
	BuildPSO(pd3dDevice);

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\image\\effect_lightning.dds", 0);
	spritenum = 8;	//스프라이트 갯수

	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 2, true);

	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
	m_pMaterial->SetReflection(1);

	for (int i = 0; i < m_nObjects; i++) {
		CGameObject *effect_object = NULL;
		effect_object = new CGameObject();
		m_ppObjects[i] = effect_object;
		m_ppObjects[i]->nowsprite = i;

		CMesh *pCubeMesh = NULL;
		pCubeMesh = new CreateQuad(pd3dDevice, pd3dCommandList, 0, 0, 60, 120, 0);	// pos(x, y), Width(w, h), depth
		m_ppObjects[i]->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
		m_ppObjects[i]->SetMesh(pCubeMesh);

		if (type == M_Map_3) {

		}
		else {
			if (i == 0) m_ppObjects[i]->SetPosition(-240.f, 100.f, 110.f);
			if (i == 1) m_ppObjects[i]->SetPosition(-120.f, 100.f, 110.f);
			if (i == 2) m_ppObjects[i]->SetPosition(60.f, 100.f, 110.f);
			if (i == 3) m_ppObjects[i]->SetPosition(180.f, 100.f, 110.f);
			if (i == 4) m_ppObjects[i]->SetPosition(-150.f, 100.f, 0.f);
			if (i == 5) m_ppObjects[i]->SetPosition(90.f, 100.f, 0.f);
			if (i == 6) m_ppObjects[i]->SetPosition(-240.f, 100.f, -110.f);
			if (i == 7) m_ppObjects[i]->SetPosition(-120.f, 100.f, -110.f);
			if (i == 8) m_ppObjects[i]->SetPosition(60.f, 100.f, -110.f);
			if (i == 9) m_ppObjects[i]->SetPosition(180.f, 100.f, -110.f);
		}
	}
	delete pTexture;
}

void EffectShader::UpdateShaderVariables(ID3D12GraphicsCommandList * pd3dCommandList)
{
	CB_GAMEOBJECT_INFO cBuffer;
	for (UINT i = 0; i < m_nObjects; ++i) {
		XMStoreFloat4x4(&cBuffer.m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppObjects[i]->m_xmf4x4World)));
		cBuffer.m_nMaterial = m_ppObjects[i]->nowsprite;
		m_ObjectCB->CopyData(i, cBuffer);
	}
}

void EffectShader::Animate(float fTimeElapsed)
{
	for (int i = 0; i < m_nObjects; i++) {
		if (m_ppObjects[i]->visible == true) {
			m_ppObjects[i]->start_time += fTimeElapsed;
			if (m_ppObjects[i]->start_time > 0.08f) {
				m_ppObjects[i]->nowsprite++;
				m_ppObjects[i]->start_time = 0;
				if (m_ppObjects[i]->nowsprite > spritenum + 1)
					m_ppObjects[i]->nowsprite = 0;
			}
			m_ppObjects[i]->duration_time += fTimeElapsed;
			if (m_ppObjects[i]->duration_time > 1.f) {	//번개 지속시간
				m_ppObjects[i]->visible = false;
				m_ppObjects[i]->duration_time = 0.f;
				m_ppObjects[i]->start_time = 0;
			}
		}
	}
}

void EffectShader::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera)
{
	CModelShader::OnPrepareRender(pd3dCommandList, 0);

	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);

	for (UINT j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j] && m_ppObjects[j]->visible)
		{
			m_ppObjects[j]->Render(pd3dCommandList, pCamera);
		}
	}
}

D3D12_RASTERIZER_DESC EffectShader::CreateRasterizerState(int index)
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));

	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	//d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;

	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	//d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;	//D3D12_CULL_MODE_BACK
	//d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_FRONT;

	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;

	d3dRasterizerDesc.DepthClipEnable = TRUE;
	//d3dRasterizerDesc.DepthClipEnable = FALSE;

	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	return(d3dRasterizerDesc);
}

///////////////////////////////////////////////////////

D3D12_SHADER_BYTECODE SkillEffectShader::CreateVertexShader(ID3DBlob ** ppd3dShaderBlob)
{
	wchar_t filename[100] = L"Model.hlsl";
	return(CShader::CompileShaderFromFile(filename, "VSSkillEffect_1", "vs_5_1", ppd3dShaderBlob));	//	//VSSkillEffect_1
}

void SkillEffectShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nRenderTargets, void * pContext)
{
	m_nPSO = 1;
	CreatePipelineParts();

	m_nObjects = 2;
	m_ppObjects = vector<CGameObject*>(m_nObjects);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, 1);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_ObjectCB->Resource(), D3DUtil::CalcConstantBufferByteSize(sizeof(CB_GAMEOBJECT_INFO)));

	CreateGraphicsRootSignature(pd3dDevice);
	BuildPSO(pd3dDevice);

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\effect\\effect_1.dds", 0);
	spritenum = 16;	//스프라이트 갯수

	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 2, true);

	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
	m_pMaterial->SetReflection(1);

	XMFLOAT3 a = XMFLOAT3(0.f, 1.f, 0.f);

	for (int i = 0; i < m_nObjects; i++) {
		CGameObject *effect_object = NULL;
		effect_object = new CGameObject();
		m_ppObjects[i] = effect_object;

		CMesh *pCubeMesh = NULL;
		pCubeMesh = new CreateQuad(pd3dDevice, pd3dCommandList, 0, 0, 50, 50, 0);	// pos(x, y), Width(w, h), depth
		m_ppObjects[i]->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * 0));
		m_ppObjects[i]->SetMesh(pCubeMesh);

		if (i == 0) m_ppObjects[i]->Rotate(&a, 45.f);
		if (i == 1) m_ppObjects[i]->Rotate(&a, -45.f);
	}
	delete pTexture;
}

void SkillEffectShader::Animate(float fTimeElapsed, XMFLOAT3 pos)
{
	for (int i = 0; i < m_nObjects; i++) {
		if (m_ppObjects[i]->visible == true) {
			m_ppObjects[i]->start_time += fTimeElapsed;
			if (m_ppObjects[i]->start_time > 0.08f) {
				m_ppObjects[i]->nowsprite++;
				m_ppObjects[i]->start_time = 0;
				if (m_ppObjects[i]->nowsprite > spritenum + 1)
					m_ppObjects[i]->nowsprite = 0;
			}
			m_ppObjects[i]->duration_time += fTimeElapsed;
			if (m_ppObjects[i]->duration_time > 10.f) {
				m_ppObjects[i]->visible = false;
				m_ppObjects[i]->duration_time = 0.f;
				m_ppObjects[i]->start_time = 0;
			}
		}
	}
}

D3D12_SHADER_BYTECODE SkillParticleShader::CreateVertexShader(ID3DBlob ** ppd3dShaderBlob)
{
	wchar_t filename[100] = L"Model.hlsl";
	return(CShader::CompileShaderFromFile(filename, "VSDiffused", "vs_5_1", ppd3dShaderBlob));
}

void SkillParticleShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int type, int nRenderTargets, void * pContext)
{
	m_nPSO = 1;
	CreatePipelineParts();

	m_nObjects = 10;
	m_ppObjects = vector<CGameObject*>(m_nObjects);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, 1);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_ObjectCB->Resource(), D3DUtil::CalcConstantBufferByteSize(sizeof(CB_GAMEOBJECT_INFO)));

	CreateGraphicsRootSignature(pd3dDevice);
	BuildPSO(pd3dDevice);

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	if (type == 0)
		pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\effect\\skill_attack.dds", 0);
	else if (type == 1)
		pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\effect\\skill_speed.dds", 0);
	else if (type == 2)
		pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\effect\\skill_health.dds", 0);

	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 2, true);

	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
	m_pMaterial->SetReflection(1);

	for (int i = 0; i < m_nObjects; i++) {
		CGameObject *effect_object = NULL;
		effect_object = new CGameObject();
		m_ppObjects[i] = effect_object;

		CMesh *pCubeMesh = NULL;
		pCubeMesh = new CreateQuad(pd3dDevice, pd3dCommandList, 0, 0, 5, 5, 0);	// pos(x, y), Width(w, h), depth
		m_ppObjects[i]->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
		m_ppObjects[i]->SetMesh(pCubeMesh);
	}
	delete pTexture;
}

void SkillParticleShader::UpdateShaderVariables(ID3D12GraphicsCommandList * pd3dCommandList)
{
	CB_GAMEOBJECT_INFO cBuffer;
	for (UINT i = 0; i < m_nObjects; ++i) {
		XMStoreFloat4x4(&cBuffer.m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppObjects[i]->m_xmf4x4World)));
		cBuffer.m_nMaterial = 0;
		m_ObjectCB->CopyData(i, cBuffer);
	}
}

void SkillParticleShader::Animate(float fTimeElapsed)
{
	if (!visible) {
		return;
	}
	if (IsZero(fTimeElapsed))
		return;
	ftime += fTimeElapsed;
	if (ftime > 0.05f) {
		ftime = 0.f;
		for (int i = 0; i < m_nObjects; ++i) {
			auto pos = m_ppObjects[i]->GetPosition();
			pos.x += rand() % 2 - 1;
			pos.z += rand() % 2 - 1;
			pos.y += 3;
			m_ppObjects[i]->SetPosition(pos);
			if (pos.y > startpos.y + 50) {
				m_ppObjects[i]->visible = false;
				for (int j = 0; j < m_nObjects; ++j) {
					if (m_ppObjects[j]->visible == false) {
						if (j == m_nObjects - 1)
							visible = false;
					}
					else break;
				}
			}
		}
	}

}


void SkillParticleShader::ShowParticle(bool show, XMFLOAT3 pos)
{
	if (show) {
		visible = true;
		startpos = pos;
		for (int i = 0; i < m_nObjects; ++i) {
			pos.x += rand() % 10 - 5;
			pos.y += rand() % 10;
			pos.z += rand() % 10 - 5;
			m_ppObjects[i]->SetPosition(pos);
			m_ppObjects[i]->visible = true;
		}
	}
	else {
		visible = false;
		for (int i = 0; i < m_nObjects; ++i) {
			m_ppObjects[i]->visible = false;
		}
	}
}

void SkillParticleShader::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera)
{
	if (!visible)
		return;
	CModelShader::OnPrepareRender(pd3dCommandList, 0);

	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);

	for (UINT j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j] && m_ppObjects[j]->visible)
		{
			m_ppObjects[j]->Render(pd3dCommandList, pCamera);
		}
	}
}

////////////////////////
D3D12_SHADER_BYTECODE ExplosionShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	wchar_t filename[100] = L"Model.hlsl";
	return(CShader::CompileShaderFromFile(filename, "VSDiffused", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE ExplosionShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	wchar_t filename[100] = L"Model.hlsl";
	return(CShader::CompileShaderFromFile(filename, "PSDiffused", "ps_5_1", ppd3dShaderBlob));
}

void ExplosionShader::UpdateShaderVariables(ID3D12GraphicsCommandList * pd3dCommandList)//, XMFLOAT4X4 *pxmf4x4World)
{
	CB_GAMEOBJECT_INFO cBuffer;
	for (UINT i = 0; i < m_nObjects; ++i) {
		XMStoreFloat4x4(&cBuffer.m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppObjects[i]->m_xmf4x4World)));
		cBuffer.m_nMaterial = 0;
		m_ObjectCB->CopyData(i, cBuffer);
	}
}

inline float RandF(float fMin, float fMax)
{
	return(fMin + ((float)rand() / (float)RAND_MAX) * (fMax - fMin));
}

XMVECTOR ExplosionShader::RandomUnitVectorOnSphere()
{
	XMVECTOR xmvOne = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR xmvZero = XMVectorZero();

	while (true)
	{
		XMVECTOR v = XMVectorSet(RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), 0.0f);
		if (!XMVector3Greater(XMVector3LengthSq(v), xmvOne)) return(XMVector3Normalize(v));
	}
}

void ExplosionShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nRenderTargets, void * pContext)
{
	m_nPSO = 1;
	CreatePipelineParts();

	m_nObjects = EXPLOSION_DEBRISES;
	m_ppObjects = vector<CGameObject*>(m_nObjects);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, 1);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_ObjectCB->Resource(), D3DUtil::CalcConstantBufferByteSize(sizeof(CB_GAMEOBJECT_INFO)));

	CreateGraphicsRootSignature(pd3dDevice);
	BuildPSO(pd3dDevice, nRenderTargets);

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\map\\black.dds", 0);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 2, true);

	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
	m_pMaterial->SetReflection(1);

	for (int i = 0; i < m_nObjects; i++) {
		m_ppObjects[i] = new CGameObject();

		CMesh *pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 2.0f, 2.0f, 2.0f);
		XMStoreFloat3(&m_pxmf3SphereVectors[i], RandomUnitVectorOnSphere());
		m_ppObjects[i]->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
		m_ppObjects[i]->SetMesh(pCubeMesh);
		//m_ppObjects[i]->SetPosition(-100, 50, -50);
	}
	delete pTexture;

}

void ExplosionShader::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera)
{
	CModelShader::OnPrepareRender(pd3dCommandList, 0);

	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{
			m_ppObjects[j]->Render(pd3dCommandList, pCamera);
		}
	}
}

void ExplosionShader::Animate(float fTimeElapsed, XMFLOAT3 pos)
{
	if (IsZero(fTimeElapsed)) return;

	if (m_bBlowingUp)
	{
		for (int i = 0; i < m_nObjects; ++i)
		{
			m_fElapsedTimes += fTimeElapsed * 0.1f;
			if (m_fElapsedTimes <= m_fDuration)
			{
				XMFLOAT3 xmf3Position = pos;
				for (int i = 0; i < EXPLOSION_DEBRISES; i++)
				{
					m_ppObjects[i]->m_xmf4x4World = Matrix4x4::Identity();
					m_ppObjects[i]->m_xmf4x4World._41 = xmf3Position.x + m_pxmf3SphereVectors[i].x * m_fExplosionSpeed * m_fElapsedTimes;
					m_ppObjects[i]->m_xmf4x4World._42 = xmf3Position.y + 10.f + m_pxmf3SphereVectors[i].y * m_fExplosionSpeed * m_fElapsedTimes;
					m_ppObjects[i]->m_xmf4x4World._43 = xmf3Position.z + m_pxmf3SphereVectors[i].z * m_fExplosionSpeed * m_fElapsedTimes;

					m_ppObjects[i]->m_xmf4x4World = Matrix4x4::Multiply(Matrix4x4::RotationAxis(m_pxmf3SphereVectors[i], m_fExplosionRotation * m_fElapsedTimes), m_ppObjects[i]->m_xmf4x4World);
				}
			}
			else
			{
				m_bBlowingUp = false;
				m_fElapsedTimes = 0.0f;
			}

			if (m_ppObjects[i]) m_ppObjects[i]->Animate(fTimeElapsed);
		}
	}
}

////////////////////////////////////

ExplosionModelShader::ExplosionModelShader(LoadModel *ma) : CModelShader(ma)
{
}

XMVECTOR ExplosionModelShader::RandomUnitVectorOnSphere()
{
	XMVECTOR xmvOne = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR xmvZero = XMVectorZero();

	while (true)
	{
		XMVECTOR v = XMVectorSet(RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), 0.0f);
		if (!XMVector3Greater(XMVector3LengthSq(v), xmvOne)) return(XMVector3Normalize(v));
	}
}

void ExplosionModelShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int type, int nRenderTargets, void * pContext)
{
	m_nPSO = 1;
	CreatePipelineParts();

	m_nObjects = EXPLOSION_DEBRISES;
	m_bbObjects = vector<ModelObject*>(m_nObjects);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, 1);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_ObjectCB->Resource(), D3DUtil::CalcConstantBufferByteSize(sizeof(CB_GAMEOBJECT_INFO)));

	CreateGraphicsRootSignature(pd3dDevice);
	BuildPSO(pd3dDevice, nRenderTargets);

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	if (type == 0)pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\effect\\star.dds", 0);
	if (type == 1)pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\effect\\star_1.dds", 0);
	if (type == 2)pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\effect\\star_2.dds", 0);
	if (type == 3)pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\effect\\star_3.dds", 0);
	if (type == 4)pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\effect\\star_4.dds", 0);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 2, true);

	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
	m_pMaterial->SetReflection(1);

	for (int i = 0; i < m_nObjects; i++) {
		ModelObject* map = new ModelObject(static_model, pd3dDevice, pd3dCommandList);
		XMStoreFloat3(&m_pxmf3SphereVectors[i], RandomUnitVectorOnSphere());
		map->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
		m_bbObjects[i] = map;
	}
	delete pTexture;
}

void ExplosionModelShader::Animate(float fTimeElapsed)
{
	if (IsZero(fTimeElapsed)) return;

	if (m_bBlowingUp)
	{
		for (int i = 0; i < m_nObjects; ++i)
		{
			m_fElapsedTimes += fTimeElapsed * 0.05f;

			if (m_fElapsedTimes <= m_fDuration)
			{
				m_bbObjects[i]->m_xmf4x4World = Matrix4x4::Identity();
				m_bbObjects[i]->m_xmf4x4World._41 = xmf3Position.x + m_pxmf3SphereVectors[i].x * m_fExplosionSpeed * m_fElapsedTimes;
				m_bbObjects[i]->m_xmf4x4World._42 = xmf3Position.y + 10.f + m_pxmf3SphereVectors[i].y * m_fExplosionSpeed * m_fElapsedTimes;
				m_bbObjects[i]->m_xmf4x4World._43 = xmf3Position.z + m_pxmf3SphereVectors[i].z * m_fExplosionSpeed * m_fElapsedTimes;

				m_bbObjects[i]->m_xmf4x4World = Matrix4x4::Multiply(Matrix4x4::RotationAxis(m_pxmf3SphereVectors[i], m_fExplosionRotation * m_fElapsedTimes), m_bbObjects[i]->m_xmf4x4World);
			}
			else
			{
				m_bBlowingUp = false;
				m_fElapsedTimes = 0.0f;
			}

			if (m_bbObjects[i]) m_bbObjects[i]->Animate(fTimeElapsed);
		}
	}
}

///////////////////////////////////////////////////////

D3D12_SHADER_BYTECODE TeamShader::CreateVertexShader(ID3DBlob ** ppd3dShaderBlob)
{
	wchar_t filename[100] = L"Model.hlsl";
	return(CShader::CompileShaderFromFile(filename, "VSDiffused", "vs_5_1", ppd3dShaderBlob));
}

void TeamShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int type, int nRenderTargets, void * pContext)
{
	m_nPSO = 1;
	CreatePipelineParts();

	m_nObjects = 8;
	m_ppObjects = vector<CGameObject*>(m_nObjects);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, 1);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_ObjectCB->Resource(), D3DUtil::CalcConstantBufferByteSize(sizeof(CB_GAMEOBJECT_INFO)));

	CreateGraphicsRootSignature(pd3dDevice);
	BuildPSO(pd3dDevice);

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	if (type == 0)
		pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\image\\Team.dds", 0);
	else if (type == 1)
		pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\image\\Enemy.dds", 0);

	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 2, true);

	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
	m_pMaterial->SetReflection(1);

	for (int i = 0; i < m_nObjects; i++) {
		CGameObject *effect_object = NULL;
		effect_object = new CGameObject();
		effect_object->visible = true;
		m_ppObjects[i] = effect_object;

		CMesh *pCubeMesh = NULL;
		pCubeMesh = new CreateQuad(pd3dDevice, pd3dCommandList, 0, 0, 5, 5, 0);	// pos(x, y), Width(w, h), depth
		m_ppObjects[i]->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
		m_ppObjects[i]->SetMesh(pCubeMesh);
	}
	delete pTexture;
}

void TeamShader::UpdateShaderVariables(ID3D12GraphicsCommandList * pd3dCommandList)
{
	CB_GAMEOBJECT_INFO cBuffer;
	for (UINT i = 0; i < m_nObjects; ++i) {
		XMStoreFloat4x4(&cBuffer.m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppObjects[i]->m_xmf4x4World)));
		cBuffer.m_nMaterial = 0;
		m_ObjectCB->CopyData(i, cBuffer);
	}
}

void TeamShader::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera, PlayerShader **ps)
{
	CModelShader::OnPrepareRender(pd3dCommandList, 0);

	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);

	for (UINT j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j] && m_ppObjects[j]->visible && ps[j]->render)
		{
			m_ppObjects[j]->Render(pd3dCommandList, pCamera);
		}
	}
}

void TeamShader::Animate(float fTimeElapsed)
{
}

/////////////////////////////////

void StunShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nRenderTargets, void * pContext)
{
	m_nPSO = 1;
	CreatePipelineParts();

	m_nObjects = 1;
	m_bbObjects = vector<ModelObject*>(m_nObjects);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, 1);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_ObjectCB->Resource(), D3DUtil::CalcConstantBufferByteSize(sizeof(CB_GAMEOBJECT_INFO)));

	CreateGraphicsRootSignature(pd3dDevice);
	BuildPSO(pd3dDevice, nRenderTargets);

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\effect\\star.dds", 0);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 2, true);

	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
	m_pMaterial->SetReflection(1);

	for (int i = 0; i < m_nObjects; i++) {
		ModelObject* map = new ModelObject(static_model, pd3dDevice, pd3dCommandList);
		map->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
		m_bbObjects[i] = map;
		m_bbObjects[i]->SetPosition(0, -100, 0);
	}
	delete pTexture;
}

void StunShader::Animate(float fTimeElapsed, XMFLOAT3 pos)
{
	if (IsZero(fTimeElapsed)) return;

	for (int i = 0; i < m_nObjects; ++i) {
		if (m_bbObjects[i]) {
			m_bbObjects[i]->Animate(fTimeElapsed);

			if (rotate_time >= 360) rotate_time = 0.f;
			else rotate_time += 2.f;

			//if (rotate_time_2 >= 360) rotate_time_2 = -360.f;
			//else rotate_time_2 += 0.1f;

			float x = cos(rotate_time / 360 * 3.14 * 2) * 10.f + pos.x;
			float z = sin(rotate_time / 360 * 3.14 * 2) * 10.f + pos.z;

			m_bbObjects[i]->SetPosition(x, pos.y + 38, z);
			m_bbObjects[i]->Rotate(&rotate_y_check, 3.f);
		}
	}
}

//////////////////////////

D3D12_SHADER_BYTECODE MagicShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	wchar_t filename[100] = L"Model.hlsl";
	return(CShader::CompileShaderFromFile(filename, "PSMagic", "ps_5_1", ppd3dShaderBlob));
}

D3D12_BLEND_DESC MagicShader::CreateBlendState(int index)
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;			// FALSE
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = TRUE;	// FALSE
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;	//D3D12_BLEND_ONE
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//D3D12_BLEND_ZERO
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;

	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	return(d3dBlendDesc);
}

void MagicShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nRenderTargets, void * pContext)
{
	m_nPSO = 1;
	CreatePipelineParts();

	m_nObjects = WEAPON_EACH_NUM;
	m_ppObjects = vector<CGameObject*>(m_nObjects);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, 1);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_ObjectCB->Resource(), D3DUtil::CalcConstantBufferByteSize(sizeof(CB_GAMEOBJECT_INFO)));

	CreateGraphicsRootSignature(pd3dDevice);
	BuildPSO(pd3dDevice, nRenderTargets);

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"resource\\effect\\magic_skill.dds", 0);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 2, true);

	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
	m_pMaterial->SetReflection(1);

	XMFLOAT3 aa = XMFLOAT3(1, 0, 0);
	for (UINT i = 0; i < m_nObjects; i++) {
		CGameObject *Map_Object = NULL;

		Map_Object = new CGameObject();
		Map_Object->SetPosition(0.f, 0.f, 0.f);
		m_ppObjects[i] = Map_Object;

		CMesh *pCubeMesh = NULL;
		pCubeMesh = new CreateQuad(pd3dDevice, pd3dCommandList, -50, 50, 100, 100, 0);	// pos(x, y), Width(w, h), depth
		m_ppObjects[i]->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
		m_ppObjects[i]->SetMesh(pCubeMesh);
		m_ppObjects[i]->Rotate(&aa, 90.f);
		m_ppObjects[i]->visible = false;
		m_ppObjects[i]->SetPosition(1000.f, 1000.f, 1000.f);
	}
	delete pTexture;
}

void MagicShader::Animate(float fTimeElapsed)
{
	if (IsZero(fTimeElapsed)) return;

	for (UINT i = 0; i < m_nObjects; ++i) {
		if (m_ppObjects[i]->visible) {
			XMFLOAT3 aa = XMFLOAT3(0, 0, 1);
			m_ppObjects[i]->Rotate(&aa, 2.f);
			if (m_ppObjects[i]) m_ppObjects[i]->Animate(fTimeElapsed);
		}
	}
}