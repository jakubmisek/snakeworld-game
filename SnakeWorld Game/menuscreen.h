#ifndef _MENUSCREEN_H_
#define _MENUSCREEN_H_

#include "device.h"
#include "soundmanager.h"

#include "list.h"
#include "string.h"
#include "textureloader.h"


//////////////////////////////////////////////////////////////////////////
class CGameMenuScreen;
class CGameMenuObserver;
class IGameMenuControl;

typedef CList<IGameMenuControl> GameMenuControlList;

//////////////////////////////////////////////////////////////////////////
// menu screen DX objects container
class CGameMenuContainer
{
public:
	CGameMenuContainer();
	virtual ~CGameMenuContainer();

	// create the menu object
	virtual bool	CreateMenu( CDXDevice&dev, CSoundManager*pSndManager );

	// render the menu screen
	virtual void	RenderMenuBackground10( CDX10Device&dev, ID3D10ShaderResourceView*pBackground, D3DXVECTOR4*pColor, D3DXVECTOR2*pPosition, float flBlendfactor );
	virtual void	RenderMenuBackground9( CDX9Device&dev, D3DXVECTOR4*pColor, D3DXVECTOR2*pPosition );

	// render the texture for menu-item-texture-select
	virtual void	RenderTexture( CDXDevice&dev, CTextureLoader&tex, float flAlpha, D3DXVECTOR2*pPos, D3DXVECTOR2*pSize );
	
protected:

	//
	// audio
	//
	CSoundManager	*m_pSndManager;
	CAudioPath		m_audioPath;

	//
	// DX10
	//
	bool	CreateMenu10(CDX10Device&dev);
	void	RenderTexture10( CDX10Device&dev, ID3D10ShaderResourceView*pTexture, float flAlpha, D3DXVECTOR2*pPos, D3DXVECTOR2*pSize );

	// effect
	ID3D10Effect	*m_pMenuEffect;

	//
	// menu screen rendering
	//
	ID3D10EffectTechnique	*m_pMenuTechnique;			// menu render technique	

	ID3D10EffectVectorVariable	*m_pVarMenuPosition;		// menu position (float2)
	ID3D10EffectVectorVariable	*m_pVarBackbufferSize;		// back-buffer size in pixels (float2)
	ID3D10EffectScalarVariable	*m_pVarMenuBlendFactor;		// blend radius (float)
	ID3D10EffectVectorVariable	*m_pVarMenuColor;			// menu diffuse color (float4)
	ID3D10EffectShaderResourceVariable *m_pVarBackground;	// diffuse background texture variable

	// menu model
	ID3D10Buffer	*m_pVB;	// menu vertex buffer (1 quad)

	ID3D10InputLayout		*m_pMenuInputLayout;		// menu vertex layout

	//
	// menu item select texture
	//
	ID3D10EffectTechnique	*m_pTexTechnique;			// menu render technique	

	ID3D10EffectScalarVariable	*m_pVarTexAlpha;		// texture transparency
	ID3D10EffectVectorVariable	*m_pVarTexPos;			// texture position
	ID3D10EffectVectorVariable	*m_pVarTexSize;			// texture position
	ID3D10EffectShaderResourceVariable *m_pVarTexture;	// texture variable

	// menu model
	ID3D10Buffer	*m_pTexVB;	// texture rendering vertex buffer (1 quad)
	ID3D10InputLayout*m_pTexInputLayout;		// texture rendering vertex layout

	//
	// DX9
	//
	bool	CreateMenu9(CDX9Device&dev);
	void	RenderTexture9( CDX9Device&dev, IDirect3DTexture9*pTexture, float flAlpha, D3DXVECTOR2*pPos, D3DXVECTOR2*pSize );

	ID3DXEffect		*m_pMenuEffect9;
	D3DXHANDLE		m_pMenuTechnique9, m_pTexTechnique9,
		m_pVarTexture9, m_pVarTexPos9, m_pVarTexSize9, m_pVarTexAlpha9,
		m_pVarMenuColor9, m_pVarMenuPosition9;
	IDirect3DVertexDeclaration9	*m_pMenuDecl9, *m_pTexDecl9;

	IDirect3DVertexBuffer9	*m_pMenuVB9, *m_pTexVB9;
};


//////////////////////////////////////////////////////////////////////////
// menu screen command observer
class CGameMenuObserver
{
public:

	// handles press button event
	virtual void	OnCommandButton( IGameMenuControl*pControlFrom, int iCmd, int cmdParam = 0 ) = 0;

};



