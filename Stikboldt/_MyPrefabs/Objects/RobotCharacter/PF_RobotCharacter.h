#pragma once
#include "../MyPrefabs_ObjectPrefabs.h"

class ModelComponent;
class ColliderComponent;
class RigidBodyComponent;
class SpherePrefab;
class PF_TheBall;
class ModelAnimator;
class ParticleEmitterComponent;


using namespace DirectX;
class PF_RobotCharacter : public GameObject
{
#pragma region helpers
	struct MovementData
	{
		float m_AccPower = 600.f;
		XMFLOAT2 m_Acceleration = {};

		XMFLOAT2 m_Velocity = {};
		float m_MaxVelocity = 40.f;

		bool m_IsDashing = false;
		float m_DashTime = 0.16f;
		float m_DashTimer = 0.0f;
		XMFLOAT2 m_DashDirection = {};
		float m_DashSpeed = 150.f;

		float m_PickupSlowdown = 15.f;
		float m_StunSlowdown = 15.f;

		float m_Drag = 150.f;

		void Reset()
		{
			m_Acceleration = {};
			m_Velocity = {};

			m_IsDashing = false;
			m_DashTimer = 0.f;
			m_DashDirection = {};
		}
	};

	struct ActionCooldownData
	{
		void StartHitCooldown();
		bool m_HitOnCooldown = false;
		float m_HitCooldownTimer = 0.0f;
		const float m_HitCooldownTime = 1.f;

		void StartDashCooldown();
		bool m_DashOnCooldown = false;
		float m_DashCooldownTimer = 0.0f;
		const float m_DashCooldownTime = 1.f;

		void StartCatchCooldown();
		bool m_HasTriedCatch = false;
		bool m_CatchOnCooldown = false;
		float m_CatchCooldownTimer = 0.0f;
		const float m_CatchCooldownTime = 1.f;

		void StartStun();
		bool m_IsStunned = false;
		float m_StunnedTimer = 0.0f;
		const float m_StunnedTime = 5.f;

		void UpdateCooldowns(const GameContext&);
		void Reset()
		{
			m_HitOnCooldown = false;
			m_HitCooldownTimer = 0.f;
			m_DashOnCooldown = false;
			m_DashCooldownTimer = 0.f;
			m_HasTriedCatch = false;
			m_CatchOnCooldown = false;
			m_CatchCooldownTimer = 0.f;
			m_IsStunned = false;
			m_StunnedTimer = 0.f;
		}
	};

	struct ActionBiasData // An action should have a lasting effect for more than just 1 frame (else pretty unplayable)
	{
		bool m_HitActive = false;
		float m_HitActiveTimer = 0.0f;
		float m_HitActiveTime = 0.5f;

		bool m_CatchActive = false;
		float m_CatchActiveTimer = 0.0f;
		float m_CatchActiveTime = 0.5f;

		void UpdateBiases(const GameContext&);
		void Reset()
		{
			m_HitActive = false;
			m_HitActiveTimer = 0.0f;
			m_CatchActive = false;
			m_CatchActiveTimer = 0.0f;
		}
	};

	enum class BallEnterState
	{
		ReadyToCatch,
		ReadyToHit,
		NotReady
	};

	enum class Controls 
		// Rest is done in code without trigger-enums (value-based)
	{
		Hit = 1,
		Dash = 2
	};

	enum class AnimationState
	{
		Idle,
		Running,
		Hitting,
		Dashing,
		Catching,
		Stunned,
		Clapping,
		Dancing,
		IsHit,
		KnockedOut,
		StoppingRun
	};

#define INVINCIBLE_GROUPFLAG CollisionGroupFlag(1 << 8)

#pragma endregion

public:
	using PlayerID = short;

	PF_RobotCharacter();
	virtual ~PF_RobotCharacter() = default;

	void SetPlayerID(PlayerID id);
	PlayerID GetPlayerID() const;

	bool IsOut() const;

	void ApplyAcceleration(const DirectX::XMFLOAT3& direction, float power, const GameContext&);
	void ApplyAcceleration(const DirectX::XMVECTOR& direction, float power, const GameContext&);
	void ApplyAcceleration(float x, float y, float z, float power, const GameContext&);

	void ProcessBallContact(PF_TheBall* pTheBall);

	void Reset();

private:
	// GameObject Overrrides:
	virtual void Initialize(const GameContext&) override;
	virtual void Update(const GameContext&) override;

	// Data:
	ModelComponent* m_pModelComp;
	ModelAnimator* m_pAnimator;
	ColliderComponent* m_pCollider;
	RigidBodyComponent* m_pRigidBody;
	ParticleEmitterComponent* m_pParticles;
	MovementData m_MovementData;
	PlayerID m_PlayerID = -1;
	XMFLOAT4 m_QRotation;

	// Socket:
	GameObject* m_pBallSocket;
	float m_SocketRotationSpeed;

	float m_Height;
	float m_Radius;

	BallEnterState m_BallEnterState;
	AnimationState m_AnimationState;

	bool m_IsOut;

	// Ball pickup:
	PF_TheBall* m_pTheBall;
	bool m_BallPickedUp;

	// Cooldowns:
	ActionCooldownData m_CoolDownData;
	ActionBiasData m_BiasData;

	// Methods:
	void InitMesh(const GameContext&);
	void InitInput(const GameContext&);
	void InitHitbox(const GameContext&);
	void InitSocket(const GameContext&);
	void InitParticles(const GameContext&);

	void UpdateAnimation(const GameContext&);
	void UpdateParticles(const GameContext&);

	void ProcessMovement(const GameContext&);
		void Move(const GameContext&);
		void Drag(const GameContext&);
		void Dash(const GameContext&);

	void ProcessInput(const GameContext&);
		void TryDash(const GameContext&);
		void TryHit(const GameContext&);
		void TryCatch(const GameContext&);
		void TryThrow(const GameContext&);

	void ProcessRotation(const GameContext&);
	void MatchBallToSocket(const GameContext&);
	void AdjustSocketToDirection();
	void RotateSocket(const DirectX::XMFLOAT2& thumbStickPos, const GameContext&);

	// Processing ball contact:
	void CatchBall(PF_TheBall* pTheBall);
	void HitBall(PF_TheBall* pTheBall);
	void GetHitByBall(PF_TheBall* pTheBall);

public:
	PF_RobotCharacter(const PF_RobotCharacter&) = delete;
	PF_RobotCharacter(PF_RobotCharacter&&) = delete;
	PF_RobotCharacter& operator=(const PF_RobotCharacter&) = delete;
	PF_RobotCharacter& operator=(PF_RobotCharacter&&) = delete;
};

