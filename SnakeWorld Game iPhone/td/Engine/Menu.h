#pragma once
#include <vector>
#include "Sizes.h"
#include "Swiper.h"
#include "EventHook.h"
#include "TextSprite.h"

class TextSprite;
class SpriteList;

typedef enum MenuAnimationTypeEnum
{
	MENUANIM_JIGGLERIGHT,
	MENUANIM_JIGGLELEFT,
	MENUANIM_INSTANT
} MenuAnimationType;

struct entry
{
	int m_keys;
	TARect m_rect;
	TAPoint m_offset;
	uint m_anim_delay;
	int m_direction;
	uint m_speed;
	TextSprite* m_spr;
	bool m_vertmove;
	SpriteAlignment m_align;
	float m_ratio;
};

class Menu : public EventHook, public Timer
{
public:
	Menu();
	virtual ~Menu();

	void SetArea(TARect area) { m_area = area; Layout(); }

	int AddString(std::string val, int key, float size_ratio = 1.0f);
	int AddDualString(std::string val1, std::string val2, int key);
	int AddButtonString(std::string val1, int key);
	void SetString(int key_no, std::string val);
	void SetColor(int key_no, float red, float green, float blue, float alpha);

	void Clear();

	int ButtonPressed();

	virtual void Render();
	virtual void TouchBegan(int finger);
	virtual void TouchDragging(int finger);
	virtual void TouchEnded(int finger);

	void AnimateIn(uint delay, uint pacing, uint speed);
	void AnimateOut(uint delay, uint pacing, uint speed);

	void Visible(bool isVisible);

	void SetCallback(void (*pushed_callback)(void*, int), void* this_ptr);

	void SetOffset(int key_no, float offset_x, float offset_y);
	void SetAlignment(SpriteAlignment alignment)  { m_default_align = alignment; }

	void Attach(int tag, Sprite* spr);

	void ScrollToIndex(int index);

	float IdealWidth();
	float IdealHeight();
	void Layout();

	FontSizeType m_font;
	TAColor m_font_color;

	MenuAnimationType m_animtype;

	bool IsIn() { return m_bAnimatedIn; }

protected:
	bool m_bAnimatedIn;

	SpriteAlignment m_default_align;
	std::vector<entry*> m_entries;
	TARect m_area;
	float m_total_height;
	float m_largest_width;
	TARect m_content;
	int m_pressed;
	uint m_start_ticks;
	int m_orient_speed;
	void HorizontalJiggleIn( int spr_no, uint ticks, TargetType rep );
	void HorizontalJiggleOut( int spr_no, uint ticks, TargetType rep );
	void (*m_callback)(void*, int);
	void* m_this_ptr;
	Swiper m_swipe;

	float curx(uint index) 
	{
		if (m_entries[index]->m_align & AlignN) return m_entries[index]->m_rect.right() + m_entries[index]->m_offset.getX() - m_swipe.GetXVal(); 
		if (m_entries[index]->m_align & AlignS) return m_entries[index]->m_rect.left() + m_entries[index]->m_offset.getX() - m_swipe.GetXVal(); 
		return m_entries[index]->m_rect.centerx() + m_entries[index]->m_offset.getX() - m_swipe.GetXVal(); 
	}
	float cury(uint index)
	{
		if (m_entries[index]->m_align & AlignE) return m_entries[index]->m_rect.bottom() + m_entries[index]->m_offset.getX(); 
		if (m_entries[index]->m_align & AlignW) return m_entries[index]->m_rect.top()  + m_entries[index]->m_offset.getX(); 
		return m_entries[index]->m_rect.centery() + m_entries[index]->m_offset.getY(); 
	}

	SpriteList m_sprites;
	int last_swipe_y;
};