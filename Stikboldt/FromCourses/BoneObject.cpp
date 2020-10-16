#include "stdafx.h"
#include "BoneObject.h"

// Component includes:
#include "ModelComponent.h"
#include "TransformComponent.h"

BoneObject::BoneObject(const int ID, const int materialID, const float length)
	: m_BoneID{ID}
	, m_MaterialID{materialID}
	, m_Length{length}
	, m_BindPose{}
	, m_pThisBone{nullptr}
{
	
}

void BoneObject::AddBone(BoneObject* pBone)
{
	// Because of the way the mesh is made:
	// Translate by the length
	pBone->GetTransform()->Translate(m_Length, 0.f, 0.f);

	AddChild(pBone);
}

DirectX::XMFLOAT4X4 BoneObject::GetBindPose() const
{
	return m_BindPose;
}

void BoneObject::CalculateBindPose()
{
	using namespace DirectX;

	XMMATRIX world = XMLoadFloat4x4(&GameObject::GetTransform()->GetWorld());;
	XMMATRIX bindPose = XMMatrixInverse(nullptr, world);

	for (BoneObject* pChildBone : GetChildren<BoneObject>())
	{
		pChildBone->CalculateBindPose();
	}
	
	XMStoreFloat4x4(&m_BindPose, bindPose);
}

void BoneObject::Initialize(const GameContext&)
{
	ModelComponent* pModelComp = new ModelComponent{ L"Resources/Meshes/Bone.ovm" };
	pModelComp->SetMaterial(m_MaterialID);
	
	m_pThisBone = new GameObject{};
	m_pThisBone->AddComponent(pModelComp);

	m_pThisBone->GetTransform()->Rotate(0.f, -90.f, 0.f, true);
	m_pThisBone->GetTransform()->Scale(m_Length, m_Length, m_Length);

	AddChild(m_pThisBone);
}

float BoneObject::GetLength() const
{
	return m_Length;
}

void BoneObject::SetLength(const float newLength)
{
	m_Length = newLength;
}
