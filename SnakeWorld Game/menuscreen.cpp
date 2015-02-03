#include "menuscreen.h"
#include "errorlog.h"

//////////////////////////////////////////////////////////////////////////
// menu vertex
struct VertexMenu
{
	D3DXVECTOR2	Pos;
	D3DXVECTOR2	Size;
};


// menu vertex
struct VertexTexture
{
	D3DXVECTOR2	Pos;
};



//////////////////////////////////////////////////////////////////////////
// menu DX objects ctor
CGameMenuContainer::CGameMenuContainer()
{
	m_pSndManager = NULL;
	
	m_pMenuEffect = NULL;
	m_pVB = NULL;
	m_pTexVB = NULL;
	m_pMenuInputLayout = NULL;
	m_pTexInputLayout = NULL;

	m_pMenuEffect9 = NULL;
	m_pMenuTechnique9 = m_pTexTechnique9 =
	m_pVarTexture9 = m_pVarTexPos9 = m_pVarTexSize9 = m_pVarTexAlpha9 =
	m_pVarMenuColor9 = m_pVarMenuPosition9 = NULL;
	m_pMenuDecl9 = m_pTexDecl9 = NULL;
	m_pMenuVB9 = m_pTexVB9 = NULL;
}


//////////////////////////////////////////////////////////////////////////
// menu DX objects dtor
CGameMenuContainer::~CGameMenuContainer()
{
	SAFE_RELEASE( m_pVB );
	SAFE_RELEASE( m_pTexVB );
	SAFE_RELEASE( m_pMenuEffect );
	SAFE_RELEASE( m_pMenuInputLayout );
	SAFE_RELEASE( m_pTexInputLayout );

	SAFE_RELEASE( m_pMenuVB9 );
	SAFE_RELEASE( m_pTexVB9 );
	SAFE_RELEASE( m_pMenuEffect9 );
	SAFE_RELEASE( m_pMenuDecl9 );
	SAFE_RELEASE( m_pTexDecl9 );
}


//////////////////////////////////////////////////////////////////////////
// create the game menu
bool	CGameMenuContainer::CreateMenu( CDXDevice&dev, CSoundManager*pSndManager )
{
	m_pSndManager = pSndManager;

	if (dev.pDX10)
		return CreateMenu10(*dev.pDX10);
	else if (dev.pDX9)
		return CreateMenu9(*dev.pDX9);
	else
		return false;
}

