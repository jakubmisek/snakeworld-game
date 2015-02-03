#include "butterflymodel.h"


CButterflyModel9::CButterflyModel9()
{
	m_pButterflyDecl = NULL;

	pVB = NULL;
	pIB = NULL;
	nIB = nVB = 0;
}


CButterflyModel9::~CButterflyModel9()
{
	SAFE_RELEASE(m_pButterflyDecl);

	SAFE_RELEASE(pVB);
	SAFE_RELEASE(pIB);
}


bool CButterflyModel9::CreateModel( CDXDevice&dev )
{
	m_pButterflyTechnique = dev.pDX9->pEffect->GetTechniqueByName("RenderButterfly");

	pVarAnimation = dev.pDX9->pEffect->GetParameterByName(NULL,"g_flButterflyAnim");

	// vertex declaration
	const D3DVERTEXELEMENT9 Decl[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12,  D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 20,  D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0 },
		D3DDECL_END()
	};

	if (FAILED(dev.pDX9->pd3dDevice->CreateVertexDeclaration( Decl, &m_pButterflyDecl )))
		return false;

	// MODEL
	VertexButterfly	*vb;
	WORD		*ib;
	
	GenerateModel( &vb, &nVB, &ib, &nIB );


	// VB
	if( FAILED( dev.pDX9->pd3dDevice->CreateVertexBuffer( sizeof(VertexButterfly)*nVB, 0 /*Usage*/, 0, D3DPOOL_MANAGED, &pVB, NULL ) ) )
		return false;

	VertexButterfly* pVertOut;
	if( FAILED( pVB->Lock( 0, sizeof(VertexButterfly)*nVB, (void**)&pVertOut, 0 ) ) )
		return false;

	memcpy( pVertOut, vb, sizeof(VertexButterfly)*nVB );

	pVB->Unlock();

	// IB
	if( FAILED( dev.pDX9->pd3dDevice->CreateIndexBuffer( sizeof(WORD)*nIB, 0 /*Usage*/, D3DFMT_INDEX16, D3DPOOL_MANAGED, &pIB, NULL ) ) )
		return false;

	WORD* pIndexOut;
	if( FAILED( pIB->Lock( 0, sizeof(WORD)*nIB, (void**)&pIndexOut, 0 ) ) )
		return false;

	memcpy( pIndexOut, ib, sizeof(WORD)*nIB );

	pIB->Unlock();


	// delete buffers
	delete[] vb;
	delete[] ib;


	// done
	return true;
}


void CButterflyModel9::RenderModel( CDXDevice&dev, D3DXMATRIX&matWorld, FLOAT flWingsAnimation, CTextureLoader&texture )
{
	CDX9Device	*pdev = dev.pDX9;
	if ( !pdev )
		return;

	pdev->pEffect->SetMatrix( pdev->pVarWorldTransform, &matWorld );
	pdev->pEffect->SetTexture( pdev->pVarDiffuseTexture, texture.GetTexture9() );
	pdev->pEffect->SetFloat( pVarAnimation, flWingsAnimation );

	pdev->pd3dDevice->SetVertexDeclaration(m_pButterflyDecl);
	pdev->pd3dDevice->SetStreamSource(0,pVB,0,sizeof(VertexButterfly));
	pdev->pd3dDevice->SetIndices(pIB);

	pdev->pEffect->SetTechnique( m_pButterflyTechnique );

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