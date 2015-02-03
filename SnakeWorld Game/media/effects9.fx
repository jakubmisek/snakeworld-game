
#define LIGHT_DIRECTION		float3( -0.883, -0.342, -0.3214 )

//
// global variables
//

float4x4 g_matViewProj;             // View*Projection matrix
float4x4 g_matWorld;				// World matrix
float4	 g_vecViewDir;				// view direction

float	flBodySegmentsDistanceFromTail;	// current segment distance from tail
float	flBodySegmentsDistanceFromHead;	// current segment distance from head

float	g_flButterflyAnim;	// butterfly wings animation [0;1]

texture  g_texDiffuse;              // texture for rendering
texture  g_texNormal;				// normal map

//
// sampler states
//

sampler g_samScene =
sampler_state
{
    Texture = <g_texDiffuse>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};

sampler g_samNormal =
sampler_state
{
    Texture = <g_texNormal>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
};


//
// structs
//

struct PS_SCENE_INPUT
{
	float4 Pos:POSITION;
	float3 Norm:TEXCOORD0;
	float2 Tex:TEXCOORD1;
};

	// TERRAIN (TER)
struct VS_TER_INPUT
{
	float3 Pos:POSITION;
};



	// STONE (ST)
struct VS_ST_INPUT
{
	float3	Pos		: POSITION;	// vertex position
	float3	Norm	: NORMAL;	// vertex normal
	float2	Tex		: TEXCOORD;	// texture coordinates
};

	// SNAKE BODY (SB)
struct VS_SB_INPUT
{
    float3 PosCenter : POSITION;         // center of the ring
    float3 Dir       : NORMAL;			 // position - center
    float3 Tangent   : TANGENT;		 // position - center
    float3 Binormal  : BINORMAL;		 // position - center
    float2 Distance  : TEXCOORD;		 // distance from the beginning of segments model and TexV
};
struct PS_SB_INPUT
{
    float4 Pos		: POSITION;			 // pixel position
    float2 Tex		: TEXCOORD0;		 // texture coordinates
    float3 Camera	: TEXCOORD1;		 // camera direction in tangent space
    float3 Light	: TEXCOORD2;		 // light direction in tangent space
};

	// SNAKE EYE (SE)
struct VS_SE_INPUT
{
	float3 Pos		: POSITION;		// position
	float3 Norm		: NORMAL;		// normal
};

struct PS_SE_INPUT
{
	float4 Pos		: POSITION;		// pixel position
	float3 Norm		: TEXCOORD0;		// surface normal
};

	// GRASS (GR)
struct VS_GR_INPUT
{
	float3	Pos	: POSITION;
	float2	Tex	: TEXCOORD;
};
struct PS_GR_INPUT
{
	float4 Pos:POSITION;
	float2 Tex:TEXCOORD0;
};

	// BUTTERFLY (BF)
struct VS_BF_INPUT
{
	float3	Pos	: POSITION;
	float2	Tex	: TEXCOORD;
	float	W	: BLENDWEIGHT;
};

struct PS_BF_INPUT
{
	float4 Pos:POSITION;
	float2 Tex:TEXCOORD0;
};


//
// VERTEX SHADER
//
PS_SCENE_INPUT TerrainVS( VS_TER_INPUT input )
{
	PS_SCENE_INPUT output;
	
	output.Pos = mul( float4( input.Pos, 1), g_matWorld );
	output.Tex = float2(output.Pos.x * 0.04, output.Pos.z * 0.04 );
	
	output.Norm = float3(0,1,0);
	
	output.Pos = mul( output.Pos, g_matViewProj );
	
	//output.Pos = float4( input.Pos.x/50-1, -input.Pos.z/50+1, 0.5,1);
	
	return output;
}


PS_SCENE_INPUT StoneWallVS( VS_ST_INPUT input )
{
	PS_SCENE_INPUT output;
	
	output.Pos = mul( float4( input.Pos, 1), g_matWorld );
	output.Pos = mul( output.Pos, g_matViewProj );
	output.Norm = mul( input.Norm, (float3x3)g_matWorld );
	output.Tex = input.Tex;
	
	return output;
}


