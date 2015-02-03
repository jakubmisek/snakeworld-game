#include "device.h"
#include "errorlog.h"

#include <Windows.h>

//////////////////////////////////////////////////////////////////////////
// 3d device constructor
CDX10Device::CDX10Device()
{
	pEffect = NULL;

	pDepthStencil = NULL;

	pRenderTargetView = NULL;
	pDepthStencilView = NULL;

	pSwapChain = NULL;

	pd3dDevice = NULL;

	flFullScreenAspectRatio = 4.0f / 3.0f;

	uWidth = 640;
	uHeight = 480;
	bWindowed = true;

	m_bWindowOccluded = false;

	m_pFont10 = NULL;

	// MSAA
	m_pRTarget = NULL;
	m_pRTRV = NULL;
	m_pDSTarget = NULL;
	m_pDSRV = NULL;
}


//////////////////////////////////////////////////////////////////////////
// 3d device destructor
CDX10Device::~CDX10Device()
{
	CleanupDevice();
}


//////////////////////////////////////////////////////////////////////////
// create device
bool	CDX10Device::CreateDevice( HWND hWnd, bool bFullScreen, unsigned int iWidth, unsigned int iHeight, unsigned int multisamplecount, float flFullScreenAspectRatio )
{
	HRESULT hr;

	// check for DX10 support
	if (!D3D10_Init())
		return false;

	//
	// Device
	//

	// swap chain description
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof(sd) );
	sd.BufferCount = 1;
	sd.BufferDesc.Width = this->uWidth = iWidth;
	sd.BufferDesc.Height = this->uHeight = iHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Windowed = bWindowed = !bFullScreen;

	this->flFullScreenAspectRatio = flFullScreenAspectRatio;

	// create d3d10 device and swap chain
	if ( FAILED(D3D10_Dynamic_D3D10CreateDevice(NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL,
		0, NULL, D3D10_SDK_VERSION, &pd3dDevice )) )
		/*if( FAILED( D3D10CreateDeviceAndSwapChain(
		NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL,
		0, D3D10_SDK_VERSION, &sd,
		&pSwapChain, &pd3dDevice ) ) )*/
	{
		AddLog( L"Error: D3D10CreateDevice FAILED." );
		return false;
	}

	hr = D3D10_Dynamic_CreateSwapChain10( pd3dDevice, &sd, &pSwapChain );
	if( FAILED(hr) )
	{
		AddLog( L"Error: CreateSwapChain10 FAILED." );
		return false;
	}

	//
	// Render target
	//

	// Create a render target view
	ID3D10Texture2D *pBackBuffer;
	hr = pSwapChain->GetBuffer( 0, __uuidof( ID3D10Texture2D ), (LPVOID*)&pBackBuffer );
	if (FAILED(hr))
	{
		AddLog(L"pSwapChain->GetBuffer FAILED.");
		return false;
	}


	hr = pd3dDevice->CreateRenderTargetView( pBackBuffer, NULL, &pRenderTargetView );
	pBackBuffer->Release();
	if( FAILED(hr) )
	{
		AddLog(L"pd3dDevice->CreateRenderTargetView FAILED.");
		return false;
	}

	//
	// Create depth stencil texture
	//
	/*D3D10_TEXTURE2D_DESC descDepth;
	descDepth.Width = iWidth;
	descDepth.Height = iHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D10_USAGE_DEFAULT;
	descDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	hr = pd3dDevice->CreateTexture2D( &descDepth, NULL, &pDepthStencil );
	if( FAILED(hr) )
	return false;

	// Create the depth stencil view
	D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = pd3dDevice->CreateDepthStencilView( pDepthStencil, &descDSV, &pDepthStencilView );

	if( FAILED(hr) )
	return false;*/

	//
	// Set render target
	//
	pd3dDevice->OMSetRenderTargets( 1, &pRenderTargetView, pDepthStencilView );

	// Setup the view-port
	D3D10_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = iWidth;
	vp.Height = iHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	pd3dDevice->RSSetViewports( 1, &vp );

	//
	// Effects
	//
	if ( !InitEffects() )
	{
		AddLog(L"Error: HLSL effect initialization FAILED.");
		return false;
	}

	//
	// MSAA
	//
	if ( FAILED(CreateRenderTarget( iWidth, iHeight, multisamplecount, 0 )) )
	{
		AddLog(L"Error: unable to create render target.");
		return false;
	}

	//
	// FONT
	//
	if ( FAILED( D3DX10CreateFont( pd3dDevice, FONT_HEIGHT, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
		L"Arial", &m_pFont10 ) ) )
		return false;

	//
	// done
	//

	return true;
}


