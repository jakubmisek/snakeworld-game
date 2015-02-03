#include "segmentsmodel.h"
#include "segmentsmodel9.h"


CSnakeHeadModel9::CSnakeHeadModel9(CDX9Device&dev)
{
	m_nVertexes = 0;
	m_pVertexBuffer = NULL;

	m_pEyeVB = NULL;
	m_pEyeIB = NULL;
	m_nEyeVertexCount = m_nEyeIndexCount = 0;

	CreateHeadModel( dev );
	CreateEyeModel( dev );
}
CSnakeHeadModel9::~CSnakeHeadModel9()
{
	SAFE_RELEASE( m_pVertexBuffer );
	SAFE_RELEASE( m_pEyeVB );
	SAFE_RELEASE( m_pEyeIB );
}

void	CSnakeHeadModel9::RenderModel( CDXDevice&dev, CSegmentsModelContainer*pContainer, D3DXVECTOR3&vecPosition, float flDirection, float flBodyLength, CCamera&camera, CTextureLoader&SnakeTexture, CTextureLoader&NormalTexture )
{
	CDX9Device*pdev = dev.pDX9;

	if (!pdev || !m_nVertexes)
		return;

	pdev->pEffect->SetFloat(pContainer->pVarDistanceFromHead9, 0.0f);
	pdev->pEffect->SetFloat(pContainer->pVarDistanceFromTail9, flBodyLength);

	//
	// HEAD
	//

	// setup matrix
	D3DXMATRIX matWorld;
	D3DXMatrixRotationY( &matWorld, (float)( -flDirection + D3DX_PI ) );
	matWorld.m[3][0] = vecPosition.x;
	matWorld.m[3][1] = vecPosition.y;
	matWorld.m[3][2] = vecPosition.z;
	pdev->pEffect->SetMatrix(pdev->pVarWorldTransform, &matWorld);

	pdev->pEffect->SetTexture( pdev->pVarDiffuseTexture, SnakeTexture.GetTexture9() );
	pdev->pEffect->SetTexture( pdev->pVarNormalTexture, NormalTexture.GetTexture9() );

	pdev->pd3dDevice->SetVertexDeclaration(pContainer->pSnakeDecl9);
	pdev->pd3dDevice->SetStreamSource(0,m_pVertexBuffer,0,sizeof(VertexSnakeBody9));
	pdev->pd3dDevice->SetIndices(pContainer->pSnakeBodyIB9);

	pdev->pEffect->SetTechnique( pContainer->pSnakeTechnique9 );

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

	//
	// EYE
	//
	// DX: init the input layout and primitive topology
	pdev->pd3dDevice->SetVertexDeclaration(pContainer->pEyeDecl9);
	pdev->pd3dDevice->SetStreamSource(0,m_pEyeVB,0,sizeof(VertexEye));
	pdev->pd3dDevice->SetIndices(m_pEyeIB);

	// setup matrix
	float flDstTail = flBodyLength + 1.0f;
	float flDstHead = - 1.0f;
	for ( int iEye = 0; iEye < 2; ++iEye )
	{
		SetupEyeMatrix( matWorld, vecPosition, flDirection, iEye, 
			0.75f *
			( min( flDstTail*0.07f, 1.0f ) * (1.0f / (abs(flDstTail*0.8f - flDstHead*1.2f)*( 0.05f ) + 1.0f) + 1.0f) )	// LINE COPIED FROM THE EFFECTS.FX <-- SnakeBodyVS::flRad
			);
		pdev->pEffect->SetMatrix(pdev->pVarWorldTransform, &matWorld);

		// Render snake eye
		pdev->pEffect->SetTechnique( pContainer->pEyeTechnique9 );

		UINT cPasses;
		if (SUCCEEDED(pdev->pEffect->Begin( &cPasses, 0 ) ))
		{
			for( UINT p = 0; p < cPasses; ++p )
			{
				if (SUCCEEDED(pdev->pEffect->BeginPass( p ) ))
				{
					pdev->pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_nEyeVertexCount, 0, m_nEyeIndexCount/3 );

					pdev->pEffect->EndPass();
				}
			}

			pdev->pEffect->End();
		}
	}
}
void	CSnakeHeadModel9::RenderGrassMove( CDXDevice&dev, CSegmentsModelContainer*pContainer, D3DXVECTOR3&vecPosition, float flDirection, float flBodyLength, D3DXVECTOR3&vecGrassTranslation )
{

}
void	CSnakeHeadModel9::RenderShadowVolume( CDXDevice&dev, CSegmentsModelContainer*pContainer, D3DXVECTOR3&vecPosition, float flDirection, float flBodyLength )
{

}
bool	CSnakeHeadModel9::CreateHeadModel( CDX9Device&dev )
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

	UINT nPts = sizeof( pts ) / sizeof( pts[0] );

	m_nVertexes = nPts * nBodyPoints;

	if( FAILED( dev.pd3dDevice->CreateVertexBuffer( sizeof(VertexSnakeBody9)*m_nVertexes, 0 /*Usage*/, 0, D3DPOOL_MANAGED, &m_pVertexBuffer, NULL ) ) )
		return false;

	VertexSnakeBody9	*pv;
	if( FAILED( m_pVertexBuffer->Lock( 0, sizeof(VertexSnakeBody9) * m_nVertexes, (void**)&pv, D3DLOCK_DISCARD  ) ) )
		return false;

	//
	for (UINT i = 0; i < nPts; ++i)
	{
		UpdateRing9( pv, pts[i].Pos, pts[i].Normal, pts[i].Distance );
		pv += nBodyPoints;
	}


	//
	m_pVertexBuffer->Unlock();

	return true;
}


//////////////////////////////////////////////////////////////////////////
// create eye model
bool	CSnakeHeadModel9::CreateEyeModel( CDX9Device&dev )
{
	//
	// eye
	//
	VertexEye *pts;
	DWORD *ids;

	CreateEyeVertexes( &pts, &m_nEyeVertexCount, &ids, &m_nEyeIndexCount );

	if( FAILED( dev.pd3dDevice->CreateVertexBuffer( sizeof(VertexEye)*m_nEyeVertexCount, 0 /*Usage*/, 0, D3DPOOL_MANAGED, &m_pEyeVB, NULL ) ) )
		return false;

	VertexEye* pVertOut;
	if( FAILED( m_pEyeVB->Lock( 0, sizeof(VertexEye)*m_nEyeVertexCount, (void**)&pVertOut, 0 ) ) )
		return false;

	memcpy( pVertOut, pts, sizeof(VertexEye)*m_nEyeVertexCount );

	m_pEyeVB->Unlock();

	if( FAILED( dev.pd3dDevice->CreateIndexBuffer( sizeof(WORD)*m_nEyeIndexCount, 0 /*Usage*/, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pEyeIB, NULL ) ) )
		return false;

	WORD* pIndexOut;
	if( FAILED( m_pEyeIB->Lock( 0, sizeof(WORD)*m_nEyeIndexCount, (void**)&pIndexOut, 0 ) ) )
		return false;

	for (UINT ii = 0; ii < m_nEyeIndexCount; ++ii )	// from 32b to 16b
		pIndexOut[ii] = (WORD)ids[ii];

	m_pEyeIB->Unlock();

	// delete buffers
	delete[] pts;
	delete[] ids;

	// done
	return true;
}