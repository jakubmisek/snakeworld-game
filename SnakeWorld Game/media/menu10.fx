//--------------------------------------------------------------------------------------
// variables
//--------------------------------------------------------------------------------------
cbuffer cbEveryMenu
{
	float4	g_color;			// color of the menu
	float	g_blurradius;		// blur factor
	float2	g_position;			// menu position add vector
	float2	g_backbuffersize;	// backbuffer size
};

cbuffer cbEveryTexture	// texture for menu-item-texture-select
{
	float	g_texalpha;	// texture transparency
	float2	g_texpos;	// texture position
	float2	g_texsize;	// texture size
};

cbuffer cbConstant
{
	float	g_shadowsize = 0.015f;
};

Texture2D texBackground;		// prerendered background texture
Texture2D texTexture;		// texture in menu item texture select

SamplerState samMenu
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Clamp;
    AddressV = Clamp;
};

SamplerState samTex
{
	Filter = ANISOTROPIC;
	AddressU = Wrap;
    AddressV = Wrap;
};

//--------------------------------------------------------------------------------------
// render states
//--------------------------------------------------------------------------------------
DepthStencilState DisableDepth
{
    DepthEnable = FALSE;
    DepthWriteMask = ZERO;
    
    StencilEnable = FALSE;
};

RasterizerState MenuRasterizer
{
	CullMode = None;
	MultisampleEnable = False;
};

BlendState NoBlending
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = FALSE;
    RenderTargetWriteMask[0] = 0x0F;
};

BlendState ShadowBlending
{
	AlphaToCoverageEnable = FALSE;
	
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = ADD;
    SrcBlendAlpha = ZERO;
    DestBlendAlpha = ZERO;
    BlendOpAlpha = ADD;
	
	RenderTargetWriteMask[0] = 0x0F;
};

BlendState TexBlending
{
	AlphaToCoverageEnable = FALSE;
	
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = ADD;
    SrcBlendAlpha = ZERO;
    DestBlendAlpha = ZERO;
    BlendOpAlpha = ADD;
	
	RenderTargetWriteMask[0] = 0x0F;
};


//--------------------------------------------------------------------------------------
// structs
//--------------------------------------------------------------------------------------
struct VS_MENU_INPUT
{
	float2	Pos		: POSITION;	// screen coordinates
	float2	Size	: SIZE;		// position relative to the menu screen
};

struct PS_MENU_INPUT
{
	float4	Pos		: SV_POSITION;
	float2	PosUV	: TEXCOORD0;	// 
	float2	Size	: TEXCOORD1;	// 
};


struct VS_TEX_INPUT
{
	float2	Pos		: POSITION;	// position
};

struct PS_TEX_INPUT
{
	float4	Pos		: SV_POSITION;	// pixel position
	float2	Tex		: TEXCOORD0;	// texture coord
};

//--------------------------------------------------------------------------------------
// vertex shaders
//--------------------------------------------------------------------------------------
PS_MENU_INPUT	MenuVS( VS_MENU_INPUT input )
{
	PS_MENU_INPUT output;
	
	output.Pos = float4( input.Pos.x + g_position.x, input.Pos.y - g_position.y, 0.5, 1.0 );
	output.PosUV = float2( output.Pos.x, 1 - output.Pos.y );
	output.Size = input.Size;
	
	// Pos to screen coordinates [-1, 1]
	output.Pos = output.Pos * 2 - 1;
	
	//
	return output;
}


PS_TEX_INPUT	TexVS( VS_TEX_INPUT input )
{
	PS_TEX_INPUT	output;
	
	output.Pos = float4( float2(input.Pos.x * g_texsize.x + g_texpos.x, 1 - (input.Pos.y * g_texsize.y + g_texpos.y) )*2 - 1, 0.5, 1.0 );
	output.Tex = input.Pos;
	
	return output;
}


//--------------------------------------------------------------------------------------
// pixel shaders
//--------------------------------------------------------------------------------------
float	GlassEffect( float2 p )
{
	float2 flGlassPos = p - g_position*0.05f;
	float flGlassX = flGlassPos.x * (7.0+flGlassPos.y*0.8+(1-flGlassPos.x)*8-g_position.x) - flGlassPos.y*0.9;
	float flGlass = frac( flGlassX )*2 - 1;
	flGlass *= flGlass * 0.08;
	
	int iGlass = ((uint)flGlassX+1) % 4;
	flGlass += (float)(iGlass) * 0.05;
	
	return flGlass;
}

float4	MenuGlassPS( PS_MENU_INPUT input ) : SV_Target
{
	float2 vecborder = abs( input.Size - 0.5f ) - (0.5f-(g_shadowsize*2));
	vecborder = saturate(vecborder);
	clip( g_shadowsize-length(vecborder) );
	
	// blur filter
	float4	col = float4(0,0,0,0);
	float	flGlass = 0.0f;
	float	flw = 0;

	float2 vecrad = (g_blurradius/g_backbuffersize);
	float flDistFromCenter = saturate(length( input.Size-0.5 ));
	float2 v = (input.PosUV-0.5)*(0.8+0.2*flDistFromCenter*flDistFromCenter) + 0.5;
	
	float2 flf;
	
	for (flf.x = -1; flf.x <= 1; flf.x += 1.0)
	for (flf.y = -1; flf.y <= 1; flf.y += 1.0)
	{
		float2 v2 = v + flf*vecrad;
		
		col += texBackground.Sample( samMenu, v2 );
		flGlass += GlassEffect( v2 );
		
		flw += 1.0f;
	}
	
	col /= flw;
	flGlass /= flw;
	
	// alpha
	float diffusealpha = saturate( flGlass + g_color.a );
	
	// blend with the diffuse color
	return lerp( float4(1,1,1,0.5),
				 col * (1-diffusealpha)  +  g_color * diffusealpha,
				 input.PosUV.x*0.2 + 0.8 );
}


float4	MenuShadowPS( PS_MENU_INPUT input ) : SV_Target
{
	float2 vecborder = abs( input.Size - 0.5f ) - (0.5f-g_shadowsize*2);
	clip( max(vecborder.x,vecborder.y) );
	
	vecborder = ( vecborder / (g_shadowsize*2) );
	
	vecborder = saturate(vecborder);
	
	float flShadow = 1.0f - (abs(length(vecborder)-0.5)*2);
	
	return float4(0,0,0, flShadow - 0.2f );
}


float4	TexPS( PS_TEX_INPUT input ) : SV_Target
{
	return float4( texTexture.Sample( samTex, input.Tex ).xyz, g_texalpha * saturate( 1.5 - length(input.Tex - 0.5)*2 ) );
}


//--------------------------------------------------------------------------------------
// techniques
//--------------------------------------------------------------------------------------
technique10 RenderMenu
{
	// inner menu area
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, MenuVS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, MenuGlassPS() ) );
        
		SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( DisableDepth, 0 );
        SetRasterizerState( MenuRasterizer );
	}
	
	// shadow
	pass P1
	{
		SetVertexShader( CompileShader( vs_4_0, MenuVS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, MenuShadowPS() ) );
        
		SetBlendState( ShadowBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( DisableDepth, 0 );
        SetRasterizerState( MenuRasterizer );
	}
	
}


technique10 RenderTexture
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, TexVS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, TexPS() ) );
        
		SetBlendState( TexBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( DisableDepth, 0 );
        SetRasterizerState( MenuRasterizer );
	}
}