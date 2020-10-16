#include "stdafx.h"
#include "UberMaterial.h"

#include "ContentManager.h"
#include "TextureData.h"

using namespace DirectX;

#define UPAR(x) (x)

#pragma region Set Static Pointers to nullptr
FXVectorVar* UberMaterial::m_pLightDirectionVariable = nullptr;
FXVectorVar* UberMaterial::m_pAmbientColorVariable = nullptr;
FXScalarVar* UberMaterial::m_pUseDiffuseTextureVariable = nullptr;
FXSRVVar* UberMaterial::m_pDiffuseSRVvariable = nullptr;
FXVectorVar* UberMaterial::m_pDiffuseColorVariable = nullptr;
FXVectorVar* UberMaterial::m_pSpecularColorVariable = nullptr;
FXScalarVar* UberMaterial::m_pUseSpecularTextureVariable = nullptr;
FXSRVVar* UberMaterial::m_pSpecularLevelSRVvariable = nullptr;
FXScalarVar* UberMaterial::m_pShininessVariable = nullptr;
FXScalarVar* UberMaterial::m_pAmbientIntensityVariable = nullptr;
FXScalarVar* UberMaterial::m_pFlipGreenChannelVariable = nullptr;
FXScalarVar* UberMaterial::m_pUseNormalMappingVariable = nullptr;
FXSRVVar* UberMaterial::m_pNormalMappingSRVvariable = nullptr;
FXScalarVar* UberMaterial::m_pUseEnviromentMappingVariable = nullptr;
FXSRVVar* UberMaterial::m_pEnviromentSRVvariable = nullptr;
FXScalarVar* UberMaterial::m_pReflectionStrengthVariable = nullptr;
FXScalarVar* UberMaterial::m_pRefractionStrengthVariable = nullptr;
FXScalarVar* UberMaterial::m_pRefractionIndexVariable = nullptr;
FXScalarVar* UberMaterial::m_pUseOpacityMapVariable = nullptr;
FXSRVVar* UberMaterial::m_pOpacitySRVvariable = nullptr;
FXScalarVar* UberMaterial::m_pOpacityVariable = nullptr;
FXScalarVar* UberMaterial::m_pUseBlinnVariable = nullptr;
FXScalarVar* UberMaterial::m_pUsePhongVariable = nullptr;
FXScalarVar* UberMaterial::m_pUseFresnelFalloffVariable = nullptr;
FXVectorVar* UberMaterial::m_pFresnelColorVariable = nullptr;
FXScalarVar* UberMaterial::m_pFresnelPowerVariable = nullptr;
FXScalarVar* UberMaterial::m_pFresnelMultiplierVariable = nullptr;
FXScalarVar* UberMaterial::m_pFresnelHardnessVariable = nullptr;
#pragma endregion

UberMaterial::UberMaterial(const std::wstring& effectFile, const std::wstring& technique)
	: Material(effectFile, technique)

#pragma region init
	, m_AmbientIntensity{1.f}
	, m_ColorAmbient{1.f, 1.f, 1.f, 1.f}
	, m_ColorDiffuse{1.f, 1.f, 1.f, 1.f}
	, m_ColorFresnel{1.f, 1.f, 1.f, 1.f}
	, m_ColorSpecular{1.f, 1.f, 1.f, 1.f}
	, m_DoFlipGreenChannel{false}
	, m_FresnelHardness{1.f}
	, m_FresnelMultiplier{1.f}
	, m_FresnelPower{1.f}
	, m_Opacity{1.f}
	, m_LightDirection{0.577f, -0.577f, 0.577f}
	, m_Shininess{15.f}
	, m_UseDiffuseTexture{false}
	, m_UseEnviromentMapping{false}
	, m_UseFresnelFalloff{false}
	, m_UseNormalMapping{false}
	, m_UseOpacityMap{false}
	, m_UseSpecularBlinn{false}
	, m_UseSpecularLevelTexture{false}
	, m_UseSpecularPhong{false}
	, m_pDiffuseTexture{nullptr}
	, m_pEnviromentCube{nullptr}
	, m_pNormalMappingTexture{nullptr}
	, m_pSpecularLevelTexture{nullptr}
	, m_pOpacityMap{nullptr}
#pragma endregion
{

}

#define SETUPSRV(x, y) if (!x) x = GetEffect()->GetVariableByName(y)->AsShaderResource(); 
#define LOGSRVWARNING(x, warning) if (!x->IsValid()) {\
														Logger::LogWarning(warning);\
														x = nullptr; \
													}
