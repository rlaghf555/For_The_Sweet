#pragma once

// �޽� : �̸�, �븻 ����, ���ؽ� ����, ������, ������, �ؽ�������
struct vertexDatas
{
	XMFLOAT3	m_pos;
	XMFLOAT3	m_normal;
	XMFLOAT2	m_tex;

	vertexDatas(){}
	vertexDatas(XMFLOAT3& pos, XMFLOAT3& normal, XMFLOAT2& tex) : m_pos(pos), m_normal(normal), m_tex(tex){}
};

// ���ؽ� ���ռ���(�ε��� ����) : �޽� ����
struct mesh
{
	std::vector<vertexDatas>	m_vertices;
	std::vector<int>			m_indices;
	UINT						m_materialIndex;
	mesh() { m_materialIndex = 0; }
};

//class LoadModel
//{
//private:
//	const aiScene*					m_pScene;	// �� ����
//	std::vector<mesh>				m_meshes;	// �Ž� ����
//	std::vector<pair<string, Bone>>	m_Bones;	// �� ����
//	UINT                            m_numVertices;
//	UINT                            m_numMaterial;
//public:
//	LoadModel(const string& fileName);
//	~LoadModel();
//	void InitScene();
//	void InitMesh(UINT index, const aiMesh* pMesh);
//	void SetMeshes(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
//	ModelMesh**    getMeshes() { return m_ModelMeshes.data(); }
//	UINT           getNumMesh() const { return (UINT)m_meshes.size(); }
//};

class CMesh
{
protected:
	ID3D12Resource * m_pd3dIndexBuffer = NULL;
	ID3D12Resource *m_pd3dIndexUploadBuffer = NULL;
	//�ε��� ����(�ε����� �迭)�� �ε��� ���۸� ���� ���ε� ���ۿ� ���� �������̽� �������̴�. �ε��� ���۴� ���� ����(�迭)�� ���� �ε����� ������.
	D3D12_INDEX_BUFFER_VIEW m_d3dIndexBufferView;
	UINT m_nIndices = 0;
	//�ε��� ���ۿ� ���ԵǴ� �ε����� �����̴�.
	UINT m_nStartIndex = 0;
	//�ε��� ���ۿ��� �޽��� �׸��� ���� ���Ǵ� ���� �ε����̴�.
	int m_nBaseVertex = 0;
	//�ε��� ������ �ε����� ������ �ε����̴�.
public:
	CMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual ~CMesh();
private:
	int m_nReferences = 0;
public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	void ReleaseUploadBuffers();
protected:
	ID3D12Resource * m_pd3dVertexBuffer = NULL;
	ID3D12Resource *m_pd3dVertexUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW m_d3dVertexBufferView;
	D3D12_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT m_nSlot = 0;
	UINT m_nVertices = 0;
	UINT m_nStride = 0;
	UINT m_nOffset = 0;
public:
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, UINT nInstances = 1);
};

class CVertex
{
protected:
	//������ ��ġ �����̴�(��� ������ �ּ��� ��ġ ���͸� ������ �Ѵ�).
	XMFLOAT3 m_xmf3Position;
public:
	CVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	CVertex(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	~CVertex() { }
};

class CDiffusedVertex : public CVertex
{
protected:
	//������ �����̴�.
	XMFLOAT4 m_xmf4Diffuse;
public:
	CDiffusedVertex() {
		m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); m_xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	CDiffusedVertex(float x, float y, float z, XMFLOAT4 xmf4Diffuse) {
		m_xmf3Position = XMFLOAT3(x, y, z); m_xmf4Diffuse = xmf4Diffuse;
	}
	CDiffusedVertex(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Diffuse) {
		m_xmf3Position = xmf3Position; m_xmf4Diffuse = xmf4Diffuse;
	}
	~CDiffusedVertex() { }
};

class CTriangleMesh : public CMesh
{
public:
	CTriangleMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual ~CTriangleMesh() { }
};

class CCubeMeshDiffused : public CMesh
{
public:
	//������ü�� ����, ����, ������ ���̸� �����Ͽ� ������ü �޽��� �����Ѵ�.
	CCubeMeshDiffused(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshDiffused();
};

class CAirplaneMeshDiffused : public CMesh
{
public:
	CAirplaneMeshDiffused(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList, float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 4.0f,
		XMFLOAT4 xmf4Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f));
	virtual ~CAirplaneMeshDiffused();
};