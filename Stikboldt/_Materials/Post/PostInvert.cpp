#include "stdafx.h"
#include "PostInvert.h"
#include "RenderTarget.h"

PostInvert::PostInvert()
	:PostProcessingMaterial(L"Resources/Effects/Post/Invert.fx", 3)
	, m_pTextureMapVariabele{nullptr}
{

}

#define SETUPSRV(x, y) if (!x) x = GetEffect()->GetVariableByName(y)->AsShaderResource(); 
#define SETUPVECTOR(Var, As) if (!Var) Var = GetEffect()->GetVariableByName(As)->AsVector();
#define SETUPSCALAR(Var, As) if (!Var) Var = GetEffect()->GetVariableByName(As)->AsScalar();
#define LOGSRVWARNING(x, warning) if (!x->IsValid()) {\
														Logger::LogWarning(warning);\
														x = nullptr; \
													}

void PostInvert::LoadEffectVariables()
{
	auto textureName = "gTexture";
	SETUPSRV(m_pTextureMapVariabele, textureName);

	std::wstring warning = L"PostInvert::LoadEffectVariables( ) >> Texture variable not found!";
	LOGSRVWARNING(m_pTextureMapVariabele, warning);
}

void PostInvert::UpdateEffectVariables(RenderTarget* pRendertarget)
{
	//TODO: Update the TextureMapVariable with the Color ShaderResourceView of the given RenderTarget
	ID3D11ShaderResourceView* RT_SRV = pRendertarget->GetShaderResourceView();

	if (m_pTextureMapVariabele && RT_SRV)
		m_pTextureMapVariabele->SetResource(RT_SRV);
}