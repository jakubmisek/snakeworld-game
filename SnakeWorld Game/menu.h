#ifndef _MENU_H_
#define _MENU_H_

//
#include "device.h"
#include "menuscreen.h"
#include "menuitems.h"

// menu object
class CGameMenu: public CGameMenuContainer, public IGameControlReceiver
{
public:
	CGameMenu();
	virtual ~CGameMenu();

	// animate the menu
	void	FrameMove( double dStep );

	// render the menu
	void	RenderMenu10( CDXDevice&dev, ID3D10ShaderResourceView*pBackground );
	void	RenderMenu9( CDXDevice&dev );

	// set current menu screen
	void	SetCurrentScreen( CGameMenuScreen*pNewScreen, bool bAnimation = true );

	// controls
	void	OnClick( CGameMenuObserver*pObserver, float x, float y );
	void	OnMouseMove( CGameMenuObserver*pObserver, float x, float y );

	bool	OnWmChar( CGameMenuObserver*pObserver, WPARAM wParam, LPARAM lParam );
	bool	OnWmKeyDown( CGameMenuObserver*pObserver, WPARAM wParam, LPARAM lParam );

	// menu wants input
	bool	WantInput()
	{
		return m_pCurrentScreen != NULL;
	}

protected:	

	CGameMenuScreen	*m_pCurrentScreen, *m_pPrevScreen;	
	float	m_flFade;	// -1 - 0
	
};

#endif//_MENU_H_
