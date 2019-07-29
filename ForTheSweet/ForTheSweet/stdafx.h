// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN						 // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#define INITGUID
#define _WINSOCK_DEPRECATED_NO_WARNINGS            
#include <WinSock2.h>
// Windows 헤더 파일:
#include <windows.h>

#pragma comment (lib, "ws2_32.lib")

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <algorithm>

#include <MMSystem.h>					// timer.cpp에 필요(timeGetTime)

#include <string>
#include <wrl.h>

#include <shellapi.h>

#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include "D3DUtil.h"

#include "d3dx12.h"
#include "SoundManager.h"

using namespace std;
using namespace chrono;
using namespace DirectX;
using namespace DirectX::PackedVector;

using Microsoft::WRL::ComPtr;


#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#pragma comment(lib, "winmm.lib")	// timer.cpp에 필요(timeGetTime)
#define FRAME_BUFFER_WIDTH 1280
#define FRAME_BUFFER_HEIGHT 720

#define DIR_FORWARD				0x01
#define DIR_BACKWARD			0x02
#define DIR_LEFT				0x04
#define DIR_RIGHT				0x08
#define DIR_UP					0x10
#define DIR_DOWN				0x20

#define STATE_LOGIN 0
#define STATE_MENU 1
#define STATE_ROOM 2
#define STATE_LOADING 3
#define STATE_GAME 4

// UI_
#define UI_READY	0
#define UI_FIGHT	1
#define UI_NONE		2

//캐릭터 번호
#define CHARATER_ID_0 0
#define CHARATER_ID_1 1
#define CHARATER_ID_2 2
#define CHARATER_ID_3 3
#define CHARATER_ID_4 4
#define CHARATER_ID_5 5
#define CHARATER_ID_6 6
#define CHARATER_ID_7 7

//무기 번호
#define WEAPON_EMPTY –1 

//무기 개수
#define WEAPON_MAX_NUM 5
#define WEAPON_EACH_NUM 10

//맵 모델 번호
#define M_Map_1					0
#define M_Map_1_cotton			1
#define M_Map_1_wall			2
#define M_Map_1_macaron			3

#define M_Map_2					4
#define M_Map_2_chocolate_bar	5

#define M_Map_3					6
#define M_Map_3_cake_2			7
#define M_Map_3_cake_3			8
#define M_Map_3_stair			9
#define M_Map_3_bridge			10
#define M_Map_3_in_black		11

#define M_Map_1_Shadow_test		12

#define M_Map_3_in				13
#define M_Map_3_in_stair_1		14
#define M_Map_3_in_stair_2		15

#define M_Map_1_macaron_1		20	//map_1 구별용
#define M_Map_1_macaron_2		21	//map_2 구별용
#define M_Map_1_macaron_3		22	//map_3 구별용

#define M_Map_2_chocolate_bar_2	23	//map_2 구별용

#define M_Map_3_2				24	//map_3 구별용
#define M_Map_3_cake_2_2		25	//map_3 구별용
#define M_Map_3_cake_3_2		26	//map_3 구별용

#define M_Map_3_in_2			27	//map_3 구별용
#define M_Map_3_in_stair_1_2	28	//map_3 구별용
#define M_Map_3_in_stair_2_2	29	//map_3 구별용

#define Map_SELECT				0	// M_Map_1 : 0, M_Map_2 : 4, M_Map_3 : 6

//무기 모델 번호
#define M_Weapon_Lollipop		0
#define M_Weapon_chupachupse	1
#define M_Weapon_pepero			2
#define M_Weapon_chocolate		3
#define M_Weapon_cupcake		4

//캐릭터 애니메이션
#define Anim_Idle					0
#define Anim_Walk					1
#define Anim_Run					2
#define Anim_Jump					3
#define Anim_Guard					4

#define Anim_Small_React			5
#define Anim_Pick_up				6
#define Anim_PowerUp				7

#define Anim_Cupckae_Eat			8

