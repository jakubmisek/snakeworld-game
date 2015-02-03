#pragma once
#include "global.h"

#include <vector>
#include <string>
#include <list>

#include "Texture.h"
#include "TargetValue.h"
#include "Sizes.h"
#include "Animation.h"

#define DECAL_MAX 3
enum SpriteAlignment
{
	AlignCenter = 0,
	AlignN = 1,
	AlignE = 2,
	AlignNE = 3,
	AlignS = 4,
	AlignSE = 6,
	AlignW = 8,
	AlignNW = 9,
	AlignSW = 12,
	AlignNESW = 0,
	AlignDefault = 16
};

class MemFile;

bool PolyHitTest(float testx, float testy, float* xarray, float* yarray, uint count);

class Sprite : public Timer, public Serialize
{
public:
	Sprite(TextureNumber tex);
	Sprite(TextureNumber tex, float width, float height);
	Sprite(TextureNumber tex, float x_pos, float y_pos, float z_pos, float width, float height);
	Sprite(Texture* attach_to);
	virtual void SetTimer(td_timer* use_timer);
	virtual bool SyncState(MemFile* mf);
protected:
	void Init(Texture* tex, float x, float y, float width, float height);
public:
	int m_serial;
	virtual ~Sprite(void);
	bool m_bDeleteWhenAnimDone;
	bool m_bDeleteWhenMoveDone;
	bool m_bDeleteWhenFadeDone;
	bool m_bRemoveAndDelete;

	bool m_bIntegerPixelAlign;

	virtual bool HitTest(TDPoint pt, int grow_x = 0, int grow_y = 0) { return HitTest(pt.x, pt.y, grow_x, grow_y); }
	virtual bool HitTest(int x, int y, int grow_x = 0, int grow_y = 0);

	uint SrcBlendMode;
	uint DstBlendMode;

public:
	virtual Animation* const GetAnim() { return singleanim; }
	TASize* const GetSize() { return &size; }
	TAPoint* const GetPos() { return &m_pos; }


	TAPoint m_pos;
	void SetCoord(float x, float y) { m_pos.setXY(x, y); }
	void SetCoord(float x, float y, float z) { m_pos.setXYZ(x, y, z); }
	void SetCoord(float x, float y, float z, float a) { m_pos.setXYZ(x, y, z); angle.setValue(a); }
	void SetCoordi(int x, int y) { m_pos.setXY((float)x, (float)y); }
	void SetCoordi(int x, int y, int z) { m_pos.setXYZ((float)x, (float)y, (float)z); }
	void SetCachedZ(bool IsStacked);
	float GetCachedZ() {return m_cached_z;}

	void AnimateTo(float x, float y, float a, uint ticks, TargetType type = Linear, TargetRepeat repeat = None)
	{
		m_pos.AnimateTo(x, y, ticks, type, repeat);
		angle.AnimateTo(a, ticks, type, repeat);
	}
	void Animate(float x, float y, float x_d, float y_d, uint ticks, TargetType type = Linear, TargetRepeat repeat = None)
	{
		m_pos.xa.Animate(x, x_d, ticks, type, repeat);
		m_pos.ya.Animate(y, y_d, ticks, type, repeat);
	}
	void Animate(float x, float y, float a, float x_d, float y_d, float a_d, uint ticks, TargetType type = Linear, TargetRepeat repeat = None)
	{
		m_pos.xa.Animate(x, x_d, ticks, type, repeat);
		m_pos.ya.Animate(y, y_d, ticks, type, repeat);
		angle.Animate(a, a_d, ticks, type, repeat);
	}

	TASize size;
	void SetSize(float w, float h) { size.setWH(w,h); CalcSize(); }
	virtual void CalcSize();

	TargetValueAngle angle;
//	void SetAngle(float new_angle) { angle.Value = new_angle; }
//	void SetAngle(float new_angle, float angle_speed);
//	void AnimateAngle(float new_angle, uint time_to_get_there, TargetType easetype = Linear);

	TAColor fade;
//	void SetFade(float fade_value) { fade.Value = fade_value; }
//	void SetFade(float fade_value, float fade_speed);
//	void AnimateFade(float fade_value, uint time_to_get_there, TargetType easetype = Linear);

	//float speed_x; // per second
	//float speed_y; // per second
	//float acc_x; // per second
	//float acc_y; // per second
	//void SetSpeed(float s_x, float s_y) { speed_x = s_x; speed_y = s_y; }
	//void SetSpeed(float s_x, float s_y, float a_x, float a_y) { SetSpeed(s_x, s_y); acc_x = a_x; acc_y = a_y; }

	SpriteAlignment alignment;
	void SetAlignment(SpriteAlignment align) { alignment = align; CalcSize(); }

	bool IsVisible() { return m_bVisible; }
	void Visible(bool isVisible) { m_bVisible = isVisible; }

	virtual void RenderAt(float x, float y, float z = -9999);
	virtual void Render();
	virtual void RenderStacked(float x, float y, float z);
	virtual void DrawVertexes();

	void SetBackground(Sprite* s) {m_background = s; } 
	bool AddDecal(Sprite* s);
	void RemoveDecal(Sprite* s);
	Sprite* Decal(int pos);
	int DecalCount();

protected:
	Animation* singleanim;

	float last_w, last_h;
	float m_cached_z;

	void LoadTexture(std::string filename);

	GLshort tex_x, tex_y, tex_w, tex_h;
	GLfloat spriteVertices[8];

	bool m_bVisible;

	Sprite* m_background;
	Sprite* m_decal_list[DECAL_MAX];

private:
	bool delete_texture;
};

bool OrderByZ(Sprite* s1, Sprite* s2);

class SpriteList : std::list<Sprite*>, public Timer
{
protected:
	bool sort_flag;
public:
	SpriteList();
	virtual ~SpriteList();
	virtual void SetTimer(td_timer* use_timer);
	void Add(Sprite* s);
	void RemoveAndDeleteDelayed(Sprite* s);
	void RemoveAndDeleteImmediate(Sprite* s);
	void RemoveAllAndDelete();
	void Sort();
	void Render();
	Sprite* GetAt(uint pos);
	bool Has(Sprite* s);

	bool IsYStacked() { return YStacked; }
	void SetYStacked(bool isstacked) { YStacked = isstacked; }

protected:
	bool YStacked;
};