//////////////////////////////////////////////////////////////////////////
// create effects, obtain techniques and variables
bool	CDX10Device::InitEffects()
{
	//
	// Effects
	//

	// Effect flags
	DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3D10_SHADER_DEBUG;
#endif
	dwShaderFlags = 0;
	ID3D10Blob* pErrShader;

	// Create the effect
	HRESULT hr;
	if( FAILED( hr = D3DX10CreateEffectFromFile( L"fx\\effects10.fxo", NULL, NULL, NULL, dwShaderFlags, 0, pd3dDevice, NULL, NULL, &pEffect, &pErrShader, NULL ) ) )
	{
		if (pErrShader)
		{
			AddLog((char*)pErrShader->GetBufferPointer());
			pErrShader->Release();
		}
		
		return false ;
	}



	// Obtain the variables
	pVarMatViewProj = pEffect->GetVariableByName( "matViewProj" )->AsMatrix();
	pVarMatWorld = pEffect->GetVariableByName( "matWorld" )->AsMatrix();
	pVarViewDir = pEffect->GetVariableByName( "vecView" )->AsVector();
	pVarViewEye = pEffect->GetVariableByName( "vecViewEye" )->AsVector();

	pVarFrameLength = pEffect->GetVariableByName( "flFrameLength" )->AsScalar();

	pVarDiffuseTexture = pEffect->GetVariableByName( "texDiffuse" )->AsShaderResource();
	pVarNormalTexture = pEffect->GetVariableByName( "texNormal" )->AsShaderResource();

	// done
	return true;
}


//////////////////////////////////////////////////////////////////////////
// cleanup the device
void	CDX10Device::CleanupDevice()
{
	if( pd3dDevice )
	{
		pd3dDevice->ClearState();
	}

	SAFE_RELEASE(m_pFont10);

	// MSAA
	SAFE_RELEASE( m_pRTarget );
	SAFE_RELEASE( m_pRTRV );
	SAFE_RELEASE( m_pDSTarget );
	SAFE_RELEASE( m_pDSRV );

	//
	SAFE_RELEASE( pEffect );

	SAFE_RELEASE( pDepthStencilView );
	SAFE_RELEASE( pDepthStencil );

	SAFE_RELEASE( pRenderTargetView );

	//	SAFE_RELEASE( pSwapChain );

	SAFE_RELEASE( pd3dDevice );
}


