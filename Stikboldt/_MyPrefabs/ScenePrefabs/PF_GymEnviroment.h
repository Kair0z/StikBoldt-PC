#pragma once
#include <GameObject.h>
#include <vector>

class PF_Ground;
class ModelComponent;
class CubePrefab;
class ColliderComponent;
class RigidBodyComponent;
class PF_Scoreboard;

// Prefab of the static enviroment of the gymscene
class PF_GymEnviroment final: public GameObject
{
	struct EnviromentMesh
	{
		ModelComponent* m_pFloor = nullptr;
		ModelComponent* m_pLeftWall = nullptr;
		ModelComponent* m_pRightWall = nullptr;
		ModelComponent* m_pBackWall = nullptr;
	};

public:
	PF_GymEnviroment();
	virtual ~PF_GymEnviroment() = default;

	PF_Ground* GetGround() const { return m_pGround; }

	void Reset();

private:
	// GameObject overrides:
	virtual void Initialize(const GameContext&) override;
	virtual void Update(const GameContext&) override;
	virtual void Draw(const GameContext&) override;

	void InitMeshes(const GameContext& gCtx);
	void InitScoreboard(const GameContext&);
	void InitCollision(const GameContext& gCtx);

	// Scoreboard:
	PF_Scoreboard* m_pScoreboard;

	// Models:
	ModelComponent* m_pPlayfield;
	ModelComponent* m_pPlint;
	EnviromentMesh m_Enviroment;
	
	RigidBodyComponent* m_pRigidBody;
	ColliderComponent* m_pPlayfieldWalls;

	// The ground:
	PF_Ground* m_pGround;

public:
	PF_GymEnviroment(const PF_GymEnviroment&) = delete;
	PF_GymEnviroment(PF_GymEnviroment&&) = delete;
	PF_GymEnviroment& operator=(const PF_GymEnviroment&) = delete;
	PF_GymEnviroment& operator=(PF_GymEnviroment&&) = delete;
};

