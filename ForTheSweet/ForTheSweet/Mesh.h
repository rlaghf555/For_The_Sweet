#pragma once

class CVertex
{
protected:
	XMFLOAT3 m_xmf3Position;		// 위치
public:
	CVertex() { m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	CVertex(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	~CVertex() { }
};

class CDiffusedVertex : public CVertex
{
protected:
	XMFLOAT4 m_xmf4Diffuse;			// 정점
public:
	CDiffusedVertex() {	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f); m_xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);}
	CDiffusedVertex(float x, float y, float z, XMFLOAT4 xmf4Diffuse) { m_xmf3Position = XMFLOAT3(x, y, z); m_xmf4Diffuse = xmf4Diffuse; }
	CDiffusedVertex(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Diffuse) { m_xmf3Position = xmf3Position; m_xmf4Diffuse = xmf4Diffuse; }
	~CDiffusedVertex() { }
};

struct Vertex
{
	XMFLOAT3	m_pos;
	XMFLOAT3	m_normal;
	XMFLOAT3	m_tan;
	XMFLOAT2	m_tex;
	UINT		m_nTextureNum = 0;

	Vertex() {}
	Vertex(XMFLOAT3& pos, XMFLOAT3& normal, XMFLOAT3& tan, XMFLOAT2& tex) : m_pos(pos), m_normal(normal), m_tan(tan), m_tex(tex) {}
	Vertex(float px, float py, float pz, float nx, float ny, float nz, float tx, float ty, float tz, float u, float v) : m_pos(px, py, pz), m_normal(nx, ny, nz), m_tan(tx, ty, tz), m_tex(u, v) {}
	Vertex(XMFLOAT3& pos, XMFLOAT3& normal, XMFLOAT3& tan, XMFLOAT2& tex, UINT texindex) : m_pos(pos), m_normal(normal), m_tan(tan), m_tex(tex), m_nTextureNum(texindex) {}
};

struct MeshData
{
	vector<Vertex>			m_vertices;
	vector<int>				m_indices;
	UINT					m_materialIndex;

	MeshData() { m_materialIndex = 0; }
	void SetMeshesTextureIndex(UINT index) {
		for (auto& d : m_vertices)
			d.m_nTextureNum = index;
	}
};

class CMesh
{
public:
	CMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual ~CMesh();
private:
	int								m_nReferences = 0;
protected:
	ID3D12Resource					* m_pd3dIndexBuffer = NULL;
	ID3D12Resource					*m_pd3dIndexUploadBuffer = NULL;
	
	D3D12_INDEX_BUFFER_VIEW			m_d3dIndexBufferView;		//인덱스 버퍼(인덱스의 배열)와 인덱스 버퍼를 위한 업로드 버퍼에 대한 인터페이스 포인터이다. 인덱스 버퍼는 정점 버퍼(배열)에 대한 인덱스를 가진다.
	UINT							m_nIndices = 0;				//인덱스 버퍼에 포함되는 인덱스의 개수이다.
	
	UINT							m_nStartIndex = 0;			//인덱스 버퍼에서 메쉬를 그리기 위해 사용되는 시작 인덱스이다.
	
	int								m_nBaseVertex = 0;			//인덱스 버퍼의 인덱스에 더해질 인덱스이다.
public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	void ReleaseUploadBuffers();
protected:
	ID3D12Resource					*m_pd3dVertexBuffer = NULL;
	ID3D12Resource					*m_pd3dVertexUploadBuffer = NULL;
	D3D12_VERTEX_BUFFER_VIEW		m_d3dVertexBufferView;
	D3D12_PRIMITIVE_TOPOLOGY		m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT							m_nSlot = 0;
	UINT							m_nVertices = 0;
	UINT							m_nStride = 0;
	UINT							m_nOffset = 0;
public:
	BoundingOrientedBox				m_xmOOBB;
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, UINT nInstances = 1);
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
	//직육면체의 가로, 세로, 깊이의 길이를 지정하여 직육면체 메쉬를 생성한다.
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

class CreateQuad : public CMesh
{
public:
	CreateQuad(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float x, float y, float w, float h, float depth);
	virtual ~CreateQuad();
};

class CreateGrid : public CMesh
{
public:
	CreateGrid(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, float width, float depth, int m, int n);
	virtual ~CreateGrid();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
	BoundingOrientedBox				m_xmOOBB;
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, UINT nInstanceCount);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
