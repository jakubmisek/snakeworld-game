#include "segmentsmodel.h"

//////////////////////////////////////////////////////////////////////////
// super segment model ctor
CSegmentsModel10::CSegmentsModel10( CSegmentsModelContainer*pContainer, CDX10Device&dev )
: CSegmentsModel(pContainer)
{
	m_pVertexBuffer = NULL;

	CreateVB( dev );
}


//////////////////////////////////////////////////////////////////////////
// super segment model dtor
CSegmentsModel10::~CSegmentsModel10()
{
	// release VB
	SAFE_RELEASE( m_pVertexBuffer );
}


//////////////////////////////////////////////////////////////////////////
// reset the state to uninitialized
void	CSegmentsModel10::Reset()
{
	CSegmentsModel::Reset();

	m_nVertexes = 0;
}


//////////////////////////////////////////////////////////////////////////
// render the model
void	CSegmentsModel10::RenderObject( CDXDevice&dev, CCamera&camera )
{
	// test is model exists
	if (!m_nVertexes || !dev.pDX10)	return;	// no model

	D3D10_TECHNIQUE_DESC techDesc;

	// set the shader variables
	m_pContainer->pSnakeVarBodySegmentsDistanceFromTail->SetFloat( m_flDistanceFromTail );
	m_pContainer->pSnakeVarBodySegmentsDistanceFromHead->SetFloat( m_flDistanceFromHead );

	dev.pDX10->pVarDiffuseTexture->SetResource( m_pSnakeTextureView );
	dev.pDX10->pVarNormalTexture->SetResource( m_pSnakeNormalTextureView );

	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	dev.pDX10->pVarMatWorld->SetMatrix( (float*)&matWorld );

	// FIRST PASS, advance the snake body

	// DX: init the input layout and primitive topology
	dev.pDX10->pd3dDevice->IASetInputLayout( m_pContainer->pSnakeInputLayout );
	dev.pDX10->pd3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP );

	UINT stride = sizeof(VertexSnakeBody), offset = 0;

	// draw the vertex buffer
	dev.pDX10->pd3dDevice->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &stride, &offset );

	ID3D10Buffer*	buffers[1] = { m_pContainer->m_pAdvanceSnakeBodyVertexBuffer };
	UINT			offsets[1] = { 0 };
	dev.pDX10->pd3dDevice->SOSetTargets( 1, buffers, offsets );

	// Render snake body
	m_pContainer->pAdvanceSnakeTechnique->GetDesc( &techDesc );
	for( UINT p = 0; p < techDesc.Passes; ++p )
	{
		m_pContainer->pAdvanceSnakeTechnique->GetPassByIndex( p )->Apply(0);
		dev.pDX10->pd3dDevice->Draw( m_nVertexes, 0 );
	}


	// SECOND PASS, render the advanced body
	buffers[0] = NULL;
	dev.pDX10->pd3dDevice->SOSetTargets( 1, buffers, offsets );

	dev.pDX10->pd3dDevice->IASetVertexBuffers( 0, 1, &m_pContainer->m_pAdvanceSnakeBodyVertexBuffer, &stride, &offset );
	dev.pDX10->pd3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP );

	// Render snake body
	m_pContainer->pSnakeTechnique->GetDesc( &techDesc );
	for( UINT p = 0; p < techDesc.Passes; ++p )
	{
		m_pContainer->pSnakeTechnique->GetPassByIndex( p )->Apply(0);
		dev.pDX10->pd3dDevice->DrawAuto();
	}
}


//////////////////////////////////////////////////////////////////////////
// render the grass move map
void	CSegmentsModel10::RenderGrassMove( CDXDevice&dev, D3DXVECTOR3&vecGrassTranslation )
{
	if (!m_nVertexes || !dev.pDX10)	return;	// no model

	D3D10_TECHNIQUE_DESC techDesc;

	// set the shader variables
	m_pContainer->pSnakeVarBodySegmentsDistanceFromTail->SetFloat( m_flDistanceFromTail );
	m_pContainer->pSnakeVarBodySegmentsDistanceFromHead->SetFloat( m_flDistanceFromHead );

	// set inverse world transform (view x projection  won't be used)
	D3DXMATRIX matWorld;
	D3DXMatrixTranslation( &matWorld, -vecGrassTranslation.x, -vecGrassTranslation.y, -vecGrassTranslation.z );
	dev.pDX10->pVarMatWorld->SetMatrix( (float*)&matWorld );

	// draw the body move mask
	UINT stride = sizeof(VertexSnakeBody), offset = 0;
	dev.pDX10->pd3dDevice->IASetInputLayout( m_pContainer->pSnakeInputLayout );
	dev.pDX10->pd3dDevice->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &stride, &offset );
	dev.pDX10->pd3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP );

	// Render snake body
	m_pContainer->pSnakeGrassMoveTechnique->GetDesc( &techDesc );
	for( UINT p = 0; p < techDesc.Passes; ++p )
	{
		m_pContainer->pSnakeGrassMoveTechnique->GetPassByIndex( p )->Apply(0);
		dev.pDX10->pd3dDevice->Draw( m_nVertexes, 0 );
	}
}


