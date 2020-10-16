#include "stdafx.h"
#include "SkinnedVertex.h"

using namespace DirectX;

SkinnedVertex::SkinnedVertex(const XMFLOAT3& pos, const XMFLOAT3& normal, const XMFLOAT4& color, const float w0, const float w1)
	: m_TransformedVertex{pos, normal, color}
	, m_OriginalVertex{pos, normal, color}
	, m_Weight0{w0}
	, m_Weight1{w1}
{

}