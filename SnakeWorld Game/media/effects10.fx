//--------------------------------------------------------------------------------------
// variables
//--------------------------------------------------------------------------------------


cbuffer cbEveryFrame
{
	matrix	matViewProj;
	float3	vecView;
	float3	vecViewEye;
	float	flFrameLength;
};

cbuffer cbEverySnakeSegment
{
	float	flBodySegmentsDistanceFromTail;
	float	flBodySegmentsDistanceFromHead;
}

cbuffer cbConstant
{
	static const uint nBodyPoints = 7;			// snake body shape points count

	static const float2 vecBodyPoints[7] =		// snake body shape
		{
			float2( -1.0f, 0.0f ),
			float2( -0.866f, 0.5f ),
			float2( -0.5f, 0.866f ),
			float2( +0.0f, 1.0f ),
			float2( +0.5f, 0.866f ),
			float2( +0.866f, 0.5f ),
			float2( +1.0f, 0.0f ),
		};
		
	static const float3 vecLightDirection = float3( -0.883, -0.342, -0.3214 );	// world light direction
	
	static const float flGrassBounce = 0.2f;		// grass bounce, 0.0 - 0.5
	
	static const float flGravity = 9.81f;	// gravitation constant (m / s^2)
	
	static const float flTerrainSize = 50.0f;	// terrain plate size ( see terrain.h -> TERRAIN_SIZE !!! )
};

cbuffer cbEveryObject
{
	matrix matWorld;			// world transform
};

cbuffer cbEveryButterfly
{
	float	flButterflyAnim;	// butterfly wings animation [0;1]
};

// textures
Texture2D texDiffuse;			// diffuse texture
Texture2D texNormal;			// normal map

Texture2DArray tex2dArray;		// grass textures

SamplerState samLinear
{
    Filter = ANISOTROPIC;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap;
    AddressV = Wrap;
};

//--------------------------------------------------------------------------------------
// structs
//--------------------------------------------------------------------------------------

	// SNAKE BODY ( SB )
struct VS_SB_INPUT
{
    float3 Pos       : POSITION;         // position
    float3 Size      : NORMAL;			 // vector in right angle with the body, normalized, y = height
    float  Distance  : DISTANCE;		 // distance from the beginning of segments model
};

struct GS_SB_INPUT
{
    float4 Pos       : POSITION;		 // position in the world       
    float3 Norm      : TEXCOORD0;		 // vector in right angle with the body + y=height, multiplied by the segment radius
    float  TexU		 : TEXCOORD1;		 // texture U-coordinate
};

struct PS_SB_INPUT
{
    float4 Pos		: SV_POSITION;		 // pixel position
    float2 Tex		: TEXCOORD0;		 // texture coordinates
    float3 Camera	: TEXCOORD1;		// camera direction in tangent space
    float3 Light	: TEXCOORD2;		// light direction in tangent space
};

	// SNAKE EYE (SE)
struct VS_SE_INPUT
{
	float3 Pos		: POSITION;		// position
	float3 Norm		: NORMAL;		// normal
};

struct PS_SE_INPUT
{
	float4 Pos		: SV_POSITION;		// pixel position
	float3 Norm		: TEXCOORD0;		// surface normal
};

	// TERRAIN (TR)
struct VS_TR_INPUT
{
    float3 Pos      : POSITION;			// vector world position
};

struct PS_TR_INPUT
{
    float4 Pos		: SV_POSITION;		// pixel position
    float3 Norm		: TEXCOORD0;		// surface normal
    float2 Tex		: TEXCOORD1;		// texture coordinates
};

	// GRASS (GR)
struct VSGS_GR_INPUT
{
	float3 Pos  		: POSITION;		// grass position in the world
	float3 Seg[2]		: SEGDIR;		// grass stalk segments normalized direction
	float3 DefSeg		: DEFSEG;		// default grass stalk vector
	float3 SegSpeed[2]	: SEGSPEED;		// grass stalk segments speed/sec
	float2 Dir			: DIRECTION;	// grass orientation
	//float2 DefDir		: DEFDIRECTION;	// grass default orientation
	float  SegHeight	: HEIGHT;		// grass segment height (= stalk height/3)
};

struct PS_GR_INPUT
{
	float4 Pos		: SV_POSITION;		// pixel position
    //float3 Norm		: TEXCOORD0;		// surface normal
    float3 Tex		: TEXCOORD0;		// texture coordinates
};


	// STONE (ST)
struct VS_ST_INPUT
{
	float3	Pos		: POSITION;	// vertex position
	float3	Norm	: NORMAL;	// vertex normal
	float2	Tex		: TEXCOORD;	// texture coordinates
};
struct GS_ST_INPUT
{
	float4	Pos		: SV_POSITION;
	float3	Norm	: TEXCOORD0;	// normal
	float2	Tex		: TEXCOORD1;	// texture coordinates
};
struct PS_ST_INPUT
{
	float4	Pos		: SV_POSITION;
	float2	Tex		: TEXCOORD0;	// texture coordinates
    float3	Light	: TEXCOORD1;	// light direction in tangent space
};

	// APPLE (AP)
struct VS_AP_INPUT
{
	float3	Pos		: POSITION;	// vertex position
	float3	Norm	: NORMAL;	// vertex normal
	float2	Tex		: TEXCOORD;	// texture coordinates
};
struct PS_AP_INPUT
{
	float4	Pos		: SV_POSITION;
	float2	Tex		: TEXCOORD0;	// texture coordinates
    float3	Norm	: TEXCOORD1;	// normal
};

	// GRASS MOVE MASK
struct PS_GRASSMOVEMASK_INPUT
{
	float4 Pos		: SV_POSITION;		// pixel position
	float2 Dir		: TEXCOORD0;		// grass move force direction
};

	// SHADOW VOLUME
struct PS_SHADOWVOLUME_INPUT
{
	float4 Pos		: SV_POSITION;		// pixel position
};

	// SHADOW
struct VS_SHADOW_INPUT
{
    float3 Pos      : POSITION;			// vertex screen pos
};
struct PS_SHADOW_INPUT
{
	float4 Pos		: SV_POSITION;		// pixel screen pos
};

	// BUTTERFLY (BF)
