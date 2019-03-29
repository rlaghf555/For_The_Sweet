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



public:
	ModelObject(Model_Animation* ma, ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual ~ModelObject();

	virtual void SetAnimations(UINT num, LoadAnimation** tmp);
	virtual UINT GetNumofAnim() const { return m_NumofAnim; }
	UINT getAnimLoop() const { return m_loopCheck; }
	virtual void Animate(float fTime);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = nullptr);

	virtual XMFLOAT4X4* GetBoneData() {
		return m_Bones.data();
	}
	virtual int GetBoneNum() const {
		return (int)m_Bones.size();
	}

	void ChangeAnimation(UINT nextIndex);

	void stopAnim(bool stop);

	virtual int getAnimIndex() { return m_AnimIndex; }

};