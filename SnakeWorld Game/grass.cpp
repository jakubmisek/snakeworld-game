#include "grass.h"
#include "terrain.h"

#include <math.h>

#define GRASS_STALKS_1X1_COUNT		(3.0)	// number of grass points per 1 square unit (0.1 - 3.0)
#define GRASS_STALKS_COUNT			(UINT)( GRASS_STALKS_1X1_COUNT * TERRAIN_SIZE * TERRAIN_SIZE )	// number of grass points per one square terrain unit

LPCTSTR g_szGrassTextureNames[] = 
{
	L"media\\grass_v1_basic_tex.dds",
	L"media\\grass_v2_light_tex.dds",
	L"media\\grass_v3_dark_tex.dds",
};

#define GRASS_MOVE_MAP_SIZE		(128)


//////////////////////////////////////////////////////////////////////////
// grass model ctor
CGrassModel::CGrassModel( CGrassContainer*pContainer )
{
	m_pContainer = pContainer;
}
// grass model dtor
CGrassModel::~CGrassModel()
{

}

//////////////////////////////////////////////////////////////////////////
// moves this object into the grass container
void	CGrassModel::Release()
{
	m_pContainer->ReleaseModel( this );
}

//////////////////////////////////////////////////////////////////////////
// grass container ctor
CGrassContainer::CGrassContainer()
{
	m_pDevice = NULL;

	pGrassInputLayout = NULL;
	pGrassRenderTechnique = NULL;

	m_pTmpVB = NULL;
	m_nTmpVertexes = 0;

	m_pRTGrassMoveMap = NULL;
	m_pRTGrassMoveMapView = NULL;

	pOrigDS = NULL;
	pOrigRT = NULL;

	flGrassDetail = 1.0f;

	// dx9
	pGrassDecl9 = NULL;
	pGrassVB9 = NULL;
	pGrassIB9 = NULL;
	nGrass9Vertices = nGrass9Faces = 0;
}


//////////////////////////////////////////////////////////////////////////
// grass container dtor
CGrassContainer::~CGrassContainer()
{
	SAFE_RELEASE(m_pRTGrassMoveMap);

	SAFE_RELEASE( m_pTmpVB );

	SAFE_RELEASE( pGrassInputLayout );

	// dx9
	SAFE_RELEASE( pGrassDecl9 );
	SAFE_RELEASE( pGrassVB9 );
	SAFE_RELEASE( pGrassIB9 );

	m_grassesToRender9.Clear(false);// not needed, m_grassesToRender9 is empty
}


//////////////////////////////////////////////////////////////////////////
// create grass container
bool	CGrassContainer::CreateContainer( CDXDevice*pdev )
{
	m_pDevice = pdev;

	if (pdev->pDX10)
		return CreateContainer10(pdev->pDX10);
	else if (pdev->pDX9)
	{
		m_texture9.CreateTexture( *pdev, /*g_szGrassTextureNames[0]*/ L"media\\grass9.dds", NULL );
		return CreateContainer9(pdev->pDX9);
	}
	else
		return false;
}


