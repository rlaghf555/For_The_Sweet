#include "stdafx.h"
#include "Texture.h"
#include "Shader.h"
CTexture::CTexture(int nTextures, UINT nTextureType, int nSamplers)
{
	m_nTextures = nTextures;
	m_nTexType = nTextureType;
	m_nSamplers = nSamplers;

	if (m_nTextures > 0)
	{
		m_pRootArgumentInfos = vector<SRVROOTARGUMENTINFO>();
		m_ppd3dTextureUploadBuffers = vector<ComPtr<ID3D12Resource>>(m_nTextures);
		m_ppd3dTextures = vector<ComPtr<ID3D12Resource>>(m_nTextures);
		m_pTextureType = vector<UINT>(m_nTextures);
		for (int i = 0; i < m_nTextures; i++) {
			m_ppd3dTextureUploadBuffers[i] = m_ppd3dTextures[i] = nullptr;
			m_pTextureType[i] = m_nTexType;
		}
	}

	if (m_nSamplers > 0)
		m_pd3dSamplerGpuDescriptorHandles = new D3D12_GPU_DESCRIPTOR_HANDLE[m_nSamplers];
}

CTexture::~CTexture()
{
	if (!m_ppd3dTextures.empty())
	{
		m_ppd3dTextures.clear();
	}

	if (m_pd3dSamplerGpuDescriptorHandles) delete[] m_pd3dSamplerGpuDescriptorHandles;
}

void CTexture::SetRootArgument(int nIndex, UINT nRootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGpuDescriptorHandle)
{
	SRVROOTARGUMENTINFO info;
	info.m_nRootParameterIndex = nRootParameterIndex;
	info.m_d3dSrvGpuDescriptorHandle = d3dSrvGpuDescriptorHandle;
	m_pRootArgumentInfos.push_back(info);
}

void CTexture::SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle)
{
	m_pd3dSamplerGpuDescriptorHandles[nIndex] = d3dSamplerGpuDescriptorHandle;
}

void CTexture::AddTexture(ID3D12Resource * texture, ID3D12Resource * uploadbuffer, UINT textureType)
{
	m_nTextures++;
	m_ppd3dTextures.push_back(texture);
	m_ppd3dTextureUploadBuffers.push_back(uploadbuffer);
	m_pTextureType.push_back(textureType);
}

void CTexture::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	if (m_pTextureType[0] == RESOURCE_TEXTURE2D_ARRAY)
	{
		pd3dCommandList->SetGraphicsRootDescriptorTable(m_pRootArgumentInfos[0].m_nRootParameterIndex, m_pRootArgumentInfos[0].m_d3dSrvGpuDescriptorHandle);
	}
	else
	{
		for (int i = 0; i < m_nTextures; i++)
		{
			pd3dCommandList->SetGraphicsRootDescriptorTable(m_pRootArgumentInfos[i].m_nRootParameterIndex, m_pRootArgumentInfos[i].m_d3dSrvGpuDescriptorHandle);
		}
	}
}

void CTexture::UpdateComputeShaderVariables(ID3D12GraphicsCommandList * pd3dCommandList)
{
	if (m_pTextureType[0] == RESOURCE_TEXTURE2D_ARRAY)
	{
		pd3dCommandList->SetComputeRootDescriptorTable(m_pRootArgumentInfos[0].m_nRootParameterIndex, m_pRootArgumentInfos[0].m_d3dSrvGpuDescriptorHandle);
	}
	else
	{
		for (int i = 0; i < m_nTextures; i++)
		{
			pd3dCommandList->SetComputeRootDescriptorTable(m_pRootArgumentInfos[i].m_nRootParameterIndex, m_pRootArgumentInfos[i].m_d3dSrvGpuDescriptorHandle);
		}
	}
}

void CTexture::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, int nIndex)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(m_pRootArgumentInfos[nIndex].m_nRootParameterIndex, m_pRootArgumentInfos[nIndex].m_d3dSrvGpuDescriptorHandle);
}

void CTexture::ReleaseUploadBuffers()
{
	if (!m_ppd3dTextureUploadBuffers.empty())
	{
		m_ppd3dTextureUploadBuffers.clear();
	}
}

void CTexture::ReleaseShaderVariables()
{
}

void CTexture::LoadTextureFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, const wchar_t *pszFileName, UINT nIndex)
{
	m_ppd3dTextures[nIndex] = D3DUtil::CreateTextureResourceFromFile(pd3dDevice, pd3dCommandList, pszFileName, m_ppd3dTextureUploadBuffers[nIndex].Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	cout << "Load Texture\n";
}

ComPtr<ID3D12Resource> CTexture::CreateTexture(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, UINT nWidth, UINT nHeight, DXGI_FORMAT dxgiFormat, D3D12_RESOURCE_FLAGS d3dResourceFlags, D3D12_RESOURCE_STATES d3dResourceStates, D3D12_CLEAR_VALUE *pd3dClearValue, UINT nIndex)
{
	m_ppd3dTextures[nIndex] = D3DUtil::CreateTexture2DResource(pd3dDevice, pd3dCommandList, nWidth, nHeight, dxgiFormat, d3dResourceFlags, d3dResourceStates, pd3dClearValue);
	if (dxgiFormat == DXGI_FORMAT_R24G8_TYPELESS) m_pTextureType[nIndex] = RESOURCE_TEXTURE2D_SHADOWMAP;

	return(m_ppd3dTextures[nIndex]);
}

CMaterial::CMaterial()
{
	m_pShader = nullptr;
}

CMaterial::~CMaterial()
{
	if (m_pShader) m_pShader->Release();
}

void CMaterial::SetTexture(CTexture *pTexture)
{
	if (m_pTexture)
		m_pTexture.release();

	m_pTexture = make_unique<CTexture>(std::move(*pTexture));
}

void CMaterial::SetShader(CModelShader *pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
}

void CMaterial::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	if (m_pTexture) m_pTexture->UpdateShaderVariables(pd3dCommandList);
}

void CMaterial::ReleaseShaderVariables()
{
	if (m_pShader) m_pShader->ReleaseShaderVariables();
	if (m_pTexture) m_pTexture->ReleaseShaderVariables();
}

void CMaterial::ReleaseUploadBuffers()
{
	if (m_pTexture) m_pTexture->ReleaseUploadBuffers();
}