PS_SB_INPUT SnakeBodyVS( VS_SB_INPUT input )
{
	PS_SB_INPUT output;
	
	float flDstTail = input.Distance.x + flBodySegmentsDistanceFromTail;
	float flDstHead = flBodySegmentsDistanceFromHead - input.Distance.x;
	
	float flRad = min( flDstTail*0.07f, 1.0f ) * (1.0f / (abs(flDstTail*0.8f - flDstHead*1.2f)*( 0.05f ) + 1.0f) + 1.0f);	// body radius
	
	output.Pos = mul( float4( input.PosCenter + (input.Dir * flRad), 1.0f ), g_matWorld );
	output.Pos = mul( output.Pos, g_matViewProj );
	
	output.Tex = float2(input.Distance.y, flDstHead * 0.2f);
	
	float3x3 tangentSpaceMatrix;
	tangentSpaceMatrix[0] = mul( input.Tangent, (float3x3)g_matWorld );
	tangentSpaceMatrix[1] = mul( input.Binormal, (float3x3)g_matWorld );
	tangentSpaceMatrix[2] = mul( input.Dir, (float3x3)g_matWorld );
	
	output.Camera = mul(tangentSpaceMatrix, normalize(g_vecViewDir));
	output.Light = mul(tangentSpaceMatrix, normalize(LIGHT_DIRECTION));
	
	return output;
}

PS_SE_INPUT SnakeEyeVS( VS_SE_INPUT input )
{
	PS_SE_INPUT output;
	
	output.Pos = mul( float4(input.Pos,1), g_matWorld );
	output.Norm = mul( input.Norm, (float3x3)g_matWorld );
	
	output.Pos = mul( output.Pos, g_matViewProj );
	
	return output;
}

PS_GR_INPUT GrassVS( VS_GR_INPUT input )
{
	PS_GR_INPUT output;
	
	output.Pos = mul( float4( input.Pos, 1), g_matWorld );
	output.Pos = mul( output.Pos, g_matViewProj );
	output.Tex = input.Tex;
	
	return output;
}

PS_BF_INPUT ButterflyVS( VS_BF_INPUT input )
{
	PS_BF_INPUT output;
	
	output.Pos = mul( float4( input.Pos.x + input.W*(g_flButterflyAnim*g_flButterflyAnim), input.Pos.y + abs(input.W)*(g_flButterflyAnim*1.3), input.Pos.z, 1), g_matWorld );
	output.Pos = mul( output.Pos, g_matViewProj );
	output.Tex = input.Tex;
	
	return output;
}

//
// PIXEL SHADER
//
float4 ScenePS( PS_SCENE_INPUT input ) : COLOR0
{
    float fLighting = saturate( -dot( input.Norm, LIGHT_DIRECTION ) ) * 0.6f + 0.4f;

    // Lookup mesh texture and modulate it with lighting
    return float4( tex2D( g_samScene, input.Tex ).xyz * fLighting, 1.0f );
}


float4 SnakeBodyPS( PS_SB_INPUT input) : COLOR0
{
	// normal map
	float3 normal = tex2D( g_samNormal, input.Tex ).xyz * 2 - 1; // normal taken from the normal map
	
	float3 light = normalize( input.Light );		// light direction in tangent space
	float3 camera = normalize( input.Camera );		// view direction in tangent space
	
	// specular
	float3 r = reflect( normal, light );			// reflect the light direction for specular lighting
	float spec = pow( saturate( dot( -r, camera ) ), 30.0f );
	
	// +diffuse
	float fLighting = saturate( -dot( normal, light ) ) * 0.6f + spec*0.2f + 0.4f;	// diffuse lighting + specular + ambient
	
	// texture
    return float4( tex2D( g_samScene, input.Tex ).xyz * fLighting + spec*0.5f, 1.0f );	// texture * lighting + pure specular
}


float4 SnakeEyePS( PS_SE_INPUT input ) : SV_Target
{
	float3 r = reflect( input.Norm, LIGHT_DIRECTION );
	float spec = pow( saturate( dot( -r, g_vecViewDir ) ), 10.0f );

	float fLighting = saturate( -dot( input.Norm, LIGHT_DIRECTION ) ) * 0.2f + spec*0.8f;
	
	return float4( float3(1,1,1)*fLighting ,1);
}


