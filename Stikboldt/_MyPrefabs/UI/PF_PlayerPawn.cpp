#include "stdafx.h"
#include "PF_PlayerPawn.h"
#include "SpriteComponent.h"

#define InputID(playerID) (42 + (playerID + 1) * 2)

PF_PlayerPawn::PF_PlayerPawn(size_t playerID)
	: m_pSprite{nullptr}
	, m_PlayerID{playerID}
	, m_Velocity{}
	, m_Speed{450.f}
	, m_PlayRequest{false}
	, m_State{State::Idle}
	, m_ExitRequest{false}
{

}

void PF_PlayerPawn::ResetState()
{
	m_State = State::Idle;
}

void PF_PlayerPawn::Initialize(const GameContext& context)
{
	m_pSprite = new SpriteComponent{ m_DefaultTexture + std::to_wstring(m_PlayerID) + L".png", {} };
	m_pSprite->SetDepth(0.4f - 0.05f * m_PlayerID);
	AddComponent(m_pSprite);

	InitInput(context);
}

void PF_PlayerPawn::InitInput(const GameContext& context)
{
	InputManager* pInput = context.pInput;

	pInput->AddInputAction(InputAction(
		InputID(m_PlayerID) + (int)Input_PlayerPawn::A, InputTriggerState::Pressed, -1, -1, XINPUT_GAMEPAD_A, (GamepadIndex)m_PlayerID ));

	pInput->AddInputAction(InputAction(
		InputID(m_PlayerID) + (int)Input_PlayerPawn::B, InputTriggerState::Pressed, -1, -1, XINPUT_GAMEPAD_B, (GamepadIndex)m_PlayerID ));
}

void PF_PlayerPawn::Update(const GameContext& context)
{
	ProcessInput(context);
	ProcessState(context);
	if (m_State != State::Pinned) Move(context);
}

SpriteComponent* PF_PlayerPawn::GetSprite()
{
	return m_pSprite;
}

void PF_PlayerPawn::SetEnterGameHitbox(const MyUtils::Circle2D& hitbox)
{
	m_GameEnterHitbox = hitbox;
}

void PF_PlayerPawn::SetExitGameHitbox(const MyUtils::Circle2D& hitbox)
{
	m_GameExitHitbox = hitbox;
}

bool PF_PlayerPawn::PlayRequest() const
{
	return m_PlayRequest;
}

bool PF_PlayerPawn::ExitRequest() const
{
	return m_ExitRequest;
}

bool PF_PlayerPawn::IsPinned() const
{
	return m_State == State::Pinned;
}

void PF_PlayerPawn::ProcessState(const GameContext&)
{
	if (m_State != State::Pinned && (IsInRange(m_GameEnterHitbox) || IsInRange(m_GameExitHitbox)))
	{
		m_State = State::Active;
	}
	else if (m_State != State::Pinned) m_State = State::Idle;


	switch (m_State)
	{
	case State::Idle:
		m_pSprite->SetTexture(m_DefaultTexture + std::to_wstring(m_PlayerID) + L".png");
		break;

	case State::Active:
		m_pSprite->SetTexture(m_ActiveTexture + std::to_wstring(m_PlayerID) + L".png");
		break;

	case State::Pinned:
		m_pSprite->SetTexture(m_PinnedTexture + std::to_wstring(m_PlayerID) + L".png");
		break;
	}
}

void PF_PlayerPawn::ProcessInput(const GameContext& context)
{
	InputManager* pInput = context.pInput;

	if (pInput->IsActionTriggered(InputID(m_PlayerID) + (int)Input_PlayerPawn::A))
	{
		TryPin(true);
	}
	if (pInput->IsActionTriggered(InputID(m_PlayerID) + (int)Input_PlayerPawn::B))
	{
		TryPin(false);
	}

	XMFLOAT2 thumbPos = pInput->GetThumbstickPosition(true, (GamepadIndex)m_PlayerID);
	m_Velocity = { thumbPos.x, -thumbPos.y };
}

void PF_PlayerPawn::TryPin(bool doPin)
{
	if (!doPin)
	{
		m_State = State::Idle;
		return;
	}
	if (m_State == State::Active) m_State = State::Pinned;
	if (IsInRange(m_GameEnterHitbox)) m_PlayRequest = true;
	if (IsInRange(m_GameExitHitbox)) m_ExitRequest = true;
}

void PF_PlayerPawn::Move(const GameContext& context)
{
	float dt = context.pGameTime->GetElapsed();

	XMFLOAT2 newPos = m_pSprite->GetPivot();
	
	m_pSprite->SetPivot({newPos.x + m_Speed * m_Velocity.x * dt, newPos.y + m_Speed*  m_Velocity.y * dt});
}

bool PF_PlayerPawn::IsInRange(MyUtils::Circle2D hitbox) const
{
	XMFLOAT2 pos = m_pSprite->GetPivot();
	XMVECTOR distanceFromHitboxCenter = XMVector2LengthEst(XMLoadFloat2(&pos) - XMLoadFloat2(&hitbox.m_Center));
	XMFLOAT2 distances;
	XMStoreFloat2(&distances, distanceFromHitboxCenter);

	if (distances.x <= hitbox.m_Radius) return true;

	return false;
}

