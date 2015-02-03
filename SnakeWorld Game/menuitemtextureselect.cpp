#include "menuitemtextureselect.h"



//////////////////////////////////////////////////////////////////////////
//
CMenuTextureSelect::CMenuTextureSelect( int iCmd, CString&strLabel, CString&strTexturesPath )
	:CMenuSelect(iCmd,strLabel)
	,m_strTexturesPath(strTexturesPath)
{
	m_flSideItemsVisibility = 0.0f;
	m_flSelectedItem = 0.0f;
}
//////////////////////////////////////////////////////////////////////////
// render texture select control
void		CMenuTextureSelect::RenderControl( CDXDevice&dev, CGameMenuContainer*pContainer, D3DXVECTOR2&position, bool bSelected )
{
	IGameMenuControl::RenderControl(dev,pContainer,position,bSelected);


	// render textures
	float flMoveX = (float)iSelectedItemIndex - m_flSelectedItem;

	int iItem = 0;
	for ( CList<SMenuSelectItem>::iterator it = m_items.GetIterator();!it.EndOfList();++it )
	{
		SMenuSelectTexture *p = (SMenuSelectTexture*)it.value();

		if (p)
		{
			float flDif = (float)abs((double)iItem-(double)m_flSelectedItem);
			float flSize = 0.08f + (0.07f * m_flSideItemsVisibility);
			float flSize2 = flSize - flDif*0.02f;

			D3DXVECTOR2 pos = D3DXVECTOR2( position.x + 0.2f + ((float)(iItem-iSelectedItemIndex) + m_flSideItemsVisibility + flMoveX) * (flSize) + ( flSize - flSize2 )*0.5f, position.y + 0.01f - flSize2*0.5f );

			if ( flSize2 > 0.0f && pos.x + flSize2 >= 0.0f && pos.x <= 1.0f && pos.y <= 1.0f && pos.y + flSize2 >= 0.0f  )
			{
				if (!p->texture.IsLoaded())
				{
					p->texture.CreateTexture( dev, m_strTexturesPath + p->str, NULL );
				}
				else
				{
					pContainer->RenderTexture( 
						dev,
						p->texture,
						(iItem == iSelectedItemIndex)?
						(0.7f + 0.3f*m_flSideItemsVisibility):		// current item
					((0.7f - flDif*0.3f )*m_flSideItemsVisibility),	// side items
						&pos, &D3DXVECTOR2(flSize2,flSize2) );
				}				
			}
			


			//if ( iItem == iSelectedItemIndex )
			//	pContainer->RenderText(dev,p->str, D3DXCOLOR(1,1,1,m_flSideItemsVisibility*0.5f), D3DXVECTOR2(pos.x,pos.y+flSize2) );
		}

		++iItem;
	}
	
	// render label
	dev.RenderText(
		m_strLabel,
		Enabled()?
		(bSelected?D3DXCOLOR(1,1,1,0.75f):D3DXCOLOR(0.0f,0.0f,0.1f,0.75f)):
		(D3DXCOLOR(0.2f,0.2f,0.2f,0.75f)),
		position );
	
}


//////////////////////////////////////////////////////////////////////////
// animate control
void	CMenuTextureSelect::FrameMove( double dStep, bool bSelected )
{
	TendTo( m_flSideItemsVisibility, bSelected?1.0f:0.0f, dStep );
	TendTo( m_flSelectedItem, (float)iSelectedItemIndex, dStep );
}


//////////////////////////////////////////////////////////////////////////
// tend to
void	CMenuTextureSelect::TendTo( float&fl, float flto, double dStep )
{
	float d = flto - fl;

	if ( d != 0.0f )
	{
		double ad = abs(d);

		if ( ad > 0.2 )
			dStep *= ad*5.0;

		if ( ad < dStep )
			fl = flto;
		else
		{
			if ( d >= 0.0f )
				fl += (float)dStep;
			else
				fl -= (float)dStep;
		}
	}
}