//////////////////////////////////////////////////////////////////////////
// create DX10 game menu
bool	CGameMenuContainer::CreateMenu10( CDX10Device&dev )
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
	if( FAILED( hr = D3DX10CreateEffectFromFile( L"fx\\menu10.fxo", NULL, NULL, NULL, dwShaderFlags, 0, dev.pd3dDevice, NULL, NULL, &m_pMenuEffect, &pErrShader, NULL ) ) )
	{
		if (pErrShader)
		{
			AddLog((char*)pErrShader->GetBufferPointer());
			pErrShader->Release();
		}
		
		return false ;
	}

	// Obtain the variables
	m_pVarMenuPosition = m_pMenuEffect->GetVariableByName( "g_position" )->AsVector();
	m_pVarBackbufferSize = m_pMenuEffect->GetVariableByName( "g_backbuffersize" )->AsVector();
	m_pVarMenuColor = m_pMenuEffect->GetVariableByName( "g_color" )->AsVector();

	m_pVarMenuBlendFactor = m_pMenuEffect->GetVariableByName( "g_blurradius" )->AsScalar();

	m_pVarBackground = m_pMenuEffect->GetVariableByName( "texBackground" )->AsShaderResource();

	m_pVarTexture = m_pMenuEffect->GetVariableByName( "texTexture" )->AsShaderResource();
	m_pVarTexAlpha = m_pMenuEffect->GetVariableByName( "g_texalpha" )->AsScalar();
	m_pVarTexPos = m_pMenuEffect->GetVariableByName( "g_texpos" )->AsVector();
	m_pVarTexSize = m_pMenuEffect->GetVariableByName( "g_texsize" )->AsVector();
	
	if ( !m_pVarBackground || !m_pVarMenuBlendFactor || !m_pVarMenuColor || !m_pVarMenuPosition || !m_pVarTexture || !m_pVarTexAlpha || !m_pVarTexPos || !m_pVarTexSize )
		return false;

	// Obtain techniques
	m_pMenuTechnique = m_pMenuEffect->GetTechniqueByName( "RenderMenu" );
	m_pTexTechnique = m_pMenuEffect->GetTechniqueByName( "RenderTexture" );

	if ( !m_pMenuTechnique )
		return false;

	// Define the input layout
	D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof(layout)/sizeof(layout[0]);

	// Create the input layout
	D3D10_PASS_DESC PassDesc;
	m_pMenuTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );
	if ( FAILED(dev.pd3dDevice->CreateInputLayout( layout, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &m_pMenuInputLayout )) )
		return false;	// unable to create the input layout

	// menu vertex buffer
	VertexMenu	v[4];

	v[0].Size = D3DXVECTOR2(0,0);
	v[1].Size = D3DXVECTOR2(1,0);
	v[2].Size = D3DXVECTOR2(0,1);
	v[3].Size = D3DXVECTOR2(1,1);

	v[0].Pos = D3DXVECTOR2(0.01f,0.03f);
	v[1].Pos = D3DXVECTOR2(0.85f,0.0f);
	v[2].Pos = D3DXVECTOR2(0.01f,0.90f);
	v[3].Pos = D3DXVECTOR2(0.85f,0.93f);
	
	// vertex buffer
	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( v );	// size
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;	// vertex buffer
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA	initdata;
	initdata.pSysMem = v;

	if( FAILED( dev.pd3dDevice->CreateBuffer( &bd, &initdata, &m_pVB ) ) )
		return false;	// failed to create



	// Define the input layout for texture
	D3D10_INPUT_ELEMENT_DESC texlayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numTexElements = sizeof(texlayout)/sizeof(texlayout[0]);

	// Create the input layout
	m_pTexTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );
	if ( FAILED(dev.pd3dDevice->CreateInputLayout( texlayout, numTexElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &m_pTexInputLayout )) )
		return false;	// unable to create the input layout

	// menu vertex buffer
	VertexTexture	tv[4];

	tv[0].Pos = D3DXVECTOR2(0,0);
	tv[1].Pos = D3DXVECTOR2(1,0);
	tv[2].Pos = D3DXVECTOR2(0,1);
	tv[3].Pos = D3DXVECTOR2(1,1);

	// vertex buffer
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( tv );	// size
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;	// vertex buffer
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	initdata.pSysMem = tv;

	if( FAILED( dev.pd3dDevice->CreateBuffer( &bd, &initdata, &m_pTexVB ) ) )
		return false;	// failed to create

	// done
	return true;
}


//////////////////////////////////////////////////////////////////////////
// render the menu background screen
void	CGameMenuContainer::RenderMenuBackground10( CDX10Device&dev, ID3D10ShaderResourceView*pBackground, D3DXVECTOR4*pColor, D3DXVECTOR2*pPosition, float flBlendfactor )
{
	m_pVarBackground->SetResource( pBackground );
	m_pVarMenuColor->SetFloatVector( (float*)pColor );
	m_pVarMenuPosition->SetFloatVector( (float*)pPosition );
	m_pVarBackbufferSize->SetFloatVector( D3DXVECTOR2((float)dev.uWidth,(float)dev.uHeight) );
	m_pVarMenuBlendFactor->SetFloat( flBlendfactor );

	// DX: init the input layout and primitive topology
	dev.pd3dDevice->IASetInputLayout( m_pMenuInputLayout );
	dev.pd3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

	UINT stride = sizeof(VertexMenu), offset = 0;

	// set the vertex buffer
	dev.pd3dDevice->IASetVertexBuffers( 0, 1, &m_pVB, &stride, &offset );

	// technique description
	D3D10_TECHNIQUE_DESC techDesc;
	m_pMenuTechnique->GetDesc( &techDesc );

	// render terrain model
	for( UINT p = 0; p < techDesc.Passes; ++p )
	{
		m_pMenuTechnique->GetPassByIndex( p )->Apply(0);
		dev.pd3dDevice->Draw( 4, 0 );
	}
}


