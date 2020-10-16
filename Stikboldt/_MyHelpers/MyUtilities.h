#pragma once
#include <sstream>
class TransformComponent;

using namespace DirectX;

//--------------------------
// Structs:
//--------------------------
namespace MyUtils
{
	struct TransformProps
	{
		TransformProps(const XMFLOAT3& trans = {}, const XMFLOAT3& rot = {}, const XMFLOAT3& scale = { 1.f, 1.f, 1.f })
			: translation{trans}
			, rotation{rot}
			, scale{scale}
		{

		}

		DirectX::XMFLOAT3 translation = { 0.f, 0.f, 0.f };
		DirectX::XMFLOAT3 rotation;
		DirectX::XMFLOAT3 scale;

		void ResetTransform(TransformComponent& transComp);
	};

	struct Circle2D
	{
		float m_Radius = 1.f;
		XMFLOAT2 m_Center = {};
	};

	struct CircleCollider
	{
		Circle2D m_Shape;

		bool PointOverlaps(XMFLOAT2 point)
		{
			XMVECTOR distanceFromHitboxCenter = XMVector2LengthEst(XMLoadFloat2(&point) - XMLoadFloat2(&m_Shape.m_Center));
			XMFLOAT2 distances;
			XMStoreFloat2(&distances, distanceFromHitboxCenter);

			if (distances.x <= m_Shape.m_Radius) return true;

			return false;
		}
	};
}

//--------------------------
// Enum classes:
//--------------------------
namespace MyUtils
{
	enum class ConsoleTextAttribute
	{
		Black,
		Blue,
		SwmpGreen,
		BBBlue, // Babyblue
		Red,
		Purple,
		Orange,
		LightGray,
		DarkGray,
		MidBlue,
		Green,
		Cyan,
		LightRed,
		PinkPurple,
		Cream,
		White
	};
}

//--------------------------
// Functions:
//--------------------------
namespace MyUtils
{
	void ChangeConsoleTextColor(ConsoleTextAttribute attribute = ConsoleTextAttribute::White);

	template <typename T>
	std::wstring to_wstring_precision(const T& value, const int n = 2)
	{
		std::wstringstream out;

		out.precision(n);
		out << std::fixed << value;

		return out.str();
	}
}

