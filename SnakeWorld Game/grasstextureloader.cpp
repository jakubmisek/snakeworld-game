#include "grasstextureloader.h"


CGrassTextureLoader::CGrassTextureLoader()
{
	pGrassTexture2D = NULL;
	pGrassTexture2DView = NULL;

	pd3dDevice = NULL;
}

CGrassTextureLoader::~CGrassTextureLoader()
{
	OnDestruct();

	SAFE_RELEASE( pGrassTexture2DView );
	SAFE_RELEASE( pGrassTexture2D );
}

void	CGrassTextureLoader::CreateTexture( CDX10Device&dev, LPCTSTR*ppFileNames, UINT nFileNames )
{
	this->pd3dDevice = dev.pd3dDevice;

	this->ppFileNames = ppFileNames;
	this->nFileNames = nFileNames;

	SAFE_RELEASE( pGrassTexture2DView );
	SAFE_RELEASE( pGrassTexture2D );

	StartThread(false);
}

DWORD CGrassTextureLoader::DoLoading()
{
	return LoadTextureArray( pd3dDevice, ppFileNames, nFileNames, &pGrassTexture2D, &pGrassTexture2DView);
}


//////////////////////////////////////////////////////////////////////////
// LoadTextureArray loads a texture array and associated view from a series
// of textures on disk.
HRESULT CGrassTextureLoader::LoadTextureArray( ID3D10Device* pd3dDevice, LPCTSTR* szTextureNames, int iNumTextures, ID3D10Texture2D** ppTex2D, ID3D10ShaderResourceView** ppSRV)
{
	HRESULT hr = S_OK;
	D3D10_TEXTURE2D_DESC desc;
	ZeroMemory( &desc, sizeof(D3D10_TEXTURE2D_DESC) );

	for(int i=0; i<iNumTextures; i++)
	{
		ID3D10Resource *pRes = NULL;
		D3DX10_IMAGE_LOAD_INFO loadInfo;
		ZeroMemory( &loadInfo, sizeof( D3DX10_IMAGE_LOAD_INFO ) );
		loadInfo.Width = D3DX10_DEFAULT;
		loadInfo.Height  = D3DX10_DEFAULT;
		loadInfo.Depth  = D3DX10_DEFAULT;
		loadInfo.FirstMipLevel = 0;
		loadInfo.MipLevels = 1;
		loadInfo.Usage = D3D10_USAGE_STAGING;
		loadInfo.BindFlags = 0;
		loadInfo.CpuAccessFlags = D3D10_CPU_ACCESS_WRITE | D3D10_CPU_ACCESS_READ;
		loadInfo.MiscFlags = 0;
		loadInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		loadInfo.Filter = D3DX10_FILTER_NONE;
		loadInfo.MipFilter = D3DX10_FILTER_NONE;

		if (FAILED( hr = D3DX10CreateTextureFromFile( pd3dDevice, szTextureNames[i], &loadInfo, NULL, &pRes, NULL ) ))
			return hr;
		if( pRes )
		{
			ID3D10Texture2D* pTemp;
			pRes->QueryInterface( __uuidof( ID3D10Texture2D ), (LPVOID*)&pTemp );
			pTemp->GetDesc( &desc );

			D3D10_MAPPED_TEXTURE2D mappedTex2D;
			if(DXGI_FORMAT_R8G8B8A8_UNORM != desc.Format)   //make sure we're R8G8B8A8
				return false;

			if(desc.MipLevels > 4)
				desc.MipLevels -= 4;
			if(!(*ppTex2D))
			{
				desc.Usage = D3D10_USAGE_DEFAULT;
				desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
				desc.CPUAccessFlags = 0;
				desc.ArraySize = iNumTextures;
				if (FAILED(hr=pd3dDevice->CreateTexture2D( &desc, NULL, ppTex2D)))
					return hr;
			}

			for(UINT iMip=0; iMip < desc.MipLevels; iMip++)
			{
				pTemp->Map( iMip, D3D10_MAP_READ, 0, &mappedTex2D );

				pd3dDevice->UpdateSubresource( (*ppTex2D), 
					D3D10CalcSubresource( iMip, i, desc.MipLevels ),
					NULL,
					mappedTex2D.pData,
					mappedTex2D.RowPitch,
					0 );

				pTemp->Unmap( iMip );
			}

			SAFE_RELEASE( pRes );
			SAFE_RELEASE( pTemp );
		}
		else
		{
			return false;
		}
	}

	D3D10_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory( &SRVDesc, sizeof(SRVDesc) );
	SRVDesc.Format = desc.Format;
	SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2DARRAY;
	SRVDesc.Texture2DArray.MipLevels = desc.MipLevels;
	SRVDesc.Texture2DArray.ArraySize = iNumTextures;
	if (FAILED(hr=pd3dDevice->CreateShaderResourceView( *ppTex2D, &SRVDesc, ppSRV )))
		return hr;

	// done
	return hr;
}

