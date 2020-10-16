//=============================================================================
//// Shader uses position and texture
//=============================================================================
SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Mirror;
    AddressV = Mirror;
};

Texture2D gTexture;

/// Create Depth Stencil State (ENABLE DEPTH WRITING)
/// Create Rasterizer State (Backface culling) 


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

	// Step 3: Create a double for loop (5 iterations each)
	int iterations = 5;

	float4 finalColor = {0.0f, 0.0f, 0.0f, 0.0f};

	for (int r = -(iterations / 2); r < iterations / 2; r += 2)
	{
		for (int c = -(iterations / 2); c < iterations / 2; c += 2)
		{
			float2 texCoordToAdd = input.TexCoord + float2(c * dx, r * dy);

			finalColor += gTexture.Sample(samPoint, texCoordToAdd);
			//finalColor = saturate(finalColor);
		}
	}

	finalColor /= iterations;
	
	return finalColor;

	//		   Inside the loop, calculate the offset in each direction. Make sure not to take every pixel but move by 2 pixels each time
	//			Do a texture lookup using your previously calculated uv coordinates + the offset, and add to the final color
	// Step 4: Divide the final color by the number of passes (in this case 5*5)	
	// Step 5: return the final color
}


//TECHNIQUE
//---------
technique11 Blur
{
    pass P0
    {
		// Set states...
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}