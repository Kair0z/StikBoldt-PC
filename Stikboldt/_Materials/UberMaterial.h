#pragma once
#include "Material.h"
#include <string>

class TextureData;

using namespace DirectX;
using namespace std;

using FXVectorVar = ID3DX11EffectVectorVariable;
using FXScalarVar = ID3DX11EffectScalarVariable;
using FXSRVVar = ID3DX11EffectShaderResourceVariable;

class UberMaterial : public Material
{
public:
	UberMaterial(const wstring& effectFile = L"Resources/Effects/Ubershader.fx", const wstring& technique = L"WithAlphaBlending");
	virtual ~UberMaterial() = default;

#pragma region MaterialFunctions

	// LIGHT
	// *****
	void SetLightDirection(const XMFLOAT3& direction);
	XMFLOAT3 GetLightDirection() const;

	// DIFFUSE
	// *******
	void EnableTextureDiffuse(const bool enabled);
	bool IsEnabledTextureDiffuse() const;
	void SetTextureDiffuse(const wstring& assetFile);
	void SetDiffuseColor(const XMFLOAT4& color);
	XMFLOAT4 GetDiffuseTexture() const;

	// SPECULAR
	// ********
	void EnableTextureSpecular(const bool enabled);
	bool IsEnabledTextureSpecular() const;
	void SetSpecularColor(const XMFLOAT4& color);
	XMFLOAT4 GetSpecularColor() const;
	void SetTextureSpecularLevel(const wstring& assetFile);
	void SetShininess(const float shininess);
	float GetShininess() const;

	// AMBIENT
	// *******
	void SetAmbientColor(const XMFLOAT4& color);
	XMFLOAT4 GetAmbientColor() const;
	void SetAmbientIntensity(const float intensity);
	float GetAmbientIntensity() const;

	// NORMAL MAPPING
	// **************
	void EnableNormalMapping(const bool enabled);
	bool IsEnabledNormalMapping() const;
	void SetTextureNormalMap(const wstring& assetFile);
	void FlipNormalGreenChannel(const bool doFlip);
	bool IsFlipNormalGreenChannel() const;

	// ENVIROMENT MAPPING
	// ******************
	void EnableEnviromentMapping(const bool enabled);
	bool IsEnabledEnviromentMapping() const;
	void SetEnviromentCube(const wstring& assetFile);
	void SetReflectionStrength(const float strength);
	float GetReflectionStrength() const;
	void SetRefractionStrength(const float strength);
	float GetRefractionStrength() const;
	void SetRefractionIndex(const float index);
	float GetRefractionIndex() const;

	// OPACITY
	// *******
	void EnableOpacity(const bool enabled);
	bool IsEnabledOpacity() const;
	void SetTextureOpacity(const wstring& assetFile);
	void SetOpacity(const float opacity);
	float GetOpacity() const;

	// SPECULAR MODELS
	// ***************
	void EnableSpecularBlinn(const bool enabled);
	bool IsEnabledSpecularBlinn() const;
	void EnableSpecularPhong(const bool enabled);
	bool IsEnabledSpecularPhong() const;

	// FRESNEL FALLOFF
	// ***************
	void EnableFresnelFalloff(const bool enabled);
	bool IsEnabledFresnelFalloff() const;
	void SetFresnelColor(const XMFLOAT4& color);
	XMFLOAT4 GetFresnelColor() const;
	void SetFresnelPower(const float power);
	float GetFresnelPower() const;
	void SetFresnelMultiplier(const float multiplier);
	float GetFresnelMultiplier() const;
	void SetFresnelHardness(const float hardness);
	float GetFresnelHardness() const;

#pragma endregion

protected:
	virtual void LoadEffectVariables() override;
	virtual void UpdateEffectVariables(const GameContext& gCtx, ModelComponent* pModelComponent) override;

private:

#pragma region MaterialData

	// LIGHT
	// *****
	XMFLOAT3 m_LightDirection;
	static FXVectorVar* m_pLightDirectionVariable;

	// DIFFUSE
	// *******
	bool m_UseDiffuseTexture;
	static FXScalarVar* m_pUseDiffuseTextureVariable;
	TextureData* m_pDiffuseTexture;
	static FXSRVVar* m_pDiffuseSRVvariable;
	XMFLOAT4 m_ColorDiffuse;
	static FXVectorVar* m_pDiffuseColorVariable;

	// SPECULAR
	// ********
	XMFLOAT4 m_ColorSpecular;
	static FXVectorVar* m_pSpecularColorVariable;
	bool m_UseSpecularLevelTexture;
	static FXScalarVar* m_pUseSpecularTextureVariable;
	TextureData* m_pSpecularLevelTexture;
	static FXSRVVar* m_pSpecularLevelSRVvariable;
	float m_Shininess;
	static FXScalarVar* m_pShininessVariable;

	// AMBIENT
	// *******
	XMFLOAT4 m_ColorAmbient;
	static FXVectorVar* m_pAmbientColorVariable;
	float m_AmbientIntensity;
	static FXScalarVar* m_pAmbientIntensityVariable;

	// NORMAL MAPPING
	// **************
	bool m_DoFlipGreenChannel;
	static FXScalarVar* m_pFlipGreenChannelVariable;
	bool m_UseNormalMapping;
	static FXScalarVar* m_pUseNormalMappingVariable;
	TextureData* m_pNormalMappingTexture;
	static FXSRVVar* m_pNormalMappingSRVvariable;

	// ENVIROMENT MAPPING
	// ******************
	bool m_UseEnviromentMapping;
	static FXScalarVar* m_pUseEnviromentMappingVariable;
	TextureData* m_pEnviromentCube;
	static FXSRVVar* m_pEnviromentSRVvariable;
	float m_ReflectionStrength;
	static FXScalarVar* m_pReflectionStrengthVariable;
	float m_RefractionStrength;
	static FXScalarVar* m_pRefractionStrengthVariable;
	float m_RefractionIndex;
	static FXScalarVar* m_pRefractionIndexVariable;

	// OPACITY
	// *******
	bool m_UseOpacityMap;
	static FXScalarVar* m_pUseOpacityMapVariable;
	TextureData* m_pOpacityMap;
	static FXSRVVar* m_pOpacitySRVvariable;
	float m_Opacity;
	static FXScalarVar* m_pOpacityVariable;

	// SPECULAR MODELS
	// ***************
	bool m_UseSpecularBlinn;
	static FXScalarVar* m_pUseBlinnVariable;
	bool m_UseSpecularPhong;
	static FXScalarVar* m_pUsePhongVariable;

	// FRESNEL FALLOFF
	// ***************
	bool m_UseFresnelFalloff;
	static FXScalarVar* m_pUseFresnelFalloffVariable;
	XMFLOAT4 m_ColorFresnel;
	static FXVectorVar* m_pFresnelColorVariable;
	float m_FresnelPower;
	static FXScalarVar* m_pFresnelPowerVariable;
	float m_FresnelMultiplier;
	static FXScalarVar* m_pFresnelMultiplierVariable;
	float m_FresnelHardness;
	static FXScalarVar* m_pFresnelHardnessVariable;

#pragma endregion

private: // ROF
	UberMaterial(const UberMaterial&) = delete;
	UberMaterial(UberMaterial&&) = delete;
	UberMaterial& operator=(const UberMaterial&) = delete;
	UberMaterial& operator=(UberMaterial&&) = delete;
};