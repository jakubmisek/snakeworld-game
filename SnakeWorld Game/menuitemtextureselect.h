#ifndef _MENUITEMTEXTURESELECT_H_
#define _MENUITEMTEXTURESELECT_H_

#include "menuitems.h"
#include "textureloader.h"

//////////////////////////////////////////////////////////////////////////
// texture select control
class CMenuTextureSelect: public CMenuSelect
{
public:

	struct SMenuSelectTexture: public SMenuSelectItem
	{
		SMenuSelectTexture(CString&fname, int val)
			: SMenuSelectItem( fname, val )
		{
			
		}

		CTextureLoader	texture;
	};

	CMenuTextureSelect( int iCmd, CString&strLabel, CString&strTexturesPath );

	virtual void	RenderControl( CDXDevice&dev, CGameMenuContainer*pContainer, D3DXVECTOR2&position, bool bSelected );
	virtual	void	FrameMove( double dStep, bool bSelected );

protected:

	CString m_strTexturesPath;

	float	m_flSideItemsVisibility, m_flSelectedItem;

	void	TendTo( float&fl, float flto, double dStep );
};

#endif//_MENUITEMTEXTURESELECT_H_