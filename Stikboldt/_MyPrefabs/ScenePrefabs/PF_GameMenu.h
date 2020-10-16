#pragma once
#include <GameObject.h>
#include "../../_MyHelpers/ILocator.h"
#include "../../_MyHelpers/MyUtilities.h"

class PF_PlayerPawn;
struct MyUtils::CircleCollider;
class SpriteComponent;

class PF_GameMenu final : public GameObject
{
	using Spawn2D = DirectX::XMFLOAT2;

public:
	PF_GameMenu();
	virtual ~PF_GameMenu();

	bool IsActive() const;
	void Activate(bool active);

private:
	virtual void Initialize(const GameContext&) override;
	void InitPawns(const GameContext&);
	void PostInit();
	bool m_HasPostInit = false;

	virtual void Update(const GameContext&) override;
	void CheckButtons();
	virtual void Draw(const GameContext&) override;

	void Hide(bool hide);
	void ResetPositions();

	bool m_IsActive = false;

	std::vector<PF_PlayerPawn*> m_pPawns;
	std::vector<Spawn2D> m_Spawns;

	SpriteComponent* m_pBackground;

	MyUtils::CircleCollider* m_pToMainMenuCollider;
	MyUtils::CircleCollider* m_pRestartCollider;
};

class GameMenuLocator : public ILocator<PF_GameMenu>{};
