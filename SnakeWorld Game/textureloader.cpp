#include "textureloader.h"

CTextureLoader::CTextureLoader()
{
	texture_ptr = 0;
	m_bLoaded = false;
}

CTextureLoader::~CTextureLoader()
{
	texture_ptr = 0;
}

void	CTextureLoader::CreateTexture( CDXDevice&dev, LPCWSTR szFileName, CDownloader*pDownloader )
{
	if (pDownloader)
		pDownloader->DownloadFile(szFileName);	// download the file if not exists yet

	// download the file if not exists
	texture_ptr = dev.LoadTexture(CString(szFileName));
	m_bLoaded = true;
}

/*DWORD CTextureLoader::DoLoading()
{
	

	// create the texture from the file
	if ( pd3dDevice10 )
	{
		HRESULT hr = S_OK;
		if ( FAILED(hr=D3DX10CreateTextureFromFile( pd3dDevice10, szFileName, NULL, NULL, &pTexture10, NULL )) )
			return hr;

		if ( FAILED(hr=pd3dDevice10->CreateShaderResourceView( pTexture10, NULL, &pTexture10View )) )
			return hr;

		return hr;
	}
	
	if (pd3dDevice9)
	{
		return D3DXCreateTextureFromFileEx(pd3dDevice9, szFileName,
			D3DX_DEFAULT,D3DX_DEFAULT,1,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_DEFAULT,D3DX_DEFAULT ,0, NULL,NULL,&pTexture9);
	}

	return E_FAIL;
}*/