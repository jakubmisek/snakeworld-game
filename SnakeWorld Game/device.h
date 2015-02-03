#ifndef _DEVICE_H_
#define _DEVICE_H_

//////////////////////////////////////////////////////////////////////////
// direct x header files and dynamic functions
#include "dx_dynamic.h"
#include "string.h"
#include "list.h"
#include "locker.h"

#define	FONT_HEIGHT	(18)

//////////////////////////////////////////////////////////////////////////
// texture loader base object
class CDeviceTextureLoader
{
public:
	
	CDeviceTextureLoader(CString&filename){texture_filename = filename;}

	virtual ~CDeviceTextureLoader(){}

	// texture file name (key)
	CString		texture_filename;
};

//////////////////////////////////////////////////////////////////////////
// DirectX 10 device object
// this is only container for direct3D 10 objects, so class members are public
class CDX10Device
{
public:
	CDX10Device();
	~CDX10Device();

	// initialize the device
	bool	CreateDevice( HWND hWnd, bool bFullScreen, unsigned int iWidth, unsigned int iHeight, unsigned int multisamplecount, float flFullScreenAspectRatio );

	// rendering
	bool	BeginRender();
	void	EndRender();
	void	Present();

	void	RenderText(wchar_t*str, D3DXCOLOR&color, D3DXVECTOR2&position, bool bCenter, bool bRight);

	// device objects
	ID3D10Device			*pd3dDevice;
	IDXGISwapChain			*pSwapChain;
	ID3D10RenderTargetView	*pRenderTargetView;
	ID3D10ShaderResourceView*pRenderTargetShaderView;

	ID3D10Texture2D			*pDepthStencil;
	ID3D10DepthStencilView	*pDepthStencilView;

	// effect
	ID3D10Effect	*pEffect;

	// effect variables
	ID3D10EffectMatrixVariable	*pVarMatWorld;		// world transform matrix
	ID3D10EffectMatrixVariable	*pVarMatViewProj;	// view x projection transform matrix
	ID3D10EffectVectorVariable	*pVarViewEye;		// view eye position (for ground sphere shape)
	ID3D10EffectVectorVariable	*pVarViewDir;		// view direction (for specular lighting)
	ID3D10EffectScalarVariable	*pVarFrameLength;	// last frame length in seconds

	ID3D10EffectShaderResourceVariable	*pVarDiffuseTexture;	// diffuse texture variable
	ID3D10EffectShaderResourceVariable	*pVarNormalTexture;		// normal map texture variable

	// swap chain description
	float					flFullScreenAspectRatio;

	// states
	bool					bWindowed;
	UINT					uWidth, uHeight;

	bool	ToggleFullscreen();	// windowed <-> full-screen mode
	bool	Resize( UINT uiWidth, UINT uiHeight, UINT uiSampleCount );

	//
	// DX10 texture loader
	//
	class TextureLoader:CDeviceTextureLoader
	{
	public:
		TextureLoader(CString&filename, CDX10Device*dev);

		// DX10
		ID3D10Resource				*pTexture10;
		ID3D10ShaderResourceView	*pTexture10View;
	};

protected:

	bool	InitEffects();	// create Effects, obtain techniques and variables
	
	// close the device
	void	CleanupDevice();

	// create multi-sampled render target
	HRESULT CreateRenderTarget( UINT uiWidth, UINT uiHeight, UINT uiSampleCount, UINT uiSampleQuality);

	ID3D10Texture2D*        m_pRTarget;
	ID3D10RenderTargetView* m_pRTRV;
	ID3D10Texture2D*        m_pDSTarget;
	ID3D10DepthStencilView* m_pDSRV;

	// used between BeginRender and EndRender
	ID3D10RenderTargetView* pOrigRT;
	ID3D10DepthStencilView* pOrigDS;

	// font
	ID3DX10Font		*m_pFont10;


	// 
	bool	m_bWindowOccluded;

};


//////////////////////////////////////////////////////////////////////////
// DirectX 9 device object
// this is only container for direct3D 9 objects, so class members are public
class CDX9Device
{
public:
	CDX9Device();
	~CDX9Device();

	// initialize the device
	bool	CreateDevice( HWND hWnd, bool bFullScreen, unsigned int iWidth, unsigned int iHeight, unsigned int multisamplecount, float flFullScreenAspectRatio );

	
	// close the device
	void	CleanupDevice();

	// rendering
	void	Present();

	void	RenderText(wchar_t*str, D3DXCOLOR&color, D3DXVECTOR2&position, bool bCenter, bool bRight);

	// DX9 objects
	IDirect3DDevice9	*pd3dDevice;
	ID3DXEffect			*pEffect;

	D3DXHANDLE			pVarWorldTransform, pVarViewProjTransform,
						pVarViewDirectionVector,
						pVarDiffuseTexture, pVarNormalTexture;

	// swap chain description
	float					flFullScreenAspectRatio;

	// states
	bool					bWindowed;
	UINT					uWidth, uHeight;

	//
	// DX10 texture loader
	//
	class TextureLoader:CDeviceTextureLoader
	{
	public:
		TextureLoader(CString&filename, CDX9Device*dev);

		// DX9
		IDirect3DTexture9			*pTexture9;
	};

protected:
	bool	InitEffects();	// create Effects, obtain techniques and variables

	bool	bDeviceIsLost;

	ID3DXFont		*m_pFont9;

	D3DPRESENT_PARAMETERS	pp;

};


//////////////////////////////////////////////////////////////////////////
// DirectX container for DX9 or DX10 objects
class CDXDevice
{
public:
	CDXDevice();
	~CDXDevice();

	//
	// DX objects
	//

	// DX9 object
	CDX9Device	*pDX9;

	// DX10 object
	CDX10Device	*pDX10;

	//
	// common methods
	//

	// initialize the device
	bool	CreateDevice( HWND hWnd, bool bFullScreen, unsigned int iWidth, unsigned int iHeight, unsigned int multisamplecount, float flFullScreenAspectRatio );

	// close the device
	void	CleanupDevice();

	// rendering
	void	Present();

	UINT ScreenWidth(){ return pDX10?( pDX10->uWidth ):( pDX9?(pDX9->uWidth):(0) ); }
	UINT ScreenHeight(){ return pDX10?( pDX10->uHeight ):( pDX9?(pDX9->uHeight):(0) ); }

	//
	// text writing
	//
	virtual void	RenderText( wchar_t*str, D3DXCOLOR&color, D3DXVECTOR2&position, bool bCenter = false, bool bRight = false );

	//
	// texture loading
	//
	CDeviceTextureLoader*	LoadTexture(CString&filename);


protected:

	// loaded device textures
	CList<CDeviceTextureLoader>	loaded_texture;

	ILocker	loaded_texture_locker;
};

#endif//_DEVICE_H_