struct VSGS_BF_INPUT
{
	float3	Pos	: POSITION;
};

struct PS_BF_INPUT
{
	float4 Pos:SV_POSITION;
	float2 Tex:TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Render States
//--------------------------------------------------------------------------------------
DepthStencilState EnableDepth
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
    
    StencilEnable = FALSE;
};

DepthStencilState DisableDepth
{
    DepthEnable = FALSE;
    DepthWriteMask = ZERO;
    
    StencilEnable = FALSE;
};

DepthStencilState ShadowVolumeDepthStencil
{
	DepthEnable = TRUE;
    DepthWriteMask = ZERO;
    
    // Setup stencil states
    StencilEnable = true;
    StencilReadMask = 0xFFFFFFFF;
    StencilWriteMask = 0xFFFFFFFF;
    
    BackFaceStencilFunc = Always;
    BackFaceStencilDepthFail = Keep;
    BackFaceStencilPass = Decr;
    BackFaceStencilFail = Keep;
    
    FrontFaceStencilFunc = Always;
    FrontFaceStencilDepthFail = Keep;
    FrontFaceStencilPass = Incr;
    FrontFaceStencilFail = Keep;
};

DepthStencilState ShadowDepthStencil
{
	DepthEnable = FALSE;
    DepthWriteMask = ZERO;
    
    // Setup stencil states
    StencilEnable = TRUE;
    StencilReadMask = 0xFFFFFFFF;
    StencilWriteMask = 0;
    
    BackFaceStencilFunc = NOT_EQUAL;
    BackFaceStencilDepthFail = Keep;
    BackFaceStencilPass = Keep;
    BackFaceStencilFail = Keep;
    
    FrontFaceStencilFunc = NOT_EQUAL;
    FrontFaceStencilDepthFail = Keep;
    FrontFaceStencilPass = Keep;
    FrontFaceStencilFail = Keep;
};

RasterizerState NormalRasterizer
{
	CullMode = Back;
	MultisampleEnable = False;
};

RasterizerState GrassRasterizer
{
	CullMode = None;
	MultisampleEnable = True;
	//AntialiasedLineEnable = True;
};

RasterizerState ButterflyRasterizer
{
	CullMode = None;
	MultisampleEnable = True;
	//AntialiasedLineEnable = True;
};

RasterizerState ShadowVolumeRasterizer
{
	CullMode = NONE;
	MultisampleEnable = False;
};

RasterizerState ShadowRasterizer
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

BlendState GrassBlendState
{
	AlphaToCoverageEnable = TRUE;
	BlendEnable[0] = FALSE;
	RenderTargetWriteMask[0] = 0x0F;
};

BlendState ButterflyBlendState
{
	AlphaToCoverageEnable = TRUE;
	BlendEnable[0] = FALSE;
	RenderTargetWriteMask[0] = 0x0F;
};

