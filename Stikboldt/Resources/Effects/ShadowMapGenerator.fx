float4x4 gWorld;
float4x4 gLightViewProj;
float4x4 gBones[70];
float4x4 gView : VIEW;
float4x4 gViewInverse : VIEWINVERSE;
 
DepthStencilState depthStencilState
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
};

RasterizerState rasterizerState
{
	FillMode = SOLID;	
	CullMode = NONE;
};
	
//--------------------------------------------------------------------------------------
// Vertex Shader [STATIC]
//--------------------------------------------------------------------------------------
float4 ShadowMapVS(float3 position : POSITION) : SV_POSITION
{
	return mul(float4(position, 1), mul(gWorld, gLightViewProj));
}

//--------------------------------------------------------------------------------------
// Vertex Shader [SKINNED]
//--------------------------------------------------------------------------------------
float4 ShadowMapVS_Skinned(float3 position:POSITION, float4 BoneIndices : BLENDINDICES, float4 BoneWeights : BLENDWEIGHTS) : SV_POSITION
{
	float4 originalPos = float4(position, 1);
	float4 transPos = 0;
	
	for (int i{}; i < 4; ++i)
	{
		float curBoneIdx = BoneIndices[i];
		float curBoneWeight = BoneWeights[i];

		if (curBoneIdx > -1)
		{
			// Vertex is attached to bone:
			transPos += curBoneWeight * mul(originalPos, gBones[curBoneIdx]);
			transPos.w = 1;
		}
	}

	float4x4 lightWorldViewProj = mul(gWorld, gLightViewProj);
	return mul(transPos, lightWorldViewProj	);
}
 
//--------------------------------------------------------------------------------------
// Pixel Shaders
//--------------------------------------------------------------------------------------
void ShadowMapPS_VOID(float4 input : SV_POSITION){}

technique11 GenerateShadows
{
	pass P0
	{
		SetRasterizerState(rasterizerState);
	    SetDepthStencilState(depthStencilState, 0);
		SetVertexShader(CompileShader(vs_4_0, ShadowMapVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, ShadowMapPS_VOID()));
	}
}

technique11 GenerateShadows_Skinned
{
	pass P0
	{
		SetRasterizerState(rasterizerState);
		SetDepthStencilState(depthStencilState, 0);
		SetVertexShader(CompileShader(vs_4_0, ShadowMapVS_Skinned()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, ShadowMapPS_VOID()));
	}
}