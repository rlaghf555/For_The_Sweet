#pragma once
#include "Object.h"
#include "Camera.h"
#include "UploadBuffer.h"
#include "ModelObject.h"
#include "Player.h"
#include "Texture.h"
#include "Physx.h"

//���� ��ü�� ������ ���̴����� �Ѱ��ֱ� ���� ����ü(��� ����)�̴�.
struct CB_GAMEOBJECT_INFO
{
	XMFLOAT4X4	m_xmf4x4World;
	UINT		m_nMaterial = 0;
};

//�ν��Ͻ� ����(���� ��ü�� ���� ��ȯ ��İ� ��ü�� ����)�� ���� ����ü�̴�.
struct VS_VB_INSTANCE
{
	XMFLOAT4X4 m_xmf4x4Transform;
	XMFLOAT4 m_xmcColor;
};

class CompiledShaders
{
public:
	CompiledShaders();
	~CompiledShaders() {};

public:
	unordered_map<string, ComPtr<ID3DBlob>> CompiledShader;

	ComPtr<ID3DBlob> GetCompiledShader(const wstring& filename,
		const D3D_SHADER_MACRO* defines,
		const string& entrypoint,
		const string& target);

	static CompiledShaders* Instance();
};

class CShader
{
public:
	CShader();
	virtual ~CShader();
private:
	int m_nReferences = 0;
public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
	D3D12_SHADER_BYTECODE CompileShaderFromFile(WCHAR *pszFileName, LPCSTR pszShaderName,
		LPCSTR pszShaderProfile, ID3DBlob **ppd3dShaderBlob);
	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature
		*pd3dGraphicsRootSignature);
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList,
		XMFLOAT4X4 *pxmf4x4World);
	virtual void OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	XMFLOAT3 Pos_act;
	void setPos(XMFLOAT3 pos) { Pos_act = pos; }
	XMFLOAT3 getPos() { return Pos_act; }
protected:
	ID3D12PipelineState * *m_ppd3dPipelineStates = NULL;
	int m_nPipelineStates = 0;
};

//��CObjectsShader�� Ŭ������ ���� ��ü���� �����ϴ� ���̴� ��ü�̴�.
class CObjectsShader : public CShader
{
public:
	CObjectsShader();
	virtual ~CObjectsShader();
	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT3 Position);
	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT3 Position, int Object_Kind);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void ReleaseObjects();
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature	*pd3dGraphicsRootSignature);
	virtual void ReleaseUploadBuffers();
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
protected:
	vector<CGameObject* >	m_ppObjects;
	vector<ModelObject* >	m_bbObjects;
	int m_nObjects = 0;
};

class CModelShader : public CObjectsShader {
protected:
	UINT											modelIndex;
	ComPtr<ID3D12RootSignature>*					m_RootSignature = nullptr;
	ComPtr<ID3D12RootSignature>*					m_ComputeRootSignature = nullptr;

	ComPtr<ID3D12DescriptorHeap>					m_CBVHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap>					m_CbvSrvDescriptorHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap>					m_ComputeCbvSrvDescriptorHeap = nullptr;

	ComPtr<ID3D12PipelineState>*					m_pPSO = nullptr;
	ComPtr<ID3D12PipelineState>*					m_pComputePSO = nullptr;

	//ComPtr<ID3DBlob>*								m_VSByteCode = nullptr;
	//ComPtr<ID3DBlob>*								m_PSByteCode = nullptr;
	ComPtr<ID3DBlob>*								m_CSByteCode = nullptr;

	vector<D3D12_INPUT_ELEMENT_DESC>				m_pInputElementDesc;
	//vector<CGameObject* >							m_ppObjects;
	CMaterial										*m_pMaterial = NULL;

	UINT											m_nObjects = 0;
	UINT											m_nPSO = 1;
	UINT											m_nComputePSO = 0;
	UINT											m_nComputeBuffers = 0;
	UINT											m_nDescriptorUAVStartIndex = 0;
	XMUINT3*										m_nComputeThreadCount;
	XMUINT2*										m_nBuffTypeElements;

	D3D12_CPU_DESCRIPTOR_HANDLE						m_d3dCbvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE						m_d3dCbvGPUDescriptorStartHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE						m_d3dSrvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE						m_d3dSrvGPUDescriptorStartHandle;

	D3D12_CPU_DESCRIPTOR_HANDLE						m_d3dComputeCbvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE						m_d3dComputeCbvGPUDescriptorStartHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE						m_d3dComputeSrvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE						m_d3dComputeSrvGPUDescriptorStartHandle;

	vector<ComPtr<ID3D12Resource>>					m_pComputeUAVBuffers;
	vector<ComPtr<ID3D12Resource>>					m_pComputeOutputBuffers;

	vector<D3D12_CPU_DESCRIPTOR_HANDLE>				m_pComputeSRVUAVCPUHandles;
	vector<D3D12_GPU_DESCRIPTOR_HANDLE>				m_pComputeSRVUAVGPUHandles;

	vector<D3D12_INPUT_ELEMENT_DESC>				m_InputLayout;

	//UploadBuffer<LIGHTS>*							m_LightsCB = nullptr;
	//UploadBuffer<MATERIALS>*						m_MatCB = nullptr;

	DXGI_FORMAT										m_Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	unique_ptr<UploadBuffer<CB_GAMEOBJECT_INFO>>	m_ObjectCB = nullptr;
	Model_Animation *model_anim;
	LoadModel	*static_model;
public:
	CModelShader();
	CModelShader(Model_Animation *ma);
	CModelShader(LoadModel *ma);
	CModelShader(UINT index);
	~CModelShader();

