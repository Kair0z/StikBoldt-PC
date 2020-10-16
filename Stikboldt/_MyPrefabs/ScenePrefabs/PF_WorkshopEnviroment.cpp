#include "stdafx.h"
#include "PF_WorkshopEnviroment.h"

#include "../Objects/MyPrefabs_ObjectPrefabs.h"

PF_WorkshopEnviroment::PF_WorkshopEnviroment()
	: m_pGround{nullptr}
{

}

void PF_WorkshopEnviroment::Initialize(const GameContext&)
{
	m_pGround = new PF_Ground{};
	AddChild(m_pGround);
}

void PF_WorkshopEnviroment::Update(const GameContext&)
{

}

void PF_WorkshopEnviroment::Draw(const GameContext&)
{

}