#define Anim_Weak_Attack1			9
#define Anim_Weak_Attack2			10
#define Anim_Weak_Attack3			11
#define Anim_Hard_Attack1			12
#define Anim_Hard_Attack2			13

#define Anim_Lollipop_Attack1		14
#define Anim_Lollipop_Attack2		15
#define Anim_Lollipop_Guard			16
#define Anim_Lollipop_Hard_Attack	17
#define Anim_Lollipop_Skill			18

#define Anim_pepero_HardAttack_1	19
#define Anim_pepero_HardAttack_2	20
#define Anim_pepero_Skill			21

#define Anim_candy_HardAttack		22
#define Anim_candy_Skill			23

#define Anim_chocolate_Attack		24
#define Anim_chocolate_HardAttack	25
#define Anim_chocolate_Guard		26
#define Anim_chocolate_Skill		27


//심판 패턴
#define PATTERN_WEAPON		1 
#define PATTERN_FOG			2
#define PATTERN_FEVER		3
#define PATTERN_LIGHTNING	4
#define PATTERN_SLIME		5

#define PATTERN_LIGHTNING_NUM		10

//오브젝트 종류
#define OBJECT_PLAYER	10
#define OBJECT_MAP		20

//DOOR 위치
#define LEFT_DOWN_OUT	11
#define LEFT_DOWN_IN	12
#define LEFT_UP_OUT		13
#define LEFT_UP_IN		14

#define RIGHT_DOWN_OUT	15
#define RIGHT_DOWN_IN	16
#define RIGHT_UP_OUT	17
#define RIGHT_UP_IN		18

//UI MESSAGE
#define MESSAGE_WEAPON 0
#define MESSAGE_FEVER 1
#define MESSAGE_FOG 2
#define MESSAGE_CUPCAKE 3
#define MESSAGE_LIGHTING 4

#define MESSAGE_NUM 5

//디펜스 키
#define KEY_A true
#define KEY_S false

const UINT LOOP_IN = 0;
const UINT LOOP_END = 1;
const UINT LOOP_TRIGGER = 2;
const UINT LOOP_STOP = 3;
const UINT LOOP_SKIP = 4;

const bool SERVER_ON = true;
const bool UI_ON = true;
#define _WITH_DIERECTX_MATH_FRUSTUM
#define _WITH_PLAYER_TOP	// 플레이어를 무조건 그려지게함.

/*정점의 색상을 무작위로(Random) 설정하기 위해 사용한다. 각 정점의 색상은 난수(Random Number)를 생성하여지정한다.*/
#define RANDOM_COLOR XMFLOAT4(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX))

extern ID3D12Resource *CreateBufferResource(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pData, UINT nBytes,
	D3D12_HEAP_TYPE d3dHeapType = D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATES d3dResourceStates = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
	ID3D12Resource **ppd3dUploadBuffer = NULL);

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

/*
class D3DUtil
{
public:
	static ComPtr<ID3D12Resource> CreateDefaultBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList,	const void* initData, UINT64 byteSize, ComPtr<ID3D12Resource>& uploadBuffer);
};

inline std::wstring AnsiToWString(const std::string& str)
{
	WCHAR buffer[512];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
	return std::wstring(buffer);
}

#ifndef ThrowIfFailed
#define ThrowIfFailed(x) \
{ \
	HRESULT hr__ = (x); \
	std::wstring wfn = AnsiToWString(__FILE__); \
	if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif // !ThrowIfFailed


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
*/
inline bool IsZero(float fValue) { return fabsf(fValue) < std::numeric_limits<float>::epsilon(); }  //fvalue의 절댓값<0


