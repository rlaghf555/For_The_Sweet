#include "stdafx.h"
#include "Model.h"
#include "Animation.h"

LoadAnimation::LoadAnimation(string filename, float trigger, float skip) :
	animation_loop(true), next_index(0), m_animSpeed(1.0f)
{
	m_pScene = aiImportFile(filename.c_str(), (aiProcessPreset_TargetRealtime_Quality | aiProcess_ConvertToLeftHanded) & ~aiProcess_FindInvalidData);

	if (m_pScene) {
		m_pAnim = m_pScene->mAnimations[0]; //단일 애니메이션만 사용하는 경우 0번 인덱스
		//m_GlobalInverse = aiMatrixToXMMatrix(m_pScene->mRootNode->mTransformation);
		m_GlobalInverse = XMMatrixIdentity();

		start_time = (float)m_pAnim->mChannels[0]->mPositionKeys[0].mTime;
		//프레임 시작 시점은 좌표 이동 프레임을 기준으로 맞춤
		end_time = (float)m_pAnim->mChannels[0]->mPositionKeys[m_pAnim->mChannels[0]->mNumPositionKeys - 1].mTime - 1.0f;
		//프레임 종료 시점에서 1.0 만큼 빼줘야 프레임이 안겹침

		//cout << filename << " start Time : " << start_time << endl;
		//cout << filename << " end Time : " << end_time << endl;

		if (IsZero(trigger))
			trigger_time = (end_time - start_time) / 2 + start_time;
		else
			trigger_time = trigger + start_time;

		if (IsZero(skip))
			posible_skip = end_time;
		else
			posible_skip = skip + start_time;

		now_time = start_time;

		m_animSpeed = (end_time - start_time) / m_pAnim->mChannels[0]->mNumPositionKeys;
		m_animSpeed = m_pAnim->mTicksPerSecond;
		//cout << m_animSpeed << endl;
	}
}

LoadAnimation::LoadAnimation(const LoadAnimation & T)
{
	m_pScene = T.m_pScene;
	m_pAnim = T.m_pAnim;
	m_GlobalInverse = XMMatrixIdentity();
	start_time = T.start_time;
	end_time = T.end_time;
	now_time = T.now_time;
	trigger_time = T.trigger_time;
	posible_skip = T.posible_skip;
	m_animSpeed = T.m_animSpeed;
}

UINT LoadAnimation::BoneTransform(UINT& index, float fTime, vector<XMFLOAT4X4>& transforms)
{
	if (stop_anim)
		return LOOP_STOP;
	XMMATRIX Identity = XMMatrixIdentity();

	if (!m_pScene) {
		//애니메이션 파일을 로드못 했을 경우 수행
		for (UINT i = 0; i < m_NumBones; ++i) {
			XMStoreFloat4x4(&transforms[i], Identity);
		}
		return LOOP_IN;
	}

	//루트노드부터 계층구조를 훝어가며 변환 수행 및 뼈에 최종변환 계산
	ReadNodeHeirarchy(now_time, m_pScene->mRootNode, Identity);

	for (int i = 0; i < m_NumBones; ++i) {
		//뼈의 최종변환을 반환
		XMStoreFloat4x4(&transforms[i], m_Bones[i].second.FinalTransformation);

		if (m_Bones[i].first == "Bip001 L Hand") {
			//XMVECTOR tmp = p.second.BoneOffset.r[3];
			//cout << "Left Hand BondeOffset: " << tmp.m128_f32[0] << "," << tmp.m128_f32[1] << "," << tmp.m128_f32[2] << endl;
			//
			//tmp = p.second.FinalTransformation.r[3];
			//cout << "Left Hand FinalTransformation: " << tmp.m128_f32[0] << "," << tmp.m128_f32[1] << "," << tmp.m128_f32[2] << endl;

			XMMATRIX tmp[2];
			tmp[0] = m_Bones[14].second.FinalTransformation;
			tmp[1] = m_Bones[16].second.FinalTransformation;

			XMFLOAT4 pos = XMFLOAT4(0, 0, 0, 1);
			float weights[2] = {0.25f, 0.75f};

			//for (int j = 0; j < 4; j++) {
			//	weights[j] = 0.25f;
			//}
			
		//	XMFLOAT4 temp = XMFLOAT4(33.7235756, 38.8664284, -3.53839922, 1);
			XMFLOAT4 temp = XMFLOAT4(31.7235756, 35.8664284, -3.53839922, 1);
			XMFLOAT4 result;
			for (int j = 0; j < 2; j++)
			{
				result = Matrix4x4::test(temp, tmp[j]);
				pos.x += weights[j] * result.x;
				pos.y += weights[j] * result.y;
				pos.z += weights[j] * result.z;		
			}
			//cout << "Left Hand Pos : " << pos.x << "," << pos.y << "," << pos.z << endl;

			//tmp2._41 = pos.x;
			//tmp2._42 = pos.y;
			//tmp2._43 = pos.z;

			//pos = Matrix4x4::test(pos, WORLDMATRIX);

			m_handpos = XMFLOAT4(pos.x, pos.y, pos.z, 1);

			int k = 0;
		}
	}

	// 미리 정해진 프레임 내에서 애니메이션 수행
	if (index != Anim_Guard)
		now_time += m_animSpeed * fTime;
	if (now_time > end_time) {
		now_time = start_time;
		if (!animation_loop) {
			index = Anim_Idle;
			animation_loop = true;
		}
		return LOOP_END; //애니메이션이 한 루프 끝남
	}

	//cout << m_animSpeed << ", " << fTime << endl;
	//cout << "현재 : " << now_time << ", " << "총 시간 : " << end_time << endl;

	if (now_time > trigger_time - 1 && now_time < trigger_time + 1) {
		return LOOP_TRIGGER;
	}

	if (now_time > posible_skip)
		return LOOP_SKIP; //애니메이션 아직 실행중이고 트리거 실행 후후반부

	return LOOP_IN; //애니메이션이 아직 실행중
}

