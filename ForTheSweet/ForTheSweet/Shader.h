#pragma once
#include "Object.h"
#include "Camera.h"
#include "UploadBuffer.h"
#include "ModelObject.h"
#include "Player.h"
#include "Texture.h"
#include "Physx.h"

//게임 객체의 정보를 셰이더에게 넘겨주기 위한 구조체(상수 버퍼)이다.
struct CB_GAMEOBJECT_INFO
{
	XMFLOAT4X4	m_xmf4x4World;
	UINT		m_nMaterial = 0;
};

//인스턴스 정보(게임 객체의 월드 변환 행렬과 객체의 색상)를 위한 구조체이다.
struct VS_VB_INSTANCE
{
	XMFLOAT4X4 m_xmf4x4Transform;
	XMFLOAT4 m_xmcColor;
};

struct CB_SHADOW_INFO
{
	XMFLOAT4X4	m_xmf4x4World;
	UINT		m_nMaterial = 0;
	XMFLOAT4X4 m_xmf4x4ShadowTransform;
};

D3D12_SHADER_RESOURCE_VIEW_DESC GetShaderResourceViewDesc(D3D12_RESOURCE_DESC d3dResourceDesc, UINT nTextureType);

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

//“CObjectsShader” 클래스는 게임 객체들을 포함하는 셰이더 객체이다.
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
	CMaterial							*m_pMaterial = NULL;
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
	//CMaterial										*m_pMaterial = NULL;

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

	bool map_3_macaron_flag = false;

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
	virtual void BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, CPhysx* physx, int map_type, int nRenderTargets = 1, void * pContext = NULL);
	virtual void BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int map_type, int nRenderTargets = 1, void * pContext = NULL);
	virtual void Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera);
	//virtual void RenderToDepthBuffer(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera, XMFLOAT3& cameraPos, float offset);
	virtual void Animate(float fTimeElapsed);
	virtual void Animate(float fTimeElapsed, int flag_up);
	virtual UINT ModelIndex() const { return modelIndex; }

	virtual void setPosition(XMFLOAT3* pos, UINT num) {}
	virtual UINT getRemainObjects() { return 0; }
	virtual CGameObject** getObjects(UINT& num) { return nullptr; }
	//virtual CGameObject** getObjects() { return m_bbObjects.data(); }

	void setScale(float scale);
};

class StairShader : public CModelShader
{
public:
	StairShader() {};
	StairShader(LoadModel *ma) { static_model = ma; };
	~StairShader() {};

	virtual void BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, CPhysx* physx, int type, int nRenderTargets = 1, void * pContext = NULL);
};

class BridgeShader : public CModelShader
{
public:
	BridgeShader() {};
	BridgeShader(LoadModel *ma) { static_model = ma; };
	~BridgeShader() {};

	virtual void BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, CPhysx* physx, int nRenderTargets = 1, void * pContext = NULL);
};

class MeshShader : public CModelShader
{
public:
	MeshShader();
	~MeshShader();

	virtual D3D12_INPUT_LAYOUT_DESC		CreateInputLayout(int index = 0);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);

	virtual D3D12_RASTERIZER_DESC CreateRasterizerState(int index);

	virtual void CreateGraphicsRootSignature(ID3D12Device * pd3dDevice);
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);//, XMFLOAT4X4 *pxmf4x4World);
	virtual void BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int type, int nRenderTargets = 1, void * pContext = NULL);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	virtual void Animate(float fTimeElapsed);
	virtual void setScale(float scale);
	virtual CGameObject* getObjects() { return m_ppObjects[0]; }
};

class MagicShader : public MeshShader
{
public:
	MagicShader() {};
	~MagicShader() {};

	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_BLEND_DESC CreateBlendState(int index);
	virtual void BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nRenderTargets = 1, void * pContext = NULL);
	virtual void Animate(float fTimeElapsed);
	virtual CGameObject* getObjects(int i) { return m_ppObjects[i]; }
};

