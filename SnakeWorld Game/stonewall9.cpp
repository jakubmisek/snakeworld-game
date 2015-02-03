#include "stonewall.h"

CStoneWallModel9::CStoneWallModel9( CDX9Device&dev, CStoneWallContainer*pContainer )
:CStoneWallModel(pContainer)
{
	pVB = NULL;
	pIB = NULL;

	nVB = nIB = 0;

	InitModel(dev);
}
CStoneWallModel9::~CStoneWallModel9()
{
	SAFE_RELEASE(pVB);
	SAFE_RELEASE(pIB);
}

void	CStoneWallModel9::RenderStoneWall( CDXDevice&dev )
{
	CDX9Device	*pdev = dev.pDX9;
	if ( !pdev )
		return;

	pdev->pEffect->SetMatrix( pdev->pVarWorldTransform, &m_matWorld );
	pdev->pEffect->SetTexture( pdev->pVarDiffuseTexture, m_pContainer->m_texture.GetTexture9() );

	pdev->pd3dDevice->SetVertexDeclaration(m_pContainer->m_pWallDecl9);
	pdev->pd3dDevice->SetStreamSource(0,pVB,0,sizeof(VertexWall));
	pdev->pd3dDevice->SetIndices(pIB);

	pdev->pEffect->SetTechnique( m_pContainer->m_pWallTechnique9 );

	UINT cPasses;
	if (SUCCEEDED(pdev->pEffect->Begin( &cPasses, 0 ) ))
	{
		for( UINT p = 0; p < cPasses; ++p )
		{
			if (SUCCEEDED(pdev->pEffect->BeginPass( p ) ))
			{
				pdev->pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, nVB, 0, nIB/3 );

				pdev->pEffect->EndPass();
			}
		}

		pdev->pEffect->End();
	}
}
void	CStoneWallModel9::RenderStoneWallGrassMove( CDXDevice&dev, D3DXVECTOR3&vecGrassTranslation )
{

}
void	CStoneWallModel9::RenderStoneWallShadowVolume( CDXDevice&dev )
{

}

bool	CStoneWallModel9::InitModel( CDX9Device&dev )
{
	VertexWall	*wallvb;
	DWORD		*wallib;
	CreateStoneWallVertexes( &wallvb, &nVB, &wallib, &nIB );


	// VB
	if( FAILED( dev.pd3dDevice->CreateVertexBuffer( sizeof(VertexWall)*nVB, 0 /*Usage*/, 0, D3DPOOL_MANAGED, &pVB, NULL ) ) )
		return false;

	VertexWall* pVertOut;
	if( FAILED( pVB->Lock( 0, sizeof(VertexWall)*nVB, (void**)&pVertOut, 0 ) ) )
		return false;

	memcpy( pVertOut, wallvb, sizeof(VertexWall)*nVB );

	pVB->Unlock();

	// IB
	if( FAILED( dev.pd3dDevice->CreateIndexBuffer( sizeof(WORD)*nIB, 0 /*Usage*/, D3DFMT_INDEX16, D3DPOOL_MANAGED, &pIB, NULL ) ) )
		return false;

	WORD* pIndexOut;
	if( FAILED( pIB->Lock( 0, sizeof(WORD)*nIB, (void**)&pIndexOut, 0 ) ) )
		return false;

	for (UINT ii = 0; ii < nIB; ++ii )	// from 32b to 16b
		pIndexOut[ii] = (WORD)wallib[ii];

	pIB->Unlock();

	// delete buffers
	delete[] wallvb;
	delete[] wallib;

	return true;
}