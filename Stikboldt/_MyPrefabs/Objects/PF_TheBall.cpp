#include "stdafx.h"
#include "PF_TheBall.h"

#include "RigidBodyComponent.h"
#include "ColliderComponent.h"
#include "ModelComponent.h"
#include "TransformComponent.h"
#include "ParticleEmitterComponent.h"

#include "../MyPrefabs.h"
#include "PhysxManager.h"
#include "../OverlordProject/_Materials/Materials.h"
#include "../UI/PF_GymUI.h"

#define BALL_PHYSICS_VELOCITY_CAP 300.f

PF_TheBall::PF_TheBall(float size)
	: m_pRigidBody{nullptr}
	, m_pCollider{nullptr}
	, m_pModel{nullptr}
	, m_Size{size}
	, m_State{State::Neutral}
	, m_HasNewHit{false}
	, m_NewHit{0.0f, 0.0f, 0.f}
	, m_pBotInRange{nullptr}
	, m_pSafeRobot{nullptr}
	, m_SpeedBeforePickup{}
	, m_HasNewThrow{false}
	, m_NewThrow{}
	, m_MinThrowSpeed{70.f}
	, m_MinHitSpeed{70.f}
{}

void PF_TheBall::Initialize(const GameContext& context)
{
	GameObject* pGO = new GameObject{};
	m_pModel = new ModelComponent
	{
		L"Resources/Models/Ball.ovm", false
	};
	m_pModel->SetMaterial((int)ActiveMaterials::UberMatte);

	pGO->AddComponent(m_pModel);
	AddChild(pGO);
	pGO->GetTransform()->Scale(0.2f, 0.2f, 0.2f);

	m_pRigidBody = new RigidBodyComponent{};
	
	m_pRigidBody->SetConstraint(RigidBodyConstraintFlag::TransY, true);
	AddComponent(m_pRigidBody);

	std::shared_ptr<physx::PxGeometry> geometry(new physx::PxSphereGeometry{ m_Size });
	m_pCollider = new ColliderComponent
	{
		geometry,
		*PhysxManager::GetInstance()->GetPhysics()->createMaterial(0.0f, 0.f, 1.f)
	};

	std::shared_ptr<physx::PxGeometry> biggerGeom(new physx::PxSphereGeometry{ m_Size + 3.f });
	m_pTrigger = new ColliderComponent
	{
		biggerGeom,
		*PhysxManager::GetInstance()->GetPhysics()->createMaterial(0.0f, 0.0f, 1.f)
	};
	m_pTrigger->EnableTrigger(true);
	std::function<void(GameObject*, GameObject*, TriggerAction)> func = std::bind(&PF_TheBall::OnTrigger, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	SetOnTriggerCallBack(func);

	m_pRigidBody->SetCollisionIgnoreGroups(INVINCIBLE_GROUPFLAG);

	AddComponent(m_pCollider);
	AddComponent(m_pTrigger);

	SetState(State::Neutral);

	InitParticles(context);
}

void PF_TheBall::Reset()
{
	m_pModel->SetMaterial((int)ActiveMaterials::UberMatte);
	SetState(State::Neutral);

	m_pRigidBody->SetKinematic(false);
	m_pRigidBody->GetPxRigidDynamic()->setLinearVelocity({ 0.f, 0.f, 0.f });
	GetTransform()->Translate(0.f, 0.f, 0.f);
}

#pragma region Init
void PF_TheBall::InitParticles(const GameContext&)
{
	m_pTrailParticles = new ParticleEmitterComponent(L"Resources/Textures/Particles/trail.png", 15);
	m_pTrailParticles->SetColor({ 1.f, 0.f, 0.f, 1.f });
	m_pTrailParticles->SetGravity(false);
	m_pTrailParticles->SetMaxEnergy(1.f);
	m_pTrailParticles->SetMinEnergy(1.f);
	m_pTrailParticles->SetMaxSizeGrow(1.f);
	m_pTrailParticles->SetMinSizeGrow(1.f);
	m_pTrailParticles->SetMinSize(1.f);
	m_pTrailParticles->SetMaxSize(1.f);
	m_pTrailParticles->SetMaxEmitterRange(1.f);
	m_pTrailParticles->SetMinEmitterRange(1.f);
	m_pTrailParticles->SetVelocity({ 0.f, 0.f, 0.f });

	m_pHitParticles = new ParticleEmitterComponent(L"Resources/Textures/Particles/trail.png", 15);
	m_pHitParticles->SetGravity(true);
	m_pHitParticles->SetMass(8.f);
	m_pHitParticles->SetMaxEnergy(1.f);
	m_pHitParticles->SetMinEnergy(1.f);
	m_pHitParticles->SetMaxSizeGrow(1.f);
	m_pHitParticles->SetMinSizeGrow(1.f);
	m_pHitParticles->SetMinSize(.7f);
	m_pHitParticles->SetMaxSize(1.f);
	m_pHitParticles->SetMaxEmitterRange(6.f);
	m_pHitParticles->SetMinEmitterRange(4.f);
	m_pHitParticles->SetVelocity({ 0.f, 40.f, 0.f });

	m_pHitParticles->Activate(false);
	m_pTrailParticles->Activate(false);

	AddComponent(m_pTrailParticles);
	AddComponent(m_pHitParticles);
}

#pragma endregion

void PF_TheBall::Update(const GameContext& context)
{
	ProcessNewHit(context);
	ProcessNewThrow(context);
	ProcessBotInRange(context);
	ProcessParticles(context);
	ProcessSpeed(context);
	ProcessSafetyDistance(context);
}

#pragma region Update
void PF_TheBall::ProcessNewHit(const GameContext&)
{
	if (m_HasNewHit)
	{
		physx::PxVec3 curVelocity = m_pRigidBody->GetPxRigidDynamic()->getLinearVelocity();
		float initMagnitude = curVelocity.magnitude();

		m_pRigidBody->GetPxRigidDynamic()->setLinearVelocity({ 0.f, 0.f, 0.f });

		XMVECTOR newHitLength = XMVector3LengthEst(XMLoadFloat3(&m_NewHit));
		XMFLOAT3 lengths{};
		XMStoreFloat3(&lengths, newHitLength);

		if (initMagnitude <= m_MinHitSpeed) initMagnitude = m_MinHitSpeed;
		XMVECTOR newVel = (XMVector3Normalize(XMLoadFloat3(&m_NewHit)) * initMagnitude) * lengths.x;
		XMFLOAT3 newVelocity{};
		XMStoreFloat3(&newVelocity, newVel);
		m_pRigidBody->GetPxRigidDynamic()->setLinearVelocity({ newVelocity.x, newVelocity.y, newVelocity.z });
		m_HasNewHit = false;
	}
}

void PF_TheBall::ProcessNewThrow(const GameContext&)
{
	if (m_HasNewThrow)
	{
		m_pRigidBody->GetPxRigidDynamic()->setLinearVelocity({ 0.f, 0.f, 0.f });
		float speed = m_SpeedBeforePickup;
		if (speed < m_MinThrowSpeed) speed = m_MinThrowSpeed; // Set min throw velocity:

		XMVECTOR newVel = XMVector3NormalizeEst(XMLoadFloat3(&m_NewThrow)) * speed;
		XMFLOAT3 newVelocity;
		XMStoreFloat3(&newVelocity, newVel);
		m_pRigidBody->GetPxRigidDynamic()->setLinearVelocity({ newVelocity.x, newVelocity.y, newVelocity.z });
		m_HasNewThrow = false;
	}
}

void PF_TheBall::ProcessBotInRange(const GameContext&)
{
	if (!m_pBotInRange) return;

	m_pBotInRange->ProcessBallContact(this);
}

void PF_TheBall::ProcessParticles(const GameContext& context)
{
	m_HitParticlesBurst.Update(context);
	m_pHitParticles->Activate(m_HitParticlesBurst.m_IsActive);

	switch (m_State)
	{
	case PF_TheBall::State::Neutral:
		m_pTrailParticles->Activate(false);
		break;

	case PF_TheBall::State::Volatile:
		m_pTrailParticles->Activate(true);
		XMFLOAT3 botCol = GetBotColor(m_pSafeRobot);
		XMFLOAT4 botColor = { botCol.x, botCol.y, botCol.z, 1.f };
		m_pTrailParticles->SetColor(botColor);
		break;

	case PF_TheBall::State::PickedUp:
		m_pTrailParticles->Activate(false);
		break;

	default:
		break;
	}
}

void PF_TheBall::ProcessSpeed(const GameContext&)
{
	auto ui = UILocator::Locate();
	float speed = m_pRigidBody->GetPxRigidDynamic()->getLinearVelocity().magnitude();
	if (ui && m_State != State::PickedUp) ui->UpdateBallSpeed(speed);
	if (speed < 10.f && m_State == State::Volatile) SetState(State::Neutral);

	if (speed > BALL_PHYSICS_VELOCITY_CAP)
	{
		if (!m_pRigidBody->IsKinematic()) m_pRigidBody->GetPxRigidDynamic()->setLinearVelocity(m_pRigidBody->GetPxRigidDynamic()->getLinearVelocity().getNormalized() * BALL_PHYSICS_VELOCITY_CAP);
	}
}

void PF_TheBall::ProcessSafetyDistance(const GameContext&)
{
	XMVECTOR distanceFromOriginSqr = XMVector3LengthSq(XMLoadFloat3(&GetTransform()->GetWorldPosition()));
	XMFLOAT3 distances;
	XMStoreFloat3(&distances, distanceFromOriginSqr);

	if (distances.x > 100.f * 100.f) Reset();
}

#pragma endregion

void PF_TheBall::OnTrigger(GameObject*, GameObject* pOtherObj, TriggerAction action)
{
	PF_RobotCharacter* pNewBot = dynamic_cast<PF_RobotCharacter*>(pOtherObj);
	if (!pNewBot) return;

	if (m_State != State::PickedUp)
	{
		auto pCamera = MainCameraLocator::Locate();
		if (pCamera)
		{
			pCamera->Shake(0.1f, 0.3f);
		}
	}
	
	// If there is already a bot, overwrite that one
	bool overwrite = true;
	if (action == TriggerAction::ENTER)
	{
		if (overwrite) m_pBotInRange = pNewBot;
		else if (!overwrite && !m_pBotInRange) m_pBotInRange = pNewBot;
	}

	else m_pBotInRange = nullptr;
}

PF_TheBall::State PF_TheBall::GetState() const
{
	return m_State;
}

void PF_TheBall::SetState(State state)
{
	m_State = state;

	if (state == State::PickedUp)
	{
		auto curVelocity = m_pRigidBody->GetPxRigidDynamic()->getLinearVelocity();
		m_SpeedBeforePickup = curVelocity.magnitude();
		m_pRigidBody->SetKinematic(true);
	}
	else m_pRigidBody->SetKinematic(false);

	if (state == State::Neutral)
	{
		m_pModel->SetMaterial((int)ActiveMaterials::UberMatte);
	}
}

void PF_TheBall::Release(const DirectX::XMFLOAT3& dir, PF_RobotCharacter* pThrower)
{
	SetState(State::Volatile); // Sets rigidbody non-kinematic!

	m_HasNewThrow = true;
	m_pSafeRobot = pThrower;
	
	XMFLOAT3 newThrowHor = { dir.x, 0.f, dir.z };
	m_NewThrow = newThrowHor;

	MatchBotColor(pThrower);
}

void PF_TheBall::GetHit(const XMFLOAT3& newHitDir, PF_RobotCharacter* pRobot, float hitMultiplier)
{
	m_HasNewHit = true;
	m_pSafeRobot = pRobot;

	XMFLOAT3 newHitHor = { newHitDir.x, 0.f, newHitDir.z };
	XMVECTOR newHit = XMLoadFloat3(&newHitHor) * hitMultiplier;
	XMStoreFloat3(&m_NewHit, newHit);

	MatchBotColor(pRobot);
}

void PF_TheBall::ProcessVolatileHit()
{
	SetState(State::Neutral);

	if (!m_pSafeRobot) return;

	m_HitParticlesBurst.m_IsActive = true;
	m_HitParticlesBurst.m_Timer = 0.f;

	XMFLOAT3 botColor = GetBotColor(m_pSafeRobot);
	m_pHitParticles->SetColor({ botColor.x, botColor.y, botColor.z, 1.f });
}

PF_RobotCharacter* PF_TheBall::GetSafeRobot() const
{
	return m_pSafeRobot;
}

void PF_TheBall::MatchBotColor(PF_RobotCharacter* pBot)
{
	switch (pBot->GetPlayerID())
	{
	case 0:
		m_pModel->SetMaterial((int)ActiveMaterials::Ball0);
		break;

	case 1:
		m_pModel->SetMaterial((int)ActiveMaterials::Ball1);
		break;

	case 2:
		m_pModel->SetMaterial((int)ActiveMaterials::Ball2);
		break;

	case 3:
		m_pModel->SetMaterial((int)ActiveMaterials::Ball3);
		break;
	}
}

XMFLOAT3 PF_TheBall::GetBotColor(PF_RobotCharacter* pBot)
{
	if (!pBot) return {};

	switch (pBot->GetPlayerID())
	{
	case 0: return ActiveColors::gPlayer0;

	case 1: return ActiveColors::gPlayer1;

	case 2: return ActiveColors::gPlayer2;

	case 3: return ActiveColors::gPlayer3;
	}

	return { 0.f, 0.f, 0.f };
}

