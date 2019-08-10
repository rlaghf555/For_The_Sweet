#include "stdafx.h"
#include "ModelObject.h"
#include "Shader.h"

ModelObject::ModelObject(Model_Animation * ma, ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	m_model = new LoadModel(*ma->getModel());
	m_ani = nullptr;
	m_NumofAnim = 0;
	m_AnimIndex = 0;
	m_Animtime = 0.0f;
	m_nMeshes = ma->getModel()->getNumMesh();
	//boundingbox = m_model->getBoudingBox();


	//매쉬 적용`
	if (m_nMeshes > 0)
	{
		m_ppMeshes = vector<unique_ptr<MMesh>>(m_nMeshes);
		for (UINT i = 0; i < m_nMeshes; i++)
			m_ppMeshes[i] = nullptr;
	}
	m_model->SetMeshes(pd3dDevice, pd3dCommandList);

	for (UINT i = 0; i < m_nMeshes; ++i) {
		if (i > 0)
			m_model->SetTextureIndex(i, i);
		SetMesh(i, m_model->getMeshes()[i]);
	}

	//뼈 정보 초기화
	m_Bones.resize(m_model->GetBones()->size());
	//cout << "!! : " << m_model->GetBones()->size() << endl;
	for (auto& p : m_Bones) {
		XMStoreFloat4x4(&p, XMMatrixIdentity());
	}
}

ModelObject::ModelObject(LoadModel* ma, ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	m_model = new LoadModel(*ma);
	m_ani = nullptr;
	m_NumofAnim = 0;
	m_AnimIndex = 0;
	m_Animtime = 0.0f;
	m_nMeshes = ma->getNumMesh();
	//boundingbox = m_model->getBoudingBox();
	//매쉬 적용
	if (m_nMeshes > 0)
	{
		m_ppMeshes = vector<unique_ptr<MMesh>>(m_nMeshes);
		for (UINT i = 0; i < m_nMeshes; i++)
			m_ppMeshes[i] = nullptr;
	}
	m_model->SetMeshes(pd3dDevice, pd3dCommandList);

	for (UINT i = 0; i < m_nMeshes; ++i) {
		if (i > 0)
			m_model->SetTextureIndex(i, i);
		SetMesh(i, m_model->getMeshes()[i]);
	}

	//뼈 정보 초기화
	m_Bones.resize(m_model->GetBones()->size());
	//cout << "!! : " << m_model->GetBones()->size() << endl;
	for (auto& p : m_Bones) {
		XMStoreFloat4x4(&p, XMMatrixIdentity());
	}
}

ModelObject::~ModelObject()
{
	//cout << "Modelobject 소멸자" << endl;
	//CGameObject::~CGameObject();
	if (m_model) {
		delete m_model;
		m_model = NULL;
	//	cout << "m_model delete" << endl;
	}
	//for(int i=0;i < m_NumofAnim;i++)
	//	if (m_ani[i]) {
	//		delete m_ani[i];
	//		m_ani[i] = NULL;
	//	}
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

		//cout << m_model->GetBones()->size() << endl;
	}
}

void ModelObject::Animate(float fTime)
{
	if (m_ani) {
		if (m_AnimIndex < m_NumofAnim) {
			//cout << "본 움직였다\n";
			m_loopCheck = m_ani[m_AnimIndex]->BoneTransform(m_AnimIndex, fTime, m_Bones);

			XMMATRIX tmp;
			tmp = DirectX::XMLoadFloat4x4(&m_xmf4x4World);

			XMFLOAT4 hand_pos = Matrix4x4::test2(m_ani[m_AnimIndex]->getHandPos(), tmp);

			SetHandPos(hand_pos);
		}
	}
}

void ModelObject::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera)
{
	CGameObject::Render(pd3dCommandList, pCamera);
}

void ModelObject::ChangeAnimation(UINT nextIndex)
{
	if (nextIndex == m_AnimIndex)
		if (nextIndex != Anim_Small_React)
			return;
	if (nextIndex == Anim_Weak_Attack2 || nextIndex == Anim_Weak_Attack3 || nextIndex == Anim_Hard_Attack2
		 || nextIndex == Anim_Lollipop_Attack2)
	{
		//cout << "~~~" << endl;
		m_ani[nextIndex]->SetAnimFrame(m_ani[m_AnimIndex]->getAnimTime());
		m_ani[m_AnimIndex]->ResetAnimation();
	}
	else {
		if (nextIndex != m_AnimIndex) {
			m_ani[m_AnimIndex]->ResetAnimation();
			//cout << "asdfsf\n";
		}
	}
	//m_loopCheck = 0;
	m_AnimIndex = nextIndex;
	if (m_AnimIndex == Anim_Jump) {
		SoundManager::GetInstance()->PlaySounds(JUMPSOUND);
		jump_pos_y = GetPosition().y;
	}
	if(m_AnimIndex == Anim_Small_React||m_AnimIndex == Anim_Hard_React)SoundManager::GetInstance()->PlaySounds(HIT);
}

void ModelObject::ChangeAnimationSpeed(UINT AnimIndex, float speed)
{
	m_ani[AnimIndex]->saveAnimSpeed();
	m_ani[AnimIndex]->SetAnimSpeed(speed);
}

void ModelObject::ResetAnimationSpeed(UINT AnimIndex)
{
	m_ani[AnimIndex]->loadAnimSpeed();
}

void ModelObject::stopAnim(bool stop)
{
	m_ani[m_AnimIndex]->StopAnim(stop);
}