	virtual D3D12_INPUT_LAYOUT_DESC		CreateInputLayout(int index = 0);

	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature	*pd3dGraphicsRootSignature);
	virtual void CreateCbvAndSrvDescriptorHeaps(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nConstantBufferViews, int nShaderResourceViews, bool bIsGraphics = true);
	virtual void CreateConstantBufferViews(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nConstantBufferViews, ID3D12Resource * pd3dConstantBuffers, UINT nStride);
	virtual void CreateGraphicsRootSignature(ID3D12Device * pd3dDevice);

	void CreateShaderResourceViews(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, CTexture * pTexture, UINT nRootParameterStartIndex, bool bAutoIncrement, bool bIsGraphics = true);

	virtual void BuildPSO(ID3D12Device * pd3dDevice, UINT nRenderTargets = 1, int index = 0);
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState(int index);
	virtual D3D12_BLEND_DESC CreateBlendState(int index);
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState(int index);

	virtual void OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList, int index);
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void CreatePipelineParts();
	virtual void BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, CPhysx* physx, int nRenderTargets = 1, void * pContext = NULL);
	virtual void Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera);
	//virtual void RenderToDepthBuffer(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera, XMFLOAT3& cameraPos, float offset);
	virtual void Animate(float fTimeElapsed);
	virtual UINT ModelIndex() const { return modelIndex; }

	virtual void setPosition(XMFLOAT3* pos, UINT num) {}
	virtual UINT getRemainObjects() { return 0; }
	virtual CGameObject** getObjects(UINT& num) { return nullptr; }
	//virtual CGameObject** getObjects() { return m_bbObjects.data(); }

	void setScale(float scale);
};

class MeshShader : public CModelShader
{
public:
	MeshShader();
	~MeshShader();

	virtual D3D12_INPUT_LAYOUT_DESC		CreateInputLayout(int index = 0);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
	
	virtual void CreateGraphicsRootSignature(ID3D12Device * pd3dDevice);
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);//, XMFLOAT4X4 *pxmf4x4World);
	virtual void BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nRenderTargets = 1, void * pContext = NULL);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	virtual void Animate(float fTimeElapsed);
	virtual void setScale(float scale);
};

class WaveShader : public MeshShader
{
public:
	WaveShader();
	~WaveShader();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
	
	virtual D3D12_BLEND_DESC CreateBlendState(int index);
	virtual void BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nRenderTargets = 1, void * pContext = NULL);
	virtual void Animate(float fTimeElapsed);
};



class WeaponShader : public CModelShader
{
protected:
	LoadModel	*weapon_model;

public:
	WeaponShader();
	WeaponShader(LoadModel *ma);
	~WeaponShader();

	virtual void BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int weapon_num, int nRenderTargets = 1, void * pContext = NULL);
	virtual ModelObject* getObject(UINT index) { return m_bbObjects[index]; }
};

class CottonCloudShader : public CModelShader
{
protected:
	LoadModel	*cloud_model;
public:
	CottonCloudShader();
	CottonCloudShader(LoadModel *ma);
	~CottonCloudShader();
	
	virtual D3D12_BLEND_DESC CreateBlendState(int index);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
	virtual void BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int kind, int nRenderTargets = 1, void * pContext = NULL);
};

class CInstancingShader : public CObjectsShader
{
public:
	CInstancingShader();
	virtual ~CInstancingShader();
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob); 
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
protected:
	//�ν��Ͻ� �����͸� �����ϴ� ���ۿ� �������̴�.
	ID3D12Resource * m_pd3dcbGameObjects = NULL;
	VS_VB_INSTANCE *m_pcbMappedGameObjects = NULL;
};

class DynamicModelShader : public CModelShader
{
protected:
	unique_ptr<UploadBuffer<CB_DYNAMICOBJECT_INFO>>	m_BoneCB = nullptr;
	//UploadBuffer<LIGHTS>*							m_LightsCB = nullptr;
	//UploadBuffer<MATERIALS>*						m_MatCB = nullptr;

public:
	CPlayer* m_player;

	DynamicModelShader(Model_Animation *ma);
	~DynamicModelShader();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void CreateGraphicsRootSignature(ID3D12Device * pd3dDevice);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nRenderTargets = 1, void * pContext = NULL);
	virtual void Animate(float fTimeElapsed);
	virtual  CGameObject* getPlayer() { return m_player; }
	virtual CGameObject* getObject(int index) { return m_ppObjects[index]; }
	virtual CGameObject** getObjects(UINT& num) {
		num = m_nObjects;
		return m_ppObjects.data();
	}
};

class PlayerShader : public DynamicModelShader
{
private:
	CCamera * m_Camera;
public:
	PlayerShader(Model_Animation *ma);
	~PlayerShader();

	virtual void CreateGraphicsRootSignature(ID3D12Device * pd3dDevice);
	virtual void BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nRenderTargets = 1, void * pContext = NULL);
	virtual void Animate(float fTimeElapsed);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList * pd3dCommandList);

	virtual  CGameObject* getPlayer() { return m_bbObjects[0]; }

};

class testBox : public MeshShader
{
public:
	BoundingOrientedBox bounding;
	testBox();
	~testBox();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState(int index);
	virtual void BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ModelObject *mo, int type,int nRenderTargets = 1, void * pContext = NULL);
	//virtual void BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ModelObject *mo, int nRenderTargets = 1, void * pContext = NULL);
	virtual void SetPosition(XMFLOAT3& pos);
	virtual void Rotate(float x, float y, float z);
	virtual CGameObject* getObjects() { return m_ppObjects[0]; }
};