#include "applemodel.h"

// apple texture
#define		APPLE_TEXTURE_FILE		L"media\\apple.jpg"


CAppleModel9::CAppleModel9()
{
	m_pAppleDecl = NULL;

	pVB = NULL;
	pIB = NULL;
	nIB = nVB = 0;
}


CAppleModel9::~CAppleModel9()
{
	SAFE_RELEASE(m_pAppleDecl);

	SAFE_RELEASE(pVB);
	SAFE_RELEASE(pIB);
}


bool	CAppleModel9::CreateModel( CDXDevice&dev )
{
	m_pAppleTechnique = dev.pDX9->pEffect->GetTechniqueByName("RenderApple");

	// vertex declaration
	const D3DVERTEXELEMENT9 Decl[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0, 24,  D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	if (FAILED(dev.pDX9->pd3dDevice->CreateVertexDeclaration( Decl, &m_pAppleDecl )))
		return false;

	// MODEL
	m_texture.CreateTexture( dev, APPLE_TEXTURE_FILE, NULL );

	VertexApple	*applevb;
	WORD		*appleib;
	
	GenerateAppleModel( &applevb, &nVB, &appleib, &nIB );


	// VB
	if( FAILED( dev.pDX9->pd3dDevice->CreateVertexBuffer( sizeof(VertexApple)*nVB, 0 /*Usage*/, 0, D3DPOOL_MANAGED, &pVB, NULL ) ) )
		return false;

	VertexApple* pVertOut;
	if( FAILED( pVB->Lock( 0, sizeof(VertexApple)*nVB, (void**)&pVertOut, 0 ) ) )
		return false;

	memcpy( pVertOut, applevb, sizeof(VertexApple)*nVB );

	pVB->Unlock();

	// IB
	if( FAILED( dev.pDX9->pd3dDevice->CreateIndexBuffer( sizeof(WORD)*nIB, 0 /*Usage*/, D3DFMT_INDEX16, D3DPOOL_MANAGED, &pIB, NULL ) ) )
		return false;

	WORD* pIndexOut;
	if( FAILED( pIB->Lock( 0, sizeof(WORD)*nIB, (void**)&pIndexOut, 0 ) ) )
		return false;

	memcpy( pIndexOut, appleib, sizeof(WORD)*nIB );

	pIB->Unlock();


	// delete buffers
	delete[] applevb;
	delete[] appleib;


	// done
	return true;
}


void	CAppleModel9::RenderApple( CDXDevice&dev, D3DXMATRIX&matWorld )
{
	CDX9Device	*pdev = dev.pDX9;
	if ( !pdev )
		return;

	pdev->pEffect->SetMatrix( pdev->pVarWorldTransform, &matWorld );
	pdev->pEffect->SetTexture( pdev->pVarDiffuseTexture, m_texture.GetTexture9() );

	pdev->pd3dDevice->SetVertexDeclaration(m_pAppleDecl);
	pdev->pd3dDevice->SetStreamSource(0,pVB,0,sizeof(VertexApple));
	pdev->pd3dDevice->SetIndices(pIB);

	pdev->pEffect->SetTechnique( m_pAppleTechnique );

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


void	CAppleModel9::RenderAppleGrassMove( CDXDevice&dev, D3DXMATRIX&matWorld, D3DXVECTOR3&vecGrassTranslation )
{

}


void	CAppleModel9::RenderAppleShadowVolume( CDXDevice&dev, D3DXMATRIX&matWorld )
{

}