//////////////////////////////////////////////////////////////////////////
// input control receiver
class IGameControlReceiver
{
public:
	IGameControlReceiver(){}
	virtual ~IGameControlReceiver(){}

	//
	// events
	//
	virtual void	OnUp( CGameMenuObserver*pObserver ){}
	virtual void	OnDown( CGameMenuObserver*pObserver ){}
	virtual void	OnLeft( CGameMenuObserver*pObserver ){}
	virtual void	OnRight( CGameMenuObserver*pObserver ){}
	virtual void	OnEnter( CGameMenuObserver*pObserver ){}
	virtual void	OnClick( CGameMenuObserver*pObserver, float x, float y ){}
	virtual void	OnMouseMove( CGameMenuObserver*pObserver, float x, float y ){}
	
	virtual void	OnBackspace( CGameMenuObserver*pObserver ){}
	virtual void	OnDelete( CGameMenuObserver*pObserver ){}
	virtual void	OnChar( CGameMenuObserver*pObserver, WCHAR wChar ){}
	virtual void	OnHome( CGameMenuObserver*pObserver ){}
	virtual void	OnEnd( CGameMenuObserver*pObserver ){}

	//
	// Windows messages
	//
	virtual bool	OnWmChar( CGameMenuObserver*pObserver, WPARAM wParam, LPARAM lParam );
	virtual bool	OnWmKeyDown( CGameMenuObserver*pObserver, WPARAM wParam, LPARAM lParam ); 

};


//////////////////////////////////////////////////////////////////////////
// menu screen instance
class CGameMenuScreen: public IGameControlReceiver
{
public:
	CGameMenuScreen( D3DXVECTOR4 vecColor, CString&strTitle );
	virtual ~CGameMenuScreen();

	// render the menu screen
	virtual void RenderScreen10( CDXDevice&dev, ID3D10ShaderResourceView*pBackground, CGameMenuContainer*pContainer, float flPosition );
	virtual void RenderScreen9( CDXDevice&dev, CGameMenuContainer*pContainer, float flPosition );

	virtual void FrameMove( double dStep );

	// controls
	virtual void	OnUp( CGameMenuObserver*pObserver );
	virtual void	OnDown( CGameMenuObserver*pObserver );
	virtual void	OnLeft( CGameMenuObserver*pObserver );
	virtual void	OnRight( CGameMenuObserver*pObserver );
	virtual void	OnEnter( CGameMenuObserver*pObserver );
	virtual void	OnClick( CGameMenuObserver*pObserver, float x, float y );
	virtual void	OnMouseMove( CGameMenuObserver*pObserver, float x, float y );
	
	virtual void	OnBackspace( CGameMenuObserver*pObserver );
	virtual void	OnDelete( CGameMenuObserver*pObserver );
	virtual void	OnChar( CGameMenuObserver*pObserver, WCHAR wChar );
	virtual void	OnHome( CGameMenuObserver*pObserver );
	virtual void	OnEnd( CGameMenuObserver*pObserver );

	//
	GameMenuControlList&Controls(){ return m_controls; }
	inline int	GetSelectedControlIndex(){ return m_iSelectedControl; }
	void SetSelectedControlIndex(int index);

	// screen title
	CString		Title;

protected:

	// menu screen background color
	D3DXVECTOR4	m_vecColor;

	// controls
	GameMenuControlList	m_controls;
	int m_iSelectedControl;
	float flCtrlHeight;
};


//////////////////////////////////////////////////////////////////////////
// game menu control interface
class IGameMenuControl: public IGameControlReceiver
{
public:
	
	IGameMenuControl()
		:vecPos(-1,-1)
	{}

	virtual ~IGameMenuControl()
	{}

	// render control
	virtual void	RenderControl( CDXDevice&dev, CGameMenuContainer*pContainer, D3DXVECTOR2&position, bool bSelected )
	{
		vecPos = position;	// update last known position

		if (bSelected)
		{
			dev.RenderText(
				L"►",
				D3DXCOLOR(1,1,1,0.25f),
				D3DXVECTOR2(position.x - (float)FONT_HEIGHT/(float)dev.ScreenWidth() ,position.y) );
		}
	}

	// animate control
	virtual void	FrameMove( double dStep, bool bSelected ){}

	// last position of the control
	// used for mouse input
	D3DXVECTOR2	vecPos;

	// returns if control is enabled for input
	virtual bool	Enabled(){ return true; }

protected:

};


//////////////////////////////////////////////////////////////////////////
#endif//_MENUSCREEN_H_