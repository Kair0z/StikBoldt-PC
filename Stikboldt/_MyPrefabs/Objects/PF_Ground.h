#pragma once
#include "GameObject.h"

class PF_Ground final : public GameObject
{
public:
	PF_Ground(float groundHeight = 0.f);
	virtual ~PF_Ground() = default;

	float GetHeight() const;

private:
	const float m_Height;

private:
	void Initialize();

public:
	PF_Ground(const PF_Ground& other) = delete;
	PF_Ground(PF_Ground&& other) noexcept = delete;
	PF_Ground& operator=(const PF_Ground& other) = delete;
	PF_Ground& operator=(PF_Ground&& other) noexcept = delete;
};

