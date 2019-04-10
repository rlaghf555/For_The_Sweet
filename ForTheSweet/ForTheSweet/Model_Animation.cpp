#include "stdafx.h"
#include "Model_Animation.h"

Model_Animation::Model_Animation()
{
	
}

Model_Animation::Model_Animation(string Model_filename , vector<pair<string, float>>* Animation_filename)
{
	UINT animCount;
	UINT index;
	model = new LoadModel(Model_filename.c_str(), false);
	animStack = new vector<LoadAnimation*>; //�ִϸ��̼��� ������ ���� ����

	animCount = Animation_filename->size();
	animStack->reserve(animCount);

	for (UINT i = 0; i < animCount; ++i) {
		LoadAnimation* tmpAnim = new LoadAnimation((Animation_filename->begin()+i)->first, (Animation_filename->begin() + i)->second, 0); //�ִϸ��̼��� �ε�
		animStack->push_back(tmpAnim); //�ִϸ��̼��� ���Ϳ� ����
	}


}
Model_Animation::~Model_Animation()

{
}
void Model_Animation::LodingModels(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	//�ε��� �������� ���� �� ������Ʈ�� ��
	//�� ��������, �ε��� ���۸� �����Ѵٴ� ��
	model->SetMeshes(pd3dDevice, pd3dCommandList);
}