#include "stonewall.h"

//////////////////////////////////////////////////////////////////////////
// stone wall ctor
CStoneWallModel10::CStoneWallModel10( CDX10Device&dev, CStoneWallContainer*pContainer )
:CStoneWallModel(pContainer)
{
	pVB = pIB = NULL;
	nIB = 0;

	InitModel(dev);
}


//////////////////////////////////////////////////////////////////////////
// stone wall dtor
CStoneWallModel10::~CStoneWallModel10()
{
	SAFE_RELEASE( pVB );
	SAFE_RELEASE( pIB );
}


//////////////////////////////////////////////////////////////////////////
// create the wall model
bool	CStoneWallModel10::InitModel( CDX10Device&dev )
{
	VertexWall	*wallvb;
	DWORD		*wallib;
	UINT		vbcount;
	CreateStoneWallVertexes( &wallvb, &vbcount, &wallib, &nIB );


	// vertex buffer
	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( VertexWall ) * vbcount;	// size
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;	// vertex buffer
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA	initdata;
	initdata.pSysMem = wallvb;

	if( FAILED( dev.pd3dDevice->CreateBuffer( &bd, &initdata, &pVB ) ) )
		return false;	// failed to create

	// index buffer
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( DWORD ) * nIB;	// size
	bd.BindFlags = D3D10_BIND_INDEX_BUFFER;	// index buffer
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	initdata.pSysMem = wallib;

	if( FAILED( dev.pd3dDevice->CreateBuffer( &bd, &initdata, &pIB ) ) )
		return false;	// failed to create

	// delete buffers
	delete[] wallvb;
	delete[] wallib;

	return true;
}




//////////////////////////////////////////////////////////////////////////
// render the stone wall model
void	CStoneWallModel10::RenderStoneWall( CDXDevice&dev )
{
	// transformation
	dev.pDX10->pVarMatWorld->SetMatrix( (float*)&m_matWorld );

	// terrain texture
	dev.pDX10->pVarDiffuseTexture->SetResource( m_pContainer->m_texture.GetTexture10View() );
	dev.pDX10->pVarNormalTexture->SetResource( m_pContainer->m_normaltexture.GetTexture10View());

	// DX: init the input layout and primitive topology
	dev.pDX10->pd3dDevice->IASetInputLayout( m_pContainer->m_pWallInputLayout );
	dev.pDX10->pd3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	UINT stride = sizeof(VertexWall), offset = 0;

	// set the vertex buffer
	dev.pDX10->pd3dDevice->IASetVertexBuffers( 0, 1, &pVB, &stride, &offset );
	dev.pDX10->pd3dDevice->IASetIndexBuffer( pIB, DXGI_FORMAT_R32_UINT, 0 );

	// technique description
	D3D10_TECHNIQUE_DESC techDesc;
	m_pContainer->m_pWallTechnique->GetDesc( &techDesc );

	// render terrain model
	for( UINT p = 0; p < techDesc.Passes; ++p )
	{
		m_pContainer->m_pWallTechnique->GetPassByIndex( p )->Apply(0);
		dev.pDX10->pd3dDevice->DrawIndexed( nIB, 0, 0 );
	}
}


//////////////////////////////////////////////////////////////////////////
// render the stone wall grass move mask
void	CStoneWallModel10::RenderStoneWallGrassMove( CDXDevice&dev, D3DXVECTOR3&vecGrassTranslation )
{
	D3DXMATRIX matWorld = m_matWorld;

	matWorld._41 -= vecGrassTranslation.x;
	matWorld._42 -= vecGrassTranslation.y;
	matWorld._43 -= vecGrassTranslation.z;

	// transformation
	dev.pDX10->pVarMatWorld->SetMatrix( (float*)&matWorld );

	// DX: init the input layout and primitive topology
	dev.pDX10->pd3dDevice->IASetInputLayout( m_pContainer->m_pWallInputLayout );
	dev.pDX10->pd3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	UINT stride = sizeof(VertexWall), offset = 0;

	// set the vertex buffer
	dev.pDX10->pd3dDevice->IASetVertexBuffers( 0, 1, &pVB, &stride, &offset );
	dev.pDX10->pd3dDevice->IASetIndexBuffer( pIB, DXGI_FORMAT_R32_UINT, 0 );

	// technique description
	D3D10_TECHNIQUE_DESC techDesc;
	m_pContainer->m_pWallGrassMoveTechnique->GetDesc( &techDesc );

	// render terrain model
	for( UINT p = 0; p < techDesc.Passes; ++p )
	{
		m_pContainer->m_pWallGrassMoveTechnique->GetPassByIndex( p )->Apply(0);
		dev.pDX10->pd3dDevice->DrawIndexed( nIB, 0, 0 );
	}
}


//////////////////////////////////////////////////////////////////////////
// render stone wall volume shadow model
void	CStoneWallModel10::RenderStoneWallShadowVolume( CDXDevice&dev )
{
	// transformation
	dev.pDX10->pVarMatWorld->SetMatrix( (float*)&m_matWorld );

	// DX: init the input layout and primitive topology
	dev.pDX10->pd3dDevice->IASetInputLayout( m_pContainer->m_pWallInputLayout );
	dev.pDX10->pd3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	UINT stride = sizeof(VertexWall), offset = 0;

	// set the vertex buffer
	dev.pDX10->pd3dDevice->IASetVertexBuffers( 0, 1, &pVB, &stride, &offset );
	dev.pDX10->pd3dDevice->IASetIndexBuffer( pIB, DXGI_FORMAT_R32_UINT, 0 );

	// technique description
	D3D10_TECHNIQUE_DESC techDesc;
	m_pContainer->m_pWallShadowVolumeTechnique->GetDesc( &techDesc );

	// render terrain model
	for( UINT p = 0; p < techDesc.Passes; ++p )
	{
		m_pContainer->m_pWallShadowVolumeTechnique->GetPassByIndex( p )->Apply(0);
		dev.pDX10->pd3dDevice->DrawIndexed( nIB, 0, 0 );
	}
}