//////////////////////////////////////////////////////////////////////////
// create the MSAA render target
HRESULT	CDX10Device::CreateRenderTarget( UINT uiWidth, UINT uiHeight, UINT uiSampleCount, UINT uiSampleQuality)
{
	HRESULT hr = S_OK;

	SAFE_RELEASE( m_pRTarget );
	SAFE_RELEASE( m_pRTRV );
	SAFE_RELEASE( m_pDSTarget );
	SAFE_RELEASE( m_pDSRV );

	ID3D10RenderTargetView* pOrigRT = NULL;
	ID3D10DepthStencilView* pOrigDS = NULL;
	pd3dDevice->OMGetRenderTargets( 1, &pOrigRT, &pOrigDS );

	D3D10_RENDER_TARGET_VIEW_DESC DescRTV;
	pOrigRT->GetDesc( &DescRTV );
	SAFE_RELEASE( pOrigRT );
	SAFE_RELEASE( pOrigDS );

	D3D10_TEXTURE2D_DESC dstex;
	dstex.Width = uiWidth;
	dstex.Height = uiHeight;
	dstex.MipLevels = 1;
	dstex.Format = DescRTV.Format;
	dstex.SampleDesc.Count = uiSampleCount;
	dstex.SampleDesc.Quality = uiSampleQuality;
	dstex.Usage = D3D10_USAGE_DEFAULT;
	dstex.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
	dstex.CPUAccessFlags = 0;
	dstex.MiscFlags = 0;
	dstex.ArraySize = 1;

	if (FAILED(hr = pd3dDevice->CreateTexture2D( &dstex, NULL, &m_pRTarget )))
		return hr;

	// Create the render target view
	D3D10_RENDER_TARGET_VIEW_DESC DescRT;
	DescRT.Format = dstex.Format;
	DescRT.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2DMS;
	if (FAILED(hr = pd3dDevice->CreateRenderTargetView( m_pRTarget, &DescRT, &m_pRTRV )))
		return hr;

	// Create render target shader resource view
	if (FAILED(hr = pd3dDevice->CreateShaderResourceView( m_pRTarget, NULL, &pRenderTargetShaderView )))
		return hr;

	// Create depth stencil texture.
	dstex.Width = uiWidth;
	dstex.Height = uiHeight;
	dstex.MipLevels = 1;
	dstex.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;	// DEPTH buffer (float32) with STENCIL (uint8)
	dstex.SampleDesc.Count = uiSampleCount;
	dstex.SampleDesc.Quality = uiSampleQuality;
	dstex.Usage = D3D10_USAGE_DEFAULT;
	dstex.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	dstex.CPUAccessFlags = 0;
	dstex.MiscFlags = 0;
	if (FAILED(hr = pd3dDevice->CreateTexture2D( &dstex, NULL, &m_pDSTarget )))
		return hr;

	// Create the depth stencil view
	D3D10_DEPTH_STENCIL_VIEW_DESC DescDS;
	DescDS.Format = dstex.Format;
	DescDS.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2DMS;
	if (FAILED(hr = pd3dDevice->CreateDepthStencilView( m_pDSTarget, &DescDS, &m_pDSRV )))
		return hr;


	// ok
	return hr;
}


//////////////////////////////////////////////////////////////////////////
// render begin
// clear back-buffers
// set the MSAA render target
bool	CDX10Device::BeginRender()
{
	if (m_bWindowOccluded)
		return false;	// do not render, jump to Present()

	pd3dDevice->OMGetRenderTargets( 1, &pOrigRT, &pOrigDS );

	pd3dDevice->OMSetRenderTargets( 1, &m_pRTRV, m_pDSRV );

	return true;
}


//////////////////////////////////////////////////////////////////////////
// end render
// copy the multi-sampled render target
// presents the back-buffer
void	CDX10Device::EndRender()
{
	// Copy it over because we can't resolve on present at the moment
	ID3D10Resource* pRT;
	pOrigRT->GetResource( &pRT );
	D3D10_RENDER_TARGET_VIEW_DESC rtDesc;
	pOrigRT->GetDesc( &rtDesc );
	pd3dDevice->ResolveSubresource( pRT, D3D10CalcSubresource( 0, 0, 1 ), m_pRTarget, D3D10CalcSubresource( 0, 0, 1 ), rtDesc.Format );
	SAFE_RELEASE( pRT );

	// Use our Old RT again
	ID3D10RenderTargetView* aRTViews[ 1 ] = { pOrigRT };
	pd3dDevice->OMSetRenderTargets( 1, aRTViews, pOrigDS );
	SAFE_RELEASE( pOrigRT );
	SAFE_RELEASE( pOrigDS );
}


