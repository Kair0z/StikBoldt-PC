#pragma once

using namespace DirectX;

struct SkinnedVertex
{
	SkinnedVertex(const XMFLOAT3& pos, const XMFLOAT3& normal, const XMFLOAT4& color, const float w0 = 0.f, const float w1 = 0.f);

	VertexPosNormCol m_TransformedVertex;
	const VertexPosNormCol m_OriginalVertex;
	
	// TODO: Make the amount of weights adjustable (not only 2)
	float m_Weight0;
	float m_Weight1;
};

