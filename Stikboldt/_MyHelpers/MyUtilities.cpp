#include "stdafx.h"
#include "MyUtilities.h"
#include "TransformComponent.h"

using namespace MyUtils;

void MyUtils::ChangeConsoleTextColor(ConsoleTextAttribute attribute)
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(consoleHandle, uint16_t(attribute));
}

void MyUtils::TransformProps::ResetTransform(TransformComponent& transComp)
{
	transComp.GetTransform()->Translate(this->translation);
	transComp.GetTransform()->Rotate(this->rotation, false);

	if (scale.x <= 0.f || scale.y <= 0.f || scale.z <= 0.f) return;

	transComp.GetTransform()->Scale(this->scale);
}
