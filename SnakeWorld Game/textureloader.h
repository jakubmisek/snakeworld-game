#ifndef _TEXTURELOADER_H_
#define _TEXTURELOADER_H_

#include "threadedloader.h"
#include "device.h"
#include "downloader.h"
#include "string.h"

class CTextureLoader
{
public:

	CTextureLoader();
	virtual ~CTextureLoader();

	inline bool	IsLoaded(){ return m_bLoaded; }

	void	CreateTexture( CDXDevice&dev, LPCWSTR szFileName, CDownloader*pDownloader );

	IDirect3DTexture9*	GetTexture9(){ return (texture_ptr?((CDX9Device::TextureLoader*)texture_ptr)->pTexture9:0); }

	ID3D10Resource*	GetTexture10(){ return (texture_ptr?((CDX10Device::TextureLoader*)texture_ptr)->pTexture10:0); }
	ID3D10ShaderResourceView* GetTexture10View(){ return (texture_ptr?((CDX10Device::TextureLoader*)texture_ptr)->pTexture10View:0); }

protected:

	bool m_bLoaded;

	CDeviceTextureLoader	*texture_ptr;	// always valid
};

#endif//_TEXTURELOADER_H_