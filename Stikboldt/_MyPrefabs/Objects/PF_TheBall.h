#pragma once
#include <GameObject.h>

class RigidBodyComponent;
class ColliderComponent;
class PF_RobotCharacter;
class ModelComponent;
class ParticleEmitterComponent;

using namespace DirectX;

class PF_TheBall : public GameObject
{
	struct BurstParticleSettings
	{
		bool m_IsActive = false;
		float m_Timer = 0.f;
		float m_Time = 0.3f;

		void Update(const GameContext& context)
		{
			if (!m_IsActive) return;

			m_Timer += context.pGameTime->GetElapsed();
			
			if (m_Timer > m_Time)
			{
				m_Timer = 0.f;
				m_IsActive = false;
			}
		}
	};

public:
	enum class State
	{
		Neutral,
		Volatile,
		PickedUp
	};

	PF_TheBall(float size);
	virtual ~PF_TheBall() = default;

	State GetState() const;
	void SetState(State state);

	// Release adds no additional speed, GetHit does:
	void Release(const DirectX::XMFLOAT3& dir, PF_RobotCharacter* pThrower);
	void GetHit(const DirectX::XMFLOAT3& dir, PF_RobotCharacter* pHitter, float hitPower = 3.f);

	void ProcessVolatileHit();

	PF_RobotCharacter* GetSafeRobot() const;

	void MatchBotColor(PF_RobotCharacter* pBot);
	XMFLOAT3 GetBotColor(PF_RobotCharacter* pBot);

	void Reset();

private:
	void Initialize(const GameContext&) override;
		void InitParticles(const GameContext&);

	void Update(const GameContext&) override;
		void ProcessNewHit(const GameContext&);
		void ProcessNewThrow(const GameContext&);
		void ProcessBotInRange(const GameContext&);
		void ProcessParticles(const GameContext&);
		void ProcessSpeed(const GameContext&);
		void ProcessSafetyDistance(const GameContext&);

	void OnTrigger(GameObject* pTriggerObj, GameObject* pOtherObj, TriggerAction action);

	float m_Size;

	ModelComponent* m_pModel;
	RigidBodyComponent* m_pRigidBody;
	ColliderComponent* m_pCollider;
	ColliderComponent* m_pTrigger;
	ParticleEmitterComponent* m_pTrailParticles;
	ParticleEmitterComponent* m_pHitParticles;
	BurstParticleSettings m_HitParticlesBurst;
	State m_State;

	bool m_HasNewHit;
	bool m_HasNewThrow;
	XMFLOAT3 m_NewHit;
	XMFLOAT3 m_NewThrow;
	float m_SpeedBeforePickup;
	float m_MinThrowSpeed;
	float m_MinHitSpeed;

	// Reference to the  bot in range:
	PF_RobotCharacter* m_pBotInRange;
	PF_RobotCharacter* m_pSafeRobot;
};

