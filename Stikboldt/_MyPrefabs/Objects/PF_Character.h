#pragma once
#include "GameObject.h"

class ControllerComponent;
class CameraComponent;

class PF_Character : public GameObject
{
public:
	enum CharacterMovement_General : UINT
	{
		LEFT = 0,
		RIGHT,
		FORWARD,
		BACKWARD,
		JUMP
	};

	PF_Character(float radius = 2, float height = 5, float moveSpeed = 100, float maxJumpHeight = 50.f, float maxSlopeAngle = 45.f, float maxMoveSpeed = 50.f, float drag = 10.f);
	virtual ~PF_Character() = default;

	void Initialize(const GameContext& gameContext) override;
	void PostInitialize(const GameContext& gameContext) override;
	void Update(const GameContext& gameContext) override;

	void SetMaxSlopeAngle(const float angle);

	CameraComponent* GetCamera() const { return m_pCamera; }
	void ViewMyCamera(bool enabled);

	void EnableGravity(const bool enabled);
	bool IsGrounded() const;
	bool GravityIsEnabled() const;

	void SimpleMove(const DirectX::XMFLOAT3& direction, float amountPerSecond, const GameContext&, bool useLocalAxes = true);
	void SimpleRotateHorizontal(bool goRight, float amountPerSecond, const GameContext&);

	void ApplyAcceleration(const DirectX::XMFLOAT3& direction, float power, const GameContext&);
	void ApplyAcceleration(const DirectX::XMVECTOR& direction, float power, const GameContext&);
	void ApplyAcceleration(float x, float y, float z, float power, const GameContext&);

protected:
	CameraComponent* m_pCamera;
	ControllerComponent* m_pController;

	float m_TotalPitch, m_TotalYaw;
	float m_MoveSpeed, m_RotationSpeed;
	float m_Radius, m_Height;

	float m_MaxJumpHeight;
	float m_SlopeMax; // Cos of the angle defined

	//Running
	float m_MaxRunVelocity, 
		m_TerminalVelocity,
		m_Gravity, 
		m_RunAccelerationTime, 
		m_JumpAccelerationTime, 
		m_RunAcceleration, 
		m_JumpAcceleration, 
		m_RunVelocity, 
		m_JumpVelocity;

	float m_Drag;

	DirectX::XMFLOAT3 m_Velocity;
	DirectX::XMFLOAT3 m_Acceleration;

	DirectX::XMFLOAT2 m_HorVelocity;
	float m_VertVelocity;

	bool m_GravityEnabled;
	bool m_IsGrounded;

private:
	void LoseHorVelocity(const GameContext&);
	void ConsumeVelocity(const GameContext&);

private:
	PF_Character(const PF_Character& other) = delete;
	PF_Character(PF_Character&& other) noexcept = delete;
	PF_Character& operator=(const PF_Character& other) = delete;
	PF_Character& operator=(PF_Character&& other) noexcept = delete;
};


