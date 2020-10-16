#include "stdafx.h"
#include "PF_Ground.h"

// Gameobject includes:
#include "GameObject.h"
#include "RigidBodyComponent.h"
#include "ColliderComponent.h"

// Physx includes:
#include "PhysxManager.h"

PF_Ground::PF_Ground(float groundHeight)
	: m_Height{groundHeight}
{
	Initialize();
}

float PF_Ground::GetHeight() const
{
	return m_Height;
}

void PF_Ground::Initialize()
{
	AddComponent(new RigidBodyComponent(true));

	// PHYSX:
	physx::PxPhysics* pPhysx = PhysxManager::GetInstance()->GetPhysics();
	physx::PxMaterial* pMat = pPhysx->createMaterial(0.0f, 0.0f, 1.0f);

	std::shared_ptr<physx::PxGeometry> geometry(new physx::PxPlaneGeometry());
	AddComponent(new ColliderComponent(geometry, *pMat, physx::PxTransform(physx::PxQuat(DirectX::XM_PIDIV2, physx::PxVec3(0, 0, 1)))));
}
