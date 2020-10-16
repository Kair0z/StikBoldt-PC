#include "stdafx.h"
#include "CameraComponent.h"
#include "OverlordGame.h"
#include "TransformComponent.h"
#include "PhysxProxy.h"
#include "GameObject.h"
#include "GameScene.h"

#include <iostream>

CameraComponent::CameraComponent() :
	m_FarPlane(2500.0f),
	m_NearPlane(0.1f),
	m_FOV(DirectX::XM_PIDIV4),
	m_Size(25.0f),
	m_IsActive(false),
	m_PerspectiveProjection(true)
{
	XMStoreFloat4x4(&m_Projection, DirectX::XMMatrixIdentity());
	XMStoreFloat4x4(&m_View, DirectX::XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewInverse, DirectX::XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjection, DirectX::XMMatrixIdentity());
	XMStoreFloat4x4(&m_ViewProjectionInverse, DirectX::XMMatrixIdentity());
}

void CameraComponent::Initialize(const GameContext&) {}

void CameraComponent::Update(const GameContext&)
{
	// see https://stackoverflow.com/questions/21688529/binary-directxxmvector-does-not-define-this-operator-or-a-conversion
	using namespace DirectX;

	const auto windowSettings = OverlordGame::GetGameSettings().Window;
	DirectX::XMMATRIX projection;

	if (m_PerspectiveProjection)
	{
		projection = DirectX::XMMatrixPerspectiveFovLH(m_FOV, windowSettings.AspectRatio, m_NearPlane, m_FarPlane);
	}
	else
	{
		const float viewWidth = (m_Size > 0) ? m_Size * windowSettings.AspectRatio : windowSettings.Width;
		const float viewHeight = (m_Size > 0) ? m_Size : windowSettings.Height;
		projection = DirectX::XMMatrixOrthographicLH(viewWidth, viewHeight, m_NearPlane, m_FarPlane);
	}

	const DirectX::XMVECTOR worldPosition = XMLoadFloat3(&GetTransform()->GetWorldPosition());
	const DirectX::XMVECTOR lookAt = XMLoadFloat3(&GetTransform()->GetForward());
	const DirectX::XMVECTOR upVec = XMLoadFloat3(&GetTransform()->GetUp());

	const DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(worldPosition, worldPosition + lookAt, upVec);
	const DirectX::XMMATRIX viewInv = XMMatrixInverse(nullptr, view);
	const DirectX::XMMATRIX viewProjectionInv = XMMatrixInverse(nullptr, view * projection);

	XMStoreFloat4x4(&m_Projection, projection);
	XMStoreFloat4x4(&m_View, view);
	XMStoreFloat4x4(&m_ViewInverse, viewInv);
	XMStoreFloat4x4(&m_ViewProjection, view * projection);
	XMStoreFloat4x4(&m_ViewProjectionInverse, viewProjectionInv);
}

void CameraComponent::Draw(const GameContext&) {}

void CameraComponent::SetActive()
{
	auto gameObject = GetGameObject();
	if (gameObject == nullptr)
	{
		Logger::LogError(L"[CameraComponent] Failed to set active camera. Parent game object is null");
		return;
	}

	auto gameScene = gameObject->GetScene();
	if (gameScene == nullptr)
	{
		Logger::LogError(L"[CameraComponent] Failed to set active camera. Parent game scene is null");
		return;
	}

	gameScene->SetActiveCamera(this);
}

GameObject* CameraComponent::Pick(const GameContext& gameContext, CollisionGroupFlag ignoreGroups) const
{
	using namespace DirectX;

	// Get Viewport
	D3D11_VIEWPORT viewport{};
	UINT amountViewports{1};
	gameContext.pDeviceContext->RSGetViewports(&amountViewports, &viewport);
	float halfWidth = viewport.Width / 2.f;
	float halfHeight = viewport.Height / 2.f;

	// Mouse coordinates to NDC
	POINT mousePos = gameContext.pInput->GetMousePosition();

	float ndcX = (mousePos.x - halfWidth) / halfWidth;
	float ndcY = (halfHeight - mousePos.y) / halfHeight;

	// Calculate nearpoint & farpoint (ray start & direction)
	XMFLOAT4 nearP{ndcX, ndcY, 0.0f, 0.0f};
	XMFLOAT4 farP{ndcX, ndcY, 1.f, 0.f};

	// Setup XM-calculation-variables:
	XMVECTOR nearPVector = XMLoadFloat4(&nearP);
	XMVECTOR farPVector = XMLoadFloat4(&farP);
	XMMATRIX invViewProj = XMLoadFloat4x4(&gameContext.pCamera->GetViewProjectionInverse());
	 
	// Transform the vectors with the inv-viewprojection
	nearPVector = XMVector3TransformCoord(nearPVector, invViewProj);
	farPVector = XMVector3TransformCoord(farPVector, invViewProj);

	// Physx Raycast
	XMFLOAT4 rayStart{}, rayDir{};
	XMStoreFloat4(&rayStart, nearPVector);
	XMStoreFloat4(&rayDir, (farPVector - nearPVector));

	physx::PxVec3 pxRayStart(rayStart.x, rayStart.y, rayStart.z);
	physx::PxVec3 pxRayDir(rayDir.x, rayDir.y, rayDir.z);
	physx::PxVec3 pxRayDirNorm = pxRayDir.getNormalized();

	physx::PxQueryFilterData filterData;
	filterData.data.word0 = ~physx::PxU32(ignoreGroups);
	physx::PxRaycastBuffer hit;

	if (gameContext.pPhysxProxy->Raycast(pxRayStart, pxRayDirNorm, PX_MAX_F32, hit, physx::PxHitFlag::eDEFAULT, filterData))
	{
		if (hit.hasBlock)
		{
			physx::PxRigidActor* hitActor = hit.block.actor;
			return static_cast<BaseComponent*>(hitActor->userData)->GetGameObject();
		}
	}
	

	// Shake, no stir!
		
	//TODO implement
	return nullptr;
}