//////////////////////////////////////////////////////////////////////////
// render the snake body shadow volume
void	CSegmentsModel10::RenderShadowVolume( CDXDevice&dev )
{
	if (!m_nVertexes || !dev.pDX10)	return;	// no model

	D3D10_TECHNIQUE_DESC techDesc;

	// set the shader variables
	m_pContainer->pSnakeVarBodySegmentsDistanceFromTail->SetFloat( m_flDistanceFromTail );
	m_pContainer->pSnakeVarBodySegmentsDistanceFromHead->SetFloat( m_flDistanceFromHead );

	// set inverse world transform (view x projection  won't be used)
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	dev.pDX10->pVarMatWorld->SetMatrix( (float*)&matWorld );

	// draw the body shadow volume
	UINT stride = sizeof(VertexSnakeBody), offset = 0;
	dev.pDX10->pd3dDevice->IASetInputLayout( m_pContainer->pSnakeInputLayout );
	dev.pDX10->pd3dDevice->IASetVertexBuffers( 0, 1, &m_pVertexBuffer, &stride, &offset );
	dev.pDX10->pd3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP );

	// render the technique
	m_pContainer->pSnakeShadowVolumeTechnique->GetDesc( &techDesc );
	for( UINT p = 0; p < techDesc.Passes; ++p )
	{
		m_pContainer->pSnakeShadowVolumeTechnique->GetPassByIndex( p )->Apply(0);
		dev.pDX10->pd3dDevice->Draw( m_nVertexes, 0 );
	}
}





//////////////////////////////////////////////////////////////////////////
// creates the model vertex buffer
bool	CSegmentsModel10::CreateVB( CDX10Device&dev )
{
	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DYNAMIC;	// mappable
	bd.ByteWidth = sizeof( VertexSnakeBody ) * ( SUPERSEGMENT_CAPACITY + 1 );	// size
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;	// vertex buffer
	bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;	// writable by CPU
	bd.MiscFlags = 0;

	if( FAILED( dev.pd3dDevice->CreateBuffer( &bd, NULL, &m_pVertexBuffer ) ) )
		return false;	// failed to create

	// ok
	return true;
}

void	CSegmentsModel10::UpdateParams(float flDistanceFromTail, float flDistanceFromHead,CTextureLoader&SnakeTexture, CTextureLoader&NormalTexture)
{
	CSegmentsModel::UpdateParams(flDistanceFromTail,flDistanceFromHead,SnakeTexture,NormalTexture);

	m_pSnakeTextureView = SnakeTexture.GetTexture10View();
	m_pSnakeNormalTextureView = NormalTexture.GetTexture10View();
}

bool	CSegmentsModel10::UpdateModel( CVector&vecFirst, CVector&vecFirstDir, CSnakeSuperSegment*pSS, CVector&vecEnd )
{
	// pSS->Segments().Count() must be >= 1

	//
	int i = 0;	// current number of initialized vertexes

	// map to the memory
	VertexSnakeBody	*pv;
	HRESULT hr = m_pVertexBuffer->Map( D3D10_MAP_WRITE_DISCARD, 0, (void**)&pv );

	if (FAILED(hr))
		return false;	// map failed

	// pv == array of VertexSnakeBody[ SUPERSEGMENT_CAPACITY + 1 ]

	SegmentList&segments = pSS->Segments();
	SegmentList::iterator it = segments.GetIterator();
	D3DXVECTOR3 vecCur, vecDir, vecNorm;

	//
	// fill the model with vertexes
	//

	// first
	++it;	// skip

	vecFirstDir.Normalize();

	pv[i].Distance = 0.0f;
	pv[i].Normal.x = (float)vecFirstDir.y;
	pv[i].Normal.y = 1.0f;
	pv[i].Normal.z = (float)-vecFirstDir.x;
	pv[i].Pos.x = (float)vecFirst.x;
	pv[i].Pos.y = 0.0f;
	pv[i].Pos.z = (float)vecFirst.y;

	m_boundBox = pv[i].Pos;
	m_flLength = 0.0;

	++i;


	// inner
	for ( ;!it.EndOfList();++it,++i )
	{
		CSnakeSegment*pCurSeg = it;

		vecCur.x = (float)pCurSeg->Position().x;
		vecCur.y = 0.0f;
		vecCur.z = (float)pCurSeg->Position().y;

		vecDir = vecCur - pv[i-1].Pos;
		D3DXVec3Normalize( &vecNorm, &vecDir );

		float	flDist = D3DXVec3Length( &vecDir );
		m_flLength += flDist;

		// insert segment
		pv[i].Distance = m_flLength;
		pv[i].Pos = vecCur;

		pv[i].Normal.x = vecNorm.z;
		pv[i].Normal.y = 1.0f;
		pv[i].Normal.z = -vecNorm.x;

		m_boundBox += vecCur;
	}

	// ending
	vecDir = D3DXVECTOR3((float)vecEnd.x,0.0f,(float)vecEnd.y) - pv[i-1].Pos;
	D3DXVec3Normalize( &vecNorm, &vecDir );
	m_flLength += D3DXVec3Length( &vecDir );

	pv[i].Distance = m_flLength;

	pv[i].Pos.x = (float)vecEnd.x;
	pv[i].Pos.y = 0.0f;
	pv[i].Pos.z = (float)vecEnd.y;

	pv[i].Normal.x = vecNorm.z;
	pv[i].Normal.y = 1.0f;
	pv[i].Normal.z = -vecNorm.x;

	m_boundBox += pv[i].Pos;

	m_boundBox += 1.0f;	// snake radius

	++i;

	// release map
	m_pVertexBuffer->Unmap();

	m_nVertexes = i;	// number of vertexes

	// done
	return true;
}

