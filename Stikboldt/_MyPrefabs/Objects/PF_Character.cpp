
#include "stdafx.h"
#include "PF_Character.h"
#include "Components.h"
#include "Prefabs.h"
#include "GameScene.h"
#include "PhysxManager.h"
#include "PhysxProxy.h"
#include "ControllerComponent.h"


PF_Character::PF_Character(float radius, float height, float moveSpeed, float maxJumpHeight, float maxSlopeAngle, float maxMoveSpeed, float drag) : 
	m_Radius(radius),
	m_Height(height),
	m_MoveSpeed(moveSpeed),
	m_pCamera(nullptr),
	m_pController(nullptr),
	m_TotalPitch(0), 
	m_TotalYaw(0),
	m_RotationSpeed(90.f),
	m_SlopeMax{cosf(maxSlopeAngle)},
	m_MaxJumpHeight{maxJumpHeight},
	m_GravityEnabled{true},

	//Running
	m_MaxRunVelocity(maxMoveSpeed), 
	m_TerminalVelocity(20),
	m_Gravity(9.81f), 
	m_RunAccelerationTime(0.3f), 
	m_JumpAccelerationTime(0.8f), 
	m_RunAcceleration(m_MaxRunVelocity/m_RunAccelerationTime), 

	m_JumpAcceleration(m_Gravity/m_JumpAccelerationTime), 
	m_RunVelocity(0), 
	m_JumpVelocity(0),
	m_Velocity(0,0,0),
	m_HorVelocity{},
	m_VertVelocity{},
	m_Acceleration{},
	m_Drag{drag}
{}

void PF_Character::Initialize(const GameContext& gameContext)
{
	using namespace physx;
	using namespace DirectX;

	UNREFERENCED_PARAMETER(gameContext);

	//TODO: Create controller
	PxMaterial* pMat = PhysxManager::GetInstance()->GetPhysics()->createMaterial(1.f, 1.f, 0.f);
	m_pController = new ControllerComponent{ pMat, m_Radius, m_Height, m_MaxJumpHeight, m_SlopeMax };
	AddComponent(m_pController);
}

void PF_Character::PostInitialize(const GameContext& gameContext)
{
	using namespace DirectX;

	UNREFERENCED_PARAMETER(gameContext);
}


void PF_Character::Update(const GameContext& gameContext)
{
	// Set acceleration to zero, input sets it to something else if needed && Lose the accumulated velocity
	LoseHorVelocity(gameContext);

	m_IsGrounded = m_pController->IsGrounded();
	if (m_GravityEnabled && !m_IsGrounded)
	{
		ApplyAcceleration(0.f, -1.f, 0.f , m_Gravity, gameContext);
	}

	// Processes movement based on velocity & acceleration
	ConsumeVelocity(gameContext);
	m_Acceleration = {};
}

void PF_Character::SetMaxSlopeAngle(const float angle)
{
	m_SlopeMax = cosf(angle);
}

void PF_Character::ViewMyCamera(bool enabled)
{
	if (m_pCamera && enabled)
	{
		m_pCamera->SetActive();
	}
}

void PF_Character::EnableGravity(const bool enabled)
{
	m_GravityEnabled = enabled;
}

bool PF_Character::IsGrounded() const
{
	return m_IsGrounded;
}

bool PF_Character::GravityIsEnabled() const
{
	return m_GravityEnabled;
}

void PF_Character::SimpleMove(const DirectX::XMFLOAT3& direction, float amountPerSecond, const GameContext& gameContext, bool useLocalAxes)
{
	using namespace DirectX;

	float dt = gameContext.pGameTime->GetElapsed();
	XMVECTOR finalDisplacement;

	if (!useLocalAxes) finalDisplacement = XMLoadFloat3(&direction);
	else
	{
		XMVECTOR forward = XMLoadFloat3(&GetTransform()->GetForward());
		XMVECTOR right = XMLoadFloat3(&GetTransform()->GetRight());
		XMVECTOR up = XMLoadFloat3(&GetTransform()->GetUp());

		XMVECTOR localDirectionVec = forward * direction.z + right * direction.x + up * direction.y;

		finalDisplacement = localDirectionVec;
	}

	finalDisplacement *= amountPerSecond;
	finalDisplacement = XMVector2ClampLength(finalDisplacement, 0.f, m_MaxRunVelocity);
	finalDisplacement *= dt;

	XMFLOAT3 displacementFloats;
	DirectX::XMStoreFloat3(&displacementFloats, finalDisplacement);

	m_pController->Move({ displacementFloats });
}