void LoadAnimation::ReadNodeHeirarchy(float AnimationTime, const aiNode * pNode, const XMMATRIX& ParentTransform)
{
	const aiAnimation* pAnim = m_pAnim;

	XMMATRIX NodeTransformation = aiMatrixToXMMatrix(pNode->mTransformation);

	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnim, pNode->mName.data);
	//현재 노드가 animation channel 에 속한지 확인
	if (pNodeAnim) {
		aiVector3D s;
		CalcInterpolatedScaling(s, AnimationTime, pNodeAnim);
		XMMATRIX ScalingM = XMMatrixScaling(s.x, s.y, s.z);


		aiQuaternion q;
		CalcInterpolatedRotation(q, AnimationTime, pNodeAnim);
		XMMATRIX RotationM = XMMatrixRotationQuaternion(XMVectorSet(q.x, q.y, q.z, q.w));


		aiVector3D t;
		CalcInterpolatedPosition(t, AnimationTime, pNodeAnim);
		XMMATRIX TranslationM = XMMatrixTranslation(t.x, t.y, t.z);


		NodeTransformation = ScalingM * RotationM * TranslationM; //스케일 * 회전 * 이동 변환
		NodeTransformation = XMMatrixTranspose(NodeTransformation);
		//transpos 안시켜주면 모델 깨짐
	}
	//부모노드에 변환값 중첩해서 곱하기
	XMMATRIX GlobalTransformation = ParentTransform * NodeTransformation;


	//현재노드가 뼈 노드이면 변환정보를 뼈에 적용
	for (auto& p : m_Bones) {
		if (p.first == pNode->mName.data) {
			p.second.FinalTransformation =
				m_GlobalInverse * GlobalTransformation * p.second.BoneOffset;

			/*if (p.first == "Bip001 L Hand") {
				XMVECTOR tmp = p.second.BoneOffset.r[3];
				cout << "Left Hand BondeOffset: " << tmp.m128_f32[0] << "," << tmp.m128_f32[1] << "," << tmp.m128_f32[2] << endl;

				tmp = p.second.FinalTransformation.r[3];
				cout << "Left Hand FinalTransformation: " << tmp.m128_f32[0] << "," << tmp.m128_f32[1] << "," << tmp.m128_f32[2] << endl;
			}*/

			break;
		}
	}

	for (UINT i = 0; i < pNode->mNumChildren; ++i) {
		//계층구조를 이룸. 자식노드 탐색 및 변환
		ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
	}
}

const aiNodeAnim * LoadAnimation::FindNodeAnim(const aiAnimation * pAnimation, const string& NodeName)
{
	for (UINT i = 0; i < pAnimation->mNumChannels; ++i) {
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (pNodeAnim->mNodeName.data == NodeName)
			return pNodeAnim;
	}
	return nullptr;
}

void LoadAnimation::CalcInterpolatedScaling(aiVector3D & Out, float AnimationTime, const aiNodeAnim * pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1) {
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	UINT ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	UINT NextScalingIndex = ScalingIndex + 1;

	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);

	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;

	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

void LoadAnimation::CalcInterpolatedRotation(aiQuaternion & Out, float AnimationTime, const aiNodeAnim * pNodeAnim)
{
	if (pNodeAnim->mNumRotationKeys == 1) {
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	UINT RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	UINT NextRotationIndex = (RotationIndex + 1);

	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);

	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;

	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}

void LoadAnimation::CalcInterpolatedPosition(aiVector3D & Out, float AnimationTime, const aiNodeAnim * pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1) {
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	UINT PositionIndex = FindPosition(AnimationTime, pNodeAnim);
	UINT NextPositionIndex = (PositionIndex + 1);


	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);


	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;

	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

UINT LoadAnimation::FindScaling(float AnimationTime, const aiNodeAnim * pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (UINT i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}
	return 0;
}

UINT LoadAnimation::FindRotation(float AnimationTime, const aiNodeAnim * pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (UINT i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}
	return 0;
}

UINT LoadAnimation::FindPosition(float AnimationTime, const aiNodeAnim * pNodeAnim)
{
	for (UINT i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}
	return 0;
}

void LoadAnimation::SetAnimFrame(float frame)
{
	if (frame >= end_time)
		frame = end_time;
	now_time = frame;
}
