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
	animStack = new vector<LoadAnimation*>; //애니메이션을 적재할 벡터 생성

	animCount = Animation_filename->size();
	animStack->reserve(animCount);

	for (UINT i = 0; i < animCount; ++i) {
		LoadAnimation* tmpAnim = new LoadAnimation((Animation_filename->begin()+i)->first, (Animation_filename->begin() + i)->second, 0); //애니메이션을 로딩
		animStack->push_back(tmpAnim); //애니메이션을 벡터에 적재
	}


}
Model_Animation::~Model_Animation()

{
}
void Model_Animation::LodingModels(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	//로딩한 모델파일을 게임 상 오브젝트에 셋
	//즉 정점버퍼, 인덱스 버퍼를 생성한다는 것
	model->SetMeshes(pd3dDevice, pd3dCommandList);
}