void PF_Character::SimpleRotateHorizontal(bool goRight, float amountPerSecond, const GameContext& gameContext)
{
	using namespace DirectX;

	XMFLOAT3 rot;
	float amount = amountPerSecond * gameContext.pGameTime->GetElapsed();

	rot = { 0.0f, goRight ? -amount : amount, 0.0f };

	m_pController->GetTransform()->Rotate(XMLoadFloat3(&rot), true);
}

#pragma region Acceleration
void PF_Character::ApplyAcceleration(const DirectX::XMVECTOR& direction, float power, const GameContext&)
{
	using namespace DirectX;

	XMFLOAT3 vector;
	XMStoreFloat3(&vector, direction);
	if (vector.x == 0.f && vector.y == 0.f && vector.z == 0.0f) return; // If zerovector, don't do anything...


	XMStoreFloat3(&m_Acceleration, XMVector3NormalizeEst(direction) * power);
}

void PF_Character::ApplyAcceleration(const DirectX::XMFLOAT3& direction, float power, const GameContext& gameContext)
{
	ApplyAcceleration(DirectX::XMLoadFloat3(&direction), power, gameContext);
}

void PF_Character::ApplyAcceleration(float x, float y, float z, float power, const GameContext& gameContext)
{
	ApplyAcceleration(DirectX::XMFLOAT3{ x, y, z }, power, gameContext);
}
#pragma endregion

void PF_Character::LoseHorVelocity(const GameContext& gameContext)
{
	if (m_Acceleration.x != 0.f || m_Acceleration.y != 0.f || m_Acceleration.z != 0.f) return;

	using namespace DirectX;

	XMFLOAT2 horVelocityLengthXYZ;
	XMStoreFloat2(&horVelocityLengthXYZ, XMVector2Length(XMLoadFloat2(&m_HorVelocity)));
	float length = horVelocityLengthXYZ.x;

	gameContext;

	float newLength = length - m_Drag * gameContext.pGameTime->GetElapsed();
	if (newLength < 0.f)
	{
		newLength = 0.f;
		m_HorVelocity = {};
		return;
	}

	XMVECTOR newHorVelocity = XMVectorScale(XMVector2NormalizeEst(XMLoadFloat2(&m_HorVelocity)), newLength);
	XMStoreFloat2(&m_HorVelocity, newHorVelocity);
}

void PF_Character::ConsumeVelocity(const GameContext& gameContext)
{
	using namespace DirectX;
	// Get Velocity & splits
	XMVECTOR horVelocity = XMLoadFloat2(&m_HorVelocity);
	float vertVelocity = m_VertVelocity;

	// Get Acceleration & splits
	XMFLOAT2 horAcc = XMFLOAT2{ m_Acceleration.x, m_Acceleration.z }; // HOR == x & z, not x & y
	XMVECTOR horAcceleration = XMLoadFloat2(&horAcc);
	float vertAcceleration = m_Acceleration.y;

	// DT:
	float dt = gameContext.pGameTime->GetElapsed();

	// Increase velocity with acceleration:
	horVelocity += horAcceleration * dt;
	horVelocity = XMVector2ClampLength(horVelocity, 0.0f, m_MaxRunVelocity);
	vertVelocity += vertAcceleration * dt;

	// Update velocity:
	XMStoreFloat2(&m_HorVelocity, horVelocity); // horizontal
	m_VertVelocity = vertVelocity; // vertical

	// Horizontal displace:
	XMVECTOR finalHorDisplacement = XMVector2ClampLength(horVelocity * dt, 0.f, m_MaxRunVelocity);
	XMFLOAT2 finalHorDisplace;
	XMStoreFloat2(&finalHorDisplace, finalHorDisplacement);

	// Vertical displace: (not clamped!)
	float finalVertDisplacement;
	finalVertDisplacement = vertVelocity * dt;

	// Merge displacement:
	XMFLOAT3 fullDisplacement = { finalHorDisplace.x, finalVertDisplacement, finalHorDisplace.y };

	// Move
	m_pController->Move(fullDisplacement);
}