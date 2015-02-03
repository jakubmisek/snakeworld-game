#include "stonewall.h"
#include "terrain.h"	// TERRAIN_SIZE

// stone size
#define STONE_HEIGHT		(3.0f)
#define STONE_SIZE			(2.0f)	// stone model radius

// wall model
#define WALL_STONES_PER_LINE	((int)(TERRAIN_SIZE / (STONE_SIZE*2.5f)))
#define WALL_STONES_LINES		(3)		// 1 - 4
#define	WALL_STONES_COUNT		(WALL_STONES_PER_LINE*WALL_STONES_LINES)

// stone model
#define STONE_NUMPOINTS		(8)
#define STONE_H_NUMPOINTS	(3)

#define STONE_TRIANGLES_COUNT	(STONE_NUMPOINTS*( 2*(STONE_H_NUMPOINTS-1) + 1 ))

#define STONE_VERTEXES_COUNT	(STONE_NUMPOINTS*(STONE_H_NUMPOINTS) + 1)
#define STONE_INDEXES_COUNT		(STONE_TRIANGLES_COUNT*3)

//
#define WALL_INDEXES_COUNT		(STONE_INDEXES_COUNT * WALL_STONES_COUNT)


//////////////////////////////////////////////////////////////////////////
// base stone wall object
CStoneWallModel::CStoneWallModel( CStoneWallContainer*pContainer )
{
	m_pContainer = pContainer;
}
CStoneWallModel::~CStoneWallModel()
{

}
//////////////////////////////////////////////////////////////////////////
// release the model (move to list of free models)
void	CStoneWallModel::Release()
{
	m_pContainer->ReleaseModel(this);
}



//////////////////////////////////////////////////////////////////////////
// create wall vb and ib
void	CStoneWallModel::CreateStoneWallVertexes( VertexWall**ppWallVBOut, UINT*pWallVBCount, DWORD**ppWallIBOut, UINT*pWallIBCount )
{
	VertexWall *wallvb = new VertexWall[WALL_STONES_COUNT * STONE_VERTEXES_COUNT];
	DWORD	*wallib = new DWORD[WALL_INDEXES_COUNT];

	// init data
	int i = 0;	// stone index

	for (int iStonesLine = 0; iStonesLine < WALL_STONES_LINES; ++iStonesLine)
	{
		for (int iStone = 0; iStone < WALL_STONES_PER_LINE; ++iStone, ++i)
		{
			// stone size
			float flHeight = ( 0.5f + (rand()&0x1f) / (float)0x1f ) * STONE_HEIGHT;
			float flRadius = STONE_SIZE * ( 0.8f + (float)(rand()&0x1f) / (float)0x1f );

			// stone position
			D3DXVECTOR3 vecStonePos(
				((float)iStone / (float)(WALL_STONES_PER_LINE-1)) * (TERRAIN_SIZE - (STONE_SIZE*2.0f)) + STONE_SIZE,
				0,
				flRadius );

			vecStonePos.x += ( (float)(rand()&0x1f) / (float)0x1f - 0.5f ) * (STONE_SIZE * (float)( (iStonesLine+1)*(iStonesLine+1) ));
			vecStonePos.z += (float)(iStonesLine*iStonesLine) * (STONE_SIZE*(2.0f + 2.0f*( (float)(rand()&0x1f) / (float)0x1f ) ));

			// init stone model
			InitStone( wallvb + i*STONE_VERTEXES_COUNT, wallib + i*STONE_INDEXES_COUNT, i*STONE_VERTEXES_COUNT, vecStonePos, flHeight, flRadius );
		}
	}

	// output
	*ppWallVBOut = wallvb;
	*pWallVBCount = WALL_STONES_COUNT * STONE_VERTEXES_COUNT;

	*ppWallIBOut = wallib;
	*pWallIBCount = WALL_INDEXES_COUNT;
}

//////////////////////////////////////////////////////////////////////////
// init the stone model
void	CStoneWallModel::InitStone( VertexWall*pv, DWORD*pi, DWORD basevertex, D3DXVECTOR3&vecPos, float flHeight, float flRadius )
{
	int i,j;

	VertexWall*curv = pv;

	// vertexes
	for (i = 0; i < STONE_NUMPOINTS; ++i)
	{
		float f = (float)i / (float)STONE_NUMPOINTS;	// [0,1)
		float fr = (float)( f * 2.0 * D3DX_PI );

		for (j = 0; j < STONE_H_NUMPOINTS; ++j, ++curv)
		{
			float g = (float)j / (float)STONE_H_NUMPOINTS;	// [0,1)
			float gr = (float)( g * 0.5f * D3DX_PI );

			float c = (float)cos(gr);

			float fradd = 0.5f * ((float)(rand()&0x1f) / (float)0x1f) * (float)( 2.0 * D3DX_PI ) / (float)STONE_H_NUMPOINTS;

			curv->Pos = D3DXVECTOR3( (float)cos(fr+fradd)*c, sqrtf( (float)sin(gr) ), (float)sin(fr+fradd)*c );

#define IJ( _i, _j )	basevertex + ( (_i) * STONE_H_NUMPOINTS + (_j) )
			if (j == (STONE_H_NUMPOINTS-1))
			{	// top triangle
				int i2 = (i + 1) % STONE_NUMPOINTS;

				pi[0] = IJ( i,j );
				pi[1] = basevertex + STONE_VERTEXES_COUNT-1;
				pi[2] = IJ( i2,j );
				pi += 3;
			}
			else
			{	// quad
				int i2 = (i + 1) % STONE_NUMPOINTS;
				int j2 = (j+1);


				pi[0] = IJ( i,j );
				pi[1] = IJ( i,j2 );
				pi[2] = IJ( i2,j2 );
				pi += 3;

				pi[0] = IJ( i,j );
				pi[1] = IJ( i2,j2 );
				pi[2] = IJ( i2,j );
				pi += 3;


			}
#undef IJ
		}
	}

	// top vertex
	curv->Pos = D3DXVECTOR3( 0,1,0 );

	//
	float flTexStretch = 0.1f + ((float)(rand()&0x1f) / (float)0x1f)*0.2f;	// 0.1 - 0.3
	D3DXVECTOR2 flTexMove = D3DXVECTOR2( ((float)(rand()&0x1f) / (float)0x1f)*2.0f, ((float)(rand()&0x1f) / (float)0x1f)*2.0f );	// [0 - 2, 0 - 2]

	D3DXVECTOR2 flPosMove( ((float)(rand()&0x1f) / (float)0x1f) - 0.5f, ((float)(rand()&0x1f) / (float)0x1f) - 0.5f );	// [-0.5 - 0.5, -0.5 - 0.5]

	// move to the given position, adjust size
	for (i = 0; i < STONE_VERTEXES_COUNT; ++i)
	{
		// adjust size
		pv[i].Pos.x *= flRadius;
		pv[i].Pos.y *= flHeight;
		pv[i].Pos.z *= flRadius;

		// texture
		float flStretch = flTexStretch * ( (pv[i].Pos.y <= 0.0f) ? 1.0f : 0.5f );

		pv[i].Tex.x = pv[i].Pos.x * flStretch;
		pv[i].Tex.y = pv[i].Pos.z * flStretch;

		pv[i].Tex += flTexMove;

		// normals
		D3DXVec3Normalize( &pv[i].Norm, &pv[i].Pos );

		// position
		pv[i].Pos.x += pv[i].Pos.y * flPosMove.x;
		pv[i].Pos.z += pv[i].Pos.y * flPosMove.y;

		pv[i].Pos += vecPos;
	}
}


