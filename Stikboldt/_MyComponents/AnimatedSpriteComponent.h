#pragma once
#include <BaseComponent.h>
#include <vector>

class TextureData;

class AnimatedSpriteComponent final : public BaseComponent
{
	struct AnimatorData
	{
		bool m_IsRunning = true;
		float m_AccumTime = 0.f;
		float m_FrameInterval = 1.f;
		size_t m_CurrentFrame = 0;
		size_t m_TotalFrames = 0;
	};

public:
	AnimatedSpriteComponent(std::wstring spriteMap, DirectX::XMFLOAT2 pivot = DirectX::XMFLOAT2(0, 0),
		DirectX::XMFLOAT4 color = {1.f, 1.f, 1.f, 1.f}, float depth = 0.3f, float frameInterval = 1.f);

	virtual void Initialize(const GameContext& gameContext) override;
	virtual void Update(const GameContext& gameContext) override;
	virtual void Draw(const GameContext& gameContext) override;

	void LoadSpriteMap(const std::wstring& filepath);

	DirectX::XMFLOAT2 GetPivot() const { return m_Pivot; }
	DirectX::XMFLOAT4 GetColor() const { return m_Color; }

	void SetPivot(DirectX::XMFLOAT2 pivot) { m_Pivot = pivot; }
	void SetColor(DirectX::XMFLOAT4 color) { m_Color = color; }

	void SetPause(bool pause);
	bool IsPaused() const;

	void SetFrameInterval(float interval);
	float GetFrameInterval() const;

	void SetDepth(const float depth) { m_Depth = depth; }

private:
	std::vector<TextureData*> m_pFrames;
	std::wstring m_SpriteMap;
	bool m_MultipleFrames;

	AnimatorData m_Animation;

	DirectX::XMFLOAT2 m_Pivot;
	DirectX::XMFLOAT4 m_Color;
	float m_Depth;
};