//3차원 벡터의 연산
namespace Vector3
{
	inline XMFLOAT3 XMVectorToFloat3(XMVECTOR& xmvVector)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, xmvVector);
		return(xmf3Result);
	}
	inline XMFLOAT3 ScalarProduct(XMFLOAT3& xmf3Vector, float fScalar, bool bNormalize =
		true)
	{
		XMFLOAT3 xmf3Result;
		if (bNormalize)
			XMStoreFloat3(&xmf3Result, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)) * fScalar);
		else
			XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector) * fScalar);
		return(xmf3Result);
	}
	inline XMFLOAT3 Add(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) +
			XMLoadFloat3(&xmf3Vector2));
		return(xmf3Result);
	}
	inline XMFLOAT3 Add(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2, float fScalar)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) + (XMLoadFloat3(&xmf3Vector2)
			* fScalar));
		return(xmf3Result);
	}
	inline XMFLOAT3 Subtract(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) -
			XMLoadFloat3(&xmf3Vector2));
		return(xmf3Result);
	}
	inline XMFLOAT3 Subtract(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2, bool bNormalize = true)
	{
		XMFLOAT3 xmf3Result;
		if (bNormalize)
			XMStoreFloat3(&xmf3Result, XMVector3Normalize(XMLoadFloat3(&xmf3Vector1) - XMLoadFloat3(&xmf3Vector2)));
		else
			XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) - XMLoadFloat3(&xmf3Vector2));

		return(xmf3Result);
	}
	inline float DotProduct(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3Dot(XMLoadFloat3(&xmf3Vector1),
			XMLoadFloat3(&xmf3Vector2)));
		return(xmf3Result.x);
	}
	inline XMFLOAT3 CrossProduct(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2, bool
		bNormalize = true)
	{
		XMFLOAT3 xmf3Result;
		if (bNormalize)
			XMStoreFloat3(&xmf3Result,
				XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&xmf3Vector1),
					XMLoadFloat3(&xmf3Vector2))));
		else
			XMStoreFloat3(&xmf3Result, XMVector3Cross(XMLoadFloat3(&xmf3Vector1),
				XMLoadFloat3(&xmf3Vector2)));
		return(xmf3Result);
	}
	inline XMFLOAT3 Normalize(XMFLOAT3& xmf3Vector)
	{
		XMFLOAT3 m_xmf3Normal;
		XMStoreFloat3(&m_xmf3Normal, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)));
		return(m_xmf3Normal);
	}
	inline float Length(XMFLOAT3& xmf3Vector)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3Length(XMLoadFloat3(&xmf3Vector)));
		return(xmf3Result.x);
	}
	inline float Angle(XMVECTOR&& xmvVector1, XMVECTOR&& xmvVector2)	//&앞뒤에하나
	{
		XMVECTOR xmvAngle = XMVector3AngleBetweenNormals(xmvVector1, xmvVector2);
		return(XMConvertToDegrees(acosf(XMVectorGetX(xmvAngle))));
	}
	inline float Angle(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)
	{
		return(Angle(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
	}
	inline XMFLOAT3 TransformNormal(XMFLOAT3& xmf3Vector, XMMATRIX& xmmtxTransform)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3TransformNormal(XMLoadFloat3(&xmf3Vector),
			xmmtxTransform));
		return(xmf3Result);
	}
	inline XMFLOAT3 TransformCoord(XMFLOAT3& xmf3Vector, XMMATRIX&& xmxm4x4Transform)	//&뒤에하나
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3TransformCoord(XMLoadFloat3(&xmf3Vector), xmxm4x4Transform));
		return(xmf3Result);
	}

	inline XMFLOAT3 TransformCoord(XMFLOAT3& xmf3Vector, XMFLOAT4X4& xmmtx4x4Matrix)
	{
		return(TransformCoord(xmf3Vector, XMLoadFloat4x4(&xmmtx4x4Matrix)));
	}
}

