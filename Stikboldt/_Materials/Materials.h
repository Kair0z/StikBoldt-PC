#pragma once

#include "ColorMaterial.h"
#include "SkinnedDiffuseMaterial.h"
#include "SpikeyMaterial.h"
#include "UberMaterial.h"
#include "TextureDiffuseMaterial.h"
#include "ShadowMapMaterial.h"
#include "Post/PostVignette.h"
#include "Post/PostInvert.h"

using namespace DirectX;
namespace ActiveColors
{
	static constexpr XMFLOAT3 gPlayer0 = { 169.f / 255.f, 19.f / 255.f, 43.f / 255.f }; // Red
	static constexpr XMFLOAT3 gPlayer1 = { 112.f / 255.f, 41.f / 255.f, 130.f / 255.f }; // Purple
	static constexpr XMFLOAT3 gPlayer2 = { 48.f / 255.f, 122.f / 255.f, 44.f / 255.f }; // Green
	static constexpr XMFLOAT3 gPlayer3 = { 16.f / 255.f, 161.f / 255.f, 170.f / 255.f }; // Blue

	inline XMFLOAT4 GetPlayerColor(size_t id)
	{
		XMFLOAT3 color{};

		switch (id)
		{
		case 0: color = gPlayer0; break;
		case 1: color = gPlayer1; break;
		case 2: color = gPlayer2; break;
		case 3: color = gPlayer3; break;
		default: color = gPlayer0; break;
		}

		return { color.x, color.y, color.z, 1.f };
	}
}

enum class ActiveMaterials
{
	DefaultRed,
	UberMatte,
	P0,
	P1,
	P2,
	P3,
	Playfield,
	EnviromentGradient,
	EnviromentFloor,
	Scoreboard,
	Ball0,
	Ball1,
	Ball2,
	Ball3,
	Planks0,
	PlanksOut = Planks0 + 10,
};

#define ID(x) (int)x
inline void ActivateMaterials(const GameContext& gameContext)
{
	MaterialManager* pMaterials = gameContext.pMaterialManager;

	pMaterials->AddMaterial(new ColorMaterial{ false }, ID(ActiveMaterials::DefaultRed));

#pragma region UbberMatte
	UberMaterial* pUber = new UberMaterial{};
	pUber->EnableEnviromentMapping(false);
	pUber->EnableFresnelFalloff(true);
	pUber->EnableNormalMapping(false);
	pUber->EnableOpacity(false);
	pUber->EnableSpecularBlinn(false);
	pUber->EnableSpecularPhong(true);
	pUber->EnableTextureDiffuse(false);
	pUber->EnableTextureSpecular(false);
	pUber->SetDiffuseColor({247.f / 255.f, 242.f / 255.f, 220.f / 255.f, 1.f});
	pUber->SetLightDirection({ -1.f, -1.f, 1.f });
	pUber->SetAmbientColor({0.f, 0.f, 0.f, 1.f});
	pMaterials->AddMaterial(pUber, ID(ActiveMaterials::UberMatte));
#pragma endregion

#pragma region P0
	SkinnedDiffuseMaterial* p0 = new SkinnedDiffuseMaterial{};
	p0->SetDiffuseTexture(L"Resources/Textures/Bot/Bot_0.png");
	pMaterials->AddMaterial(p0, ID(ActiveMaterials::P0));
#pragma endregion

#pragma region P1
	SkinnedDiffuseMaterial* p1 = new SkinnedDiffuseMaterial{};
	p1->SetDiffuseTexture(L"Resources/Textures/Bot/Bot_1.png");
	pMaterials->AddMaterial(p1, ID(ActiveMaterials::P1));
#pragma endregion

#pragma region P2
	SkinnedDiffuseMaterial* p2 = new SkinnedDiffuseMaterial{};
	p2->SetDiffuseTexture(L"Resources/Textures/Bot/Bot_2.png");
	pMaterials->AddMaterial(p2, ID(ActiveMaterials::P2));
#pragma endregion

#pragma region P3
	SkinnedDiffuseMaterial* p3 = new SkinnedDiffuseMaterial{};
	p3->SetDiffuseTexture(L"Resources/Textures/Bot/Bot_3.png");
	pMaterials->AddMaterial(p3, ID(ActiveMaterials::P3));
#pragma endregion

#pragma region Playfield
	TextureDiffuseMaterial* playfield = new TextureDiffuseMaterial{};
	playfield->SetDiffuseTexture(L"Resources/Textures/Enviroment/tex_Playfield.png");
	pMaterials->AddMaterial(playfield, ID(ActiveMaterials::Playfield));
#pragma endregion

#pragma region Enviroment
	TextureDiffuseMaterial* enviroment = new TextureDiffuseMaterial{};
	enviroment->SetDiffuseTexture(L"Resources/Textures/Enviroment/tex_Enviroment.png");
	pMaterials->AddMaterial(enviroment, ID(ActiveMaterials::EnviromentGradient));

	TextureDiffuseMaterial* floor = new TextureDiffuseMaterial{};
	floor->SetDiffuseTexture(L"Resources/Textures/Enviroment/tex_EnviromentFloor.png");
	pMaterials->AddMaterial(floor, ID(ActiveMaterials::EnviromentFloor));
#pragma endregion

#pragma region Scoreboard
	TextureDiffuseMaterial* scoreboard = new TextureDiffuseMaterial{};
	scoreboard->SetDiffuseTexture(L"Resources/Textures/Enviroment/tex_Scoreboard.png");
	pMaterials->AddMaterial(scoreboard, ID(ActiveMaterials::Scoreboard));
#pragma endregion

#pragma region Planks
	for (size_t i{}; i < 10; ++i)
	{
		TextureDiffuseMaterial* pPlank = new TextureDiffuseMaterial{};
		pPlank->SetDiffuseTexture(L"Resources/Textures/Planks/tex_Plank_" + std::to_wstring(i) + L".png");
		pMaterials->AddMaterial(pPlank, ID(ActiveMaterials::Planks0 + i));
		pPlank->Initialize(gameContext);
	}

	TextureDiffuseMaterial* pOutPlank = new TextureDiffuseMaterial{};
	pOutPlank->SetDiffuseTexture(L"Resources/Textures/Planks/tex_Plank_Out.png");
	pMaterials->AddMaterial(pOutPlank, ID(ActiveMaterials::PlanksOut));
	pOutPlank->Initialize(gameContext);
#pragma endregion
#pragma region Ball
	for (int i = 0; i < 4; ++i)
	{
		TextureDiffuseMaterial* pBall = new TextureDiffuseMaterial{};
		pBall->SetDiffuseTexture(L"Resources/Textures/Ball/Ball_" + std::to_wstring(i) + L".png");
		pMaterials->AddMaterial(pBall, ID(ActiveMaterials::Ball0) + i);
	}
#pragma endregion
}