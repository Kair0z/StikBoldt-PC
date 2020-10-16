#include "stdafx.h"
#include "PF_RobotCharacter.h"

#include "MaterialManager.h"
#include "../OverlordProject/_Materials/Materials.h"

#include "ModelComponent.h"
#include "ModelAnimator.h"
#include "TransformComponent.h"
#include "ColliderComponent.h"
#include "RigidBodyComponent.h"
#include "ParticleEmitterComponent.h"
#include "../../ScenePrefabs/MyPrefabs_ScenePrefabs.h"
#include "../OverlordProject/_MyHelpers/TimeManip.h"

#include "SpherePrefab.h"

#include "PhysxManager.h"
#include "ControllerComponent.h"


#include "../OverlordProject/Utils.h"

#include "PhysxProxy.h"

#define InputID(playerID) (7 + playerID)

PF_RobotCharacter::PF_RobotCharacter()
	: m_pModelComp{nullptr}
	, m_MovementData{}
	, m_Height{5.f}
	, m_Radius{2.f}
	, m_pBallSocket{nullptr}
	, m_BallEnterState{BallEnterState::NotReady}
	, m_BallPickedUp{false}
	, m_pTheBall{nullptr}
	, m_pCollider{nullptr}
	, m_pRigidBody{nullptr}
	, m_SocketRotationSpeed{80.f}
	, m_AnimationState{AnimationState::Idle}
	, m_QRotation{0.f, 0.0f, 0.f, 1.f}
	, m_pParticles{nullptr}
	, m_IsOut{false}

{
	
}

