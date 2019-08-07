#pragma once
#include "Shader.h"
#include "UIObject.h"
#include "Texture.h"
#define NUM_MAX_UITEXTURE 5
class UIShader : public CShader
{
public:
	UIShader();
	~UIShader();

public:
	virtual D3D12_BLEND_DESC			CreateBlendState(int index = 0);
	virtual D3D12_DEPTH_STENCIL_DESC	CreateDepthStencilState(int index = 0);

	virtual void Animate(float fTimeElapsed);
	virtual void UpdateState(int ui_state);
	virtual void RefreshTimer(float fTimeElapsed, UINT index = 0);
	virtual void SetPos(XMFLOAT2* pos, UINT index = 0) { m_pUIObjects[index]->m_xmf2ScreenPos = *pos; }
	virtual void SetNowSprite(XMUINT2& nowSprite, UINT index = 0);	// 현재 스프라이트 수정
	virtual void SetPosScreenRatio(XMFLOAT2& ratio, UINT index = 0);	// 위치를 스크린 비율로 조정
	virtual void SetPosScreenRatio(XMFLOAT2& ratio);	// 위치를 스크린 비율로 조정
	virtual void SetAlpha(float alpha, UINT index = 0);			// ALpha값 수정
	virtual void SetEnable(bool enable, UINT index = 0) { m_pUIObjects[index]->m_bEnabled = enable; }	// 시야에 보일지 확인
	virtual void SetScale(XMFLOAT2* scale, UINT index = 0) { m_pUIObjects[index]->m_xmf2Scale = *scale; }
	virtual void SetScale(XMFLOAT2* scale);	// 이 셰이더의 모든 오브젝트 Scale변경
	virtual UIObject* getObejct(UINT index) { return m_pUIObjects[index]; }
	virtual void SetNumObject(UINT nObject) { m_nObjects = nObject; }
	virtual void SetType(UINT nType, UINT index = 0) { m_pUIObjects[index]->m_nTexType = nType; }
	virtual void MovePos(XMFLOAT2& pos, UINT index = 0);	// 현재 위치 기준으로 pos 픽셀만큼 이동 
	virtual void CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	virtual void CreateCollisionBox();	// 버튼 충돌 박스생성
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void CreateCbvAndSrvDescriptorHeaps(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nConstantBufferViews, int nShaderResourceViews, bool bIsGraphics = true);
	virtual void CreateConstantBufferViews(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, int nConstantBufferViews, ID3D12Resource * pd3dConstantBuffers, UINT nStride);
	virtual void CreateShaderResourceViews(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, CTexture * pTexture, UINT nRootParameterStartIndex, bool bAutoIncrement, bool bIsGraphics = true);
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(int index);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(int index);

	virtual void BuildPSO(ID3D12Device * pd3dDevice, UINT nRenderTargets, int index = 0);
	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nRenderTargets = 1, void *pContext = NULL);
	virtual void ReleaseObjects();
	virtual void ReleaseShaderVariables();
	virtual void OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList, int index = 0);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void SetTimer(float apearTime, float existTime, float retreatTime);							// 등장 시간 Set
	virtual XMUINT2 GetSpriteSize(int texIndex, CTexture* pTexture, XMUINT2 numSprite = XMUINT2(1, 1));
	virtual void SetTime(int t) {}
	virtual void CreatePipelineParts();
	virtual void ShowMessage(bool win) {};
	virtual void SetFog() {}		//fog Shader전용
	virtual void FogOff() {}		//fog Shader전용
	virtual bool Get_Fog_Off_Flag() { return false; } //fog Shader전용
	virtual bool Get_Fog_Flag() { return false; }
	virtual void FogOn(bool flag) {};	//IDshader hpbarshader 전용

	virtual void SetID(wchar_t *str) {};

