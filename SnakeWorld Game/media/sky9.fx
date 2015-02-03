//--------------------------------------------------------------------------------------
// variables
//--------------------------------------------------------------------------------------

float4x4	matViewProj;


// textures
texture  texDiffuse;              // texture for rendering

sampler samLinear =
sampler_state
{
    Texture = <texDiffuse>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};


//--------------------------------------------------------------------------------------
// structs
//--------------------------------------------------------------------------------------


	// SKYBOX (SKY)
struct VS_SKY_INPUT
{
	float3	Pos		: POSITION;
	float2	Tex		: TEXCOORD0;
};
struct PS_SKY_INPUT
{
	float4	Pos		: POSITION;
	float2	Tex		: TEXCOORD0;
};


//--------------------------------------------------------------------------------------
// VERTEX SHADER
//--------------------------------------------------------------------------------------
PS_SKY_INPUT SkyBoxVS( VS_SKY_INPUT input )
{
	PS_SKY_INPUT output;
	
	output.Pos = mul( float4( input.Pos, 1), matViewProj );
	output.Tex = input.Tex;
	
	return output;
}

//--------------------------------------------------------------------------------------
// PIXEL SHADER
//--------------------------------------------------------------------------------------
float4 SkyBoxPS( PS_SKY_INPUT input) : COLOR0
{
	return float4( tex2D( samLinear, input.Tex ).xyz, 1.0f );
}



//--------------------------------------------------------------------------------------
// Technique Definition
//--------------------------------------------------------------------------------------

technique RenderSkyBox
{
	pass P0
	{
		VertexShader = compile vs_2_0 SkyBoxVS();
        PixelShader  = compile ps_2_0 SkyBoxPS();
        
        CullMode = None;
        ZEnable = false;
        ZWriteEnable = false;
        StencilEnable = false;
        AlphaBlendEnable = false;
	}
}