BlendState ShadowVolumeBlending
{
	AlphaToCoverageEnable = FALSE;
	BlendEnable[0] = FALSE;
	RenderTargetWriteMask[0] = 0x00;
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


//--------------------------------------------------------------------------------------
// VERTEX SHADER
//--------------------------------------------------------------------------------------
float TerrainHeight( float x, float z )
{
	/*x *= 0.01f;
	z *= 0.02f;
	
	float fx = frac(x)*2-1, fz = frac(z)*2-1;
	
	fx=fx*fx;
	fz=fz*fz;
	
	float r = (fx - fz)*0.5f + 0.5f;		// 0 - 1
	
	return r*3;*/
	
	float2 vecd = float2( x - vecViewEye.x, z - vecViewEye.z ) * (1.0f / 80.0f);	// distance from the view eye
	return - (vecd.x*vecd.x + vecd.y*vecd.y);	// - distance^2
	
	//return 0;
}

float3 TerrainNormal( float x, float z )
{
	/*float ph = TerrainHeight(x,z);
	float vh[4];
	
	vh[0] = TerrainHeight(x,z+1) - ph;
	vh[1] = TerrainHeight(x+1,z) - ph;
	vh[2] = TerrainHeight(x,z-1) - ph;
	vh[3] = TerrainHeight(x-1,z) - ph;
	
	float3 n[4];
	
	n[0] = normalize( float3( 0, 1, -vh[0] ) );
	n[1] = normalize( float3( -vh[1], 1, 0 ) );
	n[2] = normalize( float3( 0, 1, +vh[2] ) );
	n[3] = normalize( float3( +vh[3], 1, 0 ) );
	
	return normalize( n[0] + n[1] + n[2] + n[3] );*/
	
	return float3(0,1,0);
}

VS_SB_INPUT SnakeBodyVSPassThrough( VS_SB_INPUT input )
{
	return input;
}


GS_SB_INPUT SnakeBodyVS( VS_SB_INPUT input )
{
	GS_SB_INPUT output;
	
	float flDstTail = input.Distance + flBodySegmentsDistanceFromTail;
	float flDstHead = flBodySegmentsDistanceFromHead - input.Distance;
	
	float flRad = min( flDstTail*0.07f, 1.0f ) * (1.0f / (abs(flDstTail*0.8f - flDstHead*1.2f)*( 0.05f ) + 1.0f) + 1.0f);	// body radius - works fine
	
	output.Pos = mul( float4( input.Pos, 1.0f ), matWorld );
	output.Norm = mul( input.Size * flRad, (float3x3)matWorld );
	output.TexU = flDstHead * 0.2f;
	
	output.Pos.y += TerrainHeight( output.Pos.x, output.Pos.z );
	
	return output;
}


PS_SE_INPUT SnakeEyeVS( VS_SE_INPUT input )
{
	PS_SE_INPUT output;
	
	output.Pos = mul( float4(input.Pos,1), matWorld );
	output.Norm = mul( input.Norm, (float3x3)matWorld );
	
	output.Pos.y += TerrainHeight( output.Pos.x, output.Pos.z );
	output.Pos = mul( output.Pos, matViewProj );
	
	return output;
}


PS_TR_INPUT TerrainVS( VS_TR_INPUT input )
{
	PS_TR_INPUT output;
	
	output.Pos = mul( float4( input.Pos, 1), matWorld );
	output.Tex = float2(output.Pos.x * 0.04, output.Pos.z * 0.04 );
	
	output.Norm = TerrainNormal( output.Pos.x, output.Pos.z );
	output.Pos.y += TerrainHeight( output.Pos.x, output.Pos.z );
	
	output.Pos = mul( output.Pos, matViewProj );
	
	return output;
}

VSGS_GR_INPUT GrassVSPassThrough( VSGS_GR_INPUT input )
{
	return input;
}

VSGS_BF_INPUT ButterflyVSPassThrough( VSGS_BF_INPUT input )
{
	return input;
}

GS_ST_INPUT StoneWallVS( VS_ST_INPUT input )
{
	GS_ST_INPUT output;
	
	output.Pos = mul( float4( input.Pos, 1), matWorld );
	output.Norm = mul( input.Norm, (float3x3)matWorld );
	output.Tex = input.Tex;
	
	output.Pos.y += TerrainHeight( output.Pos.x, output.Pos.z );
	
	return output;
}

PS_AP_INPUT AppleVS( VS_AP_INPUT input )
{
	PS_AP_INPUT output;
	
	output.Pos = mul( float4( input.Pos, 1), matWorld );
	output.Norm = mul( input.Norm, (float3x3)matWorld );
	output.Tex = input.Tex;
	
	output.Pos.y += TerrainHeight( output.Pos.x, output.Pos.z );
	output.Pos = mul( output.Pos, matViewProj );
	
	return output;
}

PS_SHADOW_INPUT	ShadowVS( VS_SHADOW_INPUT input )
{
	PS_SHADOW_INPUT output;
	
	output.Pos = float4( input.Pos.x, input.Pos.y, 0.5f, 1.0f );
	
	return output;
}

//--------------------------------------------------------------------------------------
// GEOMETRY SHADER
//--------------------------------------------------------------------------------------
[maxvertexcount(10)]
void AdvanceSnakeBodyGS( line VS_SB_INPUT input[2], inout PointStream<VS_SB_INPUT> LnStream )
{
	if ( input[0].Distance == 0.0f )
		LnStream.Append( input[0] );	// the first vertex in vertex buffer
	
	float flLen = input[1].Distance  - input[0].Distance;
	
	// max 8 new points
	if ( flLen > 1.0f )
	{
		VS_SB_INPUT output = input[0];
		
		uint iNewPointsCount = (uint)min( flLen, 8.0f );
		
		float flStep = 1.0f / (float)(iNewPointsCount+1);
		
		float3 vecStep = (input[1].Pos - input[0].Pos) * flStep;
		float3 vecSizeStep = (input[1].Size - input[0].Size) * flStep;
		float flDistStep = (input[1].Distance  - input[0].Distance) * flStep;
		
		// i+1 new segments
		for ( ;iNewPointsCount > 0; iNewPointsCount -- )
		{
			output.Pos += vecStep;
			output.Distance += flDistStep;
			output.Size += vecSizeStep;
			
			LnStream.Append( output );
		}
	}
		
	// 
	LnStream.Append( input[1] );	// end vertex
}

[maxvertexcount(36)]	// (nBodyPoints - 1) * 6
void SnakeBodyGS( line GS_SB_INPUT input[2], inout TriangleStream<PS_SB_INPUT> TriStream )
{
	PS_SB_INPUT output[4] = { {{0,0,0,0},{0,0},{0,0,0},{0,0,0}}, {{0,0,0,0},{0,0},{0,0,0},{0,0,0}}, {{0,0,0,0},{0,0},{0,0,0},{0,0,0}}, {{0,0,0,0},{0,0},{0,0,0},{0,0,0}} };
	float4 wt[4];
	float3x3 tangentSpaceMatrix;
	
	float3	vecTangent[2];
	
	/* PT 0 */
	// vertex world position (for pt 0) (optimalized for pt 0)
	wt[2] = float4(	input[0].Pos.x - input[0].Norm.x,
					input[0].Pos.y,
					input[0].Pos.z - input[0].Norm.z,
					1.0f );
					
	wt[3] = float4(	input[1].Pos.x - input[1].Norm.x,
					input[1].Pos.y,
					input[1].Pos.z - input[1].Norm.z,
					1.0f );
	
	// tangent space matrix (for pt 0) (optimalized for pt 0)
	//tangentSpaceMatrix[0] = normalize( wt[3].xyz - wt[2].xyz ); // tangent // for each segment
	tangentSpaceMatrix[1] = float3(0,1,0);	// binormal	// for each pt
	//tangentSpaceMatrix[2] = // normal // for each vertex
	
	// output vertex (for pt 0) (optimalized for pt 0)
	tangentSpaceMatrix[2] = normalize( wt[2].xyz - input[0].Pos.xyz );// normal
	tangentSpaceMatrix[0] = vecTangent[0] = normalize( float3(tangentSpaceMatrix[2].z, 0, -tangentSpaceMatrix[2].x) ); // tangent
	
	output[2].Pos = mul( wt[2], matViewProj );
	output[2].Tex = float2( 0, input[0].TexU );
	output[2].Camera = mul(tangentSpaceMatrix, normalize(vecView));
	output[2].Light = mul(tangentSpaceMatrix, normalize(vecLightDirection));
	
	tangentSpaceMatrix[2] = normalize( wt[3].xyz - input[1].Pos.xyz );// normal
	tangentSpaceMatrix[0] = vecTangent[1] = normalize( float3(tangentSpaceMatrix[2].z, 0, -tangentSpaceMatrix[2].x) ); // tangent
	
	output[3].Pos = mul( wt[3], matViewProj );
	output[3].Tex = float2( 0, input[1].TexU );
	output[3].Camera = mul(tangentSpaceMatrix, normalize(vecView));
	output[3].Light = mul(tangentSpaceMatrix, normalize(vecLightDirection));	
	
	/* PT i+1 */
	for( uint i=1; i<nBodyPoints; ++i )
	{
		// (next pt)
		wt[0] = wt[2];
		output[0] = output[2];
		
		wt[1] = wt[3];
		output[1] = output[3];
		
		// new pt world pos
		wt[2] = float4(	input[0].Pos.x + input[0].Norm.x * vecBodyPoints[i].x,
						input[0].Pos.y + input[0].Norm.y * vecBodyPoints[i].y,
						input[0].Pos.z + input[0].Norm.z * vecBodyPoints[i].x,
						1.0f );
						
		wt[3] = float4(	input[1].Pos.x + input[1].Norm.x * vecBodyPoints[i].x,
						input[1].Pos.y + input[1].Norm.y * vecBodyPoints[i].y,
						input[1].Pos.z + input[1].Norm.z * vecBodyPoints[i].x,
						1.0f );
		
		// output vertex (for pt i)
		tangentSpaceMatrix[0] = vecTangent[0]; // tangent
		tangentSpaceMatrix[1] = normalize( wt[2].xyz - wt[0].xyz );	// binormal
		tangentSpaceMatrix[2] = normalize( wt[2].xyz - input[0].Pos.xyz );// normal
		
		output[2].Pos = mul( wt[2], matViewProj );
		output[2].Camera = mul(tangentSpaceMatrix, normalize(vecView));
		output[2].Light = mul(tangentSpaceMatrix, normalize(vecLightDirection));
		
		tangentSpaceMatrix[0] = vecTangent[1]; // tangent
		tangentSpaceMatrix[1] = normalize( wt[3].xyz - wt[1].xyz );	// binormal
		tangentSpaceMatrix[2] = normalize( wt[3].xyz - input[1].Pos.xyz );// normal
		
		output[3].Pos = mul( wt[3], matViewProj );
		output[3].Camera = mul(tangentSpaceMatrix, normalize(vecView));
		output[3].Light = mul(tangentSpaceMatrix, normalize(vecLightDirection));
		
		output[2].Tex.x = output[3].Tex.x = (float)(i) / (float)nBodyPoints;
			
		/* append triangles */
		// 0 1 2
		TriStream.Append( output[0] );
		TriStream.Append( output[1] );
		TriStream.Append( output[2] );
		
		TriStream.RestartStrip();
		
		// 2 1 3
		TriStream.Append( output[2] );
		TriStream.Append( output[1] );
		TriStream.Append( output[3] );
		
		TriStream.RestartStrip();
	}	
}


[maxvertexcount(6)]
void SnakeBodyGrassMoveMaskGS( line GS_SB_INPUT input[2], inout TriangleStream<PS_GRASSMOVEMASK_INPUT> TriStream )
{
	PS_GRASSMOVEMASK_INPUT output;
	
	float3 vecDir = normalize(input[1].Pos - input[0].Pos).xyz;
	
	output.Dir = float2( vecDir.x, vecDir.z );//*0.8f;
	
	float2 v[4];
	v[0] = float2( input[0].Pos.x - input[0].Norm.x*1.2, input[0].Pos.z - input[0].Norm.z*1.2 ) / flTerrainSize * 2.0 - 1.0;
	v[1] = float2( input[0].Pos.x + input[0].Norm.x*1.2, input[0].Pos.z + input[0].Norm.z*1.2 ) / flTerrainSize * 2.0 - 1.0;
	v[2] = float2( input[1].Pos.x - input[1].Norm.x*1.2, input[1].Pos.z - input[1].Norm.z*1.2 ) / flTerrainSize * 2.0 - 1.0;
	v[3] = float2( input[1].Pos.x + input[1].Norm.x*1.2, input[1].Pos.z + input[1].Norm.z*1.2 ) / flTerrainSize * 2.0 - 1.0;
	
	// dirst tri 023
	output.Pos = float4( v[0].x,v[0].y, 0.5f, 1 );
	output.Dir = float2( vecDir.x - input[0].Norm.x*0.5, vecDir.z - input[0].Norm.z*0.5 );
	TriStream.Append( output );														
																					
	output.Pos = float4( v[2].x,v[2].y, 0.5f, 1 );									
	output.Dir = float2( vecDir.x - input[1].Norm.x*0.5, vecDir.z - input[1].Norm.z*0.5 );
	TriStream.Append( output );														
																					
	output.Pos = float4( v[3].x,v[3].y, 0.5f, 1 );									
	output.Dir = float2( vecDir.x + input[1].Norm.x*0.5, vecDir.z + input[1].Norm.z*0.5 );
	TriStream.Append( output );
	
	TriStream.RestartStrip();
	
	// second tri 031
	output.Pos = float4( v[0].x,v[0].y, 0.5f, 1 );
	output.Dir = float2( vecDir.x - input[0].Norm.x*0.5, vecDir.z - input[0].Norm.z*0.5 );
	TriStream.Append( output );														
																					
	output.Pos = float4( v[3].x,v[3].y, 0.5f, 1 );									
	output.Dir = float2( vecDir.x + input[1].Norm.x*0.5, vecDir.z + input[1].Norm.z*0.5 );
	TriStream.Append( output );														
																					
	output.Pos = float4( v[1].x,v[1].y, 0.5f, 1 );									
	output.Dir = float2( vecDir.x + input[0].Norm.x*0.5, vecDir.z + input[0].Norm.z*0.5 );
	TriStream.Append( output );
	
	TriStream.RestartStrip();
	
	
}


[maxvertexcount(120)]	// shadow volume
void SnakeBodyShadowVolumeGS( line GS_SB_INPUT input[2], inout TriangleStream<PS_SHADOWVOLUME_INPUT> TriStream )
{
	float4	wt[4]= {{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}};
	PS_SHADOWVOLUME_INPUT output[4] = { {{0,0,0,0}},{{0,0,0,0}},{{0,0,0,0}},{{0,0,0,0}} };
	uint bShadowing = 0;	
	
	float3 vecN = float3(0,-1,0);
	
	float3 vecShadowDir = vecLightDirection * 10.0f;
	
	//
	wt[2] = float4(	input[0].Pos.x + input[0].Norm.x * vecBodyPoints[nBodyPoints-1].x,
						input[0].Pos.y,// + input[0].Norm.y * vecBodyPoints[nBodyPoints-1].y,
						input[0].Pos.z + input[0].Norm.z * vecBodyPoints[nBodyPoints-1].x,
						1.0f );
	wt[3] = float4(	input[1].Pos.x + input[1].Norm.x * vecBodyPoints[nBodyPoints-1].x,
						input[1].Pos.y,// + input[1].Norm.y * vecBodyPoints[nBodyPoints-1].y,
						input[1].Pos.z + input[1].Norm.z * vecBodyPoints[nBodyPoints-1].x,
						1.0f );
	
	//
	for ( uint i = 0; i < nBodyPoints; ++i )
	{
		wt[0] = wt[2];
		wt[1] = wt[3];
		
		//
		wt[2] = float4(	input[0].Pos.x + input[0].Norm.x * vecBodyPoints[i].x,
							input[0].Pos.y + input[0].Norm.y * vecBodyPoints[i].y,
							input[0].Pos.z + input[0].Norm.z * vecBodyPoints[i].x,
							1.0f );
		wt[3] = float4(	input[1].Pos.x + input[1].Norm.x * vecBodyPoints[i].x,
							input[1].Pos.y + input[1].Norm.y * vecBodyPoints[i].y,
							input[1].Pos.z + input[1].Norm.z * vecBodyPoints[i].x,
							1.0f );
							
		// test against light direction
		if ( dot( vecN, vecLightDirection ) < 0.0f )
		{	// we're facing the light
			if ( bShadowing == 0 )
			{
				bShadowing = 1;
			
				output[0].Pos = mul( wt[0], matViewProj );
				output[1].Pos = mul( wt[1], matViewProj );
				output[2].Pos = mul( float4( wt[0].xyz + vecShadowDir, 1.0f ), matViewProj );
				output[3].Pos = mul( float4( wt[1].xyz + vecShadowDir, 1.0f ), matViewProj );
				
				// 0 2 1
				TriStream.Append( output[0] );
				TriStream.Append( output[2] );
				TriStream.Append( output[1] );
				
				TriStream.RestartStrip();
				
				// 1 2 3
				TriStream.Append( output[1] );
				TriStream.Append( output[2] );
				TriStream.Append( output[3] );
				
				TriStream.RestartStrip();
			}
			
			//
			output[0].Pos = mul( wt[0], matViewProj );
			output[1].Pos = mul( float4( wt[0].xyz + vecShadowDir, 1.0f ), matViewProj );
			output[2].Pos = mul( wt[2], matViewProj );
			output[3].Pos = mul( float4( wt[2].xyz + vecShadowDir, 1.0f ), matViewProj );
			
			// 0 3 1
			TriStream.Append( output[0] );
			TriStream.Append( output[3] );
			TriStream.Append( output[1] );
			
			TriStream.RestartStrip();
			
			// 0 2 3
			TriStream.Append( output[0] );
			TriStream.Append( output[2] );
			TriStream.Append( output[3] );
			
			TriStream.RestartStrip();
			
			//
			output[0].Pos = mul( wt[1], matViewProj );
			output[1].Pos = mul( float4( wt[1].xyz + vecShadowDir, 1.0f ), matViewProj );
			output[2].Pos = mul( wt[3], matViewProj );
			output[3].Pos = mul( float4( wt[3].xyz + vecShadowDir, 1.0f ), matViewProj );
			
			// 0 1 3
			TriStream.Append( output[0] );
			TriStream.Append( output[1] );
			TriStream.Append( output[3] );
			
			TriStream.RestartStrip();
			
			// 0 3 2
			TriStream.Append( output[0] );
			TriStream.Append( output[3] );
			TriStream.Append( output[2] );
			
			TriStream.RestartStrip();
			
			//
			output[0].Pos = mul( float4( wt[0].xyz + vecShadowDir, 1.0f ), matViewProj );
			output[1].Pos = mul( float4( wt[1].xyz + vecShadowDir, 1.0f ), matViewProj );
			output[2].Pos = mul( float4( wt[2].xyz + vecShadowDir, 1.0f ), matViewProj );
			output[3].Pos = mul( float4( wt[3].xyz + vecShadowDir, 1.0f ), matViewProj );
			
			// 0 2 1
			TriStream.Append( output[0] );
			TriStream.Append( output[2] );
			TriStream.Append( output[1] );
			
			TriStream.RestartStrip();
			
			// 2 3 1
			TriStream.Append( output[2] );
			TriStream.Append( output[3] );
			TriStream.Append( output[1] );
			
			TriStream.RestartStrip();
			
		}
		else
		{	// we're not facing the light
			if ( bShadowing == 1 )
			{
				bShadowing = 0;
				
				output[0].Pos = mul( wt[0], matViewProj );
				output[1].Pos = mul( wt[1], matViewProj );
				output[2].Pos = mul( float4( wt[0].xyz + vecShadowDir, 1.0f ), matViewProj );
				output[3].Pos = mul( float4( wt[1].xyz + vecShadowDir, 1.0f ), matViewProj );
				
				// 0 1 2
				TriStream.Append( output[0] );
				TriStream.Append( output[1] );
				TriStream.Append( output[2] );
				
				TriStream.RestartStrip();
		
				// 2 1 3
				TriStream.Append( output[2] );
				TriStream.Append( output[1] );
				TriStream.Append( output[3] );
				
				TriStream.RestartStrip();
				
				// end of shadow
				return;
			}
		}
		
		// calc unnormalized normal for the next quad
		if ( i < nBodyPoints-1 )
		{
			vecN =(	float3(	input[0].Norm.x * vecBodyPoints[i].x,
							input[0].Norm.y * vecBodyPoints[i].y,
							input[0].Norm.z * vecBodyPoints[i].x ) +
					float3(	input[0].Norm.x * vecBodyPoints[i+1].x,
							input[0].Norm.y * vecBodyPoints[i+1].y,
							input[0].Norm.z * vecBodyPoints[i+1].x ) ) * 0.5f;
		}
	}
}


[maxvertexcount(1)]
void AnimateGrassGS( point VSGS_GR_INPUT input[1], inout PointStream<VSGS_GR_INPUT> PtStream )
{
	VSGS_GR_INPUT output = input[0];
	
	float4 normalPt = texNormal.SampleLevel( samPoint, float2(input[0].Pos.x,-input[0].Pos.z), 0 );
	
	uint i;
	
	float3 vecPrevSeg = lerp( input[0].DefSeg, (normalPt.rgb * 2.0 - 1.0), normalPt.a);
	
	// move segments ( += speed * flFrameLength, normalize )
	// update speeds (gravitation, slow down, bounces)
	for (i=0;i<2;++i)
	{
		output.Seg[i] += (output.SegSpeed[i])*flFrameLength;
		if (output.Seg[i].y < 0.2)output.Seg[i].y = 0.2;	// stop above the ground
		
		output.SegSpeed[i] *= 1.0 - ( flFrameLength );	// slow down (air resistance)
		output.SegSpeed[i].y -= flFrameLength * 0.1 * flGravity;	// add gravitation
		
		// compute bounces
		float flDist = distance( output.Seg[i], vecPrevSeg );
		
		if (flDist > flGrassBounce)
		{
			//output.SegSpeed[i] += flFrameLength * (10.0) * (vecPrevSeg - output.Seg[i]);

			output.SegSpeed[i] += ( (vecPrevSeg - output.Seg[i]) * (flDist * 10.0) - output.SegSpeed[i] ) * (flFrameLength*4.0);
			//if ( normalPt.a > 0.5 )output.SegSpeed[i]*= 6.0;
		}
		
		// use previous segment for bounces
		vecPrevSeg = output.Seg[i];
	}
	
	// update seg0 speed using grass move map
		
	PtStream.Append( output );
}


[maxvertexcount(12)]
void GrassGS( point VSGS_GR_INPUT input[1], inout TriangleStream<PS_GR_INPUT> TriStream )
{
	const float g = frac( input[0].Pos.x * 100.0f ) * 3.0f;	// select texture
	const float3 vecDir = float3( input[0].Dir.y,0,-input[0].Dir.x);

	float3 prevPos;	// grass world position
	PS_GR_INPUT output[4];
		
	// x0
	prevPos = mul( float4(input[0].Pos*flTerrainSize,1), matWorld ).xyz;
	prevPos.y += TerrainHeight( prevPos.x, prevPos.z );
	
	output[0].Pos = mul( float4(prevPos + vecDir,1), matViewProj );
	output[0].Tex = float3( 0,1,g );
	
	output[1].Pos = mul( float4(prevPos - vecDir,1), matViewProj );
	output[1].Tex = float3( 1,1,g );
	
	// x1
	prevPos += normalize(input[0].Seg[0])*input[0].SegHeight;
	
	output[2].Pos = mul( float4(prevPos + vecDir,1), matViewProj );
	output[2].Tex = float3( 0,0.5,g );
	
	output[3].Pos = mul( float4(prevPos - vecDir,1), matViewProj );
	output[3].Tex = float3( 1,0.5,g );
	
	// 012
	TriStream.Append(output[0]);
	TriStream.Append(output[1]);
	TriStream.Append(output[2]);
	TriStream.RestartStrip();
	
	// 123
	TriStream.Append(output[1]);
	TriStream.Append(output[2]);
	TriStream.Append(output[3]);
	TriStream.RestartStrip();
	
	// x2
	prevPos += normalize(input[0].Seg[1])*input[0].SegHeight;
	
	output[0].Pos = mul( float4(prevPos + vecDir,1), matViewProj );
	output[0].Tex = float3( 0,0,g );
	
	output[1].Pos = mul( float4(prevPos - vecDir,1), matViewProj );
	output[1].Tex = float3( 1,0,g );
	
	// 234
	TriStream.Append(output[2]);
	TriStream.Append(output[3]);
	TriStream.Append(output[0]);
	TriStream.RestartStrip();
	
	// 345
	TriStream.Append(output[3]);
	TriStream.Append(output[0]);
	TriStream.Append(output[1]);
	TriStream.RestartStrip();
	
}

[maxvertexcount(12)]
void ButterflyGS( point VSGS_BF_INPUT input[1], inout TriangleStream<PS_BF_INPUT> TriStream )
{
	PS_BF_INPUT output[6];
	
	output[0].Tex = float2(0,1);
	output[1].Tex = float2(0,0);
	output[2].Tex = float2(1,1);
	output[3].Tex = float2(1,0);
	output[4].Tex = float2(0,1);
	output[5].Tex = float2(0,0);
	
	matrix matAll = mul( matWorld, matViewProj );
	
	float flw = flButterflyAnim * 1.3;
	float flx = flButterflyAnim*flButterflyAnim;
	
	output[0].Pos = mul( float4( input[0].Pos + float3(-1+flx,flw,0), 1 ), matAll );
	output[1].Pos = mul( float4( input[0].Pos + float3(-1+flx,flw,1), 1 ), matAll );
	output[2].Pos = mul( float4( input[0].Pos + float3(0,0,0), 1 ), matAll );
	output[3].Pos = mul( float4( input[0].Pos + float3(0,0,1), 1 ), matAll );
	output[4].Pos = mul( float4( input[0].Pos + float3(+1-flx,flw,0), 1 ), matAll );
	output[5].Pos = mul( float4( input[0].Pos + float3(+1-flx,flw,1), 1 ), matAll );
	
	TriStream.Append(output[0]);
	TriStream.Append(output[1]);
	TriStream.Append(output[2]);
	TriStream.RestartStrip();
	
	TriStream.Append(output[2]);
	TriStream.Append(output[1]);
	TriStream.Append(output[3]);
	TriStream.RestartStrip();
	
	TriStream.Append(output[2]);
	TriStream.Append(output[3]);
	TriStream.Append(output[4]);
	TriStream.RestartStrip();
	
	TriStream.Append(output[4]);
	TriStream.Append(output[3]);
	TriStream.Append(output[5]);
	TriStream.RestartStrip();
}


[maxvertexcount(3)]
void StoneWallGS( triangle GS_ST_INPUT input[3], inout TriangleStream<PS_ST_INPUT> TriStream )
{
	PS_ST_INPUT output[3];
	
	float3x3 tangentSpaceMatrix;
	
	uint i;
	
	for (i = 0; i < 3; ++i)
	{
		output[i].Pos = mul( input[i].Pos, matViewProj );
		output[i].Tex = input[i].Tex;
		
		tangentSpaceMatrix[0] = normalize( input[(i+1)%3].Pos - input[i].Pos ).xyz; // tangent
		tangentSpaceMatrix[1] = normalize( input[(i+2)%3].Pos - input[i].Pos ).xyz;	// binormal
		tangentSpaceMatrix[2] = normalize( input[i].Norm );// normal
				
		output[i].Light = mul(tangentSpaceMatrix, normalize(vecLightDirection));
	}
	
	TriStream.Append(output[0]);
	TriStream.Append(output[1]);
	TriStream.Append(output[2]);
	TriStream.RestartStrip();
}

[maxvertexcount(3)]
void StoneWallGSGrassMoveMask( triangle GS_ST_INPUT input[3], inout TriangleStream<PS_GRASSMOVEMASK_INPUT> TriStream )
{
	PS_GRASSMOVEMASK_INPUT output;
	
	for (uint i = 0; i < 3; ++i)
	{
		float2 p = float2( input[i].Pos.x, input[i].Pos.z ) / flTerrainSize * 2.0 - 1.0;
		
		output.Pos = float4(p.x,p.y,0.5,1);
		output.Dir = float2( input[i].Norm.x, input[i].Norm.z );
		
		TriStream.Append(output);
	}
	
	TriStream.RestartStrip();
}

[maxvertexcount(21)]
void StoneWallGSShadowVolume( triangle GS_ST_INPUT input[3], inout TriangleStream<PS_SHADOWVOLUME_INPUT> TriStream )
{
	if ( dot( cross( input[1].Pos.xyz - input[0].Pos.xyz, input[2].Pos.xyz - input[0].Pos.xyz ), vecLightDirection ) > 0.0f )
		return;
	
	float3 vecShadowDir = vecLightDirection * 10.0f;
	
	PS_SHADOWVOLUME_INPUT	output[4] = { {{0,0,0,0}},{{0,0,0,0}},{{0,0,0,0}},{{0,0,0,0}} };
	
	output[2].Pos = mul( input[2].Pos, matViewProj );
	output[3].Pos = mul( float4( input[2].Pos.xyz + vecShadowDir, 1), matViewProj );
	
	for (uint i = 0; i < 3; ++i)
	{
		output[0] = output[2];
		output[1] = output[3];
		
		output[2].Pos = mul( input[i].Pos, matViewProj );
		output[3].Pos = mul( float4( input[i].Pos.xyz + vecShadowDir, 1), matViewProj );
		
		// 0 3 2
		TriStream.Append(output[0]);
		TriStream.Append(output[3]);
		TriStream.Append(output[2]);
		
		TriStream.RestartStrip();
		
		// 0 1 3
		TriStream.Append(output[0]);
		TriStream.Append(output[1]);
		TriStream.Append(output[3]);
		
		TriStream.RestartStrip();
	}
}

//--------------------------------------------------------------------------------------
// PIXEL SHADER
//--------------------------------------------------------------------------------------
float4 SnakeBodyPS( PS_SB_INPUT input) : SV_Target
{
	// normal map
	float3 normal = texNormal.Sample( samLinear, input.Tex ).xyz * 2.0f - 1.0f;	// normal taken from the normal map
	
	float3 light = normalize( input.Light );		// light direction in tangent space
	float3 camera = normalize( input.Camera );		// view direction in tangent space
	
	// specular
	float3 r = reflect( normal, light );			// reflect the light direction for specular lighting
	float spec = pow( saturate( dot( -r, camera ) ), 30.0f );
	
	// +diffuse
	float fLighting = saturate( -dot( normal, light ) ) * 0.6f + spec*0.2f + 0.4f;	// diffuse lighting + specular + ambient
	
	// texture
    return texDiffuse.Sample( samLinear, input.Tex ) * fLighting + spec*0.5f;		// texture * lighting + pure specular
}

float4 GrassMoveMaskPS( PS_GRASSMOVEMASK_INPUT input) : SV_Target
{
	float2 dir = normalize( input.Dir );
	return float4( dir.x*0.5f+0.5f, 0.5 , dir.y*0.5f+0.5f, 1);
}

float4 SnakeEyePS( PS_SE_INPUT input ) : SV_Target
{
	float3 r = reflect( input.Norm, vecLightDirection );
	float spec = pow( saturate( dot( -r, vecView ) ), 10.0f );

	float fLighting = saturate( -dot( input.Norm, vecLightDirection ) ) * 0.2f + spec*0.8f;
	
	return float4( float3(1,1,1)*fLighting ,1);
}

float4 TerrainPS( PS_TR_INPUT input) : SV_Target
{
	float fLighting = saturate( -dot( input.Norm, vecLightDirection ) ) * 0.6f + 0.4f;
	
    return texDiffuse.Sample( samLinear, input.Tex ) * fLighting;
}


float4 GrassPS( PS_GR_INPUT input) : SV_Target
{
	//float3 r = reflect( normalize(input.Norm), vecLightDirection );
	//float spec = pow( saturate( dot( -r, vecView ) ), 10.0f );

	//float fLighting = saturate( dot( normalize(input.Norm), vecLightDirection ) ) * 0.6f + 0.4f;// + spec*0.8f;
	
    return tex2dArray.Sample( samLinear, input.Tex );
}

float4 ButterflyPS( PS_BF_INPUT input) : SV_Target
{
	return texDiffuse.Sample( samLinear, input.Tex );
}

float4 StoneWallPS( PS_ST_INPUT input) : SV_Target
{
	// normal map
	float3 normal = texNormal.Sample( samLinear, input.Tex ).xyz * 2.0f - 1.0f;	// normal taken from the normal map
	
	float3 light = normalize( input.Light );		// light direction in tangent space
	
	// diffuse
	float fLighting = saturate( -dot( normal, light ) ) * 0.5f + 0.4f;
	
    return texDiffuse.Sample( samLinear, input.Tex ) * fLighting;
}

float4 ApplePS( PS_AP_INPUT input) : SV_Target
{
	float3 r = reflect( input.Norm, vecLightDirection );
	float spec = pow( saturate( dot( -r, vecView ) ), 10.0f );

	float fLighting = saturate( -dot( input.Norm, vecLightDirection ) ) * 0.5f + 0.6f;
	
	return texDiffuse.Sample( samLinear, input.Tex ) * fLighting + spec*0.6f;
}

float4 ShadowVolumePS( PS_SHADOWVOLUME_INPUT input) : SV_Target
{
	return float4(0,0,0, 1);	// black, but it doesn't matter
}

float4 ShadowPS( PS_SHADOW_INPUT input ) : SV_Target
{
	return float4( 0,0,0, 0.3f );	// black, alphablended
}



//--------------------------------------------------------------------------------------
// Technique Definition
//--------------------------------------------------------------------------------------
GeometryShader gsSnakeStreamOut = ConstructGSWithSO( CompileShader( gs_4_0,  AdvanceSnakeBodyGS() ), "POSITION.xyz; NORMAL.xyz; DISTANCE.x" );

technique10 AdvanceSnake
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, SnakeBodyVSPassThrough() ) );
        SetGeometryShader( gsSnakeStreamOut );
        SetPixelShader(NULL);
    }
}

