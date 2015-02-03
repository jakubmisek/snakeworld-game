#include "device.h"
#include "errorlog.h"

CDX9Device::CDX9Device()
{
	pd3dDevice = NULL;
	pEffect = NULL;
	bDeviceIsLost = false;
	m_pFont9 = NULL;
}

CDX9Device::~CDX9Device()
{
	CleanupDevice();
}


//////////////////////////////////////////////////////////////////////////
// cleanup the device objects
void	CDX9Device::CleanupDevice()
{
	SAFE_RELEASE(m_pFont9);

	SAFE_RELEASE(pEffect);
	SAFE_RELEASE(pd3dDevice);
}


//////////////////////////////////////////////////////////////////////////
// create DX9 render device
// create render techniques
bool	CDX9Device::CreateDevice( HWND hWnd, bool bFullScreen, unsigned int iWidth, unsigned int iHeight, unsigned int multisamplecount, float flFullScreenAspectRatio )
{
	IDirect3D9*pD3D = D3D9_GetDirect3D();

	if ( !pD3D )
		return false;

	HRESULT hr;

	this->flFullScreenAspectRatio = flFullScreenAspectRatio;
	this->uHeight = iHeight;
	this->uWidth = iWidth;
	this->bWindowed = !bFullScreen;

	bDeviceIsLost = false;

	if (bFullScreen)
	{	// make window topmost
		LONG_PTR oldstyle = GetWindowLongPtr( hWnd, GWL_EXSTYLE );
		SetWindowLongPtr( hWnd, GWL_EXSTYLE, oldstyle | WS_EX_TOPMOST );
		SetWindowLongPtr( hWnd, GWL_STYLE, WS_VISIBLE|WS_POPUP );

		SetWindowPos( hWnd, NULL, 0, 0, iWidth,iHeight, 0 );
	}

	// TODO enum present parameters

	//
	// Create D3D9 Device
	//

	pp.AutoDepthStencilFormat = D3DFMT_D16;
	pp.BackBufferCount = 1;
	pp.BackBufferFormat = D3DFMT_X8R8G8B8;
	pp.BackBufferHeight = iHeight;
	pp.BackBufferWidth = iWidth;
	pp.EnableAutoDepthStencil = TRUE;
	pp.Flags = 0;
	pp.FullScreen_RefreshRateInHz = bWindowed ? 0 : 60;
	pp.hDeviceWindow = hWnd;
	pp.MultiSampleQuality = 0;
	pp.MultiSampleType = D3DMULTISAMPLE_NONE;//(D3DMULTISAMPLE_TYPE)multisamplecount;
	pp.PresentationInterval = 0;
	pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	pp.Windowed = bWindowed;
	

	hr = pD3D->CreateDevice( 0, D3DDEVTYPE_HAL,hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &pp, &pd3dDevice );
	if( hr == D3DERR_DEVICELOST ) 
	{
		bDeviceIsLost = true;
	}
	else if( FAILED(hr) )
	{
		wchar_t buff[256];
		swprintf_s( buff, L"Error: CreateDevice FAILED. %s,D3DFMT_X8R8G8B8,D3DFMT_D16,%ux%u", bFullScreen?L"fullscreen":L"windowed",uWidth,iHeight );
		AddLog(buff);
		
		return false;
	}

	//
	// create render techniques
	//
	if ( !InitEffects() )
	{
		AddLog(L"Error: HLSL effects initialization FAILED. Pixel shaders and vertex shaders 2.0 are required.");
		return false;
	}

	//
	// FONT
	//
	if ( FAILED( D3DXCreateFont( pd3dDevice, FONT_HEIGHT, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
		L"Arial", &m_pFont9 ) ) )
		return false;

	// done
	return true;
}



//////////////////////////////////////////////////////////////////////////
// blt back buffer to the target window
// check for errors
void	CDX9Device::Present()
{
	HRESULT hr = pd3dDevice->Present(NULL,NULL,NULL,NULL);

	if( FAILED(hr) )
	{
		if( D3DERR_DEVICELOST == hr )
		{
			bDeviceIsLost = true;

			D3DPRESENT_PARAMETERS pp = this->pp;

			//pd3dDevice->Reset(&pp);

			// TODO reset
		}
		else if( D3DERR_DRIVERINTERNALERROR == hr )
		{
			// When D3DERR_DRIVERINTERNALERROR is returned from Present(),
			// the application can do one of the following:
			// 
			// - End, with the pop-up window saying that the application cannot continue 
			//   because of problems in the display adapter and that the user should 
			//   contact the adapter manufacturer.
			//
			// - Attempt to restart by calling IDirect3DDevice9::Reset, which is essentially the same 
			//   path as recovering from a lost device. If IDirect3DDevice9::Reset fails with 
			//   D3DERR_DRIVERINTERNALERROR, the application should end immediately with the message 
			//   that the user should contact the adapter manufacturer.
			// 
			// The framework attempts the path of resetting the device
			// 
			bDeviceIsLost = true;
		}
	}
}


//////////////////////////////////////////////////////////////////////////
// init effect, create techniques
bool	CDX9Device::InitEffects()
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
	if ( FAILED(D3DXCreateEffectFromFile( pd3dDevice, L"fx\\effects9.fxo", NULL, NULL, dwShaderFlags, NULL, &pEffect, &pErrOutBuff )) )
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
	pVarDiffuseTexture = pEffect->GetParameterByName(NULL,"g_texDiffuse");
	pVarNormalTexture = pEffect->GetParameterByName(NULL,"g_texNormal");
	pVarViewProjTransform = pEffect->GetParameterByName(NULL,"g_matViewProj");
	pVarWorldTransform = pEffect->GetParameterByName(NULL,"g_matWorld");
	pVarViewDirectionVector = pEffect->GetParameterByName(NULL,"g_vecViewDir");

	// done
	return true;
}


//////////////////////////////////////////////////////////////////////////
// render string using DX9
void	CDX9Device::RenderText(wchar_t*str, D3DXCOLOR&color, D3DXVECTOR2&position, bool bCenter, bool bRight, SIZE*pOutSize)
{
	if (m_pFont9)
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
		m_pFont9->DrawText( NULL, str, -1, &rc, flags, color );

		if (pOutSize)
		{
			HDC dc = m_pFont9->GetDC();
			int len = wcslen(str);
			GetTextExtentPoint32( dc, str, len, pOutSize );
		}
	}
}



//////////////////////////////////////////////////////////////////////////
// texture loader
CDX9Device::TextureLoader::TextureLoader(CString&filename, CDX9Device*dev)
:CDeviceTextureLoader(filename)
{
	//return 
	D3DXCreateTextureFromFileEx(dev->pd3dDevice, filename.str(),
		D3DX_DEFAULT,D3DX_DEFAULT,1,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_DEFAULT,D3DX_DEFAULT ,0, NULL,NULL,&pTexture9);
}