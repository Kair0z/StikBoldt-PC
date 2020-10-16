#pragma once
#include "GameObject.h"
#include <vector>
#include "../OverlordProject/_MyHelpers/ILocator.h"

class CameraComponent;

using namespace DirectX;

class PF_MultipleTargetCamera final : public GameObject
{
	struct Shake
	{
		void Update(const GameContext&);
		bool m_IsActive = false;
		float m_Amount = 1.f;

		float m_ShakeTimer = 0.0f;
		float m_ShakeTime = 1.f;
	};
	Shake m_Shake;

public:
	PF_MultipleTargetCamera();
	virtual ~PF_MultipleTargetCamera() = default;

	virtual void Initialize(const GameContext&) override;
	virtual void Update(const GameContext&) override;

	void AddToWatch(GameObject* pObject);
	void RemoveFromWatch(GameObject* pObject);
	bool IsInWatchList(GameObject* pObject) const;

	void SetOffset(const XMFLOAT3& offset);
	void SetSmoothTime(float smoothTime);
	void SetMaxSpeed(float maxSpeed);

	void WatchMe(bool enable);
	bool IsWatched() const;
	CameraComponent* GetCameraComponent() const;
	void Shake(float duration, float power);

private:
	CameraComponent* m_pCamera = nullptr;
	std::vector<GameObject*> m_pWatchList;

	XMFLOAT3 m_Offset = {};
	XMFLOAT3 m_ShakeOffset = {};
	XMFLOAT3 m_Velocity = {};
	float m_SmoothTime = 0.5f;
	float m_MaxSpeed = 50.f;

	void Move(float dt);
	void DoShake();
	XMFLOAT3 GetCenterPoint() const;
	XMFLOAT3 SmoothDamp(const XMFLOAT3& currentPos, const XMFLOAT3& targetPos, XMFLOAT3& out_Velocity, float smoothTime, float dt, float maxSpeed = 100.f) const;
	void LookAt(const XMFLOAT3& targetPos);
};

class MainCameraLocator : public ILocator<PF_MultipleTargetCamera>{};