technique10 RenderSnake
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, SnakeBodyVS() ) );
        SetGeometryShader( CompileShader( gs_4_0, SnakeBodyGS() ) );
        SetPixelShader( CompileShader( ps_4_0, SnakeBodyPS() ) );
        
        //SetBlendState( GrassBlendState, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( EnableDepth, 0 );
        SetRasterizerState( NormalRasterizer );
    }
}

technique10 RenderSnakeGrassMoveMask
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, SnakeBodyVS() ) );
        SetGeometryShader( CompileShader( gs_4_0, SnakeBodyGrassMoveMaskGS() ) );
        SetPixelShader( CompileShader( ps_4_0, GrassMoveMaskPS() ) );
        
        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( DisableDepth, 0 );
        SetRasterizerState( NormalRasterizer );
    }
}

technique10 RenderSnakeShadowVolume
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, SnakeBodyVS() ) );
        SetGeometryShader( CompileShader( gs_4_0, SnakeBodyShadowVolumeGS() ) );
        SetPixelShader( CompileShader( ps_4_0, ShadowVolumePS() ) );
        
        SetBlendState( ShadowVolumeBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( ShadowVolumeDepthStencil, 0 );
        SetRasterizerState( ShadowVolumeRasterizer );
    }
}

technique10 RenderSnakeEye
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, SnakeEyeVS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, SnakeEyePS() ) );
        
        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( EnableDepth, 0 );
        SetRasterizerState( NormalRasterizer );
    }
}

