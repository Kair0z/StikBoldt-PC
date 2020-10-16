//=====//=============================================================================
//// Shader uses position and texture
//=============================================================================
SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Mirror;
    AddressV = Mirror;
};

Texture2D gTexture;
float3 gColor;
float gIntensity;
float gScale = 1.f;

/// Create Depth Stencil State (ENABLE DEPTH WRITING)
/// Create Rasterizer State (Backface culling) 

DepthStencilState gDSS
{
	DepthEnable = TRUE;
};

RasterizerState gRS
{
	CullMode = BACK;
};


//IN/OUT STRUCTS
//--------------
struct VS_INPUT
{
    float3 Position : POSITION;
	float2 TexCoord : TEXCOORD0;

};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD1;
};


//VERTEX SHADER
//-------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;

	output.Position = float4(input.Position, 1.0f);
	output.TexCoord = input.TexCoord;
	
	return output;
}


//PIXEL SHADER
//------------
float4 PS(PS_INPUT input) : SV_Target
{
	// Step 1: find the dimensions of the texture (the texture has a method for that)	
	float x{};
	float y{};
	gTexture.GetDimensions(x, y);

	// Step 2: calculate dx and dy (UV space for 1 pixel)	
	float dx = 1.0f / x;
	float dy = 1.0f / y;
	float4 finalColor = { 0.0f, 0.0f, 0.0f, 0.0f };
	float4 vignetteColor = float4(gColor, 1.f);

	// Always [0 - 1]
	float distanceFromCenter = length(float2(input.TexCoord.x, input.TexCoord.y) - float2(0.5f, 0.5f));

	finalColor = gTexture.Sample(samPoint, input.TexCoord);
	finalColor.a = 1.f;

	finalColor *= vignetteColor * distanceFromCenter * gIntensity;

	return finalColor;
}


//TECHNIQUE
//---------
technique11 Vignette
{
    pass P0
    {
		// Set states...
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );

		SetRasterizerState(gRS);
		SetDepthStencilState(gDSS, 0);
    }
}