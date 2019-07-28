#pragma once
#include "Player.h"

struct CB_UI_INFO {
	XMFLOAT2  m_xmf2ScreenPos;
	XMFLOAT2  m_xmf2ScreenSize;

	XMUINT2	 m_nNumSprite;
	XMUINT2	 m_nNowSprite;

	XMUINT2  m_nSize;
	UINT	 m_nTexType;
	float	 m_fData;

	float m_fData2 = 0.0f;
	XMFLOAT2 m_xmf2Scale;
	float    m_fAlpha = 1.0f;
};

class UIObject
{
public:
	UIObject();
	~UIObject();

public:
	void SetCbvGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle) { m_d3dCbvGPUDescriptorHandle = d3dCbvGPUDescriptorHandle; }
	void SetCbvGPUDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr) { m_d3dCbvGPUDescriptorHandle.ptr = nCbvGPUDescriptorHandlePtr; }

	D3D12_GPU_DESCRIPTOR_HANDLE GetCbvGPUDescriptorHandle() { return(m_d3dCbvGPUDescriptorHandle); }

	virtual void Update(float fTimeElapsed);
	virtual void SetRootParameter(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual bool CollisionUI(POINT* pPoint, XMFLOAT2& trueSetData, XMFLOAT2& falseSetData);
	virtual void BuildMaterials(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) { }
	virtual void CreateCollisionBox();
	virtual void SetEnabled(bool enable) { m_bEnabled = enable; }
	virtual void SetScreenSize(XMFLOAT2 size);
	virtual void SetPosition(XMFLOAT2 pos);
	virtual void SetPosition(float x, float y)
	{
		m_xmf2ScreenPos.x = x;
		m_xmf2ScreenPos.y = y;
	}
	virtual void SetScale(XMFLOAT2& scale);
	virtual void SetSize(XMUINT2& size);
	virtual void SetAlpha(float alpha);
	virtual void SetNumSprite(XMUINT2& numSprite, XMUINT2& nowSprite);
	virtual void SetHP(float hp) {}
	virtual void SetType(UINT type) { m_nTexType = type; }
	virtual XMFLOAT2 GetPos() { return m_xmf2ScreenPos; }

public:
	bool										m_bEnabled = true;
	float										m_fAnimationTime = 0.3f;
	D3D12_GPU_DESCRIPTOR_HANDLE					m_d3dCbvGPUDescriptorHandle;

	XMFLOAT2	m_xmf2ScreenPos;
	XMFLOAT2	m_xmf2ScreenSize;

	XMUINT2		m_nNumSprite;
	XMUINT2		m_nNowSprite;

	XMUINT2		m_nSize;
	UINT		m_nTexType = -1;
	float		m_fData = 0.0f;
	float		m_fData2 = 0.0f;

	float		m_fAlpha = 1.0f;

	XMFLOAT2	m_xmf2Scale = XMFLOAT2(1.0f, 1.0f);
	XMFLOAT2	m_xmf2StartPos;
	XMFLOAT2	m_xmf2EndPos;

	int mystate = UI_NONE;
	int uistate;
};

class HPBarObject : public UIObject
{
public:
	HPBarObject() {};
	~HPBarObject() {};

public:
	void SetHP(float hp) { Now_Gauge = hp; UpdateScale(); }
	void SetMaxGauge(float Gauge) { Max_Gauge = Gauge; }
	//virtual void Update(float fTimeElapsed);
	void UpdateScale();
protected:
	float Max_Gauge = 100;		// √÷¥Î HP
	float Now_Gauge = 100;
};
class MessageObject : public UIObject {
public:
	MessageObject() {};
	~MessageObject() {};
	virtual void Update(float fTimeElapsed);

};