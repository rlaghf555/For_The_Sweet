#pragma once

#include <Windows.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <array>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "d3dx12.h"
//#include "DDSTextureLoader.h"
#include "DDSTextureLoader12.h"
#include "D3DMath.h"


enum SRVRegisterNumber { 
	SRVTexArray						= 0,
	SRVInstanceData					= 1, 
	SRVTexArrayNorm					= 2, 
	SRVAnimation					= 3,
	SRVTexture2DNorm				= 4,
	SRVInstanceEffectData			= 5,
	SRVInstanceAnimationInfo		= 6,
	SRVShadowMap					= 7,
	SRVUITextureMap					= 9,
	SRVMultiTexture					= 13,
	SRVFullScreenTexture			= 17,
	SRVFullScreenOutLineNormTexture	= 18,
	SRVFullScreenNormalTexture		= 19,
	SRVFullScreenDepthTexture		= 20,
	SRVFullScreenHDR				= 22,
	SRVAverageValues1D				= 24,
	SRVAverageValues				= 25,
	SRVHDRDownScale					= 26,
	SRVBloomInput					= 27,
	SRVHPBarData					= 28
};

enum UAVRegisterNumber {
	UAVAverageLum		= 1,
	UAVHDRDownScale		= 2,
	UAVBloom			= 3,
	UAVBloomOutput		= 4
};

enum CBVRegisterNumber {
	CBVPerObject		= 0,
	CBVCameraInfo		= 1,
	CBVObjectInfo		= 2,
	CBVAnimationInfo	= 3,
	CBVMaterial			= 4,
	CBVLights			= 5,
	CBVEffect			= 6,
	CBVFade				= 7,
	CBVFog				= 8,
	CBVBlurInfo			= 10,
	CBVHDRDownScale		= 11,
	CBVHDRToneMapp		= 12,
	CBVMonsterHP		= 13,
};


enum UICBVRegisterNumber {
	CBVUIInfo = 0,
	CBVNumberUI = 1
};

enum UABufferType {
	UAFloatBuffer = 0,
	UATexBuffer = 1
};

enum RTVType {
	RTV_COLOR, // 색상 추출
	RTV_OUTLINENRM, // 외곽선을 위한 노말 벡터 추출
	RTV_NRM, // 노말 매핑 후의 노말 벡터 추출
	RTV_HDR
};

extern UINT	gnCbvSrvDescriptorIncrementSize;
extern const int gNumFrameResources;

#define PSO_OBJECT					0
#define PSO_SHADOWMAP				1

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace std;

inline void d3dSetDebugName(IDXGIObject* obj, const char* name)
{
	if (obj)
		obj->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(name), name);
}

inline void d3dSetDebugName(ID3D12Device* obj, const char* name)
{
	if (obj)
		obj->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(name), name);
}

inline void d3dSetDebugName(ID3D12DeviceChild* obj, const char* name)
{
	if (obj)
		obj->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(name), name);
}

inline std::wstring AnsiToWString(const std::string& str)
{
	WCHAR buffer[512];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
	return std::wstring(buffer);
}

/*
#if defined(_DEBUG)
	#ifndef Assert
	#define Assert(x, description)                                  \
	{                                                               \
		static bool ignoreAssert = false;                           \
		if(!ignoreAssert && !(x))                                   \
		{                                                           \
			Debug::AssertResult result = Debug::ShowAssertDialog(   \
			(L#x), description, AnsiToWString(__FILE__), __LINE__); \
		if(result == Debug::AssertIgnore)                           \
		{                                                           \
			ignoreAssert = true;                                    \
		}                                                           \
					else if(result == Debug::AssertBreak)           \
		{                                                           \
			__debugbreak();                                         \
		}                                                           \
		}                                                           \
	}
	#endif
#else
	#ifndef Assert
	#define Assert(x, description)
	#endif
#endif
	*/


class D3DUtil
{
public:

	static bool IsKeyDown(int vkeyCode);

	static UINT CalcConstantBufferByteSize(UINT byteSize) {	return (byteSize + 255) & ~255; }
	
	static ComPtr<ID3DBlob> LoadBinary(const wstring& filename);
	
	static ComPtr<ID3D12Resource> CreateDefaultBuffer(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* cmdList,
		const void* initData,
		UINT64 byteSize,
		ComPtr<ID3D12Resource>& uploadBuffer);
	
	static ComPtr<ID3DBlob> CompileShader(
		const wstring& filename,
		const D3D_SHADER_MACRO* defines,
		const string& entrypoint,
		const string& target);

	static ComPtr<ID3D12Resource> CreateTexture2DResource(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,
		UINT nWidth, UINT nHeight, DXGI_FORMAT dxgiFormat, D3D12_RESOURCE_FLAGS d3dResourceFlags,
		D3D12_RESOURCE_STATES d3dResourceStates, D3D12_CLEAR_VALUE *pd3dClearValue);

	//static void CreateUnorderedAccessResource(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, XMUINT2& size, ComPtr<ID3D12Resource>& buffer, ComPtr<ID3D12Resource>& readbackBuffer, UINT type);

	static ComPtr<ID3D12Resource> CreateTextureResourceFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, const wchar_t *pszFileName,ID3D12Resource* ppd3dUploadBuffer, D3D12_RESOURCE_STATES d3dResourceStates);

	//static void CalculateTangentArray(long vertexCount, const XMFLOAT3 *vertex, XMFLOAT3 *normal, const XMFLOAT2 *texcoord, long triangleCount, const UINT *indeies, XMFLOAT3 *tangent);
};


