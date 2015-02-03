#ifndef _GRASSTEXTURELOADER_H_
#define _GRASSTEXTURELOADER_H_

#include "threadedloader.h"
#include "device.h"

class CGrassTextureLoader: public IThreadedLoader
{
public:

	CGrassTextureLoader();
	virtual ~CGrassTextureLoader();

	void	CreateTexture( CDX10Device&dev, LPCTSTR*ppFileNames, UINT nFileNames );

	ID3D10Resource*	GetTexture2D(){ return pGrassTexture2D; }
	ID3D10ShaderResourceView* GetTexture2DView(){ return pGrassTexture2DView; }

protected:

	virtual	DWORD DoLoading();

	HRESULT LoadTextureArray( ID3D10Device* pd3dDevice, LPCTSTR* szTextureNames, int iNumTextures, ID3D10Texture2D** ppTex2D, ID3D10ShaderResourceView** ppSRV);

	ID3D10Texture2D				*pGrassTexture2D;		// grass texture
	ID3D10ShaderResourceView	*pGrassTexture2DView;	// grass texture view

	ID3D10Device	*pd3dDevice;

	LPCTSTR*ppFileNames;
	UINT nFileNames;
};

#endif//_GRASSTEXTURELOADER_H_