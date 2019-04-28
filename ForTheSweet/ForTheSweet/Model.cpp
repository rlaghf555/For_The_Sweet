 #include "stdafx.h"
#include "Model.h"

ModelMesh::ModelMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, mesh& meshData) : MMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices = (int)meshData.m_vertices.size();
	m_nIndices = (int)meshData.m_indices.size();

	cout << "정점 개수 : " << m_nVertices << endl;

	m_nStride = sizeof(vertexDatas);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_xmOOBB = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(10.f, 10.f, 10.f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	//버텍스 버퍼
	m_pd3dVertexBuffer = D3DUtil::CreateDefaultBuffer(pd3dDevice, pd3dCommandList, meshData.m_vertices.data(), m_nStride * m_nVertices, m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	//인덱스 버퍼
	m_pd3dIndexBuffer = D3DUtil::CreateDefaultBuffer(pd3dDevice, pd3dCommandList, meshData.m_indices.data(), sizeof(int)*m_nIndices, m_pd3dIndexUploadBuffer);

	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT)*m_nIndices;
}

LoadModel::LoadModel(const string& fileName, bool isStatic)
{
	UINT flag = aiProcess_JoinIdenticalVertices |			// 동일한 꼭지점 결합, 인덱싱 최적화
		aiProcess_ValidateDataStructure |					// 로더의 출력을 검증
		aiProcess_ImproveCacheLocality |					// 출력 정점의 캐쉬위치를 개선
		aiProcess_RemoveRedundantMaterials |				// 중복된 매터리얼 제거
		aiProcess_GenUVCoords |								// 구형, 원통형, 상자 및 평면 매핑을 적절한 UV로 변환
		aiProcess_TransformUVCoords |						// UV 변환 처리기 (스케일링, 변환...)
		aiProcess_FindInstances |							// 인스턴스된 매쉬를 검색하여 하나의 마스터에 대한 참조로 제거
		aiProcess_LimitBoneWeights |						// 정점당 뼈의 가중치를 최대 4개로 제한
		aiProcess_OptimizeMeshes |							// 가능한 경우 작은 매쉬를 조인
		aiProcess_GenSmoothNormals |						// 부드러운 노말벡터(법선벡터) 생성
		aiProcess_SplitLargeMeshes |						// 거대한 하나의 매쉬를 하위매쉬들로 분활(나눔)
		aiProcess_Triangulate |								// 3개 이상의 모서리를 가진 다각형 면을 삼각형으로 만듬(나눔)
		aiProcess_ConvertToLeftHanded |						// D3D의 왼손좌표계로 변환
		aiProcess_SortByPType;								// 단일타입의 프리미티브로 구성된 '깨끗한' 매쉬를 만듬

	if (isStatic)
		flag |= aiProcess_PreTransformVertices;				// 버텍스를 미리계산 (no bone & animation flag)

	m_pScene = aiImportFile(fileName.c_str(), flag);

	if (m_pScene) {
		m_meshes.resize(m_pScene->mNumMeshes);
		m_numBones = 0;
		InitScene();
		
		m_ModelMeshes.resize(m_meshes.size());
	}
}

LoadModel::LoadModel(const LoadModel & T)
{
	m_meshes = T.m_meshes;
	m_ModelMeshes = T.m_ModelMeshes;
	m_Bones = T.m_Bones;

	m_numVertices = T.m_numVertices;
	m_numBones = T.m_numBones;
}

inline void CalculateTangentArray(UINT vertexCount, vector<vertexDatas>& vertices, long triangleCount, vector<int>& indeies)
{
	XMFLOAT3 *tan1 = new XMFLOAT3[vertexCount * 2];
	XMFLOAT3 *tan2 = tan1 + vertexCount;
	::ZeroMemory(tan1, vertexCount * sizeof(XMFLOAT3) * 2);

	for (long a = 0; a < triangleCount; a++)
	{
		UINT i1 = indeies[a * 3 + 0];
		UINT i2 = indeies[a * 3 + 1];
		UINT i3 = indeies[a * 3 + 2];

		const XMFLOAT3& v1 = vertices[i1].m_pos;
		const XMFLOAT3& v2 = vertices[i2].m_pos;
		const XMFLOAT3& v3 = vertices[i3].m_pos;

		const XMFLOAT2& w1 = vertices[i1].m_tex;
		const XMFLOAT2& w2 = vertices[i2].m_tex;
		const XMFLOAT2& w3 = vertices[i3].m_tex;

		float x1 = v2.x - v1.x;
		float x2 = v3.x - v1.x;
		float y1 = v2.y - v1.y;
		float y2 = v3.y - v1.y;
		float z1 = v2.z - v1.z;
		float z2 = v3.z - v1.z;

		float s1 = w2.x - w1.x;
		float s2 = w3.x - w1.x;
		float t1 = w2.y - w1.y;
		float t2 = w3.y - w1.y;

		float r = 1.0F / (s1 * t2 - s2 * t1);
		XMFLOAT3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
			(t2 * z1 - t1 * z2) * r);
		XMFLOAT3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
			(s1 * z2 - s2 * z1) * r);

		tan1[i1] = Vector3::Add(tan1[i1], sdir);
		tan1[i2] = Vector3::Add(tan1[i2], sdir);
		tan1[i3] = Vector3::Add(tan1[i3], sdir);

		tan2[i1] = Vector3::Add(tan2[i1], tdir);
		tan2[i2] = Vector3::Add(tan2[i2], tdir);
		tan2[i3] = Vector3::Add(tan2[i3], tdir);

	}

	for (UINT a = 0; a < vertexCount; a++)
	{
		XMFLOAT3& n = vertices[a].m_normal;
		XMFLOAT3& t = tan1[a];

		// Gram-Schmidt orthogonalize
		XMFLOAT3 SP_1 = Vector3::ScalarProduct(n, Vector3::DotProduct(n, t), false);
		XMFLOAT3 SP_2 = Vector3::Subtract(t, SP_1, false);
		vertices[a].m_tan = Vector3::Normalize(SP_2);

		// Calculate handedness
		//tangent[a].w = (Dot(Cross(n, t), tan2[a]) < 0.0F) ? -1.0F : 1.0F;
	}

	delete[] tan1;
}