technique10 RenderTerrain
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, TerrainVS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, TerrainPS() ) );
        
        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( EnableDepth, 0 );
        SetRasterizerState( NormalRasterizer );
    }
}

GeometryShader gsGrassStreamOut = ConstructGSWithSO( CompileShader( gs_4_0,  AnimateGrassGS() ), "POSITION.xyz;SEGDIR0.xyz;SEGDIR1.xyz;DEFSEG.xyz;SEGSPEED0.xyz;SEGSPEED1.xyz;DIRECTION.xy;HEIGHT.x" );

technique10 AnimateGrass
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, GrassVSPassThrough() ) );
        SetGeometryShader( gsGrassStreamOut );
        SetPixelShader(NULL);
    }
}

technique10 RenderGrass
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, GrassVSPassThrough() ) );
        SetGeometryShader( CompileShader( gs_4_0, GrassGS() ) );
        SetPixelShader( CompileShader( ps_4_0, GrassPS() ) );
        
        SetBlendState( GrassBlendState, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( EnableDepth, 0 );
		SetRasterizerState( GrassRasterizer );
    }
}

technique10 RenderButterfly
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, ButterflyVSPassThrough() ) );
        SetGeometryShader( CompileShader( gs_4_0, ButterflyGS() ) );
        SetPixelShader( CompileShader( ps_4_0, ButterflyPS() ) );
        
        SetBlendState( ButterflyBlendState, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( EnableDepth, 0 );
		SetRasterizerState( ButterflyRasterizer );
    }
}

