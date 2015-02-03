#include "stdafx.h"
#include "Touches.h"
#include "TextSprite.h"
#include "Menu.h"
#include "MainObj.h"

extern MainObj* _mainObj;

Menu::Menu()
{
	m_pressed = -1;
	//for (int i = 0; i < MENU_ARRAY_SIZE; i++)
	//{
	//	m_spr[i] = NULL;
	//	m_anim_delay[i] = 0;
	//	m_direction[i] = -1;
	//}
	m_animtype = MENUANIM_JIGGLERIGHT;
	m_default_align = AlignW;
	m_orient_speed = 0;
	m_callback = NULL;
	AnimateOut(0, 0, 0);
	m_font = FontSize_sans_24s;
	m_font_color.a.setValue(1);
	m_font_color.r.setValue(1);
	m_font_color.g.setValue(1);
	m_font_color.b.setValue(1);
	m_bAnimatedIn = false;
}

Menu::~Menu()
{
	for (uint i = 0; i < m_entries.size(); i++)
	{
		delete m_entries[i];
	}
}

void Menu::Render()
{
	SetFullScreen();
	for (uint i = 0; i < m_entries.size(); i++)
	{
		if (m_entries[i]->m_spr != NULL)
		{
			if (m_entries[i]->m_anim_delay != 0 && m_entries[i]->m_anim_delay < last_timer())
			{
				m_entries[i]->m_anim_delay = 0;
				if (m_entries[i]->m_direction == -1)
				{
					HorizontalJiggleOut(i, m_entries[i]->m_speed, JiggleOut);
				}
				if (m_entries[i]->m_direction == 1)
				{
					HorizontalJiggleIn(i, m_entries[i]->m_speed, JiggleIn);
				}
			}
			if (m_entries[i]->m_anim_delay == 0)
			{
				if (m_entries[i]->m_direction == -1)
				{
					if (m_entries[i]->m_spr->GetPos()->xa.Ended() && m_entries[i]->m_spr->GetPos()->ya.Ended())
					{
						m_entries[i]->m_spr->Visible(false);
					}
				}
			}
			m_entries[i]->m_spr->GetPos()->setX(curx(i));
			//if (i == 0)
			//	Trace("%f\n", curx(i));
		}
	}

	m_sprites.Render();
}
int Menu::AddString( std::string val, int key, float size_ratio /*= 1.0f*/ )
{
	size_t c = m_entries.size();
	m_entries.resize(c+1);
	m_entries[c] = new entry();
	m_entries[c]->m_ratio = size_ratio;
	m_entries[c]->m_spr = TextHandler::CreateText(m_font);
	m_entries[c]->m_spr->SetSize(320.0f, m_entries[c]->m_spr->IdealHeight() * size_ratio);
	m_entries[c]->m_spr->SetString(val);
	m_entries[c]->m_spr->SetCoord(0, 0, SCREENZ - (c*50) - 50, -90);
	m_entries[c]->m_spr->SetAlignment(m_default_align);
	m_entries[c]->m_align = m_default_align;
	m_entries[c]->m_keys = key;
	m_entries[c]->m_offset.setXY(0, 0);
	m_entries[c]->m_vertmove = true;
	//m_entries[c]->xpos;
	if (c == 0)
	{
		m_entries[c]->m_anim_delay = last_timer();
	}
	else
	{
		m_entries[c]->m_anim_delay = m_entries[c - 1]->m_anim_delay + 500;
	}

	m_entries[c]->m_spr->fade.a = m_font_color.a;
	m_entries[c]->m_spr->fade.r = m_font_color.r;
	m_entries[c]->m_spr->fade.g = m_font_color.g;
	m_entries[c]->m_spr->fade.b = m_font_color.b;

	m_sprites.Add(m_entries[c]->m_spr);

	return (int)c;
}
int Menu::AddDualString( std::string val1, std::string val2, int key )
{
	int c1 = AddString(val1, key);
	int c2 = AddString(val2, key + 1);
	m_entries[c1]->m_vertmove = false;
	m_entries[c1]->m_align = AlignW;
	m_entries[c1]->m_spr->SetAlignment(AlignW);
	Sprite* attach = new Sprite(tex_misc_64x64, m_entries[c1]->m_spr->IdealWidth() + 10, m_entries[c1]->m_spr->GetSize()->getH() + 10);
	attach->SetAlignment(AlignW);
	attach->SetCoord(-5, 0);
	attach->GetAnim()->SetFrame(0, 3);
	m_entries[c1]->m_spr->SetBackground(attach);
	m_entries[c1]->m_spr->fade.SetColor(1, 1, 1, 1);

	m_entries[c2]->m_align = AlignE;
	m_entries[c2]->m_spr->SetAlignment(AlignE);
	attach = new Sprite(tex_misc_64x64, m_entries[c2]->m_spr->IdealWidth() + 10, m_entries[c2]->m_spr->GetSize()->getH() + 10);
	attach->SetAlignment(AlignE);
	attach->SetCoord(5, 0);
	attach->GetAnim()->SetFrame(0, 3);
	m_entries[c2]->m_spr->SetBackground(attach);
	m_entries[c2]->m_spr->fade.SetColor(1, 1, 1, 1);
	return c1;
}
int Menu::AddButtonString( std::string val1, int key )
{
	int c1 = AddString(val1, key);
	m_entries[c1]->m_vertmove = true;
	m_entries[c1]->m_align = AlignE;
	m_entries[c1]->m_spr->SetAlignment(AlignE);
	Sprite* attach = new Sprite(tex_misc_64x64, m_entries[c1]->m_spr->IdealWidth() + 10, m_entries[c1]->m_spr->GetSize()->getH() + 10);
	attach->SetAlignment(AlignE);
	attach->SetCoord(5, 0);
	attach->GetAnim()->SetFrame(0, 3);
	m_entries[c1]->m_spr->SetBackground(attach);
	m_entries[c1]->m_spr->fade.SetColor(1, 1, 1, 1);
	return c1;
}
void Menu::SetColor( int key_no, float red, float green, float blue, float alpha )
{
	for (uint i = 0; i < m_entries.size(); i++)
	{
		if (m_entries[i]->m_keys == key_no)
		{
			TextSprite* byebye = m_entries[i]->m_spr;
			byebye->fade.SetColor(red, green, blue, alpha);
		}
	}
}
void Menu::SetString( int key_no, std::string val)
{
	for (uint i = 0; i < m_entries.size(); i++)
	{
		if (m_entries[i]->m_keys == key_no)
		{
			TextSprite* byebye = m_entries[i]->m_spr;
			if (val == byebye->TextString()) return;
			TextSprite* hello = TextHandler::CreateText(m_font);
			hello->fade.SetColor(m_font_color.r.getValue(), m_font_color.g.getValue(), m_font_color.b.getValue(), m_font_color.a.getValue());
			hello->SetSize(byebye->GetSize()->getW(), byebye->GetSize()->getH());
			hello->fade.a.Animate(0, 1, 350, Linear, None);
			hello->SetCoord(byebye->GetPos()->getX(), byebye->GetPos()->getY(), byebye->GetPos()->getZ(), byebye->angle.getValue());
			hello->SetString(val);
			hello->SetAlignment(m_entries[i]->m_align);
			m_entries[i]->m_spr = hello;
			m_sprites.Add(hello);
			byebye->fade.a.AnimateTo(0, 500, Linear, None);
			byebye->m_bDeleteWhenFadeDone = true;
		}
	}
}