#define SETUPSCALAR(x, y) if (!x) x = GetEffect()->GetVariableByName(y)->AsScalar();
#define SETUPVECTOR(x, y) if (!x) x = GetEffect()->GetVariableByName(y)->AsVector();

void UberMaterial::LoadEffectVariables()
{
#pragma region SRV
	// Diffuse:
	LPCSTR diffuseName = "gTextureDiffuse";
	SETUPSRV(m_pDiffuseSRVvariable, diffuseName);
	std::wstring warning = L"LoadEffectVariables() > Diffuse variable not found!";
	LOGSRVWARNING(m_pDiffuseSRVvariable, warning);

	// Specular:
	LPCSTR specularName = "gTextureSpecularIntensity";
	SETUPSRV(m_pSpecularLevelSRVvariable, specularName);
	warning = L"DiffuseMaterial::LoadEffectVariables() > Specular variable not found!";
	LOGSRVWARNING(m_pSpecularLevelSRVvariable, warning);

	// Normalmap:
	LPCSTR normalsName = "gTextureNormal";
	SETUPSRV(m_pNormalMappingSRVvariable, normalsName);
	warning = L"";
	LOGSRVWARNING(m_pNormalMappingSRVvariable, warning);

	// Enviroment:
	LPCSTR enviromentName = "gCubeEnvironment";
	SETUPSRV(m_pEnviromentSRVvariable, enviromentName);
	warning = L"";
	LOGSRVWARNING(m_pEnviromentSRVvariable, warning);

	// Opacity:
	LPCSTR opacityName = "gTextureOpacity";
	SETUPSRV(m_pOpacitySRVvariable, opacityName);
	warning = L"";
	LOGSRVWARNING(m_pOpacitySRVvariable, warning);
#pragma endregion

#pragma region ScalarVariables
	  SETUPSCALAR(m_pAmbientIntensityVariable, "gAmbientIntensity");
	  SETUPSCALAR(m_pFresnelHardnessVariable, "gFresnelHardness");
	  SETUPSCALAR(m_pFresnelMultiplierVariable, "gFresnelMultiplier");
	  SETUPSCALAR(m_pFresnelPowerVariable, "gFresnelPower");
	  SETUPSCALAR(m_pReflectionStrengthVariable, "gReflectionStrength");
	  SETUPSCALAR(m_pRefractionIndexVariable, "gRefractionIndex");
	  SETUPSCALAR(m_pRefractionStrengthVariable, "gRefractionStrength");
	  SETUPSCALAR(m_pShininessVariable, "gShininess");
	  SETUPSCALAR(m_pOpacityVariable, "gOpacityIntensity");
	  SETUPSCALAR(m_pFlipGreenChannelVariable, "gFlipGreenChannel");
	  SETUPSCALAR(m_pUseBlinnVariable, "gUseSpecularBlinn");
	  SETUPSCALAR(m_pUseDiffuseTextureVariable, "gUseTextureDiffuse");
	  SETUPSCALAR(m_pUseEnviromentMappingVariable, "gUseTextureEnvironment");
	  SETUPSCALAR(m_pUseFresnelFalloffVariable, "gUseFresnelFalloff");
	  SETUPSCALAR(m_pUseNormalMappingVariable, "gUseTextureNormal");
	  SETUPSCALAR(m_pUseOpacityMapVariable, "gUseTextureOpacity");
	  SETUPSCALAR(m_pUsePhongVariable, "gUseSpecularPhong");
	  SETUPSCALAR(m_pUseSpecularTextureVariable, "gUseTextureSpecularIntensity");

#pragma endregion

#pragma region VectorVariables
	  SETUPVECTOR(m_pAmbientColorVariable, "gColorAmbient");
	  SETUPVECTOR(m_pDiffuseColorVariable, "gColorDiffuse");
	  SETUPVECTOR(m_pFresnelColorVariable, "gColorFresnel");
	  SETUPVECTOR(m_pLightDirectionVariable, "gLightDirection");
	  SETUPVECTOR(m_pSpecularColorVariable, "gColorSpecular");
#pragma endregion

}

// MACROS for making life easier on updating effect-variables ('cause I'm lazy...)
#define SAFESETBOOL(x, y) if (x) x->SetBool(y);
#define SAFESETFLOAT(x, y) if (x) x->SetFloat(y);
#define SAFESETVECTOR(x, y) if (x) x->SetFloatVector(y);

