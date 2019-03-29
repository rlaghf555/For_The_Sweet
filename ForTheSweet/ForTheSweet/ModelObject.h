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
	LoadModel *			m_model;			//�� ���� (vertex, UV and ....)
	LoadAnimation**		m_ani;				//�ִϸ��̼� ���� (multi animation)

	vector<XMFLOAT4X4>	m_Bones;			//�� ���� (���� ��ȯ ����)
	float							m_Animtime;		//�ִϸ��̼� ��� �ð�
	UINT							m_AnimIndex;		//���� �ִϸ��̼�
	UINT							m_NumofAnim;	//�ִϸ��̼� ����
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