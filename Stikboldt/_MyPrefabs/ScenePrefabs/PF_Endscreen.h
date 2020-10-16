#pragma once
#include <GameObject.h>

class SpriteComponent;

class PF_Endscreen final : public GameObject
{
public:
	void Start(size_t playerID);
	bool HasStarted() const;
	void Reset()
	{
		m_HasStarted = false;
		m_ToMainMenuTimer = 10.f;
	}

private:
	virtual void Initialize(const GameContext&) override;
	virtual void Update(const GameContext&) override;
	virtual void Draw(const GameContext&) override;

	bool m_HasStarted;
	size_t m_PlayerID;
	SpriteComponent* m_pWinnerFrame = nullptr;

	float m_ToMainMenuTimer = 6.f;
};

