#pragma once

#include "Object.h"
#include "Model_Animation.h"
struct CB_DYNAMICOBJECT_INFO
{
	XMFLOAT4X4		m_xmf4x4World;
	XMFLOAT4X4		m_bone[96];
	UINT					m_nMaterial = 0;
};

class ModelObject : public CGameObject
{
protected:
	LoadModel *			m_model;			//모델 정보 (vertex, UV and ....)
	LoadAnimation**		m_ani;				//애니메이션 정보 (multi animation)

	vector<XMFLOAT4X4>	m_Bones;			//뼈 정보 (최종 변환 정보)
	float							m_Animtime;		//애니메이션 상대 시간
	UINT							m_AnimIndex;		//현재 애니메이션
	UINT							m_NumofAnim;	//애니메이션 갯수
	UINT							m_loopCheck = 0;

	CMaterial*						m_pMaterial = NULL;

public:
	ModelObject(Model_Animation* ma, ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	ModelObject(LoadModel* ma, ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual ~ModelObject();

	virtual void SetAnimations(UINT num, LoadAnimation** tmp);
	virtual UINT GetNumofAnim() const { return m_NumofAnim; }
	UINT getAnimLoop() const { return m_loopCheck; }
	virtual void Animate(float fTime);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = nullptr);
	virtual void Move(ULONG nDirection, float fDistance, bool bVelocity = false) {}
	virtual void Move(const XMFLOAT3& xmf3Shift, bool bVelocity = false) {}
	virtual void Move(float fxOffset = 0.0f, float fyOffset = 0.0f, float fzOffset = 0.0f) {}
	virtual XMFLOAT4X4* GetBoneData() {
		return m_Bones.data();
	}
	virtual int GetBoneNum() const {
		return (int)m_Bones.size();
	}

	void ChangeAnimation(UINT nextIndex);

	void stopAnim(bool stop);

	virtual int getAnimIndex() { return m_AnimIndex; }
	float getAnimtime() { return m_ani[m_AnimIndex]->getAnimTime(); }
	void SetAnimFrame(float ftime) { m_ani[m_AnimIndex]->SetAnimFrame(ftime); }
	void DisableLoop() { m_ani[m_AnimIndex]->DisableLoop(0); }
	void EnableLoop() { m_ani[m_AnimIndex]->EnableLoop(); }
	void printAniInfo() {
		cout << "애니 갯수 : " << GetNumofAnim() << endl;
		for (int i = 0; i < GetNumofAnim(); ++i)
		{
			cout << "애니 타임 : " << m_ani[i]->getAnimTime() << endl;
		}
	}
};