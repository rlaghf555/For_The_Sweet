#pragma once
#include "stdafx.h"
#include "assimp\Importer.hpp"
#include "assimp\cimport.h"
#include "assimp\postprocess.h"
#include "assimp\scene.h"


#pragma comment(lib, "lib/assimp-vc140-mt.lib")


inline XMMATRIX aiMatrixToXMMatrix(const aiMatrix4x4& offset)
{
	return XMMATRIX(&offset.a1);
}

struct vertexDatas
{
#define BONES_PER_VERTEX 4

	XMFLOAT3	m_pos;
	XMFLOAT3	m_normal;
	XMFLOAT3 m_tan;
	XMFLOAT2	m_tex;
	XMUINT4	m_bornIndex;
	XMFLOAT3	m_weights;
	UINT		m_nTextureNum = 0;

	vertexDatas() {}
	vertexDatas(XMFLOAT3& pos, XMFLOAT3& normal, XMFLOAT3& tan, XMFLOAT2& tex, UINT texindex) :
		m_pos(pos), m_normal(normal), m_tan(tan), m_tex(tex), m_nTextureNum(texindex)
	{
		m_weights = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_bornIndex = XMUINT4(0, 0, 0, 0);
	}

	void AddBoneData(UINT index, float weight) {
		if (m_weights.x == 0.0f) {
			m_bornIndex.x = index;
			m_weights.x = weight;
		}
		else if (m_weights.y == 0.0f) {
			m_bornIndex.y = index;
			m_weights.y = weight;
		}
		else if (m_weights.z == 0.0f) {
			m_bornIndex.z = index;
			m_weights.z = weight;
		}
		else {
			m_bornIndex.w = index;
		}
	}
};

struct mesh
{
	vector<vertexDatas>	m_vertices;
	vector<int>				m_indices;
	UINT						m_materialIndex;

	mesh() {
		m_materialIndex = 0;
	}
	void SetMeshesTextureIndex(UINT index) {
		for (auto& d : m_vertices)
			d.m_nTextureNum = index;
	}
};

struct Bone
{
	XMMATRIX	BoneOffset;
	XMMATRIX FinalTransformation;

	Bone() {
		BoneOffset = XMMatrixIdentity();
		FinalTransformation = XMMatrixIdentity();
	}
};
class MMesh
{
public:
	MMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual ~MMesh();

private:
	int								m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	void ReleaseUploadBuffers();

protected:
	ComPtr<ID3D12Resource>			m_pd3dVertexBuffer = NULL;
	ComPtr<ID3D12Resource>			m_pd3dVertexUploadBuffer = NULL;

	ComPtr<ID3D12Resource>			m_pd3dIndexBuffer = NULL;
	ComPtr<ID3D12Resource>			m_pd3dIndexUploadBuffer = NULL;

	D3D12_VERTEX_BUFFER_VIEW		m_d3dVertexBufferView;
	D3D12_INDEX_BUFFER_VIEW			m_d3dIndexBufferView;

	D3D12_PRIMITIVE_TOPOLOGY		m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT							m_nSlot = 0;
	UINT							m_nVertices = 0;
	UINT							m_nStride = 0;
	UINT							m_nOffset = 0;

	UINT							m_nIndices = 0;
	UINT							m_nStartIndex = 0;
	int								m_nBaseVertex = 0;

public:
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, UINT nInstanceCount);
};
class ModelMesh : public MMesh
{
public:
	ModelMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,
		mesh& meshData);
	virtual ~ModelMesh() {}
};

class LoadModel
{
private:
	const aiScene*			m_pScene;				//모델 정보
	vector<mesh>				m_meshes;			//매쉬 정보
	vector<ModelMesh*>	m_ModelMeshes;	//매쉬 정보 리소스(for 랜더링)
	vector<pair<string, Bone>> m_Bones;		//뼈 정보

	UINT							m_numVertices;
	UINT							m_numBones;
public:
	LoadModel(const string& fileName, bool isStatic);
	LoadModel(const LoadModel& T);
	~LoadModel();

	void InitScene();
	void InitMesh(UINT index, const aiMesh* pMesh);
	void SetMeshes(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	void InitBones(UINT index, const aiMesh* pMesh);
	void SetTextureIndex(UINT meshIndex, UINT textureIndex) { m_meshes[meshIndex].SetMeshesTextureIndex(textureIndex); };

	ModelMesh**	getMeshes() { return m_ModelMeshes.data(); }
	mesh*			getMesh(UINT index) { return &m_meshes[index]; }
	UINT				getNumMesh() const { return (UINT)m_meshes.size(); }
	vector<pair<string, Bone>>* GetBones() { return &m_Bones; }
	UINT				getNumVertices() const { return m_numVertices; }

};