LoadModel::~LoadModel()
{
	m_meshes.clear();
	delete m_pScene;
}

void LoadModel::InitScene()
{
	for (UINT i = 0; i < m_meshes.size(); ++i) {
		const aiMesh* pMesh = m_pScene->mMeshes[i];
		InitMesh(i, pMesh);
		
		if (pMesh->HasBones()) InitBones(i, pMesh);

		m_numVertices += (UINT)m_meshes[i].m_vertices.size();
	}
	m_numBones = (UINT)m_Bones.size();
}

void LoadModel::InitMesh(UINT index, const aiMesh * pMesh)
{
	m_meshes[index].m_vertices.reserve(pMesh->mNumVertices);
	m_meshes[index].m_indices.reserve(pMesh->mNumFaces * 3);
	//삼각형이므로 면을 이루는 꼭지점 3개

	for (UINT i = 0; i < pMesh->mNumVertices; ++i) {
		XMFLOAT3 zero_3(0.0f, 0.0f, 0.0f);
		XMFLOAT3 pos(0.0f, 0.0f, 0.0f);
		pos.x = pMesh->mVertices[i].x;
		pos.y = pMesh->mVertices[i].y;
		pos.z = pMesh->mVertices[i].z;

		XMFLOAT3 normal(0.0f, 0.0f, 0.0f); 
		normal.x = pMesh->mNormals[i].x;
		normal.y = pMesh->mNormals[i].y;
		normal.z = pMesh->mNormals[i].z;

		XMFLOAT2 tex(0.0f, 0.0f);
		if (pMesh->HasTextureCoords(0)) {
			tex.x = pMesh->mTextureCoords[0][i].x;
			tex.y = pMesh->mTextureCoords[0][i].y;
		}
		else
			tex = XMFLOAT2(0.0f, 0.0f);
		//tangent는 일단 0으로 초기화
		const vertexDatas data(pos, normal, zero_3, tex, index);
		m_meshes[index].m_vertices.push_back(data);
	}

	for (UINT i = 0; i < pMesh->mNumFaces; ++i) {
		const aiFace& face = pMesh->mFaces[i];
		m_meshes[index].m_indices.push_back(face.mIndices[0]);
		m_meshes[index].m_indices.push_back(face.mIndices[1]);
		m_meshes[index].m_indices.push_back(face.mIndices[2]);
	}
	CalculateTangentArray(pMesh->mNumVertices, m_meshes[index].m_vertices, pMesh->mNumFaces, m_meshes[index].m_indices);
}

void LoadModel::SetMeshes(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	for (UINT i = 0; i < m_ModelMeshes.size(); ++i) {
		ModelMesh* tmp = new ModelMesh(pd3dDevice, pd3dCommandList, m_meshes[i]);
		m_ModelMeshes[i] = tmp;
	}	
}

void LoadModel::InitBones(UINT index, const aiMesh* pMesh)
{
	cout << pMesh->mNumBones << endl;

	for (UINT i = 0; i < pMesh->mNumBones; ++i) {
		int BoneIndex = -1;
		const aiBone* pBone = pMesh->mBones[i];

		int tmpIndex = 0;
		for (const auto& p : m_Bones) { //이미 존재하는 뼈인지 검색
			if (p.first == pBone->mName.data) {
				BoneIndex = tmpIndex;
				//현재 뼈가 이미 벡터에 저장된 뼈일 경우
				//인덱스를 해당 뼈의 인덱스로 저장
				break;
			}
			tmpIndex++;
		}

		if (BoneIndex < 0) { //없으면 새로 추가
			BoneIndex = (int)m_Bones.size();
			//새로 저장하는 뼈일 경우 
			//인덱스는 현재 뼈의 개수 (0개일 경우 0부터 시작)

			Bone bone;
			bone.BoneOffset = aiMatrixToXMMatrix(pBone->mOffsetMatrix);
			m_Bones.emplace_back(make_pair(pBone->mName.data, bone));
		}


		for (UINT b = 0; b < pBone->mNumWeights; ++b) {
			UINT vertexID = pBone->mWeights[b].mVertexId;
			float weight = pBone->mWeights[b].mWeight;
			m_meshes[index].m_vertices[vertexID].AddBoneData(BoneIndex, weight);
		}
	}
}