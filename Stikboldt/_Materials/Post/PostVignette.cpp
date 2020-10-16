#include "stdafx.h"
#include "PostVignette.h"
#include "RenderTarget.h"

PostVignette::PostVignette()
	: PostProcessingMaterial(L"Resources/Effects/Post/Vignette.fx", 2)
	, m_pTextureMapVariabele{nullptr}
	, m_pColorVariable{nullptr}
	, m_pIntensityVariable{nullptr}
	, m_Intensity{1.f}
	, m_Color{0.f, 0.f, 0.f}
{

}

void PostVignette::SetColor(const XMFLOAT3& color)
{
	m_Color = color;
}
void PostVignette::SetIntensity(const float intensity)
{
	m_Intensity = intensity;
}

#define SETUPSRV(x, y) if (!x) x = GetEffect()->GetVariableByName(y)->AsShaderResource(); 
#define SETUPVECTOR(Var, As) if (!Var) Var = GetEffect()->GetVariableByName(As)->AsVector();
#define SETUPSCALAR(Var, As) if (!Var) Var = GetEffect()->GetVariableByName(As)->AsScalar();
#define LOGSRVWARNING(x, warning) if (!x->IsValid()) {\
														Logger::LogWarning(warning);\
														x = nullptr; \
													}

void PostVignette::LoadEffectVariables()
{
	auto textureName = "gTexture";
	SETUPSRV(m_pTextureMapVariabele, textureName);
	
	std::wstring warning = L"PostBlur::LoadEffectVariables( ) >> Texture variable not found!";
	LOGSRVWARNING(m_pTextureMapVariabele, warning);

	SETUPVECTOR(m_pColorVariable, "gColor");
	SETUPSCALAR(m_pIntensityVariable, "gIntensity");
}

void PostVignette::UpdateEffectVariables(RenderTarget* pRendertarget)
{
	//TODO: Update the TextureMapVariable with the Color ShaderResourceView of the given RenderTarget
	ID3D11ShaderResourceView* RT_SRV = pRendertarget->GetShaderResourceView();

	if (m_pTextureMapVariabele && RT_SRV)
		m_pTextureMapVariabele->SetResource(RT_SRV);

	if (m_pColorVariable) m_pColorVariable->SetFloatVector(&m_Color.x);
	if (m_pIntensityVariable) m_pIntensityVariable->SetFloat(m_Intensity);
}
