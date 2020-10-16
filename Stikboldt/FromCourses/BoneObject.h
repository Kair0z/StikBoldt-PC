#pragma once
#include "GameObject.h"

struct GameContext;

class BoneObject : public GameObject
{
public:
	BoneObject(const int ID, const int materialID, const float length = 5.f);
	virtual ~BoneObject() = default;

	void AddBone(BoneObject* pBone);

	// BindPose
	DirectX::XMFLOAT4X4 GetBindPose() const;
	void CalculateBindPose();

	// Length
	float GetLength() const;
	void SetLength(const float newLength);

protected:
	virtual void Initialize(const GameContext&);

private:
	float m_Length;
	int m_BoneID;
	int m_MaterialID;

	DirectX::XMFLOAT4X4 m_BindPose;

	GameObject* m_pThisBone;

public: // ROF
	BoneObject(const BoneObject&) = delete;
	BoneObject(BoneObject&&) = delete;
	BoneObject& operator=(const BoneObject&) = delete;
	BoneObject& operator=(BoneObject&&) = delete;
};

