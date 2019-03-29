#include "stdafx.h"
#include "ModelObject.h"

ModelObject::ModelObject(Model_Animation * ma, ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	m_model = new LoadModel(*ma->getModel());
	m_ani = nullptr;
	m_NumofAnim = ma->getAnimCount();
	m_AnimIndex = 0;
	m_Animtime = 0.0f;

	//매쉬 적용
	for (UINT i = 0; i < m_nMeshes; ++i) {
		if (i > 0)
			m_model->SetTextureIndex(i, i);
		SetMesh(i, m_model->getMeshes()[i]);
	}

	//뼈 정보 초기화
	m_Bones.resize(m_model->GetBones()->size());
	for (auto& p : m_Bones) {
		XMStoreFloat4x4(&p, XMMatrixIdentity());
	}
	SetAnimations(m_NumofAnim, ma->getAnim());
}

ModelObject::~ModelObject()
{
	CGameObject::~CGameObject();
}

void ModelObject::SetAnimations(UINT num, LoadAnimation ** tmp)
{
	if (num > 0) {
		m_NumofAnim = num;
		m_AnimIndex = 0;
		m_ani = new LoadAnimation*[m_NumofAnim];

		for (UINT i = 0; i < m_NumofAnim; ++i) {
			m_ani[i] = new LoadAnimation(*tmp[i]);
			m_ani[i]->setBones(m_model->GetBones());
		}
	}
}

void ModelObject::Animate(float fTime)
{
	if (m_ani) {
		if (m_AnimIndex < m_NumofAnim)
			m_loopCheck = m_ani[m_AnimIndex]->BoneTransform(m_AnimIndex, fTime, m_Bones);
	}
}

void ModelObject::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera)
{
	CGameObject::Render(pd3dCommandList, pCamera);
}

void ModelObject::ChangeAnimation(UINT nextIndex)
{
	if (nextIndex == m_AnimIndex)
		return;

	m_ani[m_AnimIndex]->ResetAnimation();
	//m_loopCheck = 0;
	m_AnimIndex = nextIndex;
}

void ModelObject::stopAnim(bool stop)
{
	m_ani[m_AnimIndex]->StopAnim(stop);
}
