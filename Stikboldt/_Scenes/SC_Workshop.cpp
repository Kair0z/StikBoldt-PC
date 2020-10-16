#include "stdafx.h"
#include "SC_Workshop.h"

// Prefabs include:
#include "../OverlordProject/_MyPrefabs/MyPrefabs.h"

// physx includes:
#include "PhysxProxy.h"

// component includes:
#include "TransformComponent.h"

SC_Workshop::SC_Workshop()
	: GameScene(L"Workshop")
	, m_pEnviroment{nullptr}
	, m_pPlayer0{nullptr}
{}

void SC_Workshop::Initialize()
{
	GetPhysxProxy()->EnablePhysxDebugRendering(true);
	
	InitPlayer();
	InitEnviroment();
}

#pragma region Initialize
void SC_Workshop::InitPlayer()
{
	m_pPlayer0 = new PF_RobotCharacter{};
	m_pPlayer0->GetTransform()->Rotate(0.f, 180.f, 0.f);
	AddChild(m_pPlayer0);
}

void SC_Workshop::InitEnviroment()
{
	m_pEnviroment = new PF_WorkshopEnviroment{};
	AddChild(m_pEnviroment);
}
#pragma endregion

void SC_Workshop::Update()
{
	
}

#pragma region Update
#pragma endregion

void SC_Workshop::Draw()
{

}

#pragma region Draw
#pragma endregion