#include "butterflymodel.h"

CButterflyModel10::CButterflyModel10()
{
	pVB = NULL;
	m_pButterflyInputLayout = NULL;
}


CButterflyModel10::~CButterflyModel10()
{
	SAFE_RELEASE( pVB );

	SAFE_RELEASE(m_pButterflyInputLayout);
}


bool CButterflyModel10::CreateModel( CDXDevice&dev )
{
	D3DXVECTOR3	butterflymodel(0,0,0);

	// vertex buffer
	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( butterflymodel );	// size (one xyz vertex)
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;	// vertex buffer
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA	initdata;
	initdata.pSysMem = &butterflymodel;

	if( FAILED( dev.pDX10->pd3dDevice->CreateBuffer( &bd, &initdata, &pVB ) ) )
		return false;	// failed to create

	//
	// effect
	//

	// technique
	m_pButterflyTechnique = dev.pDX10->pEffect->GetTechniqueByName( "RenderButterfly" );
	
	if ( !m_pButterflyTechnique )
		return false;

	// variables
	pVarAnimation = dev.pDX10->pEffect->GetVariableByName( "flButterflyAnim" )->AsScalar();

	// Define the input layout
	D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof(layout)/sizeof(layout[0]);

	// Create the input layout
	D3D10_PASS_DESC PassDesc;
	m_pButterflyTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );
	if ( FAILED(dev.pDX10->pd3dDevice->CreateInputLayout( layout, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &m_pButterflyInputLayout )) )
		return false;	// unable to create the input layout

	return true;
}


void CButterflyModel10::RenderModel( CDXDevice&dev, D3DXMATRIX&matWorld, FLOAT flWingsAnimation, CTextureLoader&texture )
{
	// transformation
	dev.pDX10->pVarMatWorld->SetMatrix( (float*)&matWorld );

	// wings weight
	pVarAnimation->SetFloat( flWingsAnimation );

	// terrain texture
	dev.pDX10->pVarDiffuseTexture->SetResource( texture.GetTexture10View() );
	////dev.pDX10->pVarNormalTexture->SetResource( m_normaltexture.GetTexture10View());

	// DX: init the input layout and primitive topology
	dev.pDX10->pd3dDevice->IASetInputLayout( m_pButterflyInputLayout );
	dev.pDX10->pd3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_POINTLIST );

	UINT stride = sizeof(D3DXVECTOR3), offset = 0;

	// set the vertex buffer
	dev.pDX10->pd3dDevice->IASetVertexBuffers( 0, 1, &pVB, &stride, &offset );
	
	// technique description
	D3D10_TECHNIQUE_DESC techDesc;
	m_pButterflyTechnique->GetDesc( &techDesc );

	// render terrain model
	for( UINT p = 0; p < techDesc.Passes; ++p )
	{
		m_pButterflyTechnique->GetPassByIndex( p )->Apply(0);
		dev.pDX10->pd3dDevice->Draw(1,0);
	}
}