//////////////////////////////////////////////////////////////////////////
// create the grass container for DX10
// get the effect techniques and effect variables
bool	CGrassContainer::CreateContainer10( CDX10Device*pdev )
{
	// obtain techniques
	pGrassRenderTechnique = pdev->pEffect->GetTechniqueByName( "RenderGrass" );
	pGrassAnimateTechnique = pdev->pEffect->GetTechniqueByName( "AnimateGrass" );

	if ( !pGrassRenderTechnique || !pGrassAnimateTechnique )
		return false;

	// obtain variables
	pVarGrassTextureArray = pdev->pEffect->GetVariableByName( "tex2dArray" )->AsShaderResource();

	// init grass input layout
	D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "SEGDIR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "SEGDIR", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		//{ "SEGDIR", 2, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "DEFSEG", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "SEGSPEED", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 48, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "SEGSPEED", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, 60, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		//{ "SEGSPEED", 2, DXGI_FORMAT_R32G32B32_FLOAT, 0, 84, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "DIRECTION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 72, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		//{ "DEFDIRECTION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 80, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "HEIGHT", 0, DXGI_FORMAT_R32_FLOAT, 0, 80, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof(layout)/sizeof(layout[0]);

	D3D10_PASS_DESC PassDesc;
	HRESULT hr;
	pGrassRenderTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );
	if ( FAILED(hr = pdev->pd3dDevice->CreateInputLayout( layout, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pGrassInputLayout )) )
		return false;	// unable to create the input layout

	// load textures
	m_texture.CreateTexture( *pdev, g_szGrassTextureNames, sizeof(g_szGrassTextureNames)/sizeof(g_szGrassTextureNames[0] ) );
	
	// create temporary vertex buffer for used for grass animation
	m_nTmpVertexes = GRASS_STALKS_COUNT;

	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( VertexGrass ) * m_nTmpVertexes;
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER | D3D10_BIND_STREAM_OUTPUT;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	if( FAILED( pdev->pd3dDevice->CreateBuffer( &bd, NULL, &m_pTmpVB ) ) )
		return false;	// failed to create

	// create grass move map texture
	D3D10_TEXTURE2D_DESC dstex;
	dstex.Width = GRASS_MOVE_MAP_SIZE;
	dstex.Height = GRASS_MOVE_MAP_SIZE;
	dstex.MipLevels = 1;
	dstex.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dstex.SampleDesc.Count = 1;
	dstex.SampleDesc.Quality = 0;
	dstex.Usage = D3D10_USAGE_DEFAULT;
	dstex.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
	dstex.CPUAccessFlags = 0;
	dstex.MiscFlags = 0;
	dstex.ArraySize = 1;

	if (FAILED(pdev->pd3dDevice->CreateTexture2D( &dstex, NULL, &m_pRTGrassMoveMap )))
		return false;

	// Create the render target view
	if (FAILED(pdev->pd3dDevice->CreateRenderTargetView( m_pRTGrassMoveMap, NULL, &m_pRTGrassMoveMapView )))
		return false;

	if (FAILED(pdev->pd3dDevice->CreateShaderResourceView( m_pRTGrassMoveMap, NULL, &m_pGrassMoveMapView )))
		return false;

	m_bGrassMoveMapCleared = false;

	// done
	return true;
}


