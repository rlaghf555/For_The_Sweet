#pragma once
#include "stdafx.h"
//#include "Shader.h"

class CModelShader;

#define RESOURCE_TEXTURE2D						0x01
#define RESOURCE_TEXTURE2D_ARRAY			0x02	//[]
#define RESOURCE_TEXTURE2DARRAY			0x03
#define RESOURCE_TEXTURE_CUBE				0x04
#define RESOURCE_BUFFER							0x05
#define RESOURCE_TEXTURE2D_SHADOWMAP	0x06
#define RESOURCE_TEXTURE2D_HDR	0x07
#define RESOURCE_BUFFER_FLOAT32			0x08

typedef struct SRVROOTARGUMENTINFO
{
	UINT							m_nRootParameterIndex = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dSrvGpuDescriptorHandle;
} SRVROOTARGUMENTINFO, UAVROOTARGUMENTINFO;

class CTexture
{
public:
	CTexture(int nTextureResources = 1, UINT nResourceType = RESOURCE_TEXTURE2D, int nSamplers = 0);
	virtual ~CTexture();

private:
	int								m_nReferences = 0;
	int								m_nSamplers = 0;
	int								m_nTextures = 0;
	UINT							m_nTexType = 0;

	vector<ComPtr<ID3D12Resource>>	m_ppd3dTextures;
	vector<ComPtr<ID3D12Resource>>	m_ppd3dTextureUploadBuffers;
	vector<SRVROOTARGUMENTINFO>		m_pRootArgumentInfos;
	vector<UINT>					m_pTextureType;
	D3D12_GPU_DESCRIPTOR_HANDLE		*m_pd3dSamplerGpuDescriptorHandles = NULL;

public:
	void SetRootArgument(int nIndex, UINT nRootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dsrvGpuDescriptorHandle);
	void SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle);

	void AddTexture(ID3D12Resource* texture, ID3D12Resource* uploadbuffer, UINT textureType = RESOURCE_TEXTURE2D);

	void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	void UpdateComputeShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, int nIndex);
	void ReleaseShaderVariables();
	D3D12_GPU_DESCRIPTOR_HANDLE GetDescriptorHandle(int index) { return m_pRootArgumentInfos[index].m_d3dSrvGpuDescriptorHandle; }

	void LoadTextureFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, const wchar_t *pszFileName, UINT nIndex);
	ComPtr<ID3D12Resource> CreateTexture(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, UINT nWidth, UINT nHeight, DXGI_FORMAT dxgiFormat, D3D12_RESOURCE_FLAGS d3dResourceFlags, D3D12_RESOURCE_STATES d3dResourceStates, D3D12_CLEAR_VALUE *pd3dClearValue, UINT nIndex);

	int GetTextureCount() { return(m_nTextures); }

	ComPtr<ID3D12Resource> GetTexture(int nIndex) { return(m_ppd3dTextures[nIndex]); }
	ComPtr<ID3D12Resource> GetUploadBuffer(int nIndex) { return(m_ppd3dTextureUploadBuffers[nIndex]); };


	UINT GetTextureType(int index) { return(m_pTextureType[index]); }

	void ReleaseUploadBuffers();
};

class CMaterial
{
public:
	CMaterial();
	virtual ~CMaterial();

private:
	int								m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	XMFLOAT4	m_xmf4Albedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	UINT					m_nReflection = 0;
	unique_ptr<CTexture>	m_pTexture = nullptr;
	CModelShader*				m_pShader = nullptr;

	void SetAlbedo(XMFLOAT4 xmf4Albedo) { m_xmf4Albedo = xmf4Albedo; }
	void SetReflection(UINT nReflection) { m_nReflection = nReflection; }
	void SetTexture(CTexture *pTexture);
	void SetShader(CModelShader *pShader);

	void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	void ReleaseShaderVariables();

	void ReleaseUploadBuffers();
};