int Menu::ButtonPressed()
{
	return m_entries[m_pressed]->m_keys;
}

void Menu::TouchBegan( int finger )
{
}

void Menu::TouchDragging( int finger )
{
}

void Menu::TouchEnded( int finger )
{
	m_pressed = -1;
	if (finger != 0) return;
	for (uint i = 0; i < m_entries.size(); i++)
	{
		if (m_entries[i]->m_spr != NULL)
		{
			if (m_entries[i]->m_spr->HitTest(theTouches.TouchSpot(0)->end.x, theTouches.TouchSpot(0)->end.y))
			{
				m_pressed = i;
				if (m_callback != NULL)
				{
					m_callback(m_this_ptr, m_entries[m_pressed]->m_keys);
				}
			}
		}
	}
}

void Menu::AnimateIn( uint delay, uint pacing, uint speed )
{
	m_bAnimatedIn = true;
	for (uint i = 0; i < m_entries.size(); i++)
	{
		if (m_entries[i]->m_spr != NULL)
		{
			m_entries[i]->m_spr->Visible(true);
			m_entries[i]->m_anim_delay = last_timer() + delay;
			m_entries[i]->m_direction = 1;
			m_entries[i]->m_speed = speed;
			delay += pacing;
			HorizontalJiggleOut(i, 0, JiggleOut);
		}
	}
}

