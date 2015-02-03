#include "segmentsmodel.h"

//////////////////////////////////////////////////////////////////////////
// snake head model
CSnakeHeadModel10::CSnakeHeadModel10(CDX10Device&dev)
{
	m_pVertexBuffer = NULL;
	m_nVertexes = 0;

	m_pEyeVB = m_pEyeIB = NULL;
	m_nEyeIndices = 0;

	CreateModel( dev );
}


//////////////////////////////////////////////////////////////////////////
// snake head model dtor
CSnakeHeadModel10::~CSnakeHeadModel10()
{
	SAFE_RELEASE( m_pEyeIB );
	SAFE_RELEASE( m_pEyeVB );

	SAFE_RELEASE( m_pVertexBuffer );
}


//////////////////////////////////////////////////////////////////////////
// create the model
bool	CSnakeHeadModel10::CreateModel( CDX10Device&dev )
{
	return 
		CreateHeadModel(dev) &&
		CreateEyeModel(dev);
}


//////////////////////////////////////////////////////////////////////////
// create the head model
bool	CSnakeHeadModel10::CreateHeadModel( CDX10Device&dev )
{
	//
	// head
	//
	VertexSnakeBody	pts[] =
	{
		// position					// size					// texture u
		{D3DXVECTOR3(+0.5f,0,0), D3DXVECTOR3(0,1.0f,0.8f), 0.0f},
		{D3DXVECTOR3(-0.7f,0,0), D3DXVECTOR3(0,0.95f,1.0f), 0.7f},
		{D3DXVECTOR3(-1.0f,0,0), D3DXVECTOR3(0,0.9f,0.9f), 1.0f},
		{D3DXVECTOR3(-1.8f,0.1f,0), D3DXVECTOR3(0,0.7f,0.8f), 1.5f},
		{D3DXVECTOR3(-2.6f,0.2f,0), D3DXVECTOR3(0,0.4f,0.5f), 2.6f},
		{D3DXVECTOR3(-2.7f,0.2f,0), D3DXVECTOR3(0,0.1f,0.2f), 2.7f},
	};

	m_nVertexes = sizeof( pts ) / sizeof( pts[0] );

	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( VertexSnakeBody ) * m_nVertexes;	// size
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;	// vertex buffer
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA	InitData;

	InitData.pSysMem = pts;

	if( FAILED( dev.pd3dDevice->CreateBuffer( &bd, &InitData, &m_pVertexBuffer ) ) )
		return false;	// failed to create

	// done
	return true;
}


//////////////////////////////////////////////////////////////////////////
// create the eye model
bool	CSnakeHeadModel10::CreateEyeModel( CDX10Device&dev )
{
	//
	// eye
	//
	VertexEye *pts;
	UINT npts;
	DWORD *ids;

	CreateEyeVertexes( &pts, &npts, &ids, &m_nEyeIndices );


	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( VertexEye ) * npts;	// size
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;	// vertex buffer
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA	InitData;

	InitData.pSysMem = pts;

	if( FAILED( dev.pd3dDevice->CreateBuffer( &bd, &InitData, &m_pEyeVB ) ) )
		return false;	// failed to create

	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( DWORD ) * m_nEyeIndices;	// size
	bd.BindFlags = D3D10_BIND_INDEX_BUFFER;	// vertex buffer
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	InitData.pSysMem = ids;

	if( FAILED( dev.pd3dDevice->CreateBuffer( &bd, &InitData, &m_pEyeIB ) ) )
		return false;	// failed to create

	// delete buffers
	delete[] pts;
	delete[] ids;

	// done
	return true;
}


//////////////////////////////////////////////////////////////////////////
// render the model
// texture must be already set
// flBodySegmentsDistanceFromTail,flBodySegmentsDistanceFromHead,flBodySegmentsMove must be already set
void	CSnakeHeadModel10::RenderModel(
									   CDXDevice&dev,
									   CSegmentsModelContainer*pContainer,
									   D3DXVECTOR3&vecPosition, float flDirection, float flBodyLength,
									   CCamera&camera,
									   CTextureLoader&SnakeTexture, CTextureLoader&NormalTexture )
{
	if (!m_nVertexes || !m_pVertexBuffer)	return;

	// set the shader variables
	dev.pDX10->pVarDiffuseTexture->SetResource( SnakeTexture.GetTexture10View() );
	dev.pDX10->pVarNormalTexture->SetResource( NormalTexture.GetTexture10View() );

	pContainer->pSnakeVarBodySegmentsDistanceFromTail->SetFloat( flBodyLength );
	pContainer->pSnakeVarBodySegmentsDistanceFromHead->SetFloat( 0.0f );

	//////////////////////////////////////////////////////////////////////////
	// SNAKE HEAD

	// setup matrix
	D3DXMATRIX matWorld;
	D3DXMatrixRotationY( &matWorld, (float)( -flDirection + D3DX_PI ) );
	matWorld.m[3][0] = vecPosition.x;
	matWorld.m[3][1] = vecPosition.y;
	matWorld.m[3][2] = vecPosition.z;

	dev.pDX10->pVarMatWorld->SetMatrix( (float*)&matWorld );

	D3D10_TECHNIQUE_DESC techDesc;

	// DX: init the input layout and primitive topology
	dev.pDX10->pd3dDevice->IASetInputLayout( pContainer->pSnakeInputLayout );
	dev.pDX10->pd3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP );

	UINT stride = sizeof(VertexSnakeBody), offset = 0;

	// the vertex buffer
	dev.pDX10->pd3dDevice->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &stride, &offset );

	// Render snake head
	pContainer->pSnakeTechnique->GetDesc( &techDesc );
	for( UINT p = 0; p < techDesc.Passes; ++p )
	{
		pContainer->pSnakeTechnique->GetPassByIndex( p )->Apply(0);
		dev.pDX10->pd3dDevice->Draw( m_nVertexes, 0 );
	}

	//////////////////////////////////////////////////////////////////////////
	// SNAKE EYE

	// DX: init the input layout and primitive topology
	dev.pDX10->pd3dDevice->IASetInputLayout( pContainer->pSnakeEyeInputLayout );
	dev.pDX10->pd3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	stride = sizeof(VertexEye);
	offset = 0;

	// the vertex buffer
	dev.pDX10->pd3dDevice->IASetVertexBuffers( 0, 1, &m_pEyeVB, &stride, &offset );
	dev.pDX10->pd3dDevice->IASetIndexBuffer( m_pEyeIB, DXGI_FORMAT_R32_UINT, 0 );

	// setup matrix
	float flDstTail = flBodyLength + 1.0f;
	float flDstHead = - 1.0f;
	for ( int iEye = 0; iEye < 2; ++iEye )
	{
		SetupEyeMatrix( matWorld, vecPosition, flDirection, iEye, 
			0.75f *
			( min( flDstTail*0.07f, 1.0f ) * (1.0f / (abs(flDstTail*0.8f - flDstHead*1.2f)*( 0.05f ) + 1.0f) + 1.0f) )	// LINE COPIED FROM THE EFFECTS.FX <-- SnakeBodyVS::flRad
			);
		dev.pDX10->pVarMatWorld->SetMatrix( (float*)&matWorld );

		// Render snake eye
		pContainer->pSnakeEyeTechnique->GetDesc( &techDesc );
		for( UINT p = 0; p < techDesc.Passes; ++p )
		{
			pContainer->pSnakeEyeTechnique->GetPassByIndex( p )->Apply(0);
			dev.pDX10->pd3dDevice->DrawIndexed( m_nEyeIndices, 0, 0 );
		}
	}


}


