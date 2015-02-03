#ifndef _MYCHAT_H_
#define _MYCHAT_H_


//
#include "menuscreen.h"
#include "menuitemedit.h"	// edit box



//
// in game menu screen
//
class CInGameScreen: public CGameMenuScreen
{
public:
	CInGameScreen( int iChatMessageCommand );
	virtual ~CInGameScreen();

	// render the menu screen
	virtual void RenderScreen10( CDXDevice&dev, ID3D10ShaderResourceView*pBackground, CGameMenuContainer*pContainer, float flPosition );
	virtual void RenderScreen9( CDXDevice&dev, CGameMenuContainer*pContainer, float flPosition );

	// animate
	virtual void FrameMove( double dStep );

	// controls
	void	OnClick( CGameMenuObserver*pObserver, float x, float y );
	void	OnMouseMove( CGameMenuObserver*pObserver, float x, float y );

	bool	OnWmChar( CGameMenuObserver*pObserver, WPARAM wParam, LPARAM lParam );
	bool	OnWmKeyDown( CGameMenuObserver*pObserver, WPARAM wParam, LPARAM lParam );

	// chat
	void	AddMessage( CString&strFrom, CString&strMessage );

	void	SetVisibility( float flAlpha ){ flAlphaVisibility = flAlpha; }

protected:

	void	RenderScreen( CDXDevice&dev, CGameMenuContainer*pContainer, float flPosition );

	// command id for new message to send
	int m_iChatMessageCommand;

	// chat message edit box
	CMenuEditBox	*m_pChatMessage;

	double	m_dLastSendTime;

	float	flMessagesYAdd;

	float	flAlphaVisibility;
};


#endif//_MYCHAT_H_