protected:
	unique_ptr<UploadBuffer<CB_UI_INFO>>	m_ObjectCB = nullptr;
	std::vector<UIObject*>					m_pUIObjects;
	UINT									m_nObjects = 0;
	CMaterial								*m_pMaterial = NULL;
	bool									m_bTimeUI;
	float									m_ApearTime;	// 등장하는 시간
	float									m_RetreatTime;	// 사라지는 시간
	float									m_ExistTime;	// 존재하는 시간
	float									m_ElapsedTime;

	ComPtr<ID3DBlob>*						m_CSByteCode = nullptr;

	ComPtr<ID3D12DescriptorHeap>			m_CbvSrvDescriptorHeap = nullptr;

	ComPtr<ID3D12PipelineState>*			m_pPSO = nullptr;
	ComPtr<ID3D12PipelineState>*			m_pComputePSO = nullptr;

	ComPtr<ID3D12RootSignature>*			m_RootSignature = nullptr;
	ComPtr<ID3D12RootSignature>*			m_ComputeRootSignature = nullptr;

	ComPtr<ID3D12DescriptorHeap>			m_ComputeCbvSrvDescriptorHeap = nullptr;

	vector<D3D12_CPU_DESCRIPTOR_HANDLE>		m_pComputeSRVUAVCPUHandles;
	vector<D3D12_GPU_DESCRIPTOR_HANDLE>		m_pComputeSRVUAVGPUHandles;

	vector<ComPtr<ID3D12Resource>>			m_pComputeUAVBuffers;
	vector<ComPtr<ID3D12Resource>>			m_pComputeOutputBuffers;

	D3D12_CPU_DESCRIPTOR_HANDLE				m_d3dCbvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE				m_d3dCbvGPUDescriptorStartHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE				m_d3dSrvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE				m_d3dSrvGPUDescriptorStartHandle;

	D3D12_CPU_DESCRIPTOR_HANDLE				m_d3dComputeCbvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE				m_d3dComputeCbvGPUDescriptorStartHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE				m_d3dComputeSrvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE				m_d3dComputeSrvGPUDescriptorStartHandle;

	DXGI_FORMAT								m_Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	UINT									m_nPSO = 1;

	UINT									m_nComputePSO = 0;
	UINT									m_nComputeBuffers = 0;

	ComPtr<ID3DBlob>*						m_VSByteCode = nullptr;
	ComPtr<ID3DBlob>*						m_PSByteCode = nullptr;
	//ComPtr<ID3DBlob>*						m_CSByteCode = nullptr;
};
class FogShader : public UIShader {
public:
	FogShader() {};
	~FogShader() {};
	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nRenderTargets = 1, void *pContext = NULL);
	virtual void SetFog();
	virtual void FogOff();
	virtual void Animate(float fTimeElapsed);
	virtual bool Get_Fog_Off_Flag() { return Fog_Off_Flag; }
	virtual bool Get_Fog_Flag() { return Fog_Flag; }
	bool Fog_Flag = false;
	bool Fog_Off_Flag = false;
	float ftime;
};

class UIHPBarShader : public UIShader
{
public:
	UIHPBarShader() { };
	~UIHPBarShader() { };

public:
	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nRenderTargets = 1, void *pContext = NULL);
	virtual void Animate(float fTimeElapsed);
	virtual void FogOn(bool flag);
	bool Fog_Flag = false;
	float ftime;
	float ypos = 0.f;

};
class UIIDShader : public UIShader
{
public:

	UIIDShader() {};
	~UIIDShader() {};

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nRenderTargets = 1, void *pContext = NULL);
	virtual void SetID(wchar_t *str);
	virtual void Animate(float fTimeElapsed);
	virtual void FogOn(bool flag);
	bool Fog_Flag = false;
	float ftime;
	float ypos = 0.f;
};
class UITimeShader : public UIShader
{
public:
	UITimeShader() { };
	~UITimeShader() { };

public:
	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nRenderTargets = 1, void *pContext = NULL);
	virtual void SetTime(int t);	//초(sec) 단위
	virtual void Animate(float fTimeElapsed);
	float Time=300;
};

class UIDotShader : public UIShader
{
public:
	UIDotShader() { };
	~UIDotShader() { };

public:
	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nRenderTargets = 1, void *pContext = NULL);
};

class UIReadyShader : public UIShader
{
public:
	UIReadyShader() { };
	~UIReadyShader() { };

public:
	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nRenderTargets = 1, void *pContext = NULL);
};

class UIFightShader : public UIShader
{
public:
	UIFightShader() { };
	~UIFightShader() { };

public:
	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nRenderTargets = 1, void *pContext = NULL);
};
class UILoadingShader : public UIShader
{
public :
	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nRenderTargets = 1, void *pContext = NULL);
};
class MessageShader : public UIShader
{
	float ShowTime = 0.f;
public:
	MessageShader() { };
	~MessageShader() { };

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nRenderTargets = 1, void *pContext = NULL);
	virtual void Animate(float fTimeElapsed);
	virtual void ShowMessage(int message_type);
};
class WinLoseShader : public UIShader
{
public:
	WinLoseShader() {};
	~WinLoseShader() {};
	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nRenderTargets = 1, void *pContext = NULL);
	virtual void ShowMessage(bool win);//win = true, lose = false
};
class DarkShader : public UIShader
{
public:
	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nRenderTargets = 1, void *pContext = NULL);
	bool is_dark = false;
	float ftime = 0;
	virtual void Animate(float fTimeElapsed);
};