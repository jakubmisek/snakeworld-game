#include "skybox.h"
#include "errorlog.h"

#define SKYBOX_NUMPOINTS	(16)
#define SKYBOX_H_NUMPOINTS	(8)


//////////////////////////////////////////////////////////////////////////
// sky box ctor
CSkyBox::CSkyBox()
{
	m_pSkyboxVB = m_pSkyboxIB = NULL;

	m_pSkyEffect = NULL;
	m_pSkyboxTechnique = NULL;

	m_pSkyboxInputLayout = NULL;

	m_pSkyboxVB9 = NULL;
	m_pSkyboxIB9 = NULL;
	m_pSkyboxDecl9 = NULL;
	m_pSkyboxTechnique9 = NULL;
	m_pSkyEffect9 = NULL;

}


//////////////////////////////////////////////////////////////////////////
// sky box dtor
CSkyBox::~CSkyBox()
{
	// dx10
	SAFE_RELEASE( m_pSkyboxIB );
	SAFE_RELEASE( m_pSkyboxVB );

	SAFE_RELEASE(m_pSkyEffect);

	SAFE_RELEASE( m_pSkyboxInputLayout );

	// dx9
	SAFE_RELEASE( m_pSkyboxIB9 );
	SAFE_RELEASE( m_pSkyboxVB9 );

	SAFE_RELEASE(m_pSkyEffect9);

	SAFE_RELEASE( m_pSkyboxDecl9 );
}


//////////////////////////////////////////////////////////////////////////
// init skybox model
void	CSkyBox::InitModel( VertexSkyBox*vertexes, DWORD*indexes )
{
	DWORD *pi = indexes;

	for (int i = 0; i <= SKYBOX_NUMPOINTS; ++i)
	{
		VertexSkyBox *p0 = vertexes + i*SKYBOX_H_NUMPOINTS;

		float f = (float)i / (float)SKYBOX_NUMPOINTS;	// [0.0; 1.0]
		double fr = (double)f * (D3DX_PI * 2.0);

		for ( int j = 0; j < SKYBOX_H_NUMPOINTS; ++j )
		{
			VertexSkyBox *p = p0 + j;

			float g = (float)j / (float)(SKYBOX_H_NUMPOINTS-1);	// [0.0; 1.0]
			double gr = (double)g * (D3DX_PI * 0.5);

			p->Tex.x = f;
			p->Tex.y = 1.0f - g;

			float c = (float)cos(gr);

			p->Pos = D3DXVECTOR3( (float)cos(fr)*c*10.0f, (float)sin(gr)*10.0f - 1.0f, (float)sin(fr)*c*10.0f );

			// new quad
			if ( j > 0 && i < SKYBOX_NUMPOINTS )
			{
				int i2 = (i + 1);
				int j2 = (j-1);

#define IJ( _i, _j )	( (_i) * SKYBOX_H_NUMPOINTS + (_j) )

				pi[0] = IJ(i,j);
				pi[2] = IJ(i2,j2);
				pi[1] = IJ(i,j2);
				//if ( vertexes[pi[0]] != vertexes[pi[2]] )
				pi += 3;


				pi[0] = IJ(i,j);
				pi[2] = IJ(i2,j);
				pi[1] = IJ(i2,j2);
				//if ( vertexes[pi[0]] != vertexes[pi[2]] )
				pi += 3;

#undef IJ
			}
		}


	}
}


//////////////////////////////////////////////////////////////////////////
// create sky box model
bool	CSkyBox::CreateSkyBox( CDXDevice&dev )
{
	// create terrain texture
	m_texture.CreateTexture( dev, L"media\\sky_v2b_tex.dds", NULL );

	// model
	if (dev.pDX10)
		return CreateSkyBox10(*dev.pDX10);
	else if (dev.pDX9)
		return CreateSkyBox9(*dev.pDX9);
	else
		return false;
}


