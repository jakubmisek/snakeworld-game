//--------------------------------------------------------------------------------------
// variables
//--------------------------------------------------------------------------------------
float4	g_color;			// color of the menu
float2	g_position;			// menu position add vector

float	g_texalpha;	// texture transparency
float2	g_texpos;	// texture position
float2	g_texsize;	// texture size

texture texTexture;		// texture in menu item texture select

sampler samTex =
sampler_state
{
    Texture = <texTexture>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};


//--------------------------------------------------------------------------------------
// structs
//--------------------------------------------------------------------------------------
struct VS_MENU_INPUT
{
	float2	Pos		: POSITION;	// screen coordinates
	float2	Size	: TEXCOORD0;		// position relative to the menu screen
};

struct PS_MENU_INPUT
{
	float4	Pos		: POSITION;		// pixel position
	float4	Color	: TEXCOORD0;	// vertex diffuse color
	float2	Size	: TEXCOORD1;	// pixel relative position
};


struct VS_TEX_INPUT
{
	float2	Pos		: POSITION;	// position
};

struct PS_TEX_INPUT
{
	float4	Pos		: POSITION;	// pixel position
	float2	Tex		: TEXCOORD0;	// texture coord
};

//--------------------------------------------------------------------------------------
// vertex shaders
//--------------------------------------------------------------------------------------
PS_MENU_INPUT	MenuVS( VS_MENU_INPUT input )
{
	PS_MENU_INPUT output;
	
	output.Size = input.Size;
	output.Color = lerp( float4(1,1,1,0.4), g_color, input.Size.x );
	output.Pos = float4( input.Pos.x + g_position.x, input.Pos.y - g_position.y, 0.5, 1.0 ) * 2 - 1;
	
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
float4	MenuGlassPS( PS_MENU_INPUT input ) : COLOR0
{
	const float g_shadowsize = 0.018;
	
	float2 vecborder = abs( input.Size - 0.5f ) - (0.5f-g_shadowsize*2);
	
	vecborder = saturate( vecborder / (g_shadowsize*2) );
	
	float flborder = length(vecborder);
	float flShadow = (abs(flborder-0.5)*2);
	
	if ( flborder > 0.5 )
		return lerp( g_color, float4(0,0,0,0), flShadow );
	else
		return lerp( g_color, input.Color, flShadow );
}


float4	TexPS( PS_TEX_INPUT input ) : COLOR0
{
	return float4( tex2D( samTex, input.Tex ).xyz, g_texalpha * saturate( 1.5 - length(input.Tex - 0.5)*2 ) );
}


//--------------------------------------------------------------------------------------
// techniques
//--------------------------------------------------------------------------------------
technique RenderMenu
{
	// inner menu area
	pass P0
	{
		VertexShader = compile vs_2_0 MenuVS();
        PixelShader  = compile ps_2_0 MenuGlassPS();
		
        
		CullMode = None;
        ZEnable = false;
        ZWriteEnable = false;
        StencilEnable = false;
        
        AlphaBlendEnable = true;
        SrcBlend = SrcAlpha;
        DestBlend = InvSrcAlpha;
	}
		
}


technique RenderTexture
{
	pass P0
	{
		VertexShader = compile vs_2_0 TexVS();
        PixelShader  = compile ps_2_0 TexPS();
        
        CullMode = None;
        ZEnable = false;
        ZWriteEnable = false;
        StencilEnable = false;
        
		AlphaBlendEnable = true;
        SrcBlend = SrcAlpha;
        DestBlend = InvSrcAlpha;
	}
}