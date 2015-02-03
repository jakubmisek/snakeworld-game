//--------------------------------------------------------------------------------------
// variables
//--------------------------------------------------------------------------------------

cbuffer cbEveryFrame
{
	matrix	matViewProj;
};


// textures
Texture2D texDiffuse;			// diffuse texture

SamplerState samLinear
{
    Filter = ANISOTROPIC;
    AddressU = Wrap;
    AddressV = Wrap;
};


//--------------------------------------------------------------------------------------
// structs
//--------------------------------------------------------------------------------------


	// SKYBOX (SKY)
struct VS_SKY_INPUT
{
	float3	Pos		: POSITION;
	float2	Tex		: TEXCOORD;
};
struct PS_SKY_INPUT
{
	float4	Pos		: SV_POSITION;
	float2	Tex		: TEXCOORD0;
};


//--------------------------------------------------------------------------------------
// Render States
//--------------------------------------------------------------------------------------

DepthStencilState DisableDepth
{
    DepthEnable = FALSE;
    DepthWriteMask = ZERO;
    
    StencilEnable = FALSE;
};

RasterizerState NormalRasterizer
{
	CullMode = Back;
	MultisampleEnable = False;
};

BlendState NoBlending
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = FALSE;
    RenderTargetWriteMask[0] = 0x0F;
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
float4 SkyBoxPS( PS_SKY_INPUT input) : SV_Target
{
	return texDiffuse.Sample( samLinear, input.Tex );
}



//--------------------------------------------------------------------------------------
// Technique Definition
//--------------------------------------------------------------------------------------

technique10 RenderSkyBox
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, SkyBoxVS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, SkyBoxPS() ) );
        
        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( DisableDepth, 0 );
        SetRasterizerState( NormalRasterizer );
	}
}