//////////////////////////////////////////////////////////////////////////
// stone wall container ctor
CStoneWallContainer::CStoneWallContainer()
{
	m_pWallInputLayout = NULL;
	m_pDevice = NULL;

	m_pWallDecl9 = NULL;
}


//////////////////////////////////////////////////////////////////////////
// stone wall container dtor
CStoneWallContainer::~CStoneWallContainer()
{
	SAFE_RELEASE(m_pWallInputLayout);
	SAFE_RELEASE(m_pWallDecl9);
}


//////////////////////////////////////////////////////////////////////////
// initialize the stone wall container
bool	CStoneWallContainer::CreateContainer( CDXDevice*pdev )
{
	m_pDevice = pdev;

	// create terrain texture
	m_texture.CreateTexture(*pdev,L"media\\stone_texture.jpg", NULL);

	m_normaltexture.CreateTexture(*pdev,L"media\\stone_texture_normal.jpg", NULL);

	// create techniques
	if (pdev->pDX10)
		return CreateContainer10(pdev->pDX10);
	else if (pdev->pDX9)
		return CreateContainer9(pdev->pDX9);
	else
		return false;
}


//////////////////////////////////////////////////////////////////////////
// initialize the stone wall container
// create techniques and variables
bool	CStoneWallContainer::CreateContainer10( CDX10Device*pdev )
{
	// technique
	m_pWallTechnique = pdev->pEffect->GetTechniqueByName( "RenderStoneWall" );
	m_pWallGrassMoveTechnique = pdev->pEffect->GetTechniqueByName( "RenderStoneWallGrassMoveMask" );
	m_pWallShadowVolumeTechnique = pdev->pEffect->GetTechniqueByName( "RenderStoneWallShadowVolume" );

	if ( !m_pWallTechnique || !m_pWallGrassMoveTechnique )
		return false;

	// Define the input layout
	D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof(layout)/sizeof(layout[0]);

	// Create the input layout
	D3D10_PASS_DESC PassDesc;
	m_pWallTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );
	if ( FAILED(pdev->pd3dDevice->CreateInputLayout( layout, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &m_pWallInputLayout )) )
		return false;	// unable to create the input layout

	return true;
}


//////////////////////////////////////////////////////////////////////////
// initialize the stone wall container for DX9
// create techniques and variables
bool	CStoneWallContainer::CreateContainer9( CDX9Device*pdev )
{
	m_pWallTechnique9 = pdev->pEffect->GetTechniqueByName("RenderStoneWall");

	// vertex declaration
	const D3DVERTEXELEMENT9 Decl[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0, 24,  D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	if (FAILED(pdev->pd3dDevice->CreateVertexDeclaration( Decl, &m_pWallDecl9 )))
		return false;

	// done
	return true;
}


//////////////////////////////////////////////////////////////////////////
// get stone wall model
bool	CStoneWallContainer::CreateStoneWall( CStoneWallModel**ppNewModel, D3DXMATRIX&matWorld )
{
	(*ppNewModel) = NULL;


	if ( m_freeModels.Count() > 0 )
	{
		// pop the item from the list
		*ppNewModel = m_freeModels[0];
		m_freeModels.Remove(0,false);
	}
	else
	{
		// create new
		if (m_pDevice->pDX10)
		{
			*ppNewModel = new CStoneWallModel10( *m_pDevice->pDX10, this );
		}
		else if (m_pDevice->pDX9)
		{
			*ppNewModel = new CStoneWallModel9( *m_pDevice->pDX9, this );
		}

	}

	if ( !(*ppNewModel) )
		return false;

	// set the wall parameters
	(*ppNewModel)->m_matWorld = matWorld;

	// done
	return true;
}


//////////////////////////////////////////////////////////////////////////
// move the given model into the list of free models
void	CStoneWallContainer::ReleaseModel( CStoneWallModel*pModel )
{
	m_freeModels.Add( pModel );
}