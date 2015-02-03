#include "applemodel.h"

// apple texture
#define		APPLE_TEXTURE_FILE		L"media\\apple.jpg"


//
CAppleModel10::CAppleModel10()
{
	pVB = pIB = NULL;
	nIB = 0;
	m_pAppleInputLayout = NULL;
}


CAppleModel10::~CAppleModel10()
{
	SAFE_RELEASE( pVB );
	SAFE_RELEASE( pIB );

	SAFE_RELEASE(m_pAppleInputLayout);
}


bool	CAppleModel10::CreateModel( CDXDevice&dev )
{
	m_texture.CreateTexture( dev, APPLE_TEXTURE_FILE, NULL );

	VertexApple	*applevb;
	WORD		*appleib;
	UINT		vbcount;

	GenerateAppleModel( &applevb, &vbcount, &appleib, &nIB );


	// vertex buffer
	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( VertexApple ) * vbcount;	// size
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;	// vertex buffer
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA	initdata;
	initdata.pSysMem = applevb;

	if( FAILED( dev.pDX10->pd3dDevice->CreateBuffer( &bd, &initdata, &pVB ) ) )
		return false;	// failed to create

	// index buffer
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( WORD ) * nIB;	// size
	bd.BindFlags = D3D10_BIND_INDEX_BUFFER;	// index buffer
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	initdata.pSysMem = appleib;

	if( FAILED( dev.pDX10->pd3dDevice->CreateBuffer( &bd, &initdata, &pIB ) ) )
		return false;	// failed to create

	// delete buffers
	delete[] applevb;
	delete[] appleib;

	//
	// effect
	//

	// technique
	m_pAppleTechnique = dev.pDX10->pEffect->GetTechniqueByName( "RenderApple" );
	m_pAppleGrassMoveTechnique = dev.pDX10->pEffect->GetTechniqueByName( "RenderStoneWallGrassMoveMask" );
	m_pAppleShadowVolumeTechnique = dev.pDX10->pEffect->GetTechniqueByName( "RenderStoneWallShadowVolume" );

	if ( !m_pAppleTechnique || !m_pAppleGrassMoveTechnique || !m_pAppleShadowVolumeTechnique )
		return false;

	// Define the input layout
	D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof(layout)/sizeof(layout[0]);

	// Create the input layout
	D3D10_PASS_DESC PassDesc;
	m_pAppleTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );
	if ( FAILED(dev.pDX10->pd3dDevice->CreateInputLayout( layout, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &m_pAppleInputLayout )) )
		return false;	// unable to create the input layout

	return true;
}


void	CAppleModel10::RenderApple( CDXDevice&dev, D3DXMATRIX&matWorld )
{
	// transformation
	dev.pDX10->pVarMatWorld->SetMatrix( (float*)&matWorld );

	// terrain texture
	dev.pDX10->pVarDiffuseTexture->SetResource( m_texture.GetTexture10View() );
	////dev.pDX10->pVarNormalTexture->SetResource( m_normaltexture.GetTexture10View());

	// DX: init the input layout and primitive topology
	dev.pDX10->pd3dDevice->IASetInputLayout( m_pAppleInputLayout );
	dev.pDX10->pd3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	UINT stride = sizeof(VertexApple), offset = 0;

	// set the vertex buffer
	dev.pDX10->pd3dDevice->IASetVertexBuffers( 0, 1, &pVB, &stride, &offset );
	dev.pDX10->pd3dDevice->IASetIndexBuffer( pIB, DXGI_FORMAT_R16_UINT, 0 );

	// technique description
	D3D10_TECHNIQUE_DESC techDesc;
	m_pAppleTechnique->GetDesc( &techDesc );

	// render terrain model
	for( UINT p = 0; p < techDesc.Passes; ++p )
	{
		m_pAppleTechnique->GetPassByIndex( p )->Apply(0);
		dev.pDX10->pd3dDevice->DrawIndexed( nIB, 0, 0 );
	}
}


void	CAppleModel10::RenderAppleGrassMove( CDXDevice&dev, D3DXMATRIX&matWorld, D3DXVECTOR3&vecGrassTranslation )
{
	D3DXMATRIX matWorld1 = matWorld;

	matWorld1._41 -= vecGrassTranslation.x;
	matWorld1._42 -= vecGrassTranslation.y;
	matWorld1._43 -= vecGrassTranslation.z;

	// transformation
	dev.pDX10->pVarMatWorld->SetMatrix( (float*)&matWorld1 );

	// DX: init the input layout and primitive topology
	dev.pDX10->pd3dDevice->IASetInputLayout( m_pAppleInputLayout );
	dev.pDX10->pd3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	UINT stride = sizeof(VertexApple), offset = 0;

	// set the vertex buffer
	dev.pDX10->pd3dDevice->IASetVertexBuffers( 0, 1, &pVB, &stride, &offset );
	dev.pDX10->pd3dDevice->IASetIndexBuffer( pIB, DXGI_FORMAT_R16_UINT, 0 );

	// technique description
	D3D10_TECHNIQUE_DESC techDesc;
	m_pAppleGrassMoveTechnique->GetDesc( &techDesc );

	// render terrain model
	for( UINT p = 0; p < techDesc.Passes; ++p )
	{
		m_pAppleGrassMoveTechnique->GetPassByIndex( p )->Apply(0);
		dev.pDX10->pd3dDevice->DrawIndexed( nIB, 0, 0 );
	}
}


void	CAppleModel10::RenderAppleShadowVolume( CDXDevice&dev, D3DXMATRIX&matWorld )
{
	// transformation
	dev.pDX10->pVarMatWorld->SetMatrix( (float*)&matWorld );

	// DX: init the input layout and primitive topology
	dev.pDX10->pd3dDevice->IASetInputLayout( m_pAppleInputLayout );
	dev.pDX10->pd3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	UINT stride = sizeof(VertexApple), offset = 0;

	// set the vertex buffer
	dev.pDX10->pd3dDevice->IASetVertexBuffers( 0, 1, &pVB, &stride, &offset );
	dev.pDX10->pd3dDevice->IASetIndexBuffer( pIB, DXGI_FORMAT_R16_UINT, 0 );

	// technique description
	D3D10_TECHNIQUE_DESC techDesc;
	m_pAppleShadowVolumeTechnique->GetDesc( &techDesc );

	// render terrain model
	for( UINT p = 0; p < techDesc.Passes; ++p )
	{
		m_pAppleShadowVolumeTechnique->GetPassByIndex( p )->Apply(0);
		dev.pDX10->pd3dDevice->DrawIndexed( nIB, 0, 0 );
	}
}
