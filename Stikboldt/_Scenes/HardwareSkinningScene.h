#pragma once
#include "GameScene.h"

class ModelComponent;

class HardwareSkinningScene final : public GameScene
{
public:
	HardwareSkinningScene();
	virtual ~HardwareSkinningScene() = default;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

private:
	ModelComponent* m_pModel;
	int m_AnimationIndex = 0;

public:
	HardwareSkinningScene(const HardwareSkinningScene& other) = delete;
	HardwareSkinningScene(HardwareSkinningScene&& other) noexcept = delete;
	HardwareSkinningScene& operator=(const HardwareSkinningScene& other) = delete;
	HardwareSkinningScene& operator=(HardwareSkinningScene&& other) noexcept = delete;
};