technique10 RenderStoneWall
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, StoneWallVS() ) );
        SetGeometryShader( CompileShader( gs_4_0, StoneWallGS() ) );
        SetPixelShader( CompileShader( ps_4_0, StoneWallPS() ) );
        
        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( EnableDepth, 0 );
        SetRasterizerState( NormalRasterizer );
    }
}

technique10 RenderApple
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, AppleVS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, ApplePS() ) );
        
        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( EnableDepth, 0 );
        SetRasterizerState( NormalRasterizer );
    }
}

technique10 RenderStoneWallGrassMoveMask
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, StoneWallVS() ) );
        SetGeometryShader( CompileShader( gs_4_0, StoneWallGSGrassMoveMask() ) );
        SetPixelShader( CompileShader( ps_4_0, GrassMoveMaskPS() ) );
        
        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( EnableDepth, 0 );
        SetRasterizerState( NormalRasterizer );
    }
}

technique10 RenderStoneWallShadowVolume
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, StoneWallVS() ) );
        SetGeometryShader( CompileShader( gs_4_0, StoneWallGSShadowVolume() ) );
        SetPixelShader( CompileShader( ps_4_0, ShadowVolumePS() ) );
        
        SetBlendState( ShadowVolumeBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( ShadowVolumeDepthStencil, 0 );
        SetRasterizerState( ShadowVolumeRasterizer );
    }
}

technique10 RenderShadows
{
	pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, ShadowVS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, ShadowPS() ) );
        
        SetBlendState( ShadowBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( ShadowDepthStencil, 0 );
        SetRasterizerState( ShadowRasterizer );
    }
}
