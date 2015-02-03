#include "terrain.h"

//////////////////////////////////////////////////////////////////////////
// terrain vertex struct
struct VertexTerrain
{
	D3DXVECTOR3	Pos;	// vertex position
};

//////////////////////////////////////////////////////////////////////////
// terrain model ctor
CTerrainContainer::CTerrainContainer()
{
	// DX objects
	m_pTerrainVertexBuffer = m_pTerrainIndexBuffer = NULL;
	m_pTerrainInputLayout = NULL;

	m_pTerrainDecl9 = NULL;
	m_pTerrainVB9 = NULL;
}


//////////////////////////////////////////////////////////////////////////
// terrain model dtor
CTerrainContainer::~CTerrainContainer()
{
	// DX objects
	SAFE_RELEASE( m_pTerrainIndexBuffer );
	SAFE_RELEASE( m_pTerrainVertexBuffer );
	SAFE_RELEASE( m_pTerrainInputLayout );

	SAFE_RELEASE(m_pTerrainDecl9);
	SAFE_RELEASE(m_pTerrainVB9);
}


//////////////////////////////////////////////////////////////////////////
// create the terrain model
bool	CTerrainContainer::CreateTerrainModel( CDXDevice&dev )
{
	// create terrain texture
	m_texture.CreateTexture( dev, L"media\\terrain.jpg", NULL );

	// create terrain model
	if (dev.pDX10)
		return CreateTerrainModel10(*dev.pDX10);
	else if (dev.pDX9)
		return CreateTerrainModel9(*dev.pDX9);
	else
		return false;
}

//////////////////////////////////////////////////////////////////////////
// create the terrain model using DX9
// create input layout
bool	CTerrainContainer::CreateTerrainModel9( CDX9Device&dev )
{
	if ( !CreateTerrainTechnique9(dev))
		return false;

	// vertex declaration
	const D3DVERTEXELEMENT9 Decl[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		D3DDECL_END()
	};

	if (FAILED(dev.pd3dDevice->CreateVertexDeclaration( Decl, &m_pTerrainDecl9 )))
		return false;

	// model (1 quad)
	if( FAILED( dev.pd3dDevice->CreateVertexBuffer( 4*sizeof(VertexTerrain), 0 /*Usage*/, 0, D3DPOOL_MANAGED, &m_pTerrainVB9, NULL ) ) )
		return false;

	VertexTerrain* pVertices;
	if( FAILED( m_pTerrainVB9->Lock( 0, 4*sizeof(VertexTerrain), (void**)&pVertices, 0 ) ) )
		return false;

	pVertices[0].Pos = D3DXVECTOR3(0,0,TERRAIN_SIZE);
	pVertices[1].Pos = D3DXVECTOR3(TERRAIN_SIZE,0,TERRAIN_SIZE);
	pVertices[2].Pos = D3DXVECTOR3(0,0,0);
	pVertices[3].Pos = D3DXVECTOR3(TERRAIN_SIZE,0,0);

	m_pTerrainVB9->Unlock();


	return true;
}

//////////////////////////////////////////////////////////////////////////
// obtain terrain render technique and variables
bool	CTerrainContainer::CreateTerrainTechnique9( CDX9Device&dev )
{
	m_pTerrainTechnique9 = dev.pEffect->GetTechniqueByName("RenderTerrain");

	return true;
}