//////////////////////////////////////////////////////////////////////////
// create the sky box model in DX10
// load the sky box texture
bool	CSkyBox::CreateSkyBox10( CDX10Device&dev )
{
	if ( !CreateSkyEffect10(dev) )
		return false;

	// Define the input layout
	D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof(layout)/sizeof(layout[0]);

	// Create the input layout
	D3D10_PASS_DESC PassDesc;
	m_pSkyboxTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );
	if ( FAILED(dev.pd3dDevice->CreateInputLayout( layout, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &m_pSkyboxInputLayout )) )
		return false;	// unable to create the input layout

	VertexSkyBox	vertexes[ (SKYBOX_NUMPOINTS+1)*SKYBOX_H_NUMPOINTS ];
	DWORD			indexes[ (SKYBOX_NUMPOINTS)*(SKYBOX_H_NUMPOINTS-1)*6 ];

	InitModel(vertexes,indexes);

	// vertex buffer
	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( vertexes );	// size
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;	// vertex buffer
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA	initdata;
	initdata.pSysMem = vertexes;

	if( FAILED( dev.pd3dDevice->CreateBuffer( &bd, &initdata, &m_pSkyboxVB ) ) )
		return false;	// failed to create

	// index buffer
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( indexes );	// size
	bd.BindFlags = D3D10_BIND_INDEX_BUFFER;	// vertex buffer
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	
	initdata.pSysMem = indexes;

	if( FAILED( dev.pd3dDevice->CreateBuffer( &bd, &initdata, &m_pSkyboxIB ) ) )
		return false;	// failed to create

	

	// done
	return true;
}


//////////////////////////////////////////////////////////////////////////
// create sky box effect
bool	CSkyBox::CreateSkyEffect10( CDX10Device&dev )
{
	//
	// EFFECT
	//

	// Effect flags
	DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3D10_SHADER_DEBUG;
#endif

	ID3D10Blob* pErrShader;
	dwShaderFlags = 0;

	// Create the effect
	HRESULT hr;
	if( FAILED( hr = D3DX10CreateEffectFromFile( L"fx\\sky10.fxo", NULL, NULL, NULL, dwShaderFlags, 0, dev.pd3dDevice, NULL, NULL, &m_pSkyEffect, &pErrShader, NULL ) ) )
	{
		if (pErrShader)
		{
			AddLog((char*)pErrShader->GetBufferPointer());
			pErrShader->Release();
		}
		
		return false ;
	}

	if (!CreateSkyboxTechnique10(dev))
		return false;

	return true;
}


//////////////////////////////////////////////////////////////////////////
// create the sky box technique
bool	CSkyBox::CreateSkyboxTechnique10( CDX10Device&dev )
{
	// Obtain the techniques
	m_pSkyboxTechnique = m_pSkyEffect->GetTechniqueByName( "RenderSkyBox" );

	// Obtain variables
	m_pVarTexture = m_pSkyEffect->GetVariableByName( "texDiffuse" )->AsShaderResource();
	m_pVarViewProjMatrix = m_pSkyEffect->GetVariableByName("matViewProj")->AsMatrix();

	// done
	return ( m_pSkyboxTechnique != NULL && m_pVarTexture != NULL && m_pVarViewProjMatrix != NULL );
}


