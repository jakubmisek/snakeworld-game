#pragma once

#include "Texture.h"

struct AnimationFrame
{
	float x;
	float y;
	float w;
	float h;
	uint delay;
};

typedef enum AnimRepeatEnum
{
	AnimRepeat_Loop,
	AnimRepeat_None,
	AnimRepeat_Bounce
} AnimRepeat;

class Animation : public Timer
{
protected:
	void Animation::CalculateFrame(AnimationFrame* f, uint grid_pos);
	void Animation::CalculateFrame(AnimationFrame* f, uint grid_x, uint grid_y);
	Texture* texture;
	GLfloat spriteTexcoords[8];
	uint frameno;
	uint ticks_into_frame;
	AnimRepeat m_RepeatType;

	uint last_rendered_ticks;
	bool m_bBackwards;
	bool m_bPaused;
public:
	//Animation();
	Animation(Texture* tex, td_timer* use_timer);
	virtual ~Animation();

	Texture* GetTexture() {return texture;}
	void SetTexture(Texture* tex) { texture = tex; }
	void CalcTexcoords();
	void Render();
	void SetFramePos(int f_no) { frameno = f_no; CalcTexcoords(); ticks_into_frame = 0; }

	void Continue() { m_bPaused = false; }
	void Pause() { m_bPaused = true; }

	bool AnimEnded;
	uint m_appearDelay;
	uint TotalAnimationTime();

	static const uint MasterDelay = 500;
	void AddFrame();	// single frame anim, sized to texture
	//void AddFrames(uint frames, uint delay = -1);	// film-strip based on texture size
	void AddFrame(float x, float y, float w, float h, uint delay = -1);	// absolute position
	void AddFrame(uint grid_pos, uint delay = -1);	// ordered position based on grid sizes
	void AddFrame(uint grid_x, uint grid_y, uint delay);	// x,y position based on grid sizes
	void SetFrame(uint frame_no, uint grid_pos, uint delay = -1);	// set a single frame anim, sized to texture
	void SetFrameDelay(uint frame_no, uint delay = -1);	// set a single frame anim, sized to texture
	void SetFrame(uint frame_no, float x, float y, float w, float h, uint delay = -1);	// set a single frame anim, sized to texture

	void SetRepeat(AnimRepeat repeat) { m_RepeatType = repeat; frameno = 0; AnimEnded = false; m_bBackwards = false;}

	void SetFrameCount(uint count);
	void FlipHorizontal();

protected:
	std::vector<AnimationFrame*> m_frames;
	virtual std::vector<AnimationFrame*>* GetFrames() { return &m_frames; };
};

class Sequence : public Animation
{

};