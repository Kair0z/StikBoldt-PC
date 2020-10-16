#pragma once
#include <GameObject.h>
#include "../../_MyHelpers/MyUtilities.h"

class SpriteComponent;
using namespace DirectX;

class PF_PlayerPawn : public GameObject
{
	enum class Input_PlayerPawn
	{
		A,
		B
	};

	enum class State
	{
		Idle,
		Active,
		Pinned
	};

public:
	PF_PlayerPawn(size_t playerID);
	virtual ~PF_PlayerPawn() = default;

	void ResetState();

	void Initialize(const GameContext&);
	void Update(const GameContext&);

	SpriteComponent* GetSprite();

	void SetEnterGameHitbox(const MyUtils::Circle2D& hitbox);
	void SetExitGameHitbox(const MyUtils::Circle2D& hitbox);

	bool PlayRequest() const;
	bool ExitRequest() const;
	bool IsPinned() const;

private:
	std::wstring m_DefaultTexture = L"Resources/Textures/UI/tex_Playerpawn_";
	std::wstring m_PinnedTexture = L"Resources/Textures/UI/tex_Playerpawn_Registered_";
	std::wstring m_ActiveTexture = L"Resources/Textures/UI/tex_Playerpawn_Active_";
	SpriteComponent* m_pSprite;
	size_t m_PlayerID;

	MyUtils::Circle2D m_GameEnterHitbox{};
	MyUtils::Circle2D m_GameExitHitbox{};

	XMFLOAT2 m_Velocity;
	float m_Speed;

	void InitInput(const GameContext&);

	void ProcessState(const GameContext&);
	void ProcessInput(const GameContext&);
	void TryPin(bool doPin);
	void Move(const GameContext&);

	State m_State;

	bool m_PlayRequest;
	bool m_ExitRequest;

	bool IsInRange(MyUtils::Circle2D hitbox) const;
};