void Menu::AnimateOut( uint delay, uint pacing, uint speed )
{
	m_bAnimatedIn = false;
	for (uint i = 0; i < m_entries.size(); i++)
	{
		if (m_entries[i]->m_spr != NULL)
		{
			m_entries[i]->m_anim_delay = last_timer() + delay;
			m_entries[i]->m_direction = -1;
			m_entries[i]->m_speed = speed;
			delay += pacing;
			HorizontalJiggleIn(i, 0, JiggleIn);
		}
	}
}

void Menu::Visible( bool isVisible )
{
	for (uint i = 0; i < m_entries.size(); i++)
	{
		if (m_entries[i]->m_spr != NULL)
			m_entries[i]->m_spr->Visible(isVisible);
	}
}
void Menu::HorizontalJiggleOut( int spr_no, uint ticks, TargetType rep )
{
	float y;
	if (m_animtype == MENUANIM_JIGGLERIGHT)
	{
		y = 750;
		if (m_entries[spr_no]->m_align & AlignW) y = 960.0f;
		if (m_entries[spr_no]->m_align & AlignE) y = 500.0f;
		if (m_entries[spr_no]->m_align & AlignCenter) y = 750;
		m_entries[spr_no]->m_spr->GetPos()->ya.Animate(cury(spr_no), y, ticks, rep);
		m_entries[spr_no]->m_spr->GetPos()->setX(curx(spr_no));
	}
	if (m_animtype == MENUANIM_JIGGLELEFT)
	{
		y = -250;
		if (m_entries[spr_no]->m_align & AlignW) y = -10.0f;
		if (m_entries[spr_no]->m_align & AlignE) y = -480.0f;
		if (m_entries[spr_no]->m_align & AlignCenter) y = -250.0f;
		m_entries[spr_no]->m_spr->GetPos()->ya.Animate(cury(spr_no), y, ticks, rep);
		m_entries[spr_no]->m_spr->GetPos()->setX(curx(spr_no));
	}
	if (m_animtype == MENUANIM_INSTANT)
	{
		m_entries[spr_no]->m_spr->GetPos()->ya.setValue((m_entries[spr_no]->m_align & AlignW) ? -10.0f : -480.0f);
		m_entries[spr_no]->m_spr->GetPos()->setX(curx(spr_no));
	}
}
void Menu::HorizontalJiggleIn( int spr_no, uint ticks, TargetType rep )
{
	float y;
	float int_x = floor(curx(spr_no) + .5f);
	float int_y = floor(cury(spr_no) + .5f);

	if (m_animtype == MENUANIM_JIGGLERIGHT)
	{
		y = 750;
		if (m_entries[spr_no]->m_align & AlignW) y = 960.0f;
		if (m_entries[spr_no]->m_align & AlignE) y = 500.0f;
		m_entries[spr_no]->m_spr->GetPos()->ya.Animate(y, int_y, ticks, rep);
		m_entries[spr_no]->m_spr->GetPos()->setX(int_x);
	}
	if (m_animtype == MENUANIM_JIGGLELEFT)
	{
		y = -250;
		if (m_entries[spr_no]->m_align & AlignW) y = -10.0f;
		if (m_entries[spr_no]->m_align & AlignE) y = -480.0f;
		m_entries[spr_no]->m_spr->GetPos()->ya.Animate(y, int_y, ticks, rep);
		m_entries[spr_no]->m_spr->GetPos()->setX(int_x);
	}
	if (m_animtype == MENUANIM_INSTANT)
	{
		m_entries[spr_no]->m_spr->GetPos()->setY(int_y);
		m_entries[spr_no]->m_spr->GetPos()->setX(int_x);
	}
}