//////////////////////////////////////////////////////////////////////////
// create grass model container for DX9
// techniques, effect, ...
bool	CGrassContainer::CreateContainer9( CDX9Device*pdev )
{
	// vertex declaration
	const D3DVERTEXELEMENT9 Decl[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12,  D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	if (FAILED(pdev->pd3dDevice->CreateVertexDeclaration( Decl, &pGrassDecl9 )))
		return false;

	// technique
	pGrassTechnique9 = pdev->pEffect->GetTechniqueByName("RenderGrass");

	// grass model
	UINT nStalks = GRASS_STALKS_COUNT * 3/4;
	nGrass9Faces = nStalks*2;
	nGrass9Vertices = nStalks * 4;

	// VB
	UINT i;
	if( FAILED( pdev->pd3dDevice->CreateVertexBuffer( sizeof(VertexGrass9)*nGrass9Vertices, 0 /*Usage*/, 0, D3DPOOL_MANAGED, &pGrassVB9, NULL ) ) )
		return false;

	VertexGrass9* pVertOut;
	if( FAILED( pGrassVB9->Lock( 0, sizeof(VertexGrass9)*nGrass9Vertices, (void**)&pVertOut, 0 ) ) )
		return false;

	for (i = 0; i < nStalks; ++i)
	{
		VertexGrass9* v = pVertOut + i*4;

		D3DXVECTOR3 vecPos = D3DXVECTOR3( ((float)(rand() & 0x7ff) / (float)(0x7fe)) * TERRAIN_SIZE, 0,((float)(rand() & 0x7ff) / (float)0x7fe) * TERRAIN_SIZE  );
		D3DXVECTOR3 vecDir = D3DXVECTOR3( ((float)(rand() & 0x7ff) / (float)(0x7fe)), 0, ((float)(rand() & 0x7ff) / (float)0x7fe) );
		D3DXVec3Normalize( &vecDir, &vecDir );

		v[0].Pos = v[2].Pos = vecPos - vecDir*1.5f;
		v[1].Pos = v[3].Pos = vecPos + vecDir*1.5f;

		v[2].Pos.y = v[3].Pos.y = 0.1f + ((float)(rand() & 0x7ff) / (float)(0x7fe)) * 2.0f;

		v[0].Tex = D3DXVECTOR2(0,1);
		v[1].Tex = D3DXVECTOR2(1,1);
		v[2].Tex = D3DXVECTOR2(0,0);
		v[3].Tex = D3DXVECTOR2(1,0);
	}

	pGrassVB9->Unlock();

	// IB
	if( FAILED( pdev->pd3dDevice->CreateIndexBuffer( sizeof(WORD)*nGrass9Faces*3, 0 /*Usage*/, D3DFMT_INDEX16, D3DPOOL_MANAGED, &pGrassIB9, NULL ) ) )
		return false;
	
	WORD* pIndexOut;
	if( FAILED( pGrassIB9->Lock( 0, sizeof(WORD)*nGrass9Faces*3, (void**)&pIndexOut, 0 ) ) )
		return false;

	for (i = 0; i < nStalks; ++i)
	{
		UINT ii = i*6;

		pIndexOut[ii+0] = i * 4 + 0;
		pIndexOut[ii+1] = i * 4 + 2;
		pIndexOut[ii+2] = i * 4 + 1;

		ii += 3;

		pIndexOut[ii+0] = i * 4 + 1;
		pIndexOut[ii+1] = i * 4 + 2;
		pIndexOut[ii+2] = i * 4 + 3;
	}

	pGrassIB9->Unlock();

	
	return true;
}


//////////////////////////////////////////////////////////////////////////
// get the grass model
bool	CGrassContainer::CreateModel( CGrassModel**ppNewModel )
{
	*ppNewModel = NULL;

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
			*ppNewModel = new CGrassModel10( *m_pDevice->pDX10, this, GRASS_STALKS_COUNT );
		}
		else
		{
			*ppNewModel = new CGrassModel9( *m_pDevice->pDX9, this );
		}

	}

	// done
	return (*ppNewModel != NULL);
}