//4차원 벡터의 연산
namespace Vector4
{
	inline XMFLOAT4 Add(XMFLOAT4& xmf4Vector1, XMFLOAT4& xmf4Vector2)
	{
		XMFLOAT4 xmf4Result;
		XMStoreFloat4(&xmf4Result, XMLoadFloat4(&xmf4Vector1) +
			XMLoadFloat4(&xmf4Vector2));
		return(xmf4Result);
	}
}
//행렬의 연산
namespace Matrix4x4
{
	inline XMFLOAT4 test(XMFLOAT4 a, XMMATRIX b)
	{
		XMFLOAT4 result2 = XMFLOAT4(0, 0, 0, 1);

		float result[4];

		float tmp[4];
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				tmp[j] = b.r[i].m128_f32[j];
			}

			result[i] = (a.x * tmp[0]) + (a.y * tmp[1]) + (a.z * tmp[2]) + (a.w * tmp[3]);
		}

		result2.x = result[0];
		result2.y = result[1];
		result2.z = result[2];
		//result2.w = result[3];

		return(result2);
	}

	inline XMFLOAT4 test2(XMFLOAT4 a, XMMATRIX b)
	{
		XMFLOAT4 result2 = XMFLOAT4(0, 0, 0, 1);

		float result[4];

		float tmp[4];
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				tmp[j] = b.r[j].m128_f32[i];
			}

			result[i] = (a.x * tmp[0]) + (a.y * tmp[1]) + (a.z * tmp[2]) + (a.w * tmp[3]);
		}

		result2.x = result[0];
		result2.y = result[1];
		result2.z = result[2];
		//result2.w = result[3];

		return(result2);
	}

	inline XMFLOAT4X4 Identity()
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixIdentity());
		return(xmmtx4x4Result);
	}
	inline XMFLOAT4X4 Multiply(XMFLOAT4X4& xmmtx4x4Matrix1, XMFLOAT4X4& xmmtx4x4Matrix2)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) *
			XMLoadFloat4x4(&xmmtx4x4Matrix2));
		return(xmmtx4x4Result);
	}
	inline XMFLOAT4X4 Multiply(XMFLOAT4X4& xmmtx4x4Matrix1, XMMATRIX& xmmtxMatrix2)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) * xmmtxMatrix2);
		return(xmmtx4x4Result);
	}
	inline XMFLOAT4X4 Multiply(XMMATRIX& xmmtxMatrix1, XMFLOAT4X4& xmmtx4x4Matrix2)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, xmmtxMatrix1 * XMLoadFloat4x4(&xmmtx4x4Matrix2));
		return(xmmtx4x4Result);
	}
	inline XMFLOAT4X4 Inverse(XMFLOAT4X4& xmmtx4x4Matrix)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixInverse(NULL,
			XMLoadFloat4x4(&xmmtx4x4Matrix)));
		return(xmmtx4x4Result);
	}
	inline XMFLOAT4X4 Transpose(XMFLOAT4X4& xmmtx4x4Matrix)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result,
			XMMatrixTranspose(XMLoadFloat4x4(&xmmtx4x4Matrix)));
		return(xmmtx4x4Result);
	}
	inline XMFLOAT4X4 PerspectiveFovLH(float FovAngleY, float AspectRatio, float NearZ,
		float FarZ)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixPerspectiveFovLH(FovAngleY, AspectRatio,
			NearZ, FarZ));
		return(xmmtx4x4Result);
	}
	inline XMFLOAT4X4 LookAtLH(XMFLOAT3& xmf3EyePosition, XMFLOAT3& xmf3LookAtPosition, XMFLOAT3& xmf3UpDirection)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixLookAtLH(XMLoadFloat3(&xmf3EyePosition), XMLoadFloat3(&xmf3LookAtPosition), XMLoadFloat3(&xmf3UpDirection)));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 LookToLH(XMFLOAT3& xmf3EyePosition, XMFLOAT3& xmf3Direction, XMFLOAT3& xmf3UpDirection)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixLookToLH(XMLoadFloat3(&xmf3EyePosition), XMLoadFloat3(&xmf3Direction), XMLoadFloat3(&xmf3UpDirection)));
		return(xmmtx4x4Result);
	}
}