//////////////////////////////////////////////////////////////////////////
// render texture
void	CGameMenuContainer::RenderTexture( CDXDevice&dev, CTextureLoader&tex, float flAlpha, D3DXVECTOR2*pPos, D3DXVECTOR2*pSize )
{
	if (dev.pDX10)
		RenderTexture10(*dev.pDX10, tex.GetTexture10View(),flAlpha, pPos, pSize );
	else if (dev.pDX9)
		RenderTexture9(*dev.pDX9, tex.GetTexture9(), flAlpha, pPos, pSize);
}


//////////////////////////////////////////////////////////////////////////
// render texture using DX10
void	CGameMenuContainer::RenderTexture10( CDX10Device&dev, ID3D10ShaderResourceView*pTexture, float flAlpha, D3DXVECTOR2*pPos, D3DXVECTOR2*pSize )
{
	if ( flAlpha <= 0.0f )
		return;

	m_pVarTexture->SetResource( pTexture );
	m_pVarTexAlpha->SetFloat( flAlpha );
	m_pVarTexPos->SetFloatVector( (float*)pPos );
	m_pVarTexSize->SetFloatVector( (float*)pSize );
	
	// DX: init the input layout and primitive topology
	dev.pd3dDevice->IASetInputLayout( m_pTexInputLayout );
	dev.pd3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

	UINT stride = sizeof(VertexTexture), offset = 0;

	// set the vertex buffer
	dev.pd3dDevice->IASetVertexBuffers( 0, 1, &m_pTexVB, &stride, &offset );

	// technique description
	D3D10_TECHNIQUE_DESC techDesc;
	m_pTexTechnique->GetDesc( &techDesc );

	// render terrain model
	for( UINT p = 0; p < techDesc.Passes; ++p )
	{
		m_pTexTechnique->GetPassByIndex( p )->Apply(0);
		dev.pd3dDevice->Draw( 4, 0 );
	}
}