class EffectShader : public MeshShader
{
public:
	EffectShader();
	~EffectShader();
	virtual D3D12_BLEND_DESC CreateBlendState(int index);
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState(int index);
	virtual void CreateGraphicsRootSignature(ID3D12Device * pd3dDevice);
	virtual void BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int type, int nRenderTargets = 1, void * pContext = NULL);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);//, XMFLOAT4X4 *pxmf4x4World);
	virtual void Animate(float fTimeElapsed);
	virtual void ShowEffect(int index) {
		m_ppObjects[index]->visible = true;
		SoundManager::GetInstance()->PlaySounds(LIGHTNING);
	}
	virtual CGameObject* getObject(UINT index) { return m_ppObjects[index]; }
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);


	int spritenum;
};
class SkillParticleShader : public EffectShader {
public:
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual void BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int type, int nRenderTargets = 1, void * pContext = NULL);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);//, XMFLOAT4X4 *pxmf4x4World);
	virtual void Animate(float fTimeElapsed);
	virtual void ShowParticle(bool show, XMFLOAT3 pos = XMFLOAT3(0, 0, 0));
	virtual void Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera);

	float ftime = 0.f;
	bool visible = false;
	XMFLOAT3 startpos;
};
class PlayerShader;
class TeamShader : public EffectShader {
public:
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual void BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int type, int nRenderTargets = 1, void * pContext = NULL);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);//, XMFLOAT4X4 *pxmf4x4World);
	virtual void Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera, PlayerShader **ps);
	virtual void Animate(float fTimeElapsed);
};
class SkillEffectShader : public EffectShader
{
public:
	SkillEffectShader() {};
	~SkillEffectShader() {};
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual void BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nRenderTargets = 1, void * pContext = NULL);
	virtual void Animate(float fTimeElapsed, XMFLOAT3 pos);
};

class WaveShader : public MeshShader
{
public:
	float wave_sin = 0.f;

	WaveShader();
	~WaveShader();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);

	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList * pd3dCommandList);
	virtual D3D12_BLEND_DESC CreateBlendState(int index);
	virtual void BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int map_type, int nRenderTargets = 1, void * pContext = NULL);
	virtual void Animate(float fTimeElapsed, int map_type);
};

class WeaponShader : public CModelShader
{
protected:
	LoadModel	*weapon_model;
	bool		cupcake_up_flag = false;

public:
	WeaponShader();
	WeaponShader(LoadModel *ma);
	~WeaponShader();

	virtual D3D12_RASTERIZER_DESC CreateRasterizerState(int index);
	//	virtual D3D12_BLEND_DESC CreateBlendState(int index);

	virtual void Animate(float fTimeElapsed, int flag_up, int weapon_num);
	virtual void BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int weapon_num, int mpa_type, int nRenderTargets = 1, void * pContext = NULL);
	virtual void Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera);
	virtual ModelObject* getObject(UINT index) { return m_bbObjects[index]; }
	bool get_cupcake_up_flag() { return cupcake_up_flag; }
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
	//인스턴스 데이터를 포함하는 버퍼와 포인터이다.
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
	XMMATRIX shadow_mat;

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
public:
	CCamera * m_Camera;
public:
	PlayerShader(Model_Animation *ma);
	~PlayerShader();

	virtual void CreateGraphicsRootSignature(ID3D12Device * pd3dDevice);
	virtual void BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nRenderTargets = 1, void * pContext = NULL);
	virtual void Animate(float fTimeElapsed);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList * pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera);

	virtual  CGameObject* getPlayer() { return m_bbObjects[0]; }
	bool render = true;
};

class testBox : public MeshShader
{
public:
	BoundingOrientedBox bounding;
	bool pick = false;
	testBox();
	~testBox();

	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState(int index);
	virtual void BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ModelObject *mo, int type, int nRenderTargets = 1, void * pContext = NULL);
	//virtual void BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ModelObject *mo, int nRenderTargets = 1, void * pContext = NULL);
	virtual void SetPosition(XMFLOAT3& pos);
	virtual void Rotate(float x, float y, float z);
	virtual CGameObject* getObjects() { return m_ppObjects[0]; }
	virtual void Animate(float fTimeElapsed);
};