//////////////////////////////////////////////////////////////////////////
// create the sky box model in DX9
bool	CSkyBox::CreateSkyBox9( CDX9Device&dev )
{
	//
	// EFFECT
	//
	DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE;
#ifdef DEBUG_VS
	dwShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
#endif
#ifdef DEBUG_PS
	dwShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
#endif

#ifdef DEBUG
	dwShaderFlags |= D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION;
#endif


	dwShaderFlags = 0;
	LPD3DXBUFFER pErrOutBuff = NULL;
	if ( FAILED(D3DXCreateEffectFromFile( dev.pd3dDevice, L"fx\\sky9.fxo", NULL, NULL, dwShaderFlags, NULL, &m_pSkyEffect9, &pErrOutBuff )) )
	{
		if (pErrOutBuff)
		{
			AddLog( (char*)pErrOutBuff->GetBufferPointer() );
			pErrOutBuff->Release();
		}

		return false;
	}

	//
	// VARIABLES
	//
	m_pVarTexture9 = m_pSkyEffect9->GetParameterByName(NULL,"texDiffuse");
	m_pVarViewprojTransform9 = m_pSkyEffect9->GetParameterByName(NULL,"matViewProj");

	// TECHNIQUE
	m_pSkyboxTechnique9 = m_pSkyEffect9->GetTechniqueByName("RenderSkyBox");

	// vertex+index buffer

	const D3DVERTEXELEMENT9 Decl[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12,  D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	if (FAILED(dev.pd3dDevice->CreateVertexDeclaration( Decl, &m_pSkyboxDecl9 )))
		return false;

	VertexSkyBox	vertexes[ (SKYBOX_NUMPOINTS+1)*SKYBOX_H_NUMPOINTS ];
	DWORD			indexes[ (SKYBOX_NUMPOINTS)*(SKYBOX_H_NUMPOINTS-1)*6 ];

	InitModel(vertexes,indexes);

		// VB
	if( FAILED( dev.pd3dDevice->CreateVertexBuffer( sizeof(vertexes), 0 /*Usage*/, 0, D3DPOOL_MANAGED, &m_pSkyboxVB9, NULL ) ) )
		return false;

	VertexSkyBox* pVertOut;
	if( FAILED( m_pSkyboxVB9->Lock( 0, sizeof(vertexes), (void**)&pVertOut, 0 ) ) )
		return false;

	memcpy( pVertOut, vertexes, sizeof(vertexes) );
	
	m_pSkyboxVB9->Unlock();

		// IB
	if( FAILED( dev.pd3dDevice->CreateIndexBuffer( sizeof(indexes), 0 /*Usage*/, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pSkyboxIB9, NULL ) ) )
		return false;

	WORD* pIndexOut;
	if( FAILED( m_pSkyboxIB9->Lock( 0, sizeof(indexes), (void**)&pIndexOut, 0 ) ) )
		return false;

	for (UINT ii = 0; ii < (SKYBOX_NUMPOINTS)*(SKYBOX_H_NUMPOINTS-1)*6; ++ii )	// from 32b to 16b
			pIndexOut[ii] = (WORD)indexes[ii];

	m_pSkyboxIB9->Unlock();

	return true;
}


//////////////////////////////////////////////////////////////////////////
// render sky box
void	CSkyBox::Render( CDXDevice&dev, CCamera&camera )
{
	// change view * projection matrix
	D3DXMATRIX matView, matProj;
	camera.GetView(matView);
	camera.GetProjection(matProj);

	matView._41 = matView._42 = matView._43 = 0.0f;
	matView._42 -= 1.0f;

	D3DXMATRIX matViewProj;
	D3DXMatrixMultiply( &matViewProj, &matView, &matProj );

	// render the model
	if (dev.pDX10)
		Render10(*dev.pDX10,camera,matViewProj);
	else if (dev.pDX9)
		Render9(*dev.pDX9,camera,matViewProj);

	// restore the view transform
	camera.ApplyTransform( dev );
}


//////////////////////////////////////////////////////////////////////////
// render the sky box using DX10
void	CSkyBox::Render10( CDX10Device&dev, CCamera&camera, D3DXMATRIX&matViewProj )
{
	
	m_pVarViewProjMatrix->SetMatrix( (float*)&matViewProj );

	// terrain texture
	m_pVarTexture->SetResource( m_texture.GetTexture10View() );

	// DX: init the input layout and primitive topology
	dev.pd3dDevice->IASetInputLayout( m_pSkyboxInputLayout );
	dev.pd3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	UINT stride = sizeof(VertexSkyBox), offset = 0;

	// set the vertex buffer
	dev.pd3dDevice->IASetVertexBuffers( 0, 1, &m_pSkyboxVB, &stride, &offset );
	dev.pd3dDevice->IASetIndexBuffer( m_pSkyboxIB, DXGI_FORMAT_R32_UINT, 0 );
	
	// technique description
	D3D10_TECHNIQUE_DESC techDesc;
	m_pSkyboxTechnique->GetDesc( &techDesc );

	// render terrain model
	for( UINT p = 0; p < techDesc.Passes; ++p )
	{
		m_pSkyboxTechnique->GetPassByIndex( p )->Apply(0);
		dev.pd3dDevice->DrawIndexed( (SKYBOX_NUMPOINTS)*(SKYBOX_H_NUMPOINTS-1)*6 , 0, 0 );
	}
	
}


//////////////////////////////////////////////////////////////////////////
// render the sky box using DX9
void	CSkyBox::Render9( CDX9Device&dev, CCamera&camera, D3DXMATRIX&matViewProj )
{
	m_pSkyEffect9->SetMatrix(m_pVarViewprojTransform9, &matViewProj);
	m_pSkyEffect9->SetTexture( m_pVarTexture9, m_texture.GetTexture9() );

	dev.pd3dDevice->SetVertexDeclaration(m_pSkyboxDecl9);
	dev.pd3dDevice->SetStreamSource(0,m_pSkyboxVB9,0,sizeof(VertexSkyBox));
	dev.pd3dDevice->SetIndices(m_pSkyboxIB9);

	m_pSkyEffect9->SetTechnique( m_pSkyboxTechnique9 );

	UINT cPasses;
	if (SUCCEEDED(m_pSkyEffect9->Begin( &cPasses, 0 ) ))
	{
		for( UINT p = 0; p < cPasses; ++p )
		{
			if (SUCCEEDED(m_pSkyEffect9->BeginPass( p ) ))
			{
				dev.pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, (SKYBOX_NUMPOINTS+1)*SKYBOX_H_NUMPOINTS, 0, (SKYBOX_NUMPOINTS)*(SKYBOX_H_NUMPOINTS-1)*2 );

				m_pSkyEffect9->EndPass();
			}
		}

		m_pSkyEffect9->End();
	}
}