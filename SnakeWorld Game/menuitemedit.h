#ifndef _MENUITEMEDIT_H_
#define _MENUITEMEDIT_H_

#include "menuscreen.h"

//
// EDIT BOX
//
class	CMenuEditBox: public IGameMenuControl
{
public:
	CMenuEditBox( int iCmd, CString&strLabel, CString*pStrText, int nMaxTextLength, bool bPasswordChars = false )
		:m_iCmd(iCmd)
		,m_bPasswordChars(bPasswordChars)
	{
		// label
		m_strLabel = strLabel;
		m_strLabel += L": ";

		// text
		m_nMaxTextLength = max( 0, nMaxTextLength );

		m_pText = new WCHAR[ nMaxTextLength + 1 ];

		if (pStrText)
		{
			// copy pStrText to m_pText
			int len = min( pStrText->length(), m_nMaxTextLength );
			if ( len < 0 ) len = 0;

			memcpy( m_pText, (wchar_t*)*pStrText, len * sizeof(wchar_t) );

			m_pText[len] = 0;
			m_iCursor = len;
		}
		else// zero m_pText
		{
			m_pText[0] = 0;
			m_iCursor = 0;
		}
	}

	virtual ~CMenuEditBox()
	{
		SAFE_DELETE_ARRAY( m_pText );
	}

	virtual void	RenderControl( CDXDevice&dev, CGameMenuContainer*pContainer, D3DXVECTOR2&position, bool bSelected )
	{
		IGameMenuControl::RenderControl(dev,pContainer,position,bSelected);

		CString str = m_strLabel;
		str += m_pText;

		if (m_bPasswordChars)
		{
			for (int i = m_strLabel.length(); i < str.length(); ++i)
				str.str()[i] = L'*';
		}

		if ( bSelected )
		{	// insert caret
			int iTo = m_strLabel.length() + m_iCursor;

			str.Insert( iTo, L'|' );
		}

		D3DXCOLOR color = Enabled()?(bSelected?D3DXCOLOR(1,1,1,0.75f):D3DXCOLOR(0.0f,0.0f,0.1f,0.75f)):
			(D3DXCOLOR(0.2f,0.2f,0.2f,0.75f));

		dev.RenderText(str,color,position );

		/*if (m_bTopLine)
		{
			D3DXCOLOR lineColor = D3DXCOLOR(1,1,1,0.35f);
			D3DXVECTOR2 linePosition = position;
			linePosition.y -= ((float)FONT_HEIGHT / (float)dev.ScreenHeight()) * 0.75f;

			dev.RenderText(L"...",lineColor,linePosition );
		}*/
	}

	virtual void	OnEnter( CGameMenuObserver*pObserver )
	{
		// callback
		if (pObserver)
			pObserver->OnCommandButton( this, m_iCmd );
	}
	virtual	void	OnLeft( CGameMenuObserver*pObserver )
	{
		// move cursor left
		if ( m_iCursor > 0 )
			-- m_iCursor;
	}
	virtual	void	OnRight( CGameMenuObserver*pObserver )
	{
		// move cursor right
		if ( m_pText[m_iCursor] != 0 )
			++ m_iCursor;
	}
	virtual	void	OnHome( CGameMenuObserver*pObserver )
	{
		m_iCursor = 0;
	}
	virtual	void	OnEnd( CGameMenuObserver*pObserver )
	{
		while ( m_pText[m_iCursor] != 0 )
			++ m_iCursor;
	}

	virtual void	OnBackspace( CGameMenuObserver*pObserver )
	{
		if (m_iCursor > 0)
		{
			OnLeft( pObserver );
			OnDelete( pObserver );
		}
	}
	virtual void	OnDelete( CGameMenuObserver*pObserver )
	{
		if ( m_pText[m_iCursor] != 0 )	// not on the end
		{
			for (int i = m_iCursor; m_pText[i] != 0; ++i)
				m_pText[i] = m_pText[i+1];

			// callback
			if (pObserver)
				pObserver->OnCommandButton( this, m_iCmd );
		}
	}
	virtual void	OnChar( CGameMenuObserver*pObserver, WCHAR wChar )
	{
		int i = m_iCursor;

		// find zero
		while (m_pText[i])
			++i;
		
		if ( i < m_nMaxTextLength )
		{
			// move characters right			
			while ( i >= m_iCursor )
			{
				m_pText[i+1] = m_pText[i];
				--i;
			}

			// insert the character
			m_pText[m_iCursor++] = wChar;

			// callback
			if (pObserver)
				pObserver->OnCommandButton( this, m_iCmd );
		}
		
	}

	// text
	inline WCHAR*	Text(){ return m_pText; }

	void	ResetText()
	{
		m_iCursor = 0;
		m_pText[0] = 0;
	}

protected:

	CString	m_strLabel;
	int		m_iCmd;
	bool	m_bPasswordChars;

	// edit box state
	WCHAR	*m_pText;
	int		m_nMaxTextLength;
	int		m_iCursor;
};


#endif//_MENUITEMEDIT_H_