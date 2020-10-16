#pragma once
#include <PostProcessingMaterial.h>

using namespace DirectX;
class ID3D11EffectShaderResourceVariable;

class PostInvert : public PostProcessingMaterial
{
public:
	PostInvert();
	virtual ~PostInvert() = default;

private:
	void LoadEffectVariables() override;
	void UpdateEffectVariables(RenderTarget* pRendertarget) override;

	ID3DX11EffectShaderResourceVariable* m_pTextureMapVariabele;
};