//////////////////////////////////////////////////////////////////////////
// do present
void	CDX10Device::Present()
{
	//
	// swap the back-buffer to the window
	//
	HRESULT hr = pSwapChain->Present( 1, 0 );

	// 
	switch (hr)
	{
	case DXGI_STATUS_OCCLUDED:
		// occluded
		m_bWindowOccluded = true;
		break;
	case DXGI_ERROR_DEVICE_RESET:
		// TODO
		break;
	case DXGI_ERROR_DEVICE_REMOVED:
		// TODO
		break;
	default:
		if (SUCCEEDED(hr))
		{
			// not occluded
			m_bWindowOccluded = false;
		}
		break;
	}
}


//////////////////////////////////////////////////////////////////////////
// switch between full-screen and windowed mode
bool	CDX10Device::ToggleFullscreen()
{
	bWindowed = !bWindowed;
	if ( SUCCEEDED( pSwapChain->SetFullscreenState( !bWindowed, NULL ) ) )
	{
		//ShowCursor( bWindowed );

		return true;
	}
	else
	{
		bWindowed = !bWindowed;
		return false;
	}
}


//////////////////////////////////////////////////////////////////////////
// resize the render target
bool	CDX10Device::Resize( UINT uiWidth, UINT uiHeight, UINT uiSampleCount )
{
	HRESULT hr;

	this->uWidth = uiWidth;
	this->uHeight = uiHeight;

	if (FAILED(CreateRenderTarget( uiWidth, uiHeight, uiSampleCount, 0 )))
		return false;

	pRenderTargetView->Release();

	if (FAILED( pSwapChain->ResizeBuffers( 1, uiWidth, uiHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0 )))
		return false;

	// Create a render target view
	ID3D10Texture2D *pBackBuffer;
	hr = pSwapChain->GetBuffer( 0, __uuidof( ID3D10Texture2D ), (LPVOID*)&pBackBuffer );
	if (FAILED(hr))
		return false;


	hr = pd3dDevice->CreateRenderTargetView( pBackBuffer, NULL, &pRenderTargetView );
	pBackBuffer->Release();
	if( FAILED(hr) )
		return false;

	//
	// Set render target
	//
	pd3dDevice->OMSetRenderTargets( 1, &pRenderTargetView, pDepthStencilView );

	// Setup the view-port
	D3D10_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = uWidth;
	vp.Height = uHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	pd3dDevice->RSSetViewports( 1, &vp );

	return true;
}


//////////////////////////////////////////////////////////////////////////
// render string using DX10
void	CDX10Device::RenderText( wchar_t*str, D3DXCOLOR&color, D3DXVECTOR2&position, bool bCenter, bool bRight )
{
	if (m_pFont10)
	{
		// text area
		RECT rc;
		rc.left = (LONG)( position.x * (float)uWidth );
		rc.top = (LONG)( position.y * (float)uHeight );
		rc.right = (LONG)( uWidth );
		rc.bottom = (LONG)( uHeight );

		// flags
		UINT flags = 0;
		if (bCenter)
		{
			flags |= DT_CENTER;
			rc.left -= (rc.right - rc.left);
		}

		if (bRight)
		{
			flags = DT_RIGHT;
			rc.left =0;
			rc.top = (LONG)( position.y * (float)uHeight );
			rc.right = (LONG)( position.x * (float)uWidth);
			rc.bottom = (LONG)( uHeight );
		}

		// draw the text
		m_pFont10->DrawText( NULL, str, -1, &rc, flags, color );
	}
}



//////////////////////////////////////////////////////////////////////////
// texture loader
CDX10Device::TextureLoader::TextureLoader(CString&filename, CDX10Device*dev)
:CDeviceTextureLoader(filename)
{
	HRESULT hr = S_OK;
	if ( FAILED(hr=D3DX10CreateTextureFromFile( dev->pd3dDevice, filename.str(), NULL, NULL, &pTexture10, NULL )) )
		return;

	if ( FAILED(hr=dev->pd3dDevice->CreateShaderResourceView( pTexture10, NULL, &pTexture10View )) )
		return;

	//return hr;
}