void UberMaterial::UpdateEffectVariables(const GameContext& gCtx, ModelComponent* pModelComponent)
{
	UPAR(gCtx);
	UPAR(pModelComponent);

#pragma region SRV-Variables
	if (m_pDiffuseTexture && m_pDiffuseSRVvariable)
		m_pDiffuseSRVvariable->SetResource(m_pDiffuseTexture->GetShaderResourceView());

	if (m_pOpacityMap && m_pOpacitySRVvariable)
		m_pOpacitySRVvariable->SetResource(m_pOpacityMap->GetShaderResourceView());

	if (m_pSpecularLevelTexture && m_pSpecularLevelSRVvariable)
		m_pSpecularLevelSRVvariable->SetResource(m_pSpecularLevelTexture->GetShaderResourceView());

	if (m_pNormalMappingTexture && m_pNormalMappingSRVvariable)
		m_pNormalMappingSRVvariable->SetResource(m_pNormalMappingTexture->GetShaderResourceView());

	if (m_pEnviromentCube && m_pEnviromentSRVvariable)
		m_pEnviromentSRVvariable->SetResource(m_pEnviromentCube->GetShaderResourceView());

#pragma endregion

#pragma region Scalar-Variables
	SAFESETFLOAT(m_pAmbientIntensityVariable, m_AmbientIntensity);
	SAFESETFLOAT(m_pFresnelHardnessVariable, m_FresnelHardness);
	SAFESETFLOAT(m_pFresnelMultiplierVariable, m_FresnelMultiplier);
	SAFESETFLOAT(m_pFresnelPowerVariable, m_FresnelPower);
	SAFESETFLOAT(m_pReflectionStrengthVariable, m_ReflectionStrength);
	SAFESETFLOAT(m_pRefractionIndexVariable, m_RefractionIndex);
	SAFESETFLOAT(m_pShininessVariable, m_Shininess);
	SAFESETFLOAT(m_pOpacityVariable, m_Opacity);

	// Booleans:
	SAFESETBOOL(m_pFlipGreenChannelVariable, m_DoFlipGreenChannel);
	SAFESETBOOL(m_pUseBlinnVariable, m_UseSpecularBlinn);
	SAFESETBOOL(m_pUseDiffuseTextureVariable, m_UseDiffuseTexture);
	SAFESETBOOL(m_pUseEnviromentMappingVariable, m_UseEnviromentMapping);
	SAFESETBOOL(m_pUseFresnelFalloffVariable, m_UseFresnelFalloff);
	SAFESETBOOL(m_pUseNormalMappingVariable, m_UseNormalMapping);
	SAFESETBOOL(m_pUseOpacityMapVariable, m_UseOpacityMap);
	SAFESETBOOL(m_pUsePhongVariable, m_UseSpecularPhong);
	SAFESETBOOL(m_pUseSpecularTextureVariable, m_UseSpecularLevelTexture);
#pragma endregion

#pragma region Vector-Variables
	SAFESETVECTOR(m_pAmbientColorVariable, &m_ColorAmbient.x);
	SAFESETVECTOR(m_pDiffuseColorVariable, &m_ColorDiffuse.x);
	SAFESETVECTOR(m_pFresnelColorVariable, &m_ColorFresnel.x);
	SAFESETVECTOR(m_pLightDirectionVariable, &m_LightDirection.x);
	SAFESETVECTOR(m_pSpecularColorVariable, &m_ColorSpecular.x);
	
#pragma endregion
}

#pragma region Get-n-Set

#pragma region Set Textures
#define LOADTEXTURE(x, y) x = ContentManager::Load<TextureData>(y);
void UberMaterial::SetTextureDiffuse(const wstring& assetFile)
{
	LOADTEXTURE(m_pDiffuseTexture, assetFile);
}

void UberMaterial::SetTextureSpecularLevel(const wstring& assetFile)
{
	LOADTEXTURE(m_pSpecularLevelTexture, assetFile);
}

void UberMaterial::SetTextureNormalMap(const wstring& assetFile)
{
	LOADTEXTURE(m_pNormalMappingTexture, assetFile);
}

void UberMaterial::SetEnviromentCube(const wstring& assetFile)
{
	LOADTEXTURE(m_pEnviromentCube, assetFile);
}

void UberMaterial::SetTextureOpacity(const wstring& assetFile)
{
	LOADTEXTURE(m_pOpacityMap, assetFile);
}
#pragma endregion

void UberMaterial::SetLightDirection(const XMFLOAT3& direction)
{
	XMStoreFloat3(&m_LightDirection, XMVector3NormalizeEst(XMLoadFloat3(&direction)));
}

XMFLOAT3 UberMaterial::GetLightDirection() const
{
	return m_LightDirection;
}

void UberMaterial::EnableTextureDiffuse(const bool enabled)
{
	m_UseDiffuseTexture = enabled;
}

bool UberMaterial::IsEnabledTextureDiffuse() const
{
	return m_UseDiffuseTexture;
}