//////////////////////////////////////////////////////////////////////////
// create the terrain model using DX10
// create input layout
bool	CTerrainContainer::CreateTerrainModel10( CDX10Device&dev )
{
	// create terrain render technique
	if ( !CreateTerrainTechnique10( dev ) )
		return false;

	// Define the input layout
	D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof(layout)/sizeof(layout[0]);

	// Create the input layout
	D3D10_PASS_DESC PassDesc;
	m_pTerrainTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );
	if ( FAILED(dev.pd3dDevice->CreateInputLayout( layout, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &m_pTerrainInputLayout )) )
		return false;	// unable to create the input layout

	// terrain model
	VertexTerrain	terrainvb[ (TERRAIN_PTS+1)*(TERRAIN_PTS+1) ];
	DWORD			terrainib[ TERRAIN_PTS*TERRAIN_PTS*6 ];

	int x,y,i;
	i=0;
	for (y= 0;y<=TERRAIN_PTS;++y)
	{
		for (x= 0;x<=TERRAIN_PTS;++x)
		{
			terrainvb[i].Pos = D3DXVECTOR3( (float)x / (float)TERRAIN_PTS * TERRAIN_SIZE, 0.0f, (float)y / (float)TERRAIN_PTS * TERRAIN_SIZE );

			++i;
		}
	}

	i = 0;
	for (y= 0;y<TERRAIN_PTS;++y)
	{
		for (x= 0;x<TERRAIN_PTS;++x)
		{
			int j = y*(TERRAIN_PTS+1) + x;

			terrainib[i++] = j;
			terrainib[i++] = j+TERRAIN_PTS+1;
			terrainib[i++] = j+TERRAIN_PTS+1+1;

			terrainib[i++] = j;
			terrainib[i++] = j+TERRAIN_PTS+1+1;
			terrainib[i++] = j+1;
		}
	}

	// vertex buffer
	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( VertexTerrain ) * (TERRAIN_PTS+1)*(TERRAIN_PTS+1);	// size
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;	// vertex buffer
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA	initdata;
	initdata.pSysMem = terrainvb;

	if( FAILED( dev.pd3dDevice->CreateBuffer( &bd, &initdata, &m_pTerrainVertexBuffer ) ) )
		return false;	// failed to create

	// index buffer
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( DWORD ) * TERRAIN_PTS*TERRAIN_PTS*6;	// size
	bd.BindFlags = D3D10_BIND_INDEX_BUFFER;	// index buffer
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	initdata.pSysMem = terrainib;

	if( FAILED( dev.pd3dDevice->CreateBuffer( &bd, &initdata, &m_pTerrainIndexBuffer ) ) )
		return false;	// failed to create

	// done
	return true;
}


//////////////////////////////////////////////////////////////////////////
// obtain terrain render technique and variables
bool	CTerrainContainer::CreateTerrainTechnique10( CDX10Device&dev )
{
	// Obtain the techniques
	m_pTerrainTechnique = dev.pEffect->GetTechniqueByName( "RenderTerrain" );

	// Obtain variables
	// ...

	// done
	return ( m_pTerrainTechnique != NULL );
}


//////////////////////////////////////////////////////////////////////////
// render the terrain model on the specified position
void	CTerrainContainer::RenderTerrain( CDXDevice&dev )
{
	if (dev.pDX10)
		RenderTerrain10(*dev.pDX10);
	else if (dev.pDX9)
		RenderTerrain9(*dev.pDX9);
}


//////////////////////////////////////////////////////////////////////////
// render the terrain model on the specified position using DX9
void	CTerrainContainer::RenderTerrain9( CDX9Device&dev )
{
	dev.pEffect->SetTexture( dev.pVarDiffuseTexture, m_texture.GetTexture9() );

	dev.pd3dDevice->SetVertexDeclaration(m_pTerrainDecl9);
	dev.pd3dDevice->SetStreamSource(0,m_pTerrainVB9,0,sizeof(VertexTerrain));

	dev.pEffect->SetTechnique( m_pTerrainTechnique9 );

	UINT cPasses;
	if (SUCCEEDED(dev.pEffect->Begin( &cPasses, 0 ) ))
	{
		for( UINT p = 0; p < cPasses; ++p )
		{
			if (SUCCEEDED(dev.pEffect->BeginPass( p ) ))
			{
				dev.pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

				dev.pEffect->EndPass();
			}
		}

		dev.pEffect->End();
	}
	

}

//////////////////////////////////////////////////////////////////////////
// render the terrain model on the specified position using DX10
void	CTerrainContainer::RenderTerrain10( CDX10Device&dev )
{
	// terrain texture
	dev.pVarDiffuseTexture->SetResource( m_texture.GetTexture10View() );

	// DX: init the input layout and primitive topology
	dev.pd3dDevice->IASetInputLayout( m_pTerrainInputLayout );
	dev.pd3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	UINT stride = sizeof(VertexTerrain), offset = 0;

	// set the vertex buffer
	dev.pd3dDevice->IASetVertexBuffers( 0, 1, &m_pTerrainVertexBuffer, &stride, &offset );
	dev.pd3dDevice->IASetIndexBuffer( m_pTerrainIndexBuffer, DXGI_FORMAT_R32_UINT, 0 );

	// technique description
	D3D10_TECHNIQUE_DESC techDesc;
	m_pTerrainTechnique->GetDesc( &techDesc );

	// render terrain model
	for( UINT p = 0; p < techDesc.Passes; ++p )
	{
		m_pTerrainTechnique->GetPassByIndex( p )->Apply(0);
		dev.pd3dDevice->DrawIndexed( TERRAIN_PTS*TERRAIN_PTS*6, 0, 0 );
	}
}