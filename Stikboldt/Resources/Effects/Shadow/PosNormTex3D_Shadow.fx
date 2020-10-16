float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 
float4x4 gWorldViewProj_Light;
float3 gLightDirection = float3(-0.577f, -0.577f, 0.577f);
float gShadowMapBias = 0.001f;
float gAmbient = 0.0f;

Texture2D gDiffuseMap;
Texture2D gShadowMap;

SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;// or Mirror or Clamp or Border
    AddressV = Wrap;// or Mirror or Clamp or Border
};

SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap;// or Mirror or Clamp or Border
	AddressV = Wrap;// or Mirror or Clamp or Border
};

RasterizerState Solid
{
	FillMode = SOLID;
	CullMode = FRONT;
};

DepthStencilState EnableDepth
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
};

RasterizerState NoCulling
{
	CullMode = NONE;
};

//--------------------------------------------------------------------------------------
// Structs
//--------------------------------------------------------------------------------------

struct VS_INPUT
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
	float4 lPos : TEXCOORD1;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.pos = mul(float4(input.pos, 1), gWorldViewProj);
	output.normal = input.normal;
	output.texCoord = input.texCoord;

	//Hint: Don't forget to project our position to light clip space and store it in lPos
	output.lPos = mul(float4(input.pos, 1), gWorldViewProj_Light);
	
	return output;
}

float2 texOffset(int u, int v)
{
	uint miplevel = 0;
	uint w, h, levels;

	gShadowMap.GetDimensions(miplevel, w, h, levels);
	return float2(u * 1.f / float(w), v * 1.f / float(h));
}


float EvaluateShadowMap(float4 lpos)
{
	float ambient = gAmbient;
	float shadow = 1.f;

	// 1: Re-homogenize the clip space coordinate:
	lpos /= lpos.w;

	// 2: Point inside view of the light?:
	bool isPointInView = true;
	if (lpos.x > 1.f || lpos.y > 1.f || lpos.z > 1.f) isPointInView = false;
	if (lpos.x < -1.f || lpos.y < -1.f || lpos.z < 0.f) isPointInView = false;

	if (!isPointInView) return ambient;
	else
	{
		// 3: Convert a 3D point inside light clipping space >> 2D point in shadowmap texture space
		lpos.x = lpos.x / 2.f + 0.5f;
		lpos.y = lpos.y / -2.f + 0.5f;

		// 3.5: Apply shadow map bias:
		lpos.z -= gShadowMapBias;

		// 4: Sample from shadowmap with converted coordinates
		float shadowMapDepth = gShadowMap.Sample(samPoint, lpos.xy + texOffset(lpos.x, lpos.y)).r;

		// 5: If z-value > shadowmap-value 
		// --> Pixel is further away than obstructing pixel 
		// --> Pixel is in shadow
		if (lpos.z > shadowMapDepth) return ambient;

		return shadow;
	}
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET
{
	float shadowValue = EvaluateShadowMap(input.lPos);

	float4 diffuseColor = gDiffuseMap.Sample( samLinear,input.texCoord );
	float3 color_rgb= diffuseColor.rgb;
	float color_a = diffuseColor.a;
	
	//HalfLambert Diffuse :)
	float diffuseStrength = dot(input.normal, -gLightDirection);
	diffuseStrength = diffuseStrength * 0.5 + 0.5;
	diffuseStrength = saturate(diffuseStrength);
	color_rgb = color_rgb * diffuseStrength;

	return float4( color_rgb * shadowValue , color_a );
}

//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------
technique11 Default
{
    pass P0
    {
		SetRasterizerState(NoCulling);
		SetDepthStencilState(EnableDepth, 0);

		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

