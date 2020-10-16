#include "stdafx.h"
#include "HardwareSkinningScene.h"
#include "GameObject.h"
#include "ContentManager.h"
#include "TransformComponent.h"
#include "ModelComponent.h"
#include "ModelAnimator.h"
#include "..\_Materials\SkinnedDiffuseMaterial.h"

#define ANIM_AMOUNT 5

HardwareSkinningScene::HardwareSkinningScene() :
	GameScene(L"HardwareSkinningScene"),
	m_pModel(nullptr)
{}

void HardwareSkinningScene::Initialize()
{
	const auto gameContext = GetGameContext();

	auto skinnedDiffuseMaterial = new SkinnedDiffuseMaterial();
	skinnedDiffuseMaterial->SetDiffuseTexture(L"./Resources/Textures/PeasantGirl_Diffuse.png");
	gameContext.pMaterialManager->AddMaterial(skinnedDiffuseMaterial, 0);

	m_pModel = new ModelComponent(L"./Resources/Meshes/PeasantGirl.ovm");
	m_pModel->SetMaterial(0);
	auto obj = new GameObject();
	obj->AddComponent(m_pModel);
	AddChild(obj);

	m_pModel->GetAnimator()->SetAnimation(0);

	obj->GetTransform()->Scale(0.1f, 0.1f, 0.1f);

	//Input
	gameContext.pInput->AddInputAction(InputAction(0, InputTriggerState::Released, 'R'));
	gameContext.pInput->AddInputAction(InputAction(1, InputTriggerState::Released, 'P'));
	gameContext.pInput->AddInputAction(InputAction(2, InputTriggerState::Released, 'O'));

	gameContext.pInput->AddInputAction(InputAction(3, InputTriggerState::Down, VK_UP));
	gameContext.pInput->AddInputAction(InputAction(4, InputTriggerState::Down, VK_DOWN));

	gameContext.pInput->AddInputAction(InputAction(5, InputTriggerState::Pressed, VK_LEFT));
	gameContext.pInput->AddInputAction(InputAction(6, InputTriggerState::Pressed, VK_RIGHT));
}

void HardwareSkinningScene::Update()
{
	const auto gameContext = GetGameContext();
	auto animator = m_pModel->GetAnimator();

	if (gameContext.pInput->IsActionTriggered(0))
	{
		animator->Reset();
	}

	if (gameContext.pInput->IsActionTriggered(1))
	{
		if (animator->IsPlaying())
		{
			animator->Pause();
		}
		else
		{
			animator->Play();
		}
	}

	if (gameContext.pInput->IsActionTriggered(2))
	{
		animator->SetPlayReversed(!animator->IsReversed());
	}

	if (gameContext.pInput->IsActionTriggered(3))
	{
		auto speed = animator->GetAnimationSpeed();
		speed += 0.5f * gameContext.pGameTime->GetElapsed();
		Clamp<float>(speed, 5, 0);

		animator->SetAnimationSpeed(speed);
	}

	if (gameContext.pInput->IsActionTriggered(4))
	{
		auto speed = animator->GetAnimationSpeed();
		speed -= 0.5f * gameContext.pGameTime->GetElapsed();
		Clamp<float>(speed, 5, 0);

		animator->SetAnimationSpeed(speed);
	}

	if (gameContext.pInput->IsActionTriggered(5))
	{
		--m_AnimationIndex;
		if (m_AnimationIndex < 0)
			m_AnimationIndex = ANIM_AMOUNT-1;

		animator->SetAnimation(m_AnimationIndex);
	}

	if (gameContext.pInput->IsActionTriggered(6))
	{
		++m_AnimationIndex;
		if (m_AnimationIndex >= ANIM_AMOUNT)
			m_AnimationIndex = 0;

		animator->SetAnimation(m_AnimationIndex);
	}
}

void HardwareSkinningScene::Draw() {}
