#include "stdafx.h"
#include "UIObject.h"


UIObject::UIObject()
{
}

UIObject::~UIObject()
{
}

void UIObject::SetRootParameter(ID3D12GraphicsCommandList * pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootDescriptorTable(0, m_d3dCbvGPUDescriptorHandle);
}

void UIObject::Render(ID3D12GraphicsCommandList * pd3dCommandList)
{
	SetRootParameter(pd3dCommandList);

	pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pd3dCommandList->DrawInstanced(6, 1, 0, 0);
}
void UIObject::Update(float fTimeElapsed) {

}


bool UIObject::CollisionUI(POINT * pPoint, XMFLOAT2& trueSetData, XMFLOAT2& falseSetData)
{
	if (m_fAlpha < 1.0f) {
		m_fData = falseSetData.x;
		m_fData2 = falseSetData.y;
		return false;
	}

	if (m_xmf2StartPos.x < pPoint->x && m_xmf2StartPos.y > pPoint->y) {
		if (m_xmf2EndPos.x > pPoint->x && m_xmf2EndPos.y < pPoint->y) {
			m_fData = trueSetData.x;
			m_fData2 = trueSetData.y;
			return true;
		}
	}
	m_fData = falseSetData.x;
	m_fData2 = falseSetData.y;
	return false;
}

void UIObject::CreateCollisionBox()
{
	m_xmf2StartPos = XMFLOAT2(
		m_xmf2ScreenPos.x - static_cast<float>(m_nSize.x / 2) * m_xmf2Scale.x,
		static_cast<float>(FRAME_BUFFER_HEIGHT) - (m_xmf2ScreenPos.y - static_cast<float>(m_nSize.y / 2) * m_xmf2Scale.y)
	);
	m_xmf2EndPos = XMFLOAT2(
		m_xmf2ScreenPos.x + static_cast<float>(m_nSize.x / 2) * m_xmf2Scale.x,
		static_cast<float>(FRAME_BUFFER_HEIGHT) - (m_xmf2ScreenPos.y + static_cast<float>(m_nSize.y / 2) * m_xmf2Scale.y)
	);
}

void UIObject::SetScreenSize(XMFLOAT2 size)
{
	m_xmf2ScreenSize = size;
}

void UIObject::SetPosition(XMFLOAT2 pos)
{
	m_xmf2ScreenPos = pos;
}

void UIObject::SetScale(XMFLOAT2 & scale)
{
	m_xmf2Scale = scale;
}

void UIObject::SetSize(XMUINT2 & size)
{
	m_nSize = size;
}

void UIObject::SetAlpha(float alpha)
{
	m_fAlpha = alpha;
}

void UIObject::SetNumSprite(XMUINT2 & numSprite, XMUINT2& nowSprite)
{
	m_nNumSprite = numSprite;
	m_nNowSprite = nowSprite;
}

void HPBarObject::UpdateScale()
{
	float tmp = m_nSize.x *m_xmf2Scale.x;
	m_xmf2Scale.x = 0.8f *Now_Gauge / Max_Gauge;
	float tmp2 = m_nSize.x * m_xmf2Scale.x;
	tmp -= tmp2;
	m_xmf2ScreenPos.x -= (tmp / 2);
}

void HPBarObject::UpdateScale2()
{
	float tmp = m_nSize.x *m_xmf2Scale.x;
	m_xmf2Scale.x = 0.88f *Now_Gauge / Max_Gauge;
	float tmp2 = m_nSize.x * m_xmf2Scale.x;
	tmp -= tmp2;
	m_xmf2ScreenPos.x -= (tmp / 2);
}



void MessageObject::Update(float fTimeElapsed)	//날아오는 메세지 속도조정
{
	if (mystate == uistate) {
		m_bEnabled = true;

		if (m_xmf2ScreenPos.x > 600 && m_xmf2ScreenPos.x < 750) {
			m_xmf2ScreenPos.x -= 4;
		}
		else m_xmf2ScreenPos.x -= 20;
		if (m_xmf2ScreenPos.x < -300)
			m_bEnabled = false;
	}
	else {
		m_bEnabled = false;
	}
}
