#include "stdafx.h"
#include "PF_GymEnviroment.h"

#include "../OverlordProject/_MyPrefabs/MyPrefabs.h"

#include "ModelComponent.h"
#include "TransformComponent.h"

#include "MaterialManager.h"
#include "../OverlordProject/_Materials/Materials.h"

#include "Prefabs.h"
#include "RigidBodyComponent.h"
#include "ColliderComponent.h"
#include "ContentManager.h"
#include "PhysxManager.h"

PF_GymEnviroment::PF_GymEnviroment()
	: m_pGround{nullptr}
	, m_Enviroment{}
	, m_pScoreboard{nullptr}
{

}

void PF_GymEnviroment::Reset()
{
	if (m_pScoreboard) m_pScoreboard->Reset();
}

void PF_GymEnviroment::Initialize(const GameContext& gCtx)
{
	m_pGround = new PF_Ground{};
	AddChild(m_pGround);

	InitMeshes(gCtx);
	InitCollision(gCtx);
	InitScoreboard(gCtx);
}

#pragma region Initialize
void PF_GymEnviroment::InitMeshes(const GameContext&)
{
#pragma region PlayfieldMesh
	GameObject* pGO = new GameObject{};
	pGO->GetTransform()->Rotate(90.f, 0.f, 0.f);

	m_pPlayfield = new ModelComponent(L"Resources/Models/Playfield.ovm");
	m_pPlayfield->SetMaterial((int)ActiveMaterials::Playfield);
	
	pGO->AddComponent(m_pPlayfield);
	pGO->GetTransform()->Scale(0.1f, 0.1f, 0.1f);
	AddChild(pGO);
#pragma endregion

#pragma region EnviromentMesh
	GameObject* pGO2 = new GameObject{};
	GameObject* pSecondFloor = new GameObject{};

	m_Enviroment.m_pBackWall = new ModelComponent{ L"Resources/Models/Enviroment_BackWall.ovm" };
	m_Enviroment.m_pFloor = new ModelComponent{ L"Resources/Models/Enviroment_Floor.ovm" };
	ModelComponent* pFloor = new ModelComponent{ L"Resources/Models/Enviroment_Floor.ovm" };

	m_Enviroment.m_pLeftWall = new ModelComponent{ L"Resources/Models/Enviroment_LeftWall.ovm" };
	m_Enviroment.m_pRightWall = new ModelComponent{ L"Resources/Models/Enviroment_RightWall.ovm" };

	m_Enviroment.m_pBackWall->SetMaterial((int)ActiveMaterials::EnviromentGradient);
	m_Enviroment.m_pFloor->SetMaterial((int)ActiveMaterials::EnviromentFloor);
	m_Enviroment.m_pLeftWall->SetMaterial((int)ActiveMaterials::EnviromentGradient);
	m_Enviroment.m_pRightWall->SetMaterial((int)ActiveMaterials::EnviromentGradient);
	pFloor->SetMaterial((int)ActiveMaterials::EnviromentFloor);

	pGO2->AddComponent(m_Enviroment.m_pBackWall);
	pGO2->AddComponent(m_Enviroment.m_pFloor);
	pGO2->AddComponent(m_Enviroment.m_pLeftWall);
	pGO2->AddComponent(m_Enviroment.m_pRightWall);
	pSecondFloor->AddComponent(pFloor);

	pGO2->GetTransform()->Scale(0.02f, 0.02f, 0.02f);
	pGO2->GetTransform()->Rotate(90.f, 0.f, 0.f);
	pGO2->GetTransform()->Translate(0.f, -1.f, 0.f);

	pSecondFloor->GetTransform()->Scale(0.02f, 0.02f, 0.02f);
	pSecondFloor->GetTransform()->Rotate(90.f, 0.f, 0.f);
	pSecondFloor->GetTransform()->Translate(0.f, -1.f, -160.f);
	AddChild(pSecondFloor);
	AddChild(pGO2);
#pragma endregion

#pragma region PlintMesh
	GameObject* pGO3 = new GameObject{};
	m_pPlint = new ModelComponent{ L"Resources/Models/Plint.ovm" };
	m_pPlint->SetMaterial((int)ActiveMaterials::EnviromentGradient);
	pGO3->AddComponent(m_pPlint);
	pGO3->GetTransform()->Scale(0.02f, 0.02f, 0.02f);
	pGO3->GetTransform()->Rotate(90.f, -45.f, 0.f);
	pGO3->GetTransform()->Translate(60.f, -1.f, -40.f);
	AddChild(pGO3);
#pragma endregion

#pragma region FundamentsMesh
	GameObject* pGO4 = new GameObject{};
	ModelComponent* pFundaments = new ModelComponent{ L"Resources/Models/Fundaments.ovm" };
	pFundaments->SetMaterial((int)ActiveMaterials::EnviromentGradient);
	pGO4->AddComponent(pFundaments);
	pGO4->GetTransform()->Scale(0.05f, 0.05f, 0.05f);
	pGO4->GetTransform()->Rotate(0.f, 0.f, 0.f);
	pGO4->GetTransform()->Translate(75.f, 30.f, -70.f);
	AddChild(pGO4);

#pragma endregion

#pragma region TribuneMesh
	GameObject* pGO5 = new GameObject{};
	ModelComponent* pTribune = new ModelComponent{ L"Resources/Models/Tribune.ovm" };
	pTribune->SetMaterial((int)ActiveMaterials::UberMatte);
	pGO5->AddComponent(pTribune);
	float scale = 6.5f;
	pGO5->GetTransform()->Scale(scale, scale, scale);
	pGO5->GetTransform()->Rotate(90.f, 0.f, 0.f);
	pGO5->GetTransform()->Translate(-5.f , 0.f, -5.f);
	AddChild(pGO5);
#pragma endregion
}

void PF_GymEnviroment::InitScoreboard(const GameContext&)
{
	m_pScoreboard = new PF_Scoreboard{};

	AddChild(m_pScoreboard);

	m_pScoreboard->GetTransform()->Rotate(0.f, -55.f, 0.f);
	m_pScoreboard->GetTransform()->Translate(-50.f, 8.f, 75.f);
	m_pScoreboard->GetTransform()->Scale(1.2f, 1.2f, 1.2f);

	ScoreboardLocator::Provide(m_pScoreboard);
}

void PF_GymEnviroment::InitCollision(const GameContext&)
{
	using namespace physx;
	auto triangleMesh = ContentManager::Load<PxTriangleMesh>(L"Resources/Models/Hitboxes/Hitbox_Playfield.ovpt");
	std::shared_ptr<PxGeometry> wallGeometry(new PxTriangleMeshGeometry{ triangleMesh, PxMeshScale(7.5f) });

	PxTransform rot = PxTransform{ PxQuat(physx::PxHalfPi, {1.f, 0.f, 0.f}) };
	PxTransform pos = PxTransform{ 0.f, 0.f, 70.f };

	m_pPlayfieldWalls = new ColliderComponent
	{
		wallGeometry,
		*PhysxManager::GetInstance()->GetPhysics()->createMaterial(0.0f, 0.0f, 1.f),
		rot * pos
	};

	m_pRigidBody = new RigidBodyComponent{ true };
	AddComponent(m_pRigidBody);
	AddComponent(m_pPlayfieldWalls);
}
#pragma endregion

void PF_GymEnviroment::Update(const GameContext&)
{

}

void PF_GymEnviroment::Draw(const GameContext&)
{

}