class DxException
{
public:
	DxException() = default;
	DxException(HRESULT hr, const wstring& functionName, const wstring& filename, int lineNumber);

public:
	wstring ToString() const;

	HRESULT ErrorCode = S_OK;
	wstring FunctionName;
	wstring Filename;

	int LineNumber = -1;
};

struct SubmeshGeometry
{
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	INT BaseVertexLocation = 0;
};

struct MeshGeometry 
{
	string Name;

	ComPtr<ID3DBlob>		VertexBufferCPU = nullptr;
	ComPtr<ID3DBlob>		IndexBufferCPU = nullptr;

	ComPtr<ID3D12Resource>	VertexBufferGPU = nullptr;
	ComPtr<ID3D12Resource>	IndexBufferGPU = nullptr;

	ComPtr<ID3D12Resource>	VertexBufferUploader = nullptr;
	ComPtr<ID3D12Resource>	IndexBufferUploader = nullptr;

	UINT VertexByteStride = 0;
	UINT VertexBufferByteSize = 0;
	DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
	UINT IndexBufferByteSize = 0;

	unordered_map<string, SubmeshGeometry> DrawArgs;

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation	= VertexBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes	= VertexByteStride;
		vbv.SizeInBytes		= VertexBufferByteSize;
		
		return vbv;
	}

	D3D12_INDEX_BUFFER_VIEW IndexBufferView() const
	{
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
		ibv.Format = IndexFormat;
		ibv.SizeInBytes = IndexBufferByteSize;

		return ibv;
	}

	void DisposeUploaders()
	{
		VertexBufferUploader = nullptr;
		IndexBufferUploader = nullptr;
	}

};

struct Light
{
	XMFLOAT3 Strength = { 0.5f, 0.5f, 0.5f };
	float FalloffStart = 1.0f;					// point/spot light only
	XMFLOAT3 Direction = { 0.0f, -1.0f, 0.0f };	// directional/spot light only
	float FalloffEnd = 10.0f;					// point/spot light only
	XMFLOAT3 Position = { 0.0f,0.0f,0.0f };		// point/spot light only
	float SpotPower = 64.0f;					// spot light only
};

#define MaxLights 16

struct MaterialConstants
{
	XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
	float Roughness = 0.25f;

	XMFLOAT4X4 MatTransform = D3DMath::Identity4x4();	// Used in texture mapping.
};


struct Material
{
	string Name;										// Unique material name for lookup.

	int MatCBIndex = -1;								// Index into constant buffer corresponding to this material.
	int DiffuseSrvHeapIndex = -1;						// Index into SRV heap for diffuse texture.
	int NormalSrvHeapIndex = -1;						// Index into SRV heap for normal texture.
	int NumFramesDirty = gNumFrameResources;			// NumFramesDirty = gNumFrameResources so that each frame resource gets the update.

	XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };// Material constant buffer data used for shading.
	XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
	float Roughness = 0.25f;

	XMFLOAT4X4 MatTransform = D3DMath::Identity4x4();
};

struct Texture
{
	string Name;
	wstring Filename;

	ComPtr<ID3D12Resource> Resource = nullptr;
	ComPtr<ID3D12Resource> UploadHeap = nullptr;
};

/*
#define MAX_LIGHTS	  8
#define MAX_MATERIALS 8

#define PSO_OBJECT					0
#define PSO_SHADOWMAP				1

#define POINT_LIGHT				1	// 주변광
#define SPOT_LIGHT				2
#define DIRECTIONAL_LIGHT		3

struct MATERIAL
{
	XMFLOAT4				m_xmf4Ambient;	// 앰비언트 반사 색상 
	XMFLOAT4				m_xmf4Diffuse;
	XMFLOAT4				m_xmf4Specular; //(r,g,b,a=power)
	XMFLOAT4				m_xmf4Emissive;
};

struct MATERIALS
{
	MATERIAL				m_pReflections[MAX_MATERIALS];
};

struct LIGHT
{
	XMFLOAT4				m_xmf4Ambient;
	XMFLOAT4				m_xmf4Diffuse;
	XMFLOAT4				m_xmf4Specular;
	XMFLOAT3				m_xmf3Position;	// 광원의 위치 
	float 					m_fFalloff;		// Direction 조명에서 원의 바깥쪽으로 일어나는 감쇠 효과
	XMFLOAT3				m_xmf3Direction;
	float 					m_fTheta; //cos(m_fTheta)
	XMFLOAT3				m_xmf3Attenuation;
	float					m_fPhi; //cos(m_fPhi)
	int					m_bEnable;		// 조명 온오프
	int						m_nType;		// 조명 종류
	float					m_fRange;		// 조명 길이
	float					padding;
};

struct LIGHTS
{
	LIGHT					m_pLights[MAX_LIGHTS];
	XMFLOAT4				m_xmf4GlobalAmbient;
};
*/


#ifndef ThrowIfFailed
#define ThrowIfFailed(x) \
{ \
	HRESULT hr__ = (x); \
	std::wstring wfn = AnsiToWString(__FILE__); \
	if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif // !ThrowIfFailed


#ifndef ReleaseCom
#define ReleaseCom(x) { if(x) { x->Release(); x = 0; }}
#endif