//////////////////////////////////////////////////////////////////////////
// create DX9 menu
bool	CGameMenuContainer::CreateMenu9(CDX9Device&dev)
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


	LPD3DXBUFFER pErrOutBuff = NULL;
	if ( FAILED(D3DXCreateEffectFromFile( dev.pd3dDevice, L"fx\\menu9.fxo", NULL, NULL, dwShaderFlags, NULL, &m_pMenuEffect9, &pErrOutBuff )) )
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
	m_pVarTexture9 = m_pMenuEffect9->GetParameterByName(NULL,"texTexture");
	m_pVarTexPos9 = m_pMenuEffect9->GetParameterByName(NULL,"g_texpos");
	m_pVarTexSize9 = m_pMenuEffect9->GetParameterByName(NULL,"g_texsize");
	m_pVarTexAlpha9 = m_pMenuEffect9->GetParameterByName(NULL,"g_texalpha");
	m_pVarMenuColor9 = m_pMenuEffect9->GetParameterByName(NULL,"g_color");
	m_pVarMenuPosition9 =  m_pMenuEffect9->GetParameterByName(NULL,"g_position");

	// TECHNIQUE
	m_pMenuTechnique9 = m_pMenuEffect9->GetTechniqueByName("RenderMenu");
	m_pTexTechnique9 = m_pMenuEffect9->GetTechniqueByName("RenderTexture");
	
	// vertex+index buffer

	const D3DVERTEXELEMENT9 DeclMenu[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 8,  D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	if (FAILED(dev.pd3dDevice->CreateVertexDeclaration( DeclMenu, &m_pMenuDecl9 )))
		return false;

	const D3DVERTEXELEMENT9 DeclTex[] =
	{
		{ 0, 0,  D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		D3DDECL_END()
	};

	if (FAILED(dev.pd3dDevice->CreateVertexDeclaration( DeclTex, &m_pTexDecl9 )))
		return false;

	//
	// MENU
	//

	// VB
	if( FAILED( dev.pd3dDevice->CreateVertexBuffer( sizeof(VertexMenu)*4, 0 /*Usage*/, 0, D3DPOOL_MANAGED, &m_pMenuVB9, NULL ) ) )
		return false;

	VertexMenu* v;
	if( FAILED( m_pMenuVB9->Lock( 0, sizeof(VertexMenu)*4, (void**)&v, 0 ) ) )
		return false;

	v[0].Size = D3DXVECTOR2(0,0);
	v[1].Size = D3DXVECTOR2(1,0);
	v[2].Size = D3DXVECTOR2(0,1);
	v[3].Size = D3DXVECTOR2(1,1);

	v[0].Pos = D3DXVECTOR2(0.01f,0.03f);
	v[1].Pos = D3DXVECTOR2(0.85f,0.03f);
	v[2].Pos = D3DXVECTOR2(0.01f,0.90f);
	v[3].Pos = D3DXVECTOR2(0.85f,0.90f);

	m_pMenuVB9->Unlock();


	//
	// TEX
	//

	// VB
	if( FAILED( dev.pd3dDevice->CreateVertexBuffer( sizeof(VertexTexture)*4, 0 /*Usage*/, 0, D3DPOOL_MANAGED, &m_pTexVB9, NULL ) ) )
		return false;

	VertexTexture* tv;
	if( FAILED( m_pTexVB9->Lock( 0, sizeof(VertexTexture)*4, (void**)&tv, 0 ) ) )
		return false;

	tv[0].Pos = D3DXVECTOR2(0,0);
	tv[1].Pos = D3DXVECTOR2(1,0);
	tv[2].Pos = D3DXVECTOR2(0,1);
	tv[3].Pos = D3DXVECTOR2(1,1);

	m_pTexVB9->Unlock();

	return true;
}


//////////////////////////////////////////////////////////////////////////
// render the texture using D3D9
void	CGameMenuContainer::RenderTexture9( CDX9Device&dev, IDirect3DTexture9*pTexture, float flAlpha, D3DXVECTOR2*pPos, D3DXVECTOR2*pSize )
{
	m_pMenuEffect9->SetFloat( m_pVarTexAlpha9, flAlpha );
	m_pMenuEffect9->SetVector( m_pVarTexPos9, &D3DXVECTOR4( pPos->x, pPos->y, 0,0 ) );
	m_pMenuEffect9->SetVector( m_pVarTexSize9, &D3DXVECTOR4( pSize->x, pSize->y, 0,0 ) );
	m_pMenuEffect9->SetTexture( m_pVarTexture9, pTexture );

	dev.pd3dDevice->SetVertexDeclaration(m_pTexDecl9);
	dev.pd3dDevice->SetStreamSource(0,m_pTexVB9,0,sizeof(VertexTexture));

	m_pMenuEffect9->SetTechnique( m_pTexTechnique9 );

	UINT cPasses;
	if (SUCCEEDED(m_pMenuEffect9->Begin( &cPasses, 0 ) ))
	{
		for( UINT p = 0; p < cPasses; ++p )
		{
			if (SUCCEEDED(m_pMenuEffect9->BeginPass( p ) ))
			{
				dev.pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

				m_pMenuEffect9->EndPass();
			}
		}

		m_pMenuEffect9->End();
	}
}
//////////////////////////////////////////////////////////////////////////
// render menu background using DX9
void	CGameMenuContainer::RenderMenuBackground9( CDX9Device&dev, D3DXVECTOR4*pColor, D3DXVECTOR2*pPosition )
{
	m_pMenuEffect9->SetVector( m_pVarMenuColor9, pColor );
	m_pMenuEffect9->SetVector( m_pVarMenuPosition9, &D3DXVECTOR4( pPosition->x, pPosition->y, 0,0 ) );
	dev.pd3dDevice->SetVertexDeclaration(m_pMenuDecl9);
	dev.pd3dDevice->SetStreamSource(0,m_pMenuVB9,0,sizeof(VertexMenu));

	m_pMenuEffect9->SetTechnique( m_pMenuTechnique9 );

	UINT cPasses;
	if (SUCCEEDED(m_pMenuEffect9->Begin( &cPasses, 0 ) ))
	{
		for( UINT p = 0; p < cPasses; ++p )
		{
			if (SUCCEEDED(m_pMenuEffect9->BeginPass( p ) ))
			{
				dev.pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

				m_pMenuEffect9->EndPass();
			}
		}

		m_pMenuEffect9->End();
	}

}
//////////////////////////////////////////////////////////////////////////
// menu screen ctor
CGameMenuScreen::CGameMenuScreen( D3DXVECTOR4 vecColor, CString&strTitle )
{
	m_vecColor = vecColor;
	Title = strTitle;

	m_iSelectedControl = 0;

	flCtrlHeight = 0.0f;
}


//////////////////////////////////////////////////////////////////////////
// menu screen dtor
CGameMenuScreen::~CGameMenuScreen()
{

}


//////////////////////////////////////////////////////////////////////////
// render menu screen
void	CGameMenuScreen::RenderScreen10( CDXDevice&dev, ID3D10ShaderResourceView*pBackground, CGameMenuContainer*pContainer, float flPosition )
{
	D3DXVECTOR2	vecPos(0.05f + flPosition, 0.48f);
	flCtrlHeight = (float)FONT_HEIGHT / (float)dev.pDX10->uHeight;
	const float flLineHeight = flCtrlHeight * 1.33f;

	// background
	pContainer->RenderMenuBackground10( *dev.pDX10, pBackground, &m_vecColor, &D3DXVECTOR2(flPosition,0), 2.0f );

	// title
	dev.RenderText( Title, D3DXCOLOR(0,0,0,0.75f), vecPos );
	vecPos.y += flLineHeight + 0.02f;

	// controls
	int iControl = 0;

	for (GameMenuControlList::iterator it = m_controls.GetIterator();!it.EndOfList();++it)
	{
		IGameMenuControl*pCtrl = it;

		if (pCtrl)
			pCtrl->RenderControl( dev, pContainer, vecPos, m_iSelectedControl == iControl  );

		// new line
		vecPos.y += flLineHeight;
		++iControl;
	}
}



//////////////////////////////////////////////////////////////////////////
// render menu screen using DX9
void	CGameMenuScreen::RenderScreen9( CDXDevice&dev, CGameMenuContainer*pContainer, float flPosition )
{
	D3DXVECTOR2	vecPos(0.05f + flPosition, 0.48f);
	flCtrlHeight = (float)FONT_HEIGHT / (float)dev.pDX9->uHeight;
	const float flLineHeight = flCtrlHeight * 1.33f;

	// background
	pContainer->RenderMenuBackground9( *dev.pDX9, &m_vecColor, &D3DXVECTOR2(flPosition,0) );
	
	// title
	dev.RenderText( Title, D3DXCOLOR(0,0,0,0.75f), vecPos );
	vecPos.y += flLineHeight + 0.02f;

	// controls
	int iControl = 0;

	for (GameMenuControlList::iterator it = m_controls.GetIterator();!it.EndOfList();++it)
	{
		IGameMenuControl*pCtrl = it;

		if (pCtrl)
			pCtrl->RenderControl( dev, pContainer, vecPos, m_iSelectedControl == iControl  );

		// new line
		vecPos.y += flLineHeight;
		++iControl;
	}
}
//////////////////////////////////////////////////////////////////////////
// animate controls
void	CGameMenuScreen::FrameMove( double dStep )
{
	// controls
	int iControl = 0;

	for (GameMenuControlList::iterator it = m_controls.GetIterator();!it.EndOfList();++it)
	{
		IGameMenuControl*pCtrl = it;

		if (pCtrl)
			pCtrl->FrameMove(dStep, m_iSelectedControl == iControl);

		++iControl;
	}
}


//////////////////////////////////////////////////////////////////////////
// controls
void	CGameMenuScreen::SetSelectedControlIndex(int index)
{
	if (m_controls.Count() > 0)
	{
		// normalize index [0;m_controls.Count()]
		while (index < 0)	index += m_controls.Count();
		while (index >= m_controls.Count())	index -= m_controls.Count();

		// set valid index
		m_iSelectedControl = index;
	}
	else
	{
		m_iSelectedControl = 0;
	}
}
void	CGameMenuScreen::OnUp( CGameMenuObserver*pObserver )
{
	SetSelectedControlIndex(m_iSelectedControl - 1);
}
void	CGameMenuScreen::OnDown( CGameMenuObserver*pObserver )
{
	SetSelectedControlIndex(m_iSelectedControl + 1);
}
void	CGameMenuScreen::OnLeft( CGameMenuObserver*pObserver )
{
	IGameMenuControl*pCtrl = m_controls[ m_iSelectedControl ];

	if (pCtrl)
		pCtrl->OnLeft( pObserver );
}
void	CGameMenuScreen::OnRight( CGameMenuObserver*pObserver )
{
	IGameMenuControl*pCtrl = m_controls[ m_iSelectedControl ];

	if (pCtrl)
		pCtrl->OnRight( pObserver );
}
void	CGameMenuScreen::OnEnter( CGameMenuObserver*pObserver )
{
	IGameMenuControl*pCtrl = m_controls[ m_iSelectedControl ];

	if (pCtrl && pCtrl->Enabled())
		pCtrl->OnEnter( pObserver );
}
void	CGameMenuScreen::OnClick( CGameMenuObserver*pObserver, float x, float y )
{
	OnMouseMove( pObserver, x,y );

	IGameMenuControl*pCtrl = m_controls[ m_iSelectedControl ];

	if (pCtrl && pCtrl->Enabled())
		pCtrl->OnEnter( pObserver );
}
void	CGameMenuScreen::OnMouseMove( CGameMenuObserver*pObserver, float x, float y )
{
	int iControl = 0;

	if (x >= 0.05f && x <= 0.5f)
	for (GameMenuControlList::iterator it = m_controls.GetIterator();!it.EndOfList();++it)
	{
		IGameMenuControl*pCtrl = it;

		if (pCtrl && pCtrl->Enabled())
		{
			if ( pCtrl->vecPos.y <= y && pCtrl->vecPos.y+flCtrlHeight>= y )
			{
				m_iSelectedControl = iControl;
				return;
			}
		}

		// new line
		
		++iControl;
	}
}

void	CGameMenuScreen::OnBackspace( CGameMenuObserver*pObserver )
{
	IGameMenuControl*pCtrl = m_controls[ m_iSelectedControl ];

	if (pCtrl && pCtrl->Enabled())
		pCtrl->OnBackspace( pObserver );
}
void	CGameMenuScreen::OnDelete( CGameMenuObserver*pObserver )
{
	IGameMenuControl*pCtrl = m_controls[ m_iSelectedControl ];

	if (pCtrl && pCtrl->Enabled())
		pCtrl->OnDelete( pObserver );
}
void	CGameMenuScreen::OnChar( CGameMenuObserver*pObserver, WCHAR wChar )
{
	IGameMenuControl*pCtrl = m_controls[ m_iSelectedControl ];

	if (pCtrl && pCtrl->Enabled())
		pCtrl->OnChar( pObserver, wChar );
}
void	CGameMenuScreen::OnHome( CGameMenuObserver*pObserver )
{
	IGameMenuControl*pCtrl = m_controls[ m_iSelectedControl ];

	if (pCtrl && pCtrl->Enabled())
		pCtrl->OnHome( pObserver );
}
void	CGameMenuScreen::OnEnd( CGameMenuObserver*pObserver )
{
	IGameMenuControl*pCtrl = m_controls[ m_iSelectedControl ];

	if (pCtrl && pCtrl->Enabled())
		pCtrl->OnEnd( pObserver );
}

//////////////////////////////////////////////////////////////////////////
// Windows messages
bool	IGameControlReceiver::OnWmChar( CGameMenuObserver*pObserver, WPARAM wParam, LPARAM lParam )
{
	switch( (WCHAR)wParam )
	{
		// Backspace
	case VK_BACK:
		{
			OnBackspace( pObserver );
			break;
		}

	case 24:        // Ctrl-X Cut
	case VK_CANCEL: // Ctrl-C Copy
		{
			// // // CopyToClipboard();

			// If the key is Ctrl-X, delete the selection too.
			if( (WCHAR)wParam == 24 )
			{
				// // // m_pCurrentScreen->OnDelete(m_pObserver);
			}

			break;
		}

		// Ctrl-V Paste
	case 22:
		{
			// // // PasteFromClipboard();
			break;
		}

		// Ctrl-A Select All
	case 1:
		
		break;

	case VK_RETURN:
		// Invoke the callback when the user presses Enter.
		OnEnter( pObserver );
		break;

		// Junk characters we don't want in the string
	case 26:  // Ctrl Z
	case 2:   // Ctrl B
	case 14:  // Ctrl N
	case 19:  // Ctrl S
	case 4:   // Ctrl D
	case 6:   // Ctrl F
	case 7:   // Ctrl G
	case 10:  // Ctrl J
	case 11:  // Ctrl K
	case 12:  // Ctrl L
	case 17:  // Ctrl Q
	case 23:  // Ctrl W
	case 5:   // Ctrl E
	case 18:  // Ctrl R
	case 20:  // Ctrl T
	case 25:  // Ctrl Y
	case 21:  // Ctrl U
	case 9:   // Ctrl I
	case 15:  // Ctrl O
	case 16:  // Ctrl P
	case 27:  // Ctrl [
	case 29:  // Ctrl ]
	case 28:  // Ctrl \ 
		break;

	default:
		{
			OnChar( pObserver, (WCHAR)wParam );
		}
	}

	return true;
}

bool	IGameControlReceiver::OnWmKeyDown( CGameMenuObserver*pObserver, WPARAM wParam, LPARAM lParam )
{
	switch( wParam )
	{
	case VK_TAB:
		// We don't process Tab in case keyboard input is enabled and the user
		// wishes to Tab to other controls.
		return true;

	case VK_HOME:
		OnHome( pObserver );
		return true;

	case VK_END:
		OnEnd( pObserver );
		return true;

	case VK_INSERT:
		if( GetKeyState( VK_CONTROL ) < 0 )
		{
			// Control Insert. Copy to clipboard
			// // //CopyToClipboard();
		} else
			if( GetKeyState( VK_SHIFT ) < 0 )
			{
				// Shift Insert. Paste from clipboard
				// // //PasteFromClipboard();
			} else
			{
				// Toggle caret insert mode
				// // // m_bInsertMode = !m_bInsertMode;
			}
		return true;

	case VK_DELETE:
		// Check if there is a text selection.
		OnDelete( pObserver );
		return true;

	case VK_LEFT:
		
		OnLeft( pObserver );

		/*if( GetKeyState( VK_SHIFT ) >= 0 )
			// Shift is not down. Update selection
			// start along with the caret.
			m_nSelStart = m_nCaret;*/
		return true;

	case VK_RIGHT:
		OnRight( pObserver );

		/*if( GetKeyState( VK_SHIFT ) >= 0 )
			// Shift is not down. Update selection
			// start along with the caret.
			m_nSelStart = m_nCaret;*/
		return true;

	case VK_UP:
		OnUp(pObserver);
		return true;
	case VK_DOWN:
		OnDown(pObserver);
		return true;

	default:
		//bHandled = wParam != VK_ESCAPE;  // Let the application handle Esc.
		break;
	}

	return false;
}