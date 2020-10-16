#pragma once
#include <GameObject.h>

class PF_Ground;

class PF_WorkshopEnviroment final : public GameObject
{
public:
	PF_WorkshopEnviroment();
	virtual ~PF_WorkshopEnviroment() = default;

	PF_Ground* GetGround() const { return m_pGround; }

private:
	// GameObject overrides:
	virtual void Initialize(const GameContext&) override;
	virtual void Update(const GameContext&) override;
	virtual void Draw(const GameContext&) override;

	// Data:
	PF_Ground* m_pGround;

public:
	PF_WorkshopEnviroment(const PF_WorkshopEnviroment&) = delete;
	PF_WorkshopEnviroment(PF_WorkshopEnviroment&&) = delete;
	PF_WorkshopEnviroment& operator=(const PF_WorkshopEnviroment&) = delete;
	PF_WorkshopEnviroment& operator=(PF_WorkshopEnviroment&&) = delete;
};

