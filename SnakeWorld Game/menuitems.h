#ifndef _MENUITEMS_H_
#define _MENUITEMS_H_


#include "menuscreen.h"
#include "texts.h"

//////////////////////////////////////////////////////////////////////////
// menu controls

//
// label
//
class CMenuLabel: public IGameMenuControl
{
public:
	CMenuLabel( CString&strLabel, D3DXCOLOR&colSelected, D3DXCOLOR&colNotSelected, D3DXCOLOR&colDisabled )
		:m_strLabel(strLabel)
		,m_colSelected(colSelected)
		,m_colNotSelected(colNotSelected)
		,m_colDisabled(colDisabled)
	{
	}

	virtual void	RenderControl( CDXDevice&dev, CGameMenuContainer*pContainer, D3DXVECTOR2&position, bool bSelected )
	{
		IGameMenuControl::RenderControl(dev,pContainer,position,bSelected);

		dev.RenderText(
			m_strLabel,
			Enabled()?
			(bSelected?m_colSelected:m_colNotSelected):
			(m_colDisabled),
			position );
	}

protected:

	CString m_strLabel;

	D3DXCOLOR m_colSelected,m_colNotSelected,m_colDisabled;

};

//
// button
//
class CMenuButton: public CMenuLabel
{
public:
	CMenuButton( int iCmd, CString&strLabel )
		:CMenuLabel(strLabel,D3DXCOLOR(1,1,1,0.75f),D3DXCOLOR(0,0,0.1f,0.75f),D3DXCOLOR(0.2f,0.2f,0.2f,0.75f))
		,m_iCmd(iCmd)
	{
	}

	virtual void	OnEnter( CGameMenuObserver*pObserver )
	{
		pObserver->OnCommandButton( this, m_iCmd );
	}

protected:

	int m_iCmd;

};


//
// select
//
class CMenuSelect: public IGameMenuControl
{
public:

	struct SMenuSelectItem
	{
		CString str;
		int		val;

		SMenuSelectItem(CString&str, int val)
		{
			this->str = str;
			this->val = val;
		}
	};

	CMenuSelect( int iCmd, CString&strLabel )
		:m_iCmd(iCmd)
		,m_strLabel(strLabel)
	{
		pSelectedItem = NULL;
		iSelectedItemIndex = 0;
		bShowStar = false;
	}

	virtual void	RenderControl( CDXDevice&dev, CGameMenuContainer*pContainer, D3DXVECTOR2&position, bool bSelected )
	{
		vecPos = position;
		CString str = m_strLabel;

		if ( bShowStar )
		{
			str += L"* ";
		}

		if ( pSelectedItem )
		{
			str += L": ";
			str += pSelectedItem->str;
		}

		dev.RenderText(
			str,
			Enabled()?
			(bSelected?D3DXCOLOR(1,1,1,0.75f):D3DXCOLOR(0.0f,0.0f,0.1f,0.75f)):
			(D3DXCOLOR(0.2f,0.2f,0.2f,0.75f)),
			position );
	}

	virtual void	OnEnter( CGameMenuObserver*pObserver )
	{
		OnRight(pObserver);
	}
	virtual	void	OnLeft( CGameMenuObserver*pObserver )
	{
		SetSelectedItemIndex( pObserver,iSelectedItemIndex-1 );
	}
	virtual	void	OnRight( CGameMenuObserver*pObserver )
	{
		SetSelectedItemIndex( pObserver,iSelectedItemIndex+1 );
	}

	// selected items
	int				GetSelectedItemIndex(){ return iSelectedItemIndex; }
	SMenuSelectItem*GetSelectedItem(){ return pSelectedItem; }

	CList<SMenuSelectItem>&Items(){ return m_items; }

	void	SetSelectedItemIndex( CGameMenuObserver*pObserver, int iNewSelectedIndex )
	{
		if (m_items.Count() > 0)
		{
			iSelectedItemIndex = (iNewSelectedIndex + m_items.Count()) % m_items.Count();
		}
		else
		{
			iSelectedItemIndex = -1;
		}

		pSelectedItem = m_items[iSelectedItemIndex];

		//
		if (pObserver)
			pObserver->OnCommandButton( this, m_iCmd );
	}

	bool	bShowStar;

protected:

	int m_iCmd;
	CString m_strLabel;

	CList<SMenuSelectItem>	m_items;

	SMenuSelectItem			*pSelectedItem;
	int						iSelectedItemIndex;
};


//////////////////////////////////////////////////////////////////////////
// custom menu controls


//////////////////////////////////////////////////////////////////////////
// connect button
// contains IP address and port number
class	CMenuConnectButton: public CMenuButton
{
public:
	CMenuConnectButton( int iCmd, CString&strLabel, char*szAddress, int iPort  )
		: CMenuButton(iCmd,strLabel)
	{

		strcpy_s(m_szAddress,16,szAddress);
		m_iPort = iPort;
	}

	char m_szAddress[16];
	int m_iPort;
};


//////////////////////////////////////////////////////////////////////////
// goto button
// contains target screen pointer
class	CMenuGoToButton: public CMenuButton
{
public:
	CMenuGoToButton( int iCmd, CString&strLabel, CGameMenuScreen*pScreen )
		: CMenuButton(iCmd,strLabel)
	{
		m_pScreen = pScreen;
	}

	CGameMenuScreen*m_pScreen;
};



//
// CHECK BOX
//
class	CMenuCheckBox: public IGameMenuControl
{
public:
	CMenuCheckBox( int iCmd, CString&strLabel, bool bChecked )
		:m_iCmd(iCmd)
		,m_bChecked(bChecked)
	{
		// label
		m_strLabel = strLabel;
		m_strLabel += L": ";
	}

	virtual ~CMenuCheckBox()
	{
		
	}

	virtual void	RenderControl( CDXDevice&dev, CGameMenuContainer*pContainer, D3DXVECTOR2&position, bool bSelected )
	{
		IGameMenuControl::RenderControl(dev,pContainer,position,bSelected);

		CString str = m_strLabel;
		
		if (m_bChecked)
			str += g_gameTexts[tYes];
		else
			str += g_gameTexts[tNo];
	

		D3DXCOLOR color = Enabled()?(bSelected?D3DXCOLOR(1,1,1,0.75f):D3DXCOLOR(0.0f,0.0f,0.1f,0.75f)):
			(D3DXCOLOR(0.2f,0.2f,0.2f,0.75f));

		dev.RenderText(str,color,position );

	}

	virtual void	OnEnter( CGameMenuObserver*pObserver )
	{
		
		m_bChecked = !m_bChecked;

		if (pObserver)
			pObserver->OnCommandButton( this, m_iCmd );
	}
	virtual	void	OnLeft( CGameMenuObserver*pObserver )
	{
		OnEnter(pObserver);
	}
	virtual	void	OnRight( CGameMenuObserver*pObserver )
	{
		OnEnter(pObserver);
	}
	
	virtual void	OnDelete( CGameMenuObserver*pObserver )
	{
		if (m_bChecked)
		{
			m_bChecked = false;

			// callback
			if (pObserver)
				pObserver->OnCommandButton( this, m_iCmd );
		}
	}
	virtual void	OnChar( CGameMenuObserver*pObserver, WCHAR wChar )
	{
		if (wChar == L' ')
			OnEnter(pObserver);
	}

	bool IsChecked()
	{
		return m_bChecked;
	}

protected:

	CString	m_strLabel;
	int		m_iCmd;
	
	bool m_bChecked;
};

#endif//_MENUITEMS_H_