#include "segmentsmodel.h"
#include "segmentsmodel9.h"

//////////////////////////////////////////////////////////////////////////
// super segment model ctor
CSegmentsModel9::CSegmentsModel9( CSegmentsModelContainer*pContainer, CDX9Device&dev )
: CSegmentsModel(pContainer)
{
	m_pSnakeTexture = m_pSnakeNormal = NULL;

	m_nVertexes = 0;
	m_pVertexBuffer = NULL;

	CreateVB( dev );
}


//////////////////////////////////////////////////////////////////////////
// super segment model dtor
CSegmentsModel9::~CSegmentsModel9()
{
	SAFE_RELEASE(m_pVertexBuffer);
}


//////////////////////////////////////////////////////////////////////////
// reset the state to uninitialized
void	CSegmentsModel9::Reset()
{
	CSegmentsModel::Reset();

	m_nVertexes = 0;
	m_pSnakeTexture = m_pSnakeNormal = NULL;
}


//////////////////////////////////////////////////////////////////////////
// render the model
void	CSegmentsModel9::RenderObject( CDXDevice&dev, CCamera&camera )
{
	CDX9Device*pdev = dev.pDX9;

	if (!pdev || !m_nVertexes)
		return;

	pdev->pEffect->SetFloat(m_pContainer->pVarDistanceFromHead9, m_flDistanceFromHead);
	pdev->pEffect->SetFloat(m_pContainer->pVarDistanceFromTail9, m_flDistanceFromTail);
	
	D3DXMATRIX matId;
	D3DXMatrixIdentity(&matId);
	pdev->pEffect->SetMatrix(pdev->pVarWorldTransform, &matId);

	pdev->pEffect->SetTexture( pdev->pVarDiffuseTexture, m_pSnakeTexture );
	pdev->pEffect->SetTexture( pdev->pVarNormalTexture, m_pSnakeNormal );

	pdev->pd3dDevice->SetVertexDeclaration(m_pContainer->pSnakeDecl9);
	pdev->pd3dDevice->SetStreamSource(0,m_pVertexBuffer,0,sizeof(VertexSnakeBody9));
	pdev->pd3dDevice->SetIndices(m_pContainer->pSnakeBodyIB9);

	pdev->pEffect->SetTechnique( m_pContainer->pSnakeTechnique9 );

	UINT cPasses;
	if (SUCCEEDED(pdev->pEffect->Begin( &cPasses, 0 ) ))
	{
		for( UINT p = 0; p < cPasses; ++p )
		{
			if (SUCCEEDED(pdev->pEffect->BeginPass( p ) ))
			{
				pdev->pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_nVertexes, 0, (m_nVertexes/nBodyPoints - 1)*(nBodyPoints-1)*2 );
				
				pdev->pEffect->EndPass();
			}
		}

		pdev->pEffect->End();
	}
}


//////////////////////////////////////////////////////////////////////////
// render the grass move map
void	CSegmentsModel9::RenderGrassMove( CDXDevice&dev, D3DXVECTOR3&vecGrassTranslation )
{

}


//////////////////////////////////////////////////////////////////////////
// render the snake body shadow volume
void	CSegmentsModel9::RenderShadowVolume( CDXDevice&dev )
{

}





//////////////////////////////////////////////////////////////////////////
// creates the model vertex buffer
bool	CSegmentsModel9::CreateVB( CDX9Device&dev )
{
	const UINT nVB = (SUPERSEGMENT_CAPACITY+1) * nBodyPoints;

	if( FAILED( dev.pd3dDevice->CreateVertexBuffer( sizeof(VertexSnakeBody9)*nVB, D3DUSAGE_DYNAMIC /*Usage*/, 0, D3DPOOL_DEFAULT, &m_pVertexBuffer, NULL ) ) )
		return false;

	// ok
	return true;
}

void	CSegmentsModel9::UpdateParams(float flDistanceFromTail, float flDistanceFromHead,CTextureLoader&SnakeTexture, CTextureLoader&NormalTexture)
{
	CSegmentsModel::UpdateParams(flDistanceFromTail,flDistanceFromHead,SnakeTexture,NormalTexture);

	m_pSnakeTexture = SnakeTexture.GetTexture9();
	m_pSnakeNormal = NormalTexture.GetTexture9();
}