void PF_RobotCharacter::Initialize(const GameContext& gCtx)
{
	InitMesh(gCtx);
	InitHitbox(gCtx);
	InitInput(gCtx);
	InitSocket(gCtx);
	InitParticles(gCtx);

	std::function<void(GameObject*, GameObject*, ContactAction)> func = std::bind(&PF_RobotCharacter::OnContact, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	SetOnContactCallBack(func);
}

void PF_RobotCharacter::Reset()
{
	m_AnimationState = AnimationState::Idle;

	m_BiasData.Reset();
	m_CoolDownData.Reset();
	m_MovementData.Reset();

	m_IsOut = false;
	m_BallEnterState = BallEnterState::NotReady;
	m_BallPickedUp = false;
	m_pTheBall = nullptr;
}

#pragma region Initialize
void PF_RobotCharacter::InitMesh(const GameContext&)
{
	m_pModelComp = new ModelComponent{ L"Resources/Models/Robot_Anim.ovm" };

	GameObject* pGO = new GameObject{};

	m_pModelComp->SetMaterial(ID(ActiveMaterials::DefaultRed));
	pGO->AddComponent(m_pModelComp);
	pGO->GetTransform()->Scale(1.9f, 1.9f, 1.9f);
	pGO->GetTransform()->Translate(0.f, -3.f, 0.f);
	AddChild(pGO);

	if (m_PlayerID < 0) return;
	switch (m_PlayerID)
	{
	case 0:
		m_pModelComp->SetMaterial((int)ActiveMaterials::P0);
		break;

	case 1:
		m_pModelComp->SetMaterial((int)ActiveMaterials::P1);
		break;

	case 2:
		m_pModelComp->SetMaterial((int)ActiveMaterials::P2);
		break;

	case 3:
		m_pModelComp->SetMaterial((int)ActiveMaterials::P3);
		break;
	}
}

void PF_RobotCharacter::InitInput(const GameContext& context)
{
	context.pInput->AddInputAction(
		InputAction{ (int)Controls::Hit * InputID(m_PlayerID), InputTriggerState::Pressed, -1, -1, XINPUT_GAMEPAD_RIGHT_SHOULDER, GamepadIndex(m_PlayerID) });

	context.pInput->AddInputAction(
		InputAction{ (int)Controls::Dash * InputID(m_PlayerID), InputTriggerState::Pressed, -1, -1, XINPUT_GAMEPAD_LEFT_SHOULDER, GamepadIndex(m_PlayerID) });
}

void PF_RobotCharacter::InitHitbox(const GameContext&)
{
	m_pRigidBody = new RigidBodyComponent{};
	m_pRigidBody->SetKinematic(true);
	m_pRigidBody->SetConstraint(RigidBodyConstraintFlag::RotZ, true);
	m_pRigidBody->SetConstraint(RigidBodyConstraintFlag::RotX, true);

	AddComponent(m_pRigidBody);

	// Selfcollider
	physx::PxTransform transform = physx::PxTransform{ physx::PxQuat{physx::PxHalfPi, {0.f, 0.f, -1.f}} };
	std::shared_ptr<physx::PxGeometry> pGeom2(new physx::PxCapsuleGeometry{ m_Radius, m_Height / 2.f });
	m_pCollider = new ColliderComponent
	{
		pGeom2,
		*PhysxManager::GetInstance()->GetPhysics()->createMaterial(0.5f, 0.5f, 0.f),
		transform
	};
	m_pCollider->EnableTrigger(false);
 
	AddComponent(m_pCollider);
}

void PF_RobotCharacter::InitSocket(const GameContext& )
{
	m_pBallSocket = new GameObject{};

	float distance = 3.f;
	XMVECTOR forward = XMLoadFloat3(&GetTransform()->GetForward()) * distance;

	XMFLOAT3 offsetPos;
	XMStoreFloat3(&offsetPos, forward);
	m_pBallSocket->GetTransform()->Translate(offsetPos);

	AddChild(m_pBallSocket);
}

void PF_RobotCharacter::InitParticles(const GameContext&)
{
	m_pParticles = new ParticleEmitterComponent{ L"Resources/Textures/Particles/star.png", 8 };

	m_pParticles->SetMaxEmitterRange(4.f);
	m_pParticles->SetMinEmitterRange(4.f);

	m_pParticles->SetMaxEnergy(2.f);
	m_pParticles->SetMinEnergy(2.f);
	m_pParticles->SetGravity(true);
	m_pParticles->SetMass(8.f);

	m_pParticles->SetColor({ 1.f, 1.f, 1.f, 1.f });
	m_pParticles->SetMaxSize(5.f);
	m_pParticles->SetMinSize(1.f);

	m_pParticles->SetMaxSizeGrow(0.5f);
	m_pParticles->SetMinSizeGrow(0.5f);

	float speed = 40.f;
	m_pParticles->SetVelocity({ 0.f, speed, 0.f });

	AddComponent(m_pParticles);
	m_pParticles->Activate(false);
}
#pragma endregion

void PF_RobotCharacter::Update(const GameContext& gCtx)
{
	bool gamePaused = GameMenuLocator::Locate()->IsActive();
	if (gamePaused) return;

	ProcessInput(gCtx);
	ProcessRotation(gCtx);
	ProcessMovement(gCtx);
	MatchBallToSocket(gCtx);
	UpdateAnimation(gCtx);
	UpdateParticles(gCtx);
	
	// Update cooldowndata & biasdata
	m_CoolDownData.UpdateCooldowns(gCtx);
	m_BiasData.UpdateBiases(gCtx);

	// Update dashing timer:
	if (m_MovementData.m_IsDashing)
	{
		float dt = gCtx.pGameTime->GetElapsed();
		TimeManip* pTimeManip = TimeManipLocator::Locate();
		if (pTimeManip) dt = pTimeManip->GetSimulatedElapsedSeconds();

		m_MovementData.m_DashTimer += dt;
		if (m_MovementData.m_DashTimer >= m_MovementData.m_DashTime)
		{
			m_MovementData.m_IsDashing = false;
			m_MovementData.m_DashTimer = 0.0f;
		}
	}
	else m_pRigidBody->SetCollisionGroup(CollisionGroupFlag::Group0);

	if (m_IsOut) m_pRigidBody->SetCollisionGroup(INVINCIBLE_GROUPFLAG);
}

#pragma region Update
void PF_RobotCharacter::UpdateAnimation(const GameContext&)
{
	m_pAnimator = m_pModelComp->GetAnimator();
	if (!m_pAnimator) return;
	m_pAnimator->SetAnimationSpeed(1.f);
	m_pAnimator->Loop(true);

	// Prioritylist
	if (m_CoolDownData.m_IsStunned) m_AnimationState = AnimationState::Stunned;
	if (m_MovementData.m_IsDashing) m_AnimationState = AnimationState::Dashing;
	if (m_BiasData.m_HitActive) m_AnimationState = AnimationState::Hitting;
	if (m_BiasData.m_CatchActive) m_AnimationState = AnimationState::Catching;
	if (m_IsOut) m_AnimationState = AnimationState::KnockedOut;

	std::wstring clipName = L"";

	switch (m_AnimationState)
	{
	case AnimationState::Catching:
		clipName = L"Robot_Catch";
		m_pAnimator->SetAnimationSpeed(2.f);
		break;

	case AnimationState::KnockedOut:
		clipName = L"Robot_Fall";
		break;

	case AnimationState::Dashing:
		clipName = L"Robot_roll";
		m_pAnimator->SetAnimationSpeed(1.5f);
		break;

	case AnimationState::Hitting:
		clipName = L"Robot_PunchLeft";
		m_pAnimator->SetAnimationSpeed(2.f);
		break;

	case AnimationState::Idle:
		clipName = L"Robot_Idle";
		break;

	case AnimationState::Running:
		switch (m_PlayerID)
		{
		case 0: clipName = L"Robot_Run1"; break;
		case 1: clipName = L"Robot_Run3"; break;
		case 2: clipName = L"Robot_Run2"; break;
		case 3: clipName = L"Robot_Run1"; break;
		}
		break;

	case AnimationState::Stunned:
		clipName = L"Robot_Dizzy";
		m_pAnimator->SetAnimationSpeed(2.f);
		break;

	default:
		return; // If none of these states, return;
	}

	if (!m_pAnimator->IsPlaying(clipName))
	{
		m_pAnimator->SetAnimation(clipName);
		m_pAnimator->Play();
	}
}

void PF_RobotCharacter::UpdateParticles(const GameContext&)
{
	if (m_CoolDownData.m_IsStunned && !m_IsOut) m_pParticles->Activate(true);
	else m_pParticles->Activate(false);
}

void PF_RobotCharacter::ProcessBallContact(PF_TheBall* pTheBall)
{
	if (!pTheBall) return;
	if (m_IsOut) return;

	// hit has priority over catch!
	if (m_BiasData.m_HitActive) HitBall(pTheBall);
	else if (m_BiasData.m_CatchActive && pTheBall->GetState() != PF_TheBall::State::PickedUp) CatchBall(pTheBall);
	else if (pTheBall->GetSafeRobot() != this) // Don't get hit if this is the safe robot! (last robot that got off a hit!)
	{
		GetHitByBall(pTheBall);
	}
}

void PF_RobotCharacter::ProcessMovement(const GameContext& context)
{
	if (m_MovementData.m_IsDashing)
	{
		Dash(context);
		return;
	}

	Drag(context);

	// Apply gravity: not needed?

	Move(context);

	// Set animation state according to speed:
	if (m_MovementData.m_Velocity.x != 0 || m_MovementData.m_Velocity.y != 0) m_AnimationState = AnimationState::Running;
	else m_AnimationState = AnimationState::Idle;

	m_MovementData.m_Acceleration = {};
}

#define ISTRIGGERED(c, action) c.pInput->IsActionTriggered((int)action)
void PF_RobotCharacter::ProcessInput(const GameContext& context)
{
	using namespace DirectX;

	// No player controller:
	if (m_PlayerID < 0) return; // No player controller set

	// No gamepad connnected:
	if (!InputManager::IsGamepadConnected(GamepadIndex(m_PlayerID)))
	{
		//Logger::LogWarning(L"Bot with playerID" + std::to_wstring(m_PlayerID) + L" has no connected controller!");
		return;
	}

	// IsOut:
	if (m_IsOut) return;

	// Movement:
	const XMFLOAT2& thumbStickPos = InputManager::GetThumbstickPosition(true, GamepadIndex(m_PlayerID));
	ApplyAcceleration(thumbStickPos.x, 0.0f, thumbStickPos.y, m_MovementData.m_AccPower, context);

	const XMFLOAT2& rightThumbStickPos = InputManager::GetThumbstickPosition(false, GamepadIndex(m_PlayerID));
	RotateSocket(rightThumbStickPos, context);

	// Actions:
	if (ISTRIGGERED(context, Controls::Dash * InputID(m_PlayerID))) TryDash(context);
	if (ISTRIGGERED(context, Controls::Hit * InputID(m_PlayerID))) TryHit(context);
	float rightPressure = InputManager::GetTriggerPressure(false, GamepadIndex(m_PlayerID));
	if (rightPressure > 0.f) TryCatch(context);
	if (rightPressure == 0.f) TryThrow(context);
}
#pragma endregion

#pragma region Actions
void PF_RobotCharacter::TryDash(const GameContext&)
{
	if (m_CoolDownData.m_IsStunned) return;

	const XMFLOAT2& thumbStickPos = InputManager::GetThumbstickPosition(true, GamepadIndex(m_PlayerID));
	if (thumbStickPos.x == 0.f && thumbStickPos.y == 0.f) return; // don't dash without direction

	if (m_CoolDownData.m_DashOnCooldown) return;

	m_pRigidBody->SetCollisionGroup(INVINCIBLE_GROUPFLAG);

	std::cout << "Dash! \n";
	m_MovementData.m_IsDashing = true;
	m_MovementData.m_DashTimer = 0.0f;
	m_MovementData.m_DashDirection = { thumbStickPos.x, thumbStickPos.y };

	m_CoolDownData.StartDashCooldown();
}

void PF_RobotCharacter::TryHit(const GameContext&)
{
	if (m_CoolDownData.m_HitOnCooldown) return;

	m_BiasData.m_HitActive = true;

	m_CoolDownData.StartHitCooldown();
}

void PF_RobotCharacter::TryCatch(const GameContext&)
{
	if (m_CoolDownData.m_CatchOnCooldown || m_CoolDownData.m_HasTriedCatch) return;

	m_BiasData.m_CatchActive = true;
	m_CoolDownData.m_HasTriedCatch = true;
	AdjustSocketToDirection(); // Adjust the socket to be in the direction you're facing

	m_CoolDownData.StartCatchCooldown();
}

void PF_RobotCharacter::TryThrow(const GameContext&)
{
	using namespace DirectX;

	// Reset new catch try:
	m_CoolDownData.m_HasTriedCatch = false;

	if (!m_BallPickedUp || !m_pTheBall) return;

	XMVECTOR dir = 
		XMVector3NormalizeEst((XMLoadFloat3(&m_pTheBall->GetTransform()->GetPosition()) - XMLoadFloat3(&GetTransform()->GetPosition())));

	XMFLOAT3 res{};
	XMStoreFloat3(&res, dir);
	
	m_pTheBall->Release(res, this);
	m_BiasData.m_CatchActive = false;
	m_BiasData.m_CatchActiveTimer = 0.0f;

	m_BallPickedUp = false;
	m_pTheBall = nullptr;
}
#pragma endregion

#pragma region ApplyAcceleration
void PF_RobotCharacter::ApplyAcceleration(const DirectX::XMVECTOR& direction, float power, const GameContext&)
{
	using namespace DirectX;

	XMFLOAT3 vector;
	XMStoreFloat3(&vector, direction);
	XMFLOAT2 vector2D = { vector.x, vector.z };

	if (vector.x == 0.f && vector.y == 0.f && vector.z == 0.0f) return; // If zerovector, don't do anything...

	XMStoreFloat2(&m_MovementData.m_Acceleration, XMVector2NormalizeEst(XMLoadFloat2(&vector2D)) * power);
}

void PF_RobotCharacter::ApplyAcceleration(const DirectX::XMFLOAT3& direction, float power, const GameContext& gameContext)
{
	ApplyAcceleration(DirectX::XMLoadFloat3(&direction), power, gameContext);
}

void PF_RobotCharacter::ApplyAcceleration(float x, float y, float z, float power, const GameContext& gameContext)
{
	ApplyAcceleration(DirectX::XMFLOAT3{ x, y, z }, power, gameContext);
}
#pragma endregion

#pragma region Movement
void PF_RobotCharacter::Drag(const GameContext& gameContext)
{
	if (m_MovementData.m_Acceleration.x != 0.f || m_MovementData.m_Acceleration.y != 0.f) return;

	using namespace DirectX;

	XMFLOAT2 horVelocityLengthXYZ;
	XMStoreFloat2(&horVelocityLengthXYZ, XMVector2Length(XMLoadFloat2(&m_MovementData.m_Velocity)));
	float length = horVelocityLengthXYZ.x;

	float dt = gameContext.pGameTime->GetElapsed();
	TimeManip* pTimeManip = TimeManipLocator::Locate();
	if (pTimeManip) dt = pTimeManip->GetSimulatedElapsedSeconds();

	float newLength = length - m_MovementData.m_Drag * dt;
	if (newLength < 0.f)
	{
		newLength = 0.f;
		m_MovementData.m_Velocity = {};
		return;
	}

	XMVECTOR newHorVelocity = XMVectorScale(XMVector2NormalizeEst(XMLoadFloat2(&m_MovementData.m_Velocity)), newLength);
	XMStoreFloat2(&m_MovementData.m_Velocity, newHorVelocity);
}

void PF_RobotCharacter::Move(const GameContext& gameContext)
{
	using namespace DirectX;
	// Get Velocity & splits
	XMVECTOR horVelocity = XMLoadFloat2(&m_MovementData.m_Velocity);

	// Get Acceleration & splits
	XMFLOAT2 horAcc = m_MovementData.m_Acceleration; 
	XMVECTOR horAcceleration = XMLoadFloat2(&horAcc);

	// DT:
	float dt = gameContext.pGameTime->GetElapsed();
	TimeManip* pTimeManip = TimeManipLocator::Locate();
	if (pTimeManip) dt = pTimeManip->GetSimulatedElapsedSeconds();

	float maxVelocity = m_MovementData.m_MaxVelocity;
	if (m_BallPickedUp) maxVelocity -= m_MovementData.m_PickupSlowdown;
	if (m_CoolDownData.m_IsStunned) maxVelocity -= m_MovementData.m_StunSlowdown;
	if (maxVelocity < 0.0f) maxVelocity = 0.1f;

	// Increase velocity with acceleration:
	horVelocity += horAcceleration * dt;
	horVelocity = XMVector2ClampLength(horVelocity, 0.0f, maxVelocity);

	// Update velocity:
	XMStoreFloat2(&m_MovementData.m_Velocity, horVelocity); // horizontal

	// Horizontal displace:
	XMVECTOR finalHorDisplacement = XMVector2ClampLength(horVelocity * dt, 0.f, maxVelocity);
	XMFLOAT2 finalHorDisplace;
	XMStoreFloat2(&finalHorDisplace, finalHorDisplacement);

	// Merge displacement:
	XMFLOAT3 fullDisplacement = { finalHorDisplace.x, 0.0f, finalHorDisplace.y };
	XMFLOAT3 currentPos{ GetTransform()->GetPosition() };
	XMFLOAT3 newPos = { currentPos.x + fullDisplacement.x, currentPos.y + fullDisplacement.y, currentPos.z + fullDisplacement.z };
	XMVECTOR lengthFromOrigin = XMVector3Length(XMLoadFloat3(&newPos));
	XMFLOAT3 lengths;
	XMStoreFloat3(&lengths, lengthFromOrigin);

	float range = 55;
	if (lengths.x < range) m_pRigidBody->GetPxRigidDynamic()->setKinematicTarget({ newPos.x, newPos.y, newPos.z });
}

void PF_RobotCharacter::Dash(const GameContext& gameContext)
{
	float dt = gameContext.pGameTime->GetElapsed();
	TimeManip* pTimeManip = TimeManipLocator::Locate();
	if (pTimeManip) dt = pTimeManip->GetSimulatedElapsedSeconds();

	XMFLOAT3 dashDirection = { m_MovementData.m_DashDirection.x, 0.f, m_MovementData.m_DashDirection.y };
	XMVECTOR dashDir = XMVector3NormalizeEst(XMLoadFloat3(&dashDirection));

	XMVECTOR dashVelocity = dashDir * m_MovementData.m_DashSpeed;

	XMVECTOR fullDisplacement = dashVelocity * dt;
	XMFLOAT3 fullDisplacements;
	XMStoreFloat3(&fullDisplacements, fullDisplacement);
	XMFLOAT3 curPos = GetTransform()->GetPosition();
	XMFLOAT3 newPos = { curPos.x + fullDisplacements.x, curPos.y + fullDisplacements.y, curPos.z + fullDisplacements.z };
	XMVECTOR lengthFromOrigin = XMVector3Length(XMLoadFloat3(&newPos));
	XMFLOAT3 lengths;
	XMStoreFloat3(&lengths, lengthFromOrigin);
	float range = 55;
	if (lengths.x < range) m_pRigidBody->GetPxRigidDynamic()->setKinematicTarget({ newPos.x, newPos.y, newPos.z });
}
#pragma endregion

#pragma region Ball interaction
void PF_RobotCharacter::CatchBall(PF_TheBall* pTheBall)
{
	if (m_BallPickedUp) return;

	std::cout << "CAUGHT!\n";
	pTheBall->SetState(PF_TheBall::State::PickedUp);
	m_pTheBall = pTheBall;
	m_BallPickedUp = true;

	m_pTheBall->MatchBotColor(this);
}

void PF_RobotCharacter::HitBall(PF_TheBall* pTheBall)
{
	using namespace DirectX;
	if (pTheBall->GetState() == PF_TheBall::State::PickedUp) return;

	XMVECTOR dir = XMVector3NormalizeEst((XMLoadFloat3(&pTheBall->GetTransform()->GetPosition()) - XMLoadFloat3(&GetTransform()->GetPosition())));
	XMFLOAT3 res;
	XMStoreFloat3(&res, dir);

	pTheBall->SetState(PF_TheBall::State::Volatile);
	pTheBall->GetHit(res, this, 1.5f);
	m_BiasData.m_HitActive = false;
	m_BiasData.m_HitActiveTimer = 0.0f;
}

void PF_RobotCharacter::GetHitByBall(PF_TheBall* pTheBall)
{
	if (!pTheBall) return;

	if (pTheBall->GetState() == PF_TheBall::State::Volatile)
	{
		// Notify scoreboard:
		auto pScoreboard = ScoreboardLocator::Locate();
		if (pScoreboard) pScoreboard->AddScore(pTheBall->GetSafeRobot());


		// Process the hit:
		if (!m_CoolDownData.m_IsStunned) m_CoolDownData.StartStun();
		else
		{
			if (pScoreboard) pScoreboard->RegisterOut(this);
			std::cout << "Player " << m_PlayerID << " is out! \n";
			m_IsOut = true;
		}


		// Camera shake:
		auto pCamera = MainCameraLocator::Locate();
		if (pCamera)
		{
			pCamera->Shake(0.3f, 2.f);
		}


		// Set ball to be neutral
		pTheBall->ProcessVolatileHit();
	}
}
#pragma endregion

#pragma region Cooldowns
void PF_RobotCharacter::ActionCooldownData::StartHitCooldown()
{
	m_HitOnCooldown = true;
	m_HitCooldownTimer = 0.0f;
}
void PF_RobotCharacter::ActionCooldownData::StartDashCooldown()
{
	m_DashOnCooldown = true;
	m_DashCooldownTimer = 0.0f;
}
void PF_RobotCharacter::ActionCooldownData::StartCatchCooldown()
{
	m_CatchOnCooldown = true;
	m_CatchCooldownTimer = 0.0f;
}
void PF_RobotCharacter::ActionCooldownData::StartStun()
{
	m_IsStunned = true;
	m_StunnedTimer = 0.0f;
}
void PF_RobotCharacter::ActionCooldownData::UpdateCooldowns(const GameContext& context)
{
	float dt = context.pGameTime->GetElapsed();
	TimeManip* pTimeManip = TimeManipLocator::Locate();
	if (pTimeManip) dt = pTimeManip->GetSimulatedElapsedSeconds();

	// update timers
	if (m_CatchOnCooldown) m_CatchCooldownTimer += dt;
	if (m_DashOnCooldown) m_DashCooldownTimer += dt;
	if (m_HitOnCooldown) m_HitCooldownTimer += dt;
	if (m_IsStunned) m_StunnedTimer += dt;

	// check for reset
	if (m_CatchOnCooldown && m_CatchCooldownTimer >= m_CatchCooldownTime)
	{
		m_CatchCooldownTimer = 0.0f;
		m_CatchOnCooldown = false;
	}
	if (m_HitOnCooldown && m_HitCooldownTimer >= m_HitCooldownTime)
	{
		m_HitCooldownTimer = 0.0f;
		m_HitOnCooldown = false;
	}
	if (m_DashOnCooldown && m_DashCooldownTimer >= m_DashCooldownTime)
	{
		m_DashCooldownTimer = 0.0f;
		m_DashOnCooldown = false;
	}
	if (m_IsStunned && m_StunnedTimer >= m_StunnedTime)
	{
		m_IsStunned = false;
		m_StunnedTimer = 0.0f;
	}
}

#pragma endregion

#pragma region Biases
void PF_RobotCharacter::ActionBiasData::UpdateBiases(const GameContext& context)
{
	float dt = context.pGameTime->GetElapsed();
	TimeManip* pTimeManip = TimeManipLocator::Locate();
	if (pTimeManip) dt = pTimeManip->GetSimulatedElapsedSeconds();

	if (m_CatchActive) m_CatchActiveTimer += dt;
	if (m_HitActive) m_HitActiveTimer += dt;

	if (m_HitActive && m_HitActiveTimer >= m_HitActiveTime)
	{
		m_HitActiveTimer = 0.0f;
		m_HitActive = false;
	}
	if (m_CatchActive && m_CatchActiveTimer >= m_CatchActiveTime)
	{
		m_CatchActiveTimer = 0.0f;
		m_CatchActive = false;
	}
}
#pragma endregion

// Helper functions
void PF_RobotCharacter::AdjustSocketToDirection()
{
	using namespace DirectX;

	XMFLOAT2 vel = m_MovementData.m_Velocity;

	XMVECTOR targetPos = XMVector2NormalizeEst(XMLoadFloat2(&vel));
	XMFLOAT2 newPos;
	XMStoreFloat2(&newPos, targetPos * 4.f);
	m_pBallSocket->GetTransform()->Translate(newPos.x, 0.f, newPos.y);
}

void PF_RobotCharacter::RotateSocket(const DirectX::XMFLOAT2& thumbStickPos, const GameContext& c)
{
	if (thumbStickPos.x == 0.0f && thumbStickPos.y == 0.0f) return;

	XMFLOAT2 cur = { m_pBallSocket->GetTransform()->GetPosition().x, m_pBallSocket->GetTransform()->GetPosition().z };
	XMVECTOR currentPos = XMLoadFloat2(&cur);
	XMVECTOR targetPos = XMVector2NormalizeEst(XMLoadFloat2(&thumbStickPos));

	XMVECTOR norm = XMVector2NormalizeEst(XMVectorLerp(currentPos, targetPos, m_SocketRotationSpeed * c.pGameTime->GetElapsed()));
	XMFLOAT2 newPos;

	XMStoreFloat2(&newPos, norm * 4.f);
	m_pBallSocket->GetTransform()->Translate(newPos.x, 0.f, newPos.y);
}

void PF_RobotCharacter::ProcessRotation(const GameContext&)
{
	if (m_MovementData.m_Velocity.x == 0.0f && m_MovementData.m_Velocity.y == 0.0f)
	{
		return;
	}

	XMVECTOR velNorm = XMVector2NormalizeEst(XMLoadFloat2(&m_MovementData.m_Velocity));
	XMFLOAT2 velocityNormalized;
	XMStoreFloat2(&velocityNormalized, velNorm);
	float angle = physx::PxAcos(-velocityNormalized.y);
	if (velocityNormalized.x > 0) angle = -angle;

	GetTransform()->Rotate(0.f, angle, 0.f, false);
}

void PF_RobotCharacter::MatchBallToSocket(const GameContext&)
{
	if (m_pTheBall && m_BallPickedUp)
	{
		XMFLOAT3 pos = m_pBallSocket->GetTransform()->GetPosition();
		XMFLOAT3 or = GetTransform()->GetPosition();

		m_pTheBall->GetTransform()->Translate(pos.x + or .x, pos.y + or .y, pos.z + or .z);
	}
}

void PF_RobotCharacter::SetPlayerID(PlayerID id)
{
	// Invalid player id's
	if (id > 3) return;
	if (id < 0) return;

	m_PlayerID = id;

	if (!m_pModelComp) return;
	switch (m_PlayerID)
	{
	case 0:
		m_pModelComp->SetMaterial((int)ActiveMaterials::P0);
		break;

	case 1:
		m_pModelComp->SetMaterial((int)ActiveMaterials::P1);
		break;

	case 2:
		m_pModelComp->SetMaterial((int)ActiveMaterials::P2);
		break;

	case 3:
		m_pModelComp->SetMaterial((int)ActiveMaterials::P3);
		break;
	}
}

PF_RobotCharacter::PlayerID PF_RobotCharacter::GetPlayerID() const
{
	return m_PlayerID;
}

bool PF_RobotCharacter::IsOut() const
{
	return m_IsOut;
}
