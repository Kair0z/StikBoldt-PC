#pragma once
#include <GameObject.h>

class CameraComponent;

class PF_OrthographicCamera final : public GameObject
{
public:
	PF_OrthographicCamera() = default;
	virtual ~PF_OrthographicCamera() = default;

	void Initialize(const GameContext&);
	void Update(const GameContext&);

	void WatchMe(bool enable);
	bool IsWatched() const;
	CameraComponent* GetCameraComponent() const;

private:
	CameraComponent* m_pCamera;

};