//////////////////////////////////////////////////////////////////////////
// release the used grass model
// add them into the list of not used models
bool	CGrassContainer::ReleaseModel( CGrassModel*pModel )
{
	if (pModel)
	{
		m_freeModels.Add( pModel );
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////////
// animate grass vertex buffer
ID3D10Buffer*	CGrassContainer::AnimateAndSwapGrass10( ID3D10Buffer*pVB, UINT nVertexes )
{
	if ( !nVertexes || nVertexes > m_nTmpVertexes )
		return pVB;	// too large VB

	D3D10_TECHNIQUE_DESC techDesc;

	// set the shader variables
	m_pDevice->pDX10->pVarNormalTexture->SetResource( m_pGrassMoveMapView );

	// animate given grass VB

	// DX: init the input layout and primitive topology
	m_pDevice->pDX10->pd3dDevice->IASetInputLayout( pGrassInputLayout );
	m_pDevice->pDX10->pd3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );

	UINT stride = sizeof(VertexGrass), offset = 0;

	// draw the vertex buffer
	m_pDevice->pDX10->pd3dDevice->IASetVertexBuffers( 0, 1, &pVB, &stride, &offset );

	ID3D10Buffer*	buffers[1] = { m_pTmpVB };
	UINT			offsets[1] = { 0 };
	m_pDevice->pDX10->pd3dDevice->SOSetTargets( 1, buffers, offsets );

	// animate the grass
	pGrassAnimateTechnique->GetDesc( &techDesc );
	for( UINT p = 0; p < techDesc.Passes; ++p )
	{
		pGrassAnimateTechnique->GetPassByIndex( p )->Apply(0);
		m_pDevice->pDX10->pd3dDevice->Draw( nVertexes, 0 );
	}

	// set NULL stream output
	buffers[0] = NULL;
	m_pDevice->pDX10->pd3dDevice->SOSetTargets( 1, buffers, offsets );

	// swap pVB and m_pTmpVB
	ID3D10Buffer*pLastTmpVB = m_pTmpVB;
	m_pTmpVB = pVB;

	return pLastTmpVB;
}


//////////////////////////////////////////////////////////////////////////
// clears the grass move map
void	CGrassContainer::ClearGrassMoveMap()
{
	// Clear the render target and DSV
	if (!m_bGrassMoveMapCleared)
	{
		m_bGrassMoveMapCleared = true;

		if (m_pDevice->pDX10)
		{
			float fForceColorVector[4] = { 0.5f,0.5f,0.5f, 0 };
			m_pDevice->pDX10->pd3dDevice->ClearRenderTargetView( m_pRTGrassMoveMapView, fForceColorVector);
		}
		else if (m_pDevice->pDX9)
		{
			// TODO
		}
		
	}
}


//////////////////////////////////////////////////////////////////////////
// set grass move map as render target
void	CGrassContainer::BeginRenderToGrassMoveMap()
{
	if (m_pDevice->pDX10)
	{
		m_pDevice->pDX10->pd3dDevice->OMGetRenderTargets( 1, &pOrigRT, &pOrigDS );

		ID3D10RenderTargetView* aRTViews[ 1 ] = { m_pRTGrassMoveMapView };
		m_pDevice->pDX10->pd3dDevice->OMSetRenderTargets( 1, aRTViews, NULL );

		UINT nViewports = 1;
		m_pDevice->pDX10->pd3dDevice->RSGetViewports( &nViewports, &OrigVP );

		// Setup the view-port
		D3D10_VIEWPORT vp;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		vp.Width = GRASS_MOVE_MAP_SIZE;
		vp.Height = GRASS_MOVE_MAP_SIZE;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		m_pDevice->pDX10->pd3dDevice->RSSetViewports( 1, &vp );
	}
	else if (m_pDevice->pDX9)
	{
		// TODO
	}
}


//////////////////////////////////////////////////////////////////////////
// restore the original render target and depth-stencil target
void	CGrassContainer::EndRenderToGrassMoveMap()
{
	if (m_pDevice->pDX10)
	{
		/* DEBUG: DRAW THE GRASS MOVE MAP ON FRONT BUFFER */
		/*ID3D10Resource* pRT;
		pOrigRT->GetResource( &pRT );
		D3D10_RENDER_TARGET_VIEW_DESC rtDesc;
		pOrigRT->GetDesc( &rtDesc );
		m_pDevice->pd3dDevice->ResolveSubresource( pRT, D3D10CalcSubresource( 0, 0, 1 ), m_pRTGrassMoveMap, D3D10CalcSubresource( 0, 0, 1 ), rtDesc.Format );
		SAFE_RELEASE( pRT );

		m_pDevice->pSwapChain->Present(1,0);
		*/

		// Use our Old RT again
		ID3D10RenderTargetView* aRTViews[ 1 ] = { pOrigRT };
		m_pDevice->pDX10->pd3dDevice->OMSetRenderTargets( 1, aRTViews, pOrigDS );

		SAFE_RELEASE( pOrigRT );
		SAFE_RELEASE( pOrigDS );

		// Setup the view-port
		m_pDevice->pDX10->pd3dDevice->RSSetViewports( 1, &OrigVP );
	}
	else if (m_pDevice->pDX9)
	{
		// TODO
	}
}