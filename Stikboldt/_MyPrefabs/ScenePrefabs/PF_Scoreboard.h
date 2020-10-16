#pragma once
#include <GameObject.h>
#include <array>
#include "../OverlordProject/_MyHelpers/ILocator.h"

class PF_RobotCharacter;
class ModelComponent;
class TextureDiffuseMaterial;
class CubePrefab;

class PF_Scoreboard final : public GameObject
{
public:
	PF_Scoreboard();
	virtual ~PF_Scoreboard() = default;

	void AddScore(PF_RobotCharacter* pWinner);
	PF_RobotCharacter* GetWinner() const;

	void RegisterOut(PF_RobotCharacter* pOut);
	void RegisterOut(size_t out);

	void Reset();
	
private:
	// GameObject overrides:
	virtual void Initialize(const GameContext&) override;
		void InitMeshes(const GameContext&);

	virtual void Update(const GameContext&) override;
	virtual void Draw(const GameContext&) override;

	GameObject* m_pBoard;
	PF_RobotCharacter* m_pWinner;
	std::array<GameObject*, 4> m_pPlanks;
	std::array<CubePrefab*, 4> m_pColorCubes;
	std::array<size_t, 4> m_Scores;
	std::array<bool, 4> m_PlayersAlive;
};

class ScoreboardLocator : public ILocator<PF_Scoreboard>{};

