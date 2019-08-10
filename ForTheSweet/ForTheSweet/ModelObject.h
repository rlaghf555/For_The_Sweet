#pragma once

#include "Object.h"
#include "Model_Animation.h"
struct CB_DYNAMICOBJECT_INFO
{
	XMFLOAT4X4		m_xmf4x4World;
	XMFLOAT4X4		m_bone[96];
	UINT			m_nMaterial = 0;
	XMFLOAT4X4		m_xmf4x4ShadowTransform;
};

class ModelObject : public CGameObject
{
protected:
	LoadModel *			m_model;			//모델 정보 (vertex, UV and ....)

	vector<XMFLOAT4X4>				m_Bones;			//뼈 정보 (최종 변환 정보)
	float							m_Animtime;		//애니메이션 상대 시간
	UINT							m_AnimIndex;		//현재 애니메이션
	UINT							m_NumofAnim;	//애니메이션 갯수
	UINT							m_loopCheck = 0;

	CMaterial*						m_pMaterial = NULL;

	XMFLOAT4						m_handpos;

	float							jump_pos_y = 0.f;
public:
	LoadAnimation**		m_ani;				//애니메이션 정보 (multi animation)
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
	void ChangeAnimationSpeed(UINT AnimIndex, float speed = 2.0f);
	void ResetAnimationSpeed(UINT AnimIndex);
	void stopAnim(bool stop);

	virtual int getAnimIndex() { return m_AnimIndex; }
	float getAnimtime() { return m_ani[m_AnimIndex]->getAnimTime(); }
	void SetAnimFrame(float ftime) { m_ani[m_AnimIndex]->SetAnimFrame(ftime); }
	void DisableLoop() { m_ani[m_AnimIndex]->DisableLoop(0); }
	void EnableLoop() { m_ani[m_AnimIndex]->EnableLoop(); }
	void printAniInfo() {
		cout << "애니 갯수 : " << GetNumofAnim() << endl;
		for (UINT i = 0; i < GetNumofAnim(); ++i)
		{
			cout << "애니 타임 : " << m_ani[i]->getAnimTime() << endl;
		}
	}

	void SetHandPos(XMFLOAT4 pos) { m_handpos = pos; }
	XMFLOAT4 GetHandPos() { return m_handpos; }

	virtual float GetJumpPos() { return jump_pos_y; };
	virtual void SetJumpPos(float posy) { jump_pos_y = posy; };
};