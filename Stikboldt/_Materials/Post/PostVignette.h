#pragma once
#include "PostProcessingMaterial.h"

using namespace DirectX;
class ID3D11EffectShaderResourceVariable;
struct ID3DX11EffectScalarVariable;
struct ID3DX11EffectVectorVariable;

class PostVignette final : public PostProcessingMaterial
{
public:
	PostVignette();
	virtual ~PostVignette() = default;

	void SetColor(const XMFLOAT3& color);
	void SetIntensity(const float intensity);

private:
	void LoadEffectVariables() override;
	void UpdateEffectVariables(RenderTarget* pRendertarget) override;

	XMFLOAT3 m_Color;
	float m_Intensity;

	ID3DX11EffectScalarVariable* m_pIntensityVariable;
	ID3DX11EffectVectorVariable* m_pColorVariable;

	ID3DX11EffectShaderResourceVariable* m_pTextureMapVariabele;
};

