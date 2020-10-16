#include "stdafx.h"
#include "ModelAnimator.h"

ModelAnimator::ModelAnimator(MeshFilter* pMeshFilter):
m_pMeshFilter(pMeshFilter),
m_Transforms(std::vector<DirectX::XMFLOAT4X4>()),
m_IsPlaying(false), 
m_Reversed(false),
m_ClipSet(false),
m_TickCount(0),
m_AnimationSpeed(1.0f)
{
	SetAnimation(0);
}

void ModelAnimator::SetAnimation(UINT clipNumber)
{
	m_ClipSet = false;

	//Check if clipNumber is smaller than the actual m_AnimationClips vector size
	if (clipNumber >= m_pMeshFilter->m_AnimationClips.size())
	{
		Reset();
		Logger::LogWarning(L"ModelAnimator::SetAnimation( ) >> clipNumber >= animationSize! Out of bounds therefore!");
		return;
	}
	else // Good
	{
		SetAnimation(m_pMeshFilter->m_AnimationClips[clipNumber]);
	}
}

void ModelAnimator::SetAnimation(std::wstring clipName)
{
	m_ClipSet = false;

	//Iterate the m_AnimationClips vector and search for an AnimationClip with the given name (clipName)
	auto it = 
		std::find_if(m_pMeshFilter->m_AnimationClips.cbegin(), m_pMeshFilter->m_AnimationClips.cend(), 
		[&clipName](const AnimationClip& clip) 
		{
			return clipName == clip.Name;
		});

	if (it != m_pMeshFilter->m_AnimationClips.cend())
	{
		SetAnimation(*it);
	}
	else
	{
		Reset();
		Logger::LogWarning(L"ModelAnimator::SetAnimation( ) >> Unknown Animation name!");
		return;
	}
}

void ModelAnimator::SetAnimation(AnimationClip clip)
{
	UNREFERENCED_PARAMETER(clip);
	//TODO: complete
	//Set m_ClipSet to true
	m_ClipSet = true;

	//Set m_CurrentClip
	m_CurrentClip = clip;

	//Call Reset(false)
	Reset(false);
}

void ModelAnimator::Reset(bool pause)
{
	//If pause is true, set m_IsPlaying to false
	m_IsPlaying = !pause;

	//Set m_TickCount to zero
	m_TickCount = 0;

	//Set m_AnimationSpeed to 1.0f
	m_AnimationSpeed = 1.f;

	//If m_ClipSet is true
	if (m_ClipSet)
	{
		const std::vector<DirectX::XMFLOAT4X4>& rTransforms = m_CurrentClip.Keys[0].BoneTransforms;
		m_Transforms.assign(rTransforms.cbegin(), rTransforms.cend());
	}
	else
	{
		DirectX::XMMATRIX identity = DirectX::XMMatrixIdentity();
		DirectX::XMFLOAT4X4 idFloats{};
		DirectX::XMStoreFloat4x4(&idFloats, identity);

		m_Transforms.assign(m_pMeshFilter->m_BoneCount, idFloats);
	}
}

void ModelAnimator::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	//We only update the transforms if the animation is running and the clip is set
	if (!m_IsPlaying || !m_ClipSet) return;

	float elapsedSec = gameContext.pGameTime->GetElapsed();
	float durationTicks = m_CurrentClip.Duration;
	float ticksPerSecond = m_CurrentClip.TicksPerSecond;

	//1. 
	//Calculate the passedTicks (see the lab document)
	float passedTicks = elapsedSec * m_AnimationSpeed * ticksPerSecond;
	Clamp(passedTicks, m_CurrentClip.Duration, 0.f);

	//2. 
	//IF m_Reversed is true
	if (!m_Reversed)
	{
		m_TickCount += passedTicks;
		if (m_TickCount > durationTicks)
		{
			m_TickCount -= durationTicks;
			if (!m_Loop) Pause();
		}
	}
	else
	{
		m_TickCount -= passedTicks;
	}

	//3.
	//Find the enclosing keys
	AnimationKey keyA, keyB;
	//Iterate all the keys of the clip and find the following keys:

	keyA = m_CurrentClip.Keys[0];
	keyB = m_CurrentClip.Keys.back();

	for (const AnimationKey& key : m_CurrentClip.Keys)
	{
		if (key.Tick >= keyA.Tick && key.Tick <= m_TickCount) keyA = key;
		if (key.Tick <= keyB.Tick && key.Tick >= m_TickCount) keyB = key;
	}

	//4.
	float blendFactorA = (m_TickCount - keyA.Tick) / (keyB.Tick - keyA.Tick);
	if ((keyB.Tick - keyA.Tick) == 0.f) return;

	//Interpolate between keys
	//Clear the m_Transforms vector
	size_t nrBoneTransforms = m_Transforms.size();
	m_Transforms.clear();

	using namespace DirectX;
	//FOR every boneTransform in a key (So for every bone)
	for (size_t i{}; i < nrBoneTransforms; ++i)
	{
		XMFLOAT4X4 transformA = keyA.BoneTransforms[i];
		XMFLOAT4X4 transformB = keyB.BoneTransforms[i];

		XMMATRIX matA = XMLoadFloat4x4(&transformA);
		XMMATRIX matB = XMLoadFloat4x4(&transformB);

		XMVECTOR scaleA;
		XMVECTOR rotA;
		XMVECTOR transA;
		XMMatrixDecompose(&scaleA, &rotA, &transA, matA);

		XMVECTOR scaleB;
		XMVECTOR rotB;
		XMVECTOR transB;
		XMMatrixDecompose(&scaleB, &rotB, &transB, matB);

		XMVECTOR finalScale = XMVectorLerp(scaleA, scaleB, blendFactorA);
 		XMVECTOR finalRot = XMQuaternionSlerp(rotA, rotB, blendFactorA);
		XMVECTOR finalTrans = XMVectorLerp(transA, transB, blendFactorA);

		XMMATRIX finalMat = XMMatrixAffineTransformation(finalScale, g_XMZero, finalRot, finalTrans);
		
		XMFLOAT4X4 finalFloats;
		XMStoreFloat4x4(&finalFloats, finalMat);

		m_Transforms.push_back(finalFloats);
	}
}