class ShadowDebugShader : public CModelShader
{
public:
	unique_ptr<UploadBuffer<CB_SHADOW_INFO>>		m_ShadowCB = nullptr;
	XMMATRIX		shadow_mat;

public:
	ShadowDebugShader();
	ShadowDebugShader(LoadModel *ma);
	//ShadowDebugShader(Model_Animation *ma);
	~ShadowDebugShader();

	virtual D3D12_BLEND_DESC CreateBlendState(int index);
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState(int index);
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
	virtual void CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int type, int nRenderTargets = 1, void *pContext = NULL);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList * pd3dCommandList);
};

class ShadowReverseModelShader : public CModelShader
{
public:
	ShadowReverseModelShader() {};
	ShadowReverseModelShader(LoadModel *ma);
	~ShadowReverseModelShader() {};

	virtual D3D12_BLEND_DESC CreateBlendState(int index);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState(int index);
	virtual void BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int type, int nRenderTargets = 1, void * pContext = NULL);
};

class ShadowREverseShader : public MeshShader
{
public:
	ShadowREverseShader() {};
	~ShadowREverseShader() {};

	virtual D3D12_BLEND_DESC CreateBlendState(int index);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState(int index);
	virtual void BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int type, int nRenderTargets = 1, void * pContext = NULL);
};

class PlayerShadowShader : public PlayerShader
{
public:
	PlayerShadowShader(Model_Animation *ma);
	~PlayerShadowShader();

	virtual void Animate(float fTimeElapsed, XMFLOAT3 pos);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList * pd3dCommandList);
	virtual D3D12_BLEND_DESC CreateBlendState(int index);
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState(int index);
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nRenderTargets = 1, void *pContext = NULL);
	virtual void ChangeAnimation(int m_animindex);
	virtual int getAnimIndex();
};

class ExplosionShader : public MeshShader
{
public:
	XMFLOAT3					m_pxmf3SphereVectors[EXPLOSION_DEBRISES];

	bool						m_bBlowingUp = false;

	float						m_fElapsedTimes = 0.0f;
	float						m_fDuration = 3.0f;					// 폭발시간
	float						m_fExplosionSpeed = 15.0f;
	float						m_fExplosionRotation = 720.0f;

public:
	ExplosionShader() {};
	~ExplosionShader() {};

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);

	XMVECTOR RandomUnitVectorOnSphere();

	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nRenderTargets = 1, void * pContext = NULL);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
	virtual void Animate(float fTimeElapsed, XMFLOAT3 pos);
	virtual CGameObject* getObject(UINT index) { return m_ppObjects[index]; }
};

class ExplosionModelShader : public CModelShader
{
public:
	XMFLOAT3					m_pxmf3SphereVectors[EXPLOSION_DEBRISES];
	XMFLOAT3					xmf3Position = XMFLOAT3(1000, 1000, 1000);

	bool						m_bBlowingUp = false;

	float						m_fElapsedTimes = 0.0f;
	float						m_fDuration = 3.0f;					// 폭발시간
	float						m_fExplosionSpeed = 14.0f;
	float						m_fExplosionRotation = 720.0f;

public:
	ExplosionModelShader() {};
	ExplosionModelShader(LoadModel *ma);
	~ExplosionModelShader() {};

	XMVECTOR RandomUnitVectorOnSphere();

	virtual void Setposition(float x, float y, float z) { xmf3Position = XMFLOAT3(x, y, z); };
	virtual void BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int type, int nRenderTargets = 1, void * pContext = NULL);
	virtual void Animate(float fTimeElapsed);
	virtual ModelObject* getObject(UINT index) { return m_bbObjects[index]; }
};

class StunShader : public CModelShader
{
public:
	float rotate_time = 0.f;
	float rotate_time_2 = 0.f;
	bool visible = false;
	XMFLOAT3 rotate_y_check = XMFLOAT3(0.f, 1.f, 0.f);

	StunShader() {};
	StunShader(LoadModel *ma) { static_model = ma; };
	~StunShader() {};

	virtual void BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nRenderTargets = 1, void * pContext = NULL);
	virtual void Animate(float fTimeElapsed, XMFLOAT3 pos);
};