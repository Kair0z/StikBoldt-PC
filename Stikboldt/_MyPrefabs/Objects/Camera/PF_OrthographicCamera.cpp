#include "stdafx.h"
#include "PF_OrthographicCamera.h"

#include "CameraComponent.h"

void PF_OrthographicCamera::Initialize(const GameContext&)
{
	m_pCamera = new CameraComponent{};

	m_pCamera->UseOrthographicProjection();
	m_pCamera->SetFarClippingPlane(100.f);
	m_pCamera->SetNearClippingPlane(1.0f);
	AddComponent(m_pCamera);
}

void PF_OrthographicCamera::Update(const GameContext&)
{

}

void PF_OrthographicCamera::WatchMe(bool enable)
{
	if (enable) m_pCamera->SetActive();
}

bool PF_OrthographicCamera::IsWatched() const
{
	return m_pCamera->IsActive();
}

CameraComponent* PF_OrthographicCamera::GetCameraComponent() const
{
	return m_pCamera;
}