//////////////////////////////////////////////////////////////////////////
// render the head grass move map
void	CSnakeHeadModel10::RenderGrassMove(
	CDXDevice&dev,
	CSegmentsModelContainer*pContainer,
	D3DXVECTOR3&vecPosition, float flDirection, float flBodyLength,
	D3DXVECTOR3&vecGrassTranslation )
{
	if (!m_nVertexes || !m_pVertexBuffer)	return;

	// set the shader variables
	pContainer->pSnakeVarBodySegmentsDistanceFromTail->SetFloat( flBodyLength );
	pContainer->pSnakeVarBodySegmentsDistanceFromHead->SetFloat( 0.0f );

	//////////////////////////////////////////////////////////////////////////
	// SNAKE HEAD

	// setup matrix
	D3DXMATRIX matWorld;
	D3DXMatrixRotationY( &matWorld, (float)( -flDirection + D3DX_PI ) );
	matWorld.m[3][0] = vecPosition.x - vecGrassTranslation.x;
	matWorld.m[3][1] = vecPosition.y - vecGrassTranslation.y;
	matWorld.m[3][2] = vecPosition.z - vecGrassTranslation.z;

	dev.pDX10->pVarMatWorld->SetMatrix( (float*)&matWorld );

	D3D10_TECHNIQUE_DESC techDesc;

	// DX: init the input layout and primitive topology
	dev.pDX10->pd3dDevice->IASetInputLayout( pContainer->pSnakeInputLayout );
	dev.pDX10->pd3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP );

	UINT stride = sizeof(VertexSnakeBody), offset = 0;

	// the vertex buffer
	dev.pDX10->pd3dDevice->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &stride, &offset );

	// Render snake head
	pContainer->pSnakeGrassMoveTechnique->GetDesc( &techDesc );
	for( UINT p = 0; p < techDesc.Passes; ++p )
	{
		pContainer->pSnakeGrassMoveTechnique->GetPassByIndex( p )->Apply(0);
		dev.pDX10->pd3dDevice->Draw( m_nVertexes, 0 );
	}
}


void	CSnakeHeadModel10::RenderShadowVolume( CDXDevice&dev, CSegmentsModelContainer*pContainer, D3DXVECTOR3&vecPosition, float flDirection, float flBodyLength )
{
	if (!m_nVertexes || !m_pVertexBuffer)	return;

	// set the shader variables
	pContainer->pSnakeVarBodySegmentsDistanceFromTail->SetFloat( flBodyLength );
	pContainer->pSnakeVarBodySegmentsDistanceFromHead->SetFloat( 0.0f );

	//////////////////////////////////////////////////////////////////////////
	// SNAKE HEAD

	// setup matrix
	D3DXMATRIX matWorld;
	D3DXMatrixRotationY( &matWorld, (float)( -flDirection + D3DX_PI ) );
	matWorld.m[3][0] = vecPosition.x;
	matWorld.m[3][1] = vecPosition.y;
	matWorld.m[3][2] = vecPosition.z;

	dev.pDX10->pVarMatWorld->SetMatrix( (float*)&matWorld );

	D3D10_TECHNIQUE_DESC techDesc;

	// DX: init the input layout and primitive topology
	dev.pDX10->pd3dDevice->IASetInputLayout( pContainer->pSnakeInputLayout );
	dev.pDX10->pd3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP );

	UINT stride = sizeof(VertexSnakeBody), offset = 0;

	// the vertex buffer
	dev.pDX10->pd3dDevice->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &stride, &offset );

	// Render snake head
	pContainer->pSnakeShadowVolumeTechnique->GetDesc( &techDesc );
	for( UINT p = 0; p < techDesc.Passes; ++p )
	{
		pContainer->pSnakeShadowVolumeTechnique->GetPassByIndex( p )->Apply(0);
		dev.pDX10->pd3dDevice->Draw( m_nVertexes, 0 );
	}
}


