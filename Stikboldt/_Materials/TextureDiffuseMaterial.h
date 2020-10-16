#pragma once
#include <Material.h>

class TextureData;

class TextureDiffuseMaterial final: public Material
{
public:
	TextureDiffuseMaterial();
	void SetDiffuseTexture(const std::wstring& assetFile);

protected:
	void LoadEffectVariables() override;
	void UpdateEffectVariables(const GameContext& gameContext, ModelComponent* pModelComponent) override;

private:
	TextureData* m_pDiffuseTexture;
	DirectX::XMFLOAT3 m_DiffuseColor;
	static ID3DX11EffectShaderResourceVariable* m_pDiffuseSRVvariable;
};