bool	CSegmentsModel9::UpdateModel( CVector&vecFirst, CVector&vecFirstDir, CSnakeSuperSegment*pSS, CVector&vecEnd )
{
	// pSS->Segments().Count() must be >= 1

	//
	int i = 0;	// current number of initialized vertexes

	// map to the memory
	VertexSnakeBody9	*pv;
	if( FAILED( m_pVertexBuffer->Lock( 0, sizeof(VertexSnakeBody9) * (pSS->Segments().Count()+1) * nBodyPoints, (void**)&pv, D3DLOCK_DISCARD  ) ) )
		return false;

	// pv == array of VertexSnakeBody[ SUPERSEGMENT_CAPACITY + 1 ]

	SegmentList&segments = pSS->Segments();
	SegmentList::iterator it = segments.GetIterator();
	D3DXVECTOR3 vecDir, vecNorm;

	D3DXVECTOR3	vecCenter1, vecNorm1, vecCenter0;

	//
	// fill the model with vertexes
	//

	// first
	++it;	// skip

	vecFirstDir.Normalize();

	vecCenter0 = vecCenter1 = D3DXVECTOR3((float)vecFirst.x,0.0f,(float)vecFirst.y);
	vecNorm1 = D3DXVECTOR3((float)vecFirstDir.y,1.0f,(float)-vecFirstDir.x);
	m_flLength = 0.0;

	UpdateRing9( pv+i, vecCenter1, vecNorm1, m_flLength );
	i+= nBodyPoints;
	
	m_boundBox = vecCenter1;



	// inner
	for ( ;!it.EndOfList();++it )
	{
		CSnakeSegment*pCurSeg = it;

		vecCenter1.x = (float)pCurSeg->Position().x;
		vecCenter1.y = 0.0f;
		vecCenter1.z = (float)pCurSeg->Position().y;

		vecDir = vecCenter1 - vecCenter0;
		D3DXVec3Normalize( &vecNorm, &vecDir );

		float	flDist = D3DXVec3Length( &vecDir );
		m_flLength += flDist;

		vecNorm1.x = vecNorm.z;
		vecNorm1.y = 1.0f;
		vecNorm1.z = -vecNorm.x;


		// insert segment
		UpdateRing9( pv+i, vecCenter1, vecNorm1, m_flLength );
		i += nBodyPoints;

		m_boundBox += vecCenter1;

		vecCenter0 = vecCenter1;
	}

	// ending
	vecCenter1.x = (float)vecEnd.x;
	vecCenter1.y = 0.0f;
	vecCenter1.z = (float)vecEnd.y;

	vecDir = vecCenter1 - vecCenter0;
	D3DXVec3Normalize( &vecNorm, &vecDir );

	m_flLength += D3DXVec3Length( &vecDir );

	vecNorm1.x = vecNorm.z;
	vecNorm1.y = 1.0f;
	vecNorm1.z = -vecNorm.x;

	UpdateRing9( pv+i, vecCenter1, vecNorm1, m_flLength );
	i += nBodyPoints;

	m_boundBox += vecCenter1;
	m_boundBox += 1.0f;	// snake radius

	// release mapping
	m_pVertexBuffer->Unlock();

	m_nVertexes = i;	// number of vertexes

	// done
	return true;
}


///////////////////////////////////////////////////////////////////////////
// init ring vertexes
void	UpdateRing9( VertexSnakeBody9*pv, D3DXVECTOR3&vecCenter, D3DXVECTOR3&vecNorm, float flDist )
{
	D3DXVECTOR3	vecPrevDir = D3DXVECTOR3(0,1,0);
	for (int i = 0; i < nBodyPoints; ++i, ++pv)
	{
		pv->PosCenter = vecCenter;
		pv->Dir = D3DXVECTOR3(	vecNorm.x * vecBodyPoints[i].x,
			vecNorm.y * vecBodyPoints[i].y,
			vecNorm.z * vecBodyPoints[i].x );
		pv->Tangent = D3DXVECTOR3( -vecNorm.z, 0.0f, vecNorm.x );
		pv->Binormal = pv->Dir - vecPrevDir;
		pv->Distance = D3DXVECTOR2(flDist, (float)(i)/(float)(nBodyPoints-1));

		vecPrevDir = pv->Dir;
	}
}