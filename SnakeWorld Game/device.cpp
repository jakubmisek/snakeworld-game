#include "device.h"
#include "errorlog.h"

CDXDevice::CDXDevice()
{
	pDX9 = NULL;
	pDX10 = NULL;
}

CDXDevice::~CDXDevice()
{
	CleanupDevice();

	// clean dx9 dynamic objects
	D3D9_EnsureCleanup();

	// clean dx10 dynamic objects
	D3D10_EnsureCleanup();
}

// initialize the device
bool	CDXDevice::CreateDevice( HWND hWnd, bool bFullScreen, unsigned int iWidth, unsigned int iHeight, unsigned int multisamplecount, float flFullScreenAspectRatio )
{
	// try DX10
	if (D3D10_Init())
	{
		pDX10 = new CDX10Device();

		if ( pDX10->CreateDevice( hWnd, bFullScreen, iWidth, iHeight, multisamplecount, flFullScreenAspectRatio ) )
			return true;
		else
		{
			SAFE_DELETE(pDX10);
			AddLog(L"Info: DirectX10 device cannot be created.");
		}
	}

	// try DX9
	if (D3D9_Init())
	{
		pDX9 = new CDX9Device();

		if ( pDX9->CreateDevice( hWnd, bFullScreen, iWidth, iHeight, multisamplecount, flFullScreenAspectRatio ) )
			return true;
		else
		{
			SAFE_DELETE(pDX9);
			AddLog(L"Info: DirectX9 device cannot be created.");
		}
	}
	
	// else no device
	return false;
}

// close the device
void	CDXDevice::CleanupDevice()
{
	SAFE_DELETE(pDX9);
	SAFE_DELETE(pDX10);
}


void	CDXDevice::Present()
{
	if (pDX9)
		pDX9->Present();

	if (pDX10)
		pDX10->Present();
}


// render text using some existing device
void	CDXDevice::RenderText( wchar_t*str, D3DXCOLOR&color, D3DXVECTOR2&position, bool bCenter /*= false*/, bool bRight /*= false*/, SIZE*pOutSize )
{
	if (pDX10)
		pDX10->RenderText(str,color,position,bCenter,bRight,pOutSize);
	else if (pDX9)
		pDX9->RenderText(str,color,position,bCenter,bRight,pOutSize);
}


// texture loader
CDeviceTextureLoader*	CDXDevice::LoadTexture(CString&filename)
{
	CDeviceTextureLoader	*pReturn = 0;

	loaded_texture_locker.Lock();

	for (CList<CDeviceTextureLoader>::iterator it = loaded_texture.GetIterator();!it.EndOfList();++it)
	{
		CDeviceTextureLoader *ptex = it;

		if (ptex)
		{
			if (ptex->texture_filename == filename)
			{
				pReturn = ptex;
				break;
			}
		}
	}

	if ( !pReturn )
	{
		// texture not loaded yet
		// load the texture
		if (pDX10)
		{
			loaded_texture.Add( pReturn = (CDeviceTextureLoader*)new CDX10Device::TextureLoader(filename,pDX10) );
		}
		else if (pDX9)
		{
			loaded_texture.Add( pReturn = (CDeviceTextureLoader*)new CDX9Device::TextureLoader(filename,pDX9) );
		}
	}

	loaded_texture_locker.Unlock();

	// DONE
	return pReturn;
}