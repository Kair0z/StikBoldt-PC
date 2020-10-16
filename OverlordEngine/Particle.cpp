#include "stdafx.h"
#include "Particle.h"

// see https://stackoverflow.com/questions/21688529/binary-directxxmvector-does-not-define-this-operator-or-a-conversion
using namespace DirectX;

Particle::Particle(const ParticleEmitterSettings& emitterSettings):
	m_VertexInfo(ParticleVertex()),
	m_EmitterSettings(emitterSettings),
	m_IsActive(false),
	m_TotalEnergy(0),
	m_CurrentEnergy(0),
	m_SizeGrow(0),
	m_InitSize(0)
{}

void Particle::Init(XMFLOAT3 initPosition, const XMFLOAT4& color )
{
	UNREFERENCED_PARAMETER(initPosition);
	m_IsActive = true;

	m_TotalEnergy = m_CurrentEnergy = randF(m_EmitterSettings.MinEnergy, m_EmitterSettings.MaxEnergy);
	m_GravityVelocity = 0.0f;

	m_VertexInfo.Color = color;

	InitPosition(initPosition);
	InitRotation();
	InitSize();
}

#pragma region Init
void Particle::InitPosition(XMFLOAT3 initPos)
{
	// Position:
	XMFLOAT3 temp = { 1.f, 0.f, 0.f };
	XMVECTOR randomDirection = XMLoadFloat3(&temp);

	XMMATRIX randomRotMatrix = XMMatrixRotationRollPitchYaw(
		randF(-XM_PI, XM_PI),
		randF(-XM_PI, XM_PI),
		randF(-XM_PI, XM_PI));

	randomDirection = XMVector3TransformNormal(randomDirection, randomRotMatrix);

	XMVECTOR randomPos = XMLoadFloat3(&initPos) + randomDirection * randF(m_EmitterSettings.MinEmitterRange, m_EmitterSettings.MaxEmitterRange);

	XMStoreFloat3(&m_VertexInfo.Position, randomPos);
}

void Particle::InitRotation()
{
	m_VertexInfo.Rotation = randF(-XM_PI, XM_PI);
}

void Particle::InitSize()
{
	m_VertexInfo.Size = m_InitSize = randF(m_EmitterSettings.MinSize, m_EmitterSettings.MaxSize);
	m_SizeGrow = randF(m_EmitterSettings.MinSizeGrow, m_EmitterSettings.MaxSizeGrow);
}
#pragma endregion

void Particle::Deactivate()
{
	m_IsActive = false;
	m_GravityVelocity = 0.0f;
}

void Particle::Update(const GameContext& gameContext)
{
	UNREFERENCED_PARAMETER(gameContext);

	if (!m_IsActive) return;

	float elapsedSec = gameContext.pGameTime->GetElapsed();
	m_CurrentEnergy -= elapsedSec;

	if (m_CurrentEnergy < 0.0f)
	{
		Deactivate();
		return;
	}

	UpdatePosition(elapsedSec);
	UpdateColor(elapsedSec);
	UpdateSize(elapsedSec);
}

#pragma region Updating
void Particle::UpdatePosition(float dt)
{
	XMVECTOR newPos = XMLoadFloat3(&m_VertexInfo.Position) + XMLoadFloat3(&m_EmitterSettings.Velocity) * dt;

	if (m_EmitterSettings.EnableGravity)
	{
		m_GravityVelocity += -9.81f * m_EmitterSettings.Mass * dt;
		XMFLOAT3 gravVelocity = { 0.f, m_GravityVelocity, 0.f };

		newPos += XMLoadFloat3(&gravVelocity) * dt;
	}
	
	XMStoreFloat3(&m_VertexInfo.Position, newPos);
}

void Particle::UpdateColor(float dt)
{
	dt;
	float lifePercent = m_CurrentEnergy / m_TotalEnergy;

	m_VertexInfo.Color.w = lifePercent * 2.f;
}

void Particle::UpdateSize(float dt)
{
	dt;
	float safeGrow = m_SizeGrow;
	if (m_SizeGrow <= 0.f) safeGrow = 0.1f; // Cap at 0.1f shrinking factor

	float lifePercent = m_CurrentEnergy / m_TotalEnergy;

	m_VertexInfo.Size = m_InitSize + safeGrow * lifePercent;
}
#pragma endregion
