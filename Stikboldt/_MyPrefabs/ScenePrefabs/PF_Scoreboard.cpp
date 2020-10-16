#include "stdafx.h"
#include "PF_Scoreboard.h"
#include "ModelComponent.h"
#include "TransformComponent.h"
#include "ModelAnimator.h"

#include "../OverlordProject/_MyPrefabs/MyPrefabs.h"
#include "CubePrefab.h"
#include "../OverlordProject/_Materials/Materials.h"

PF_Scoreboard::PF_Scoreboard()
	: m_pPlanks{nullptr, nullptr, nullptr, nullptr}
	, m_PlayersAlive{true, true, true, true}
	, m_pBoard{nullptr}
	, m_Scores{0, 0, 0, 0}
	, m_pWinner{nullptr}
{
	std::cout << "Scoreboard make! \n";
}

void PF_Scoreboard::AddScore(PF_RobotCharacter* pWinner)
{
	size_t playerID = pWinner->GetPlayerID();

	if (!m_PlayersAlive[playerID]) return; // if this player isn't alive anymore then he cannot recieve any more points

	if (m_Scores[playerID] == 9 && !m_pWinner)
	{
		m_pWinner = pWinner;
		return; // FULL SCORE!
	}

	++m_Scores[playerID];
	ModelComponent* pModel = m_pPlanks[playerID]->GetComponent<ModelComponent>();
	if (!pModel) return;

	// Set material to the pallete material + score
	pModel->SetMaterial((int)ActiveMaterials::Planks0 + m_Scores[playerID]);
}

PF_RobotCharacter* PF_Scoreboard::GetWinner() const
{
	return m_pWinner;
}

void PF_Scoreboard::RegisterOut(PF_RobotCharacter* pOut)
{
	size_t playerID = pOut->GetPlayerID();

	RegisterOut(playerID);
}

void PF_Scoreboard::RegisterOut(size_t out)
{
	m_Scores[out] = 0;
	m_PlayersAlive[out] = false;

	ModelComponent* pModel = m_pPlanks[out]->GetComponent<ModelComponent>();
	if (!pModel) return;

	pModel->SetMaterial((int)ActiveMaterials::PlanksOut);
}

void PF_Scoreboard::Reset()
{
	m_pWinner = nullptr;

	// reset plank textures
	for (GameObject* pPlank : m_pPlanks)
	{
		ModelComponent* pModel = pPlank->GetComponent<ModelComponent>();
		if (!pModel) continue;

		pModel->SetMaterial((int)ActiveMaterials::Planks0);
	}

	// reset scores:
	for (size_t& score : m_Scores)
	{
		score = 0;
	}

	// reset alive players:
	for (bool& alive : m_PlayersAlive)
	{
		alive = true;
	}
}

void PF_Scoreboard::Initialize(const GameContext& context)
{
	InitMeshes(context);
}

#pragma region Init

void PF_Scoreboard::InitMeshes(const GameContext&)
{
	using namespace DirectX;
	XMFLOAT3 offset = {-57.f, 15.f, -8.f};
	XMFLOAT3 cubeOffset = {0.f, 5.f, 0.f};

	// Plank meshes:
	for (size_t i{}; i < 4; ++i)
	{
		GameObject*& pPlank = m_pPlanks[i];
		CubePrefab*& pCube = m_pColorCubes[i];
		XMFLOAT3 perPlankOffset = { i * 16.f, i * 0.f, i * 0.f };

		pPlank = new GameObject{};
		pCube = new CubePrefab{ 2.3f, 2.3f, 2.3f, ActiveColors::GetPlayerColor(i) };

		ModelComponent* pModel = new ModelComponent(L"Resources/Models/Scoreplank.ovm", true);
		pModel->SetMaterial((int)ActiveMaterials::Planks0);
		pPlank->AddComponent(pModel);
		float scaler = 4.7f;
		pPlank->GetTransform()->Scale(scaler, scaler, scaler);
		XMFLOAT3 trans = { offset.x + perPlankOffset.x, offset.y + perPlankOffset.y, offset.z + perPlankOffset.z };
		pPlank->GetTransform()->Translate(trans);
		pCube->GetTransform()->Translate(trans.x + cubeOffset.x, trans.y + cubeOffset.y, trans.z + cubeOffset.z);
		AddChild(pPlank);
		AddChild(pCube);
	}

	// BoardMesh:
	m_pBoard = new GameObject{};
	ModelComponent* pBoardMesh = new ModelComponent{ L"Resources/Models/Scoreboard.ovm" };
	pBoardMesh->SetMaterial((int)ActiveMaterials::Scoreboard);
	m_pBoard->AddComponent(pBoardMesh);

	float scale = 0.025f;
	m_pBoard->GetTransform()->Scale(scale, scale, scale);
	m_pBoard->GetTransform()->Rotate(-90.f, 0.f, 0.f);
	m_pBoard->GetTransform()->Translate(0.f, 0.f, 0.f);
	AddChild(m_pBoard);
}

#pragma endregion

void PF_Scoreboard::Update(const GameContext& context)
{
	context.pCamera;

	for (GameObject* pPlank : m_pPlanks)
	{
		ModelComponent* pModel = pPlank->GetComponent<ModelComponent>();
		if (!pModel) continue;

		ModelAnimator* pAnimator = pModel->GetAnimator();
		if (!pAnimator) continue;

		pAnimator->SetAnimation(L"Plank_Change");
		pAnimator->SetAnimationSpeed(2.f);
		pAnimator->Play();
	}
}

void PF_Scoreboard::Draw(const GameContext&)
{
}
