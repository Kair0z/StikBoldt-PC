#include "stdafx.h"
#include "PF_MultipleTargetCamera.h"
#include <algorithm>

#include "CameraComponent.h"
#include "TransformComponent.h"

PF_MultipleTargetCamera::PF_MultipleTargetCamera()
	: m_Shake{}
{

}

void PF_MultipleTargetCamera::Initialize(const GameContext&)
{
	m_pCamera = new CameraComponent{};
	AddComponent(m_pCamera);
}

void PF_MultipleTargetCamera::Update(const GameContext& gCtx)
{
	m_Shake.Update(gCtx);

	DoShake();
	Move(gCtx.pGameTime->GetElapsed());

	//LookAt(GetCenterPoint());
}

void PF_MultipleTargetCamera::Move(float dt)
{
	if (m_pWatchList.size() == 0) return;

	XMFLOAT3 centerPoint = GetCenterPoint();

	XMFLOAT3 currentPos = GetTransform()->GetPosition();
	XMVECTOR target = XMLoadFloat3(&centerPoint) + XMLoadFloat3(&m_Offset);
	XMFLOAT3 targetPos;
	XMStoreFloat3(&targetPos, target);

	XMFLOAT3 damp = SmoothDamp(currentPos, targetPos, m_Velocity, m_SmoothTime, dt, m_MaxSpeed);
	GetTransform()->Translate
	(
		damp.x + m_ShakeOffset.x, damp.y + m_ShakeOffset.y, damp.z + m_ShakeOffset.z
	);
}

void PF_MultipleTargetCamera::DoShake()
{
	if (!m_Shake.m_IsActive)
	{
		m_ShakeOffset = {};
		return;
	}

	float radius = m_Shake.m_Amount;

	XMFLOAT3 randomPoint{};
	randomPoint.x = randF(-radius, radius);
	randomPoint.y = randF(-radius, radius);
	randomPoint.z = randF(-radius, radius);
	XMVECTOR newPos = XMVector3NormalizeEst(XMLoadFloat3(&randomPoint)) * radius;

	XMStoreFloat3(&m_ShakeOffset, newPos);
}

XMFLOAT3 PF_MultipleTargetCamera::GetCenterPoint() const
{
	if (m_pWatchList.size() == 0) return XMFLOAT3{};
	if (m_pWatchList.size() == 1) return m_pWatchList[0]->GetTransform()->GetPosition();

	XMFLOAT3 averagePos = {};
	XMVECTOR averagePosition = XMLoadFloat3(&averagePos);
	for (GameObject* pObject : m_pWatchList)
	{
		averagePosition += XMLoadFloat3(&pObject->GetTransform()->GetPosition());
	}

	averagePosition /= (float)m_pWatchList.size();

	XMStoreFloat3(&averagePos, averagePosition);
	return averagePos;
}

XMFLOAT3 PF_MultipleTargetCamera::SmoothDamp(const XMFLOAT3& currentPos, const XMFLOAT3& targetPos, XMFLOAT3& out_Velocity, float smoothTime, float dt, float maxSpeed) const
{
	if (smoothTime <= 0.001f) smoothTime = 0.001f;

	float a = (2.f / smoothTime) * dt;
	float b = 1.f / (1.f + a + 0.48f * a * a + 0.235f * a * a * a);

	XMVECTOR diff = XMLoadFloat3(&currentPos) - XMLoadFloat3(&targetPos);
	XMVECTOR target = XMLoadFloat3(&targetPos);
	XMVECTOR current = XMLoadFloat3(&currentPos);

	float c = maxSpeed * smoothTime;

	diff = XMVector3ClampLength(diff, 0.0f, c);
	target = current - diff;

	XMVECTOR calcVelocity = (XMLoadFloat3(&out_Velocity) + ((2.f / smoothTime) * diff)) * dt;
	XMStoreFloat3(&out_Velocity, (b * (XMLoadFloat3(&out_Velocity) - ((2.f / smoothTime) * calcVelocity))));

	XMVECTOR num8 = target + (diff + calcVelocity) * b;
	XMFLOAT3 lengthHolder{};
	XMStoreFloat3(&lengthHolder, XMVector3LengthSq(current - target));

	if (lengthHolder.x > 0.0f == XMVector3Greater(num8, target))
	{
		num8 = target;
		XMStoreFloat3(&out_Velocity, (num8 - target) / dt);
	}

	XMFLOAT3 result{};
	XMStoreFloat3(&result, num8);
	return result;
}

void PF_MultipleTargetCamera::LookAt(const XMFLOAT3& target)
{
	XMVECTOR rotQuat;

	XMVECTOR thisPos = XMLoadFloat3(&GetTransform()->GetPosition());
	XMVECTOR targetPos = XMLoadFloat3(&target);

	XMVECTOR up = XMLoadFloat3(&GetTransform()->GetUp());

	XMMATRIX rotMat = XMMatrixLookAtLH(thisPos, targetPos, up);
	rotQuat = XMQuaternionRotationMatrix(rotMat);
	
	XMFLOAT4 rotQ;
	XMStoreFloat4(&rotQ, rotQuat);
	rotQ.x = -rotQ.x;

	GetTransform()->Rotate(XMLoadFloat4(&rotQ));
}

void PF_MultipleTargetCamera::AddToWatch(GameObject* pObject)
{
	if (IsInWatchList(pObject)) return;
	else m_pWatchList.push_back(pObject);
}

void PF_MultipleTargetCamera::RemoveFromWatch(GameObject* pObject)
{
	auto it = std::find(m_pWatchList.cbegin(), m_pWatchList.cend(), pObject);

	if (it == m_pWatchList.cend()) return; // This object is not in the watchlist
	else m_pWatchList.erase(it); // This object is getting removed from watchlist
}

void PF_MultipleTargetCamera::WatchMe(bool enable)
{
	if (m_pCamera && enable) m_pCamera->SetActive();
}

bool PF_MultipleTargetCamera::IsWatched() const
{
	if (!m_pCamera) return false;
	return m_pCamera->IsActive();
}

CameraComponent* PF_MultipleTargetCamera::GetCameraComponent() const
{
	return m_pCamera;
}

void PF_MultipleTargetCamera::Shake(float duration, float power)
{
	m_Shake.m_IsActive = true;
	m_Shake.m_Amount = power;
	m_Shake.m_ShakeTime = duration;
}

bool PF_MultipleTargetCamera::IsInWatchList(GameObject* pObject) const
{
	auto it = std::find(m_pWatchList.cbegin(), m_pWatchList.cend(), pObject);
	return (it != m_pWatchList.cend());
}

void PF_MultipleTargetCamera::SetOffset(const XMFLOAT3& offset)
{
	m_Offset = offset;
}

void PF_MultipleTargetCamera::SetSmoothTime(float smoothTime)
{
	if (smoothTime <= 0.0f)
		smoothTime = 0.0f;

	m_SmoothTime = smoothTime;
}

void PF_MultipleTargetCamera::SetMaxSpeed(float maxSpeed)
{
	m_MaxSpeed = maxSpeed;
}

#pragma region CameraShake
void PF_MultipleTargetCamera::Shake::Update(const GameContext& ctx)
{
	if (!m_IsActive) return;

	float dt = ctx.pGameTime->GetElapsed();

	m_ShakeTimer += dt;
	
	if (m_ShakeTimer >= m_ShakeTime)
	{
		m_ShakeTimer = 0.0f;
		m_IsActive = false;
	}
}
#pragma endregion