void Menu::SetCallback( void (*pushed_callback)(void*, int), void* this_ptr)
{
	m_callback = pushed_callback;
	m_this_ptr = this_ptr;
}

void Menu::Layout()
{
	m_content = m_area;

	//_mainObj->ShowDebugPoint(m_area.x, m_area.y);
	//_mainObj->ShowDebugPoint(m_area.x+m_area.w, m_area.y+m_area.h);

	float xx = m_content.x + m_content.w; // 400
	float cw = 0;
	m_total_height = 0;
	m_largest_width = 0;
	for (uint i = 0; i < m_entries.size(); i++)
	{
		cw = m_entries[i]->m_spr->GetSize()->getH();
		m_entries[i]->m_spr->SetSize(m_content.h, m_entries[i]->m_spr->IdealHeight() * m_entries[i]->m_ratio);
		m_entries[i]->m_rect.x = xx - cw;
		m_entries[i]->m_rect.y = m_area.y;
		m_entries[i]->m_rect.w = cw;
		m_entries[i]->m_rect.h = m_area.h;
		//		Trace("%f %f - %f %f\n", m_entries[i]->m_rect.x, m_entries[i]->m_rect.y, m_entries[i]->m_rect.w, m_entries[i]->m_rect.h);
		float iw = m_entries[i]->m_spr->IdealWidth();
		if (iw > m_largest_width) m_largest_width = iw;
		if (m_entries[i]->m_vertmove)
		{
			m_total_height += m_entries[i]->m_spr->GetSize()->getH();
			xx -= cw;
		}
		else
		{
			if (i < m_entries.size() - 1)
			{
				iw += m_entries[i + 1]->m_spr->IdealWidth() + 30;
				if (iw > m_largest_width) m_largest_width = iw;
			}
		}
	}

	m_swipe.SetArea(0, 0, 320, 480);
	if (m_total_height > m_area.w)
	{
		m_swipe.SetDirections(true, false);
		m_swipe.SetRange(0 - (int)(m_total_height - m_area.w) - 10 , 0, 0, 0);
	}
	else
	{
		m_swipe.SetDirections(false, false);
	}
}

void Menu::SetOffset( int key_no, float offset_x, float offset_y )
{
	for (uint i = 0; i < m_entries.size(); i++)
	{
		if (m_entries[i]->m_keys == key_no)
		{
			m_entries[i]->m_offset.setXY(offset_x, offset_y);
		}
	}

}

void Menu::Attach( int tag, Sprite* spr )
{
	spr->m_bIntegerPixelAlign = true;
	m_entries[tag]->m_spr->SetBackground(spr);
}

void Menu::Clear()
{
	for(uint i = 0; i < m_entries.size(); i++)
	{
		m_sprites.RemoveAndDeleteImmediate(m_entries[i]->m_spr);
	}
	m_entries.clear();
}

float Menu::IdealWidth()
{
	Layout();
	return m_largest_width;
}

float Menu::IdealHeight()
{
	Layout();
	return m_total_height;
}

void Menu::ScrollToIndex( int index )
{
	if (curx(index) > m_area.right())
	{
		int x = (int)curx(index);
		int r = (int)m_area.right();
		m_swipe.SetXVal(m_swipe.GetXVal() + (x-r));
	}	
	if (curx(index) < m_area.left())
	{
		int x = (int)curx(index);
		int r = (int)m_area.left();
		m_swipe.SetXVal(m_swipe.GetXVal() - (x-r));
	}	
}
