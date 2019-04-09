#pragma once
#include "Model.h"
#include "Animation.h"

class Model_Animation
{
protected:
	LoadModel *model;
	vector<LoadAnimation*>* animStack;
	vector<string>		matList;
	vector<string>		normMatList;
public:
	Model_Animation();
	Model_Animation(string Model_filename, vector<pair<string,float>>* Animation_filename);
	~Model_Animation();

	void LodingModels(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	LoadModel* getModel() { return model; }
	UINT getAnimCount() { return (UINT)animStack->size(); }
	LoadAnimation** getAnim(UINT index) { return animStack[index].data(); }
	wstring getMat(UINT index) {
		return wstring(matList[index].begin(), matList[index].end());
	}
	wstring getNorMat(UINT index) {
		return wstring(normMatList[index].begin(), normMatList[index].end());
	}
	bool isMat(UINT index) {
		if (matList[index] != "null")
			return true;
		return false;
	}
	bool isNormat(UINT index) {
		if (normMatList[index] != "null")
			return true;
		return false;
	}

};