void UberMaterial::SetDiffuseColor(const XMFLOAT4& color)
{
	m_ColorDiffuse = color;
}

XMFLOAT4 UberMaterial::GetDiffuseTexture() const
{
	return m_ColorDiffuse;
}

void UberMaterial::EnableTextureSpecular(const bool enabled)
{
	m_UseSpecularLevelTexture = enabled;
}

bool UberMaterial::IsEnabledTextureSpecular() const
{
	return m_UseSpecularLevelTexture;
}

void UberMaterial::SetSpecularColor(const XMFLOAT4& color)
{
	m_ColorSpecular = color;
}

XMFLOAT4 UberMaterial::GetSpecularColor() const
{
	return m_ColorSpecular;
}

void UberMaterial::SetShininess(const float shininess)
{
	m_Shininess = shininess;
}

float UberMaterial::GetShininess() const
{
	return m_Shininess;
}

void UberMaterial::SetAmbientColor(const XMFLOAT4& color)
{
	m_ColorAmbient = color;
}

XMFLOAT4 UberMaterial::GetAmbientColor() const
{
	return m_ColorAmbient;
}

void UberMaterial::SetAmbientIntensity(const float intensity)
{
	m_AmbientIntensity = intensity;
}

float UberMaterial::GetAmbientIntensity() const
{
	return m_AmbientIntensity;
}

void UberMaterial::EnableNormalMapping(const bool enabled)
{
	m_UseNormalMapping = enabled;
}

bool UberMaterial::IsEnabledNormalMapping() const
{
	return m_UseNormalMapping;
}

void UberMaterial::FlipNormalGreenChannel(const bool doFlip)
{
	m_DoFlipGreenChannel = doFlip;
}

bool UberMaterial::IsFlipNormalGreenChannel() const
{
	return m_DoFlipGreenChannel;
}

void UberMaterial::EnableEnviromentMapping(const bool enabled)
{
	m_UseEnviromentMapping = enabled;
}

bool UberMaterial::IsEnabledEnviromentMapping() const
{
	return m_UseEnviromentMapping;
}

void UberMaterial::SetReflectionStrength(const float strength)
{
	m_ReflectionStrength = strength;
}

float UberMaterial::GetReflectionStrength() const
{
	return m_ReflectionStrength;
}

void UberMaterial::SetRefractionStrength(const float strength)
{
	m_RefractionStrength = strength;
}

float UberMaterial::GetRefractionStrength() const
{
	return m_RefractionStrength;
}

void UberMaterial::SetRefractionIndex(const float index)
{
	m_RefractionIndex = index;
}

float UberMaterial::GetRefractionIndex() const
{
	return m_RefractionIndex;
}

void UberMaterial::EnableOpacity(const bool enabled)
{
	m_UseOpacityMap = enabled;
}

bool UberMaterial::IsEnabledOpacity() const
{
	return m_UseOpacityMap;
}

void UberMaterial::SetOpacity(const float opacity)
{
	m_Opacity = opacity;
}

float UberMaterial::GetOpacity() const
{
	return m_Opacity;
}

void UberMaterial::EnableSpecularBlinn(const bool enabled)
{
	m_UseSpecularBlinn = enabled;
}

bool UberMaterial::IsEnabledSpecularBlinn() const
{
	return m_UseSpecularBlinn;
}

void UberMaterial::EnableSpecularPhong(const bool enabled)
{
	m_UseSpecularPhong = enabled;
}

bool UberMaterial::IsEnabledSpecularPhong() const
{
	return m_UseSpecularPhong;
}

void UberMaterial::EnableFresnelFalloff(const bool enabled)
{
	m_UseFresnelFalloff = enabled;
}

bool UberMaterial::IsEnabledFresnelFalloff() const
{
	return m_UseFresnelFalloff;
}

void UberMaterial::SetFresnelColor(const XMFLOAT4& color)
{
	m_ColorFresnel = color;
}

XMFLOAT4 UberMaterial::GetFresnelColor() const
{
	return m_ColorFresnel;
}

void UberMaterial::SetFresnelPower(const float power)
{
	m_FresnelPower = power;
}

float UberMaterial::GetFresnelPower() const
{
	return m_FresnelPower;
}

void UberMaterial::SetFresnelMultiplier(const float multiplier)
{
	m_FresnelMultiplier = multiplier;
}

float UberMaterial::GetFresnelMultiplier() const
{
	return m_FresnelMultiplier;
}

void UberMaterial::SetFresnelHardness(const float hardness)
{
	m_FresnelHardness = hardness;
}

float UberMaterial::GetFresnelHardness() const
{
	return m_FresnelHardness;
}

#pragma endregion