float4 GrassPS( PS_GR_INPUT input ) : COLOR0
{
	return tex2D( g_samScene, input.Tex );
}

float4 ButterflyPS( PS_BF_INPUT input ) : COLOR0
{
	return tex2D( g_samScene, input.Tex );
}


float4 ApplePS( PS_SCENE_INPUT input ) : COLOR0
{
	float3 r = reflect( input.Norm, LIGHT_DIRECTION );
	float spec = pow( saturate( dot( -r, g_vecViewDir ) ), 10.0f );

    float fLighting = saturate( -dot( input.Norm, LIGHT_DIRECTION ) ) * 0.6f + 0.4f;
	
    // Lookup mesh texture and modulate it with lighting
    return float4( tex2D( g_samScene, input.Tex ).xyz * fLighting, 1.0f ) + spec*0.8f;
}

//
// TECHNIQUES
//

technique RenderTerrain
{
    pass P0
    {
        VertexShader = compile vs_2_0 TerrainVS();
        PixelShader  = compile ps_2_0 ScenePS();
        
        CullMode = CCW;
        ZEnable = true;
        ZWriteEnable = true;
        ZFunc = LessEqual;
        StencilEnable = false;
        AlphaBlendEnable = false;
        AlphaTestEnable = false;
    }
    
}

technique RenderStoneWall
{
    pass P0
    {
        VertexShader = compile vs_2_0 StoneWallVS();
        PixelShader  = compile ps_2_0 ScenePS();
        
        CullMode = CCW;
        ZEnable = true;
        ZWriteEnable = true;
        ZFunc = LessEqual;
        StencilEnable = false;
        AlphaBlendEnable = false;        
        AlphaTestEnable = false;
    }
    
}

technique RenderApple
{
    pass P0
    {
        VertexShader = compile vs_2_0 StoneWallVS();
        PixelShader  = compile ps_2_0 ApplePS();
        
        CullMode = CCW;
        ZEnable = true;
        ZWriteEnable = true;
        ZFunc = LessEqual;
        StencilEnable = false;
        AlphaBlendEnable = false;        
        AlphaTestEnable = false;
    }
    
}

technique RenderSnakeBody
{
    pass P0
    {
        VertexShader = compile vs_2_0 SnakeBodyVS();
        PixelShader  = compile ps_2_0 SnakeBodyPS();
        
        CullMode = CCW;
        ZEnable = true;
        ZWriteEnable = true;
        ZFunc = LessEqual;
        StencilEnable = false;
        AlphaBlendEnable = false;  
        AlphaTestEnable = false;      
    }
    
}


technique RenderSnakeEye
{
    pass P0
    {
        VertexShader = compile vs_2_0 SnakeEyeVS();
        PixelShader  = compile ps_2_0 SnakeEyePS();
        
        CullMode = CCW;
        ZEnable = true;
        ZWriteEnable = true;
        ZFunc = LessEqual;
        StencilEnable = false;
        AlphaBlendEnable = false;
        AlphaTestEnable = false;
    }
    
}

technique RenderGrass
{
	pass P0
	{
		VertexShader = compile vs_2_0 GrassVS();
        PixelShader  = compile ps_2_0 GrassPS();
        
        CullMode = None;
        ZEnable = true;
        ZWriteEnable = true;
        ZFunc = LessEqual;
        StencilEnable = false;
        
		AlphaBlendEnable = true;
		AlphaTestEnable = true;
		
		AlphaFunc = 5;
		AlphaRef = 0x60;
		
		SrcBlend = SrcAlpha;
        DestBlend = InvSrcAlpha;
	}
}

technique RenderButterfly
{
	pass P0
	{
		VertexShader = compile vs_2_0 ButterflyVS();
        PixelShader  = compile ps_2_0 ButterflyPS();
        
        CullMode = None;
        ZEnable = true;
        ZWriteEnable = true;
        ZFunc = LessEqual;
        StencilEnable = false;
        
		AlphaBlendEnable = true;
		AlphaTestEnable = true;
		
		AlphaFunc = 5;
		AlphaRef = 0x60;
		
		SrcBlend = SrcAlpha;
        DestBlend = InvSrcAlpha;
	}
}