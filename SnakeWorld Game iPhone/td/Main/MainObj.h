#pragma once
// This code will be executed by the OpenGL base app's render thread
#ifndef _WINDOWS
#include "stdafx.h"
#endif

#include "Sprite.h"
#include "EventHook.h"

//class Game;
//extern Game* theGame;

class SpriteList;
class StarSprite;

enum enumState
{
	STATE_game = 0,
	STATE_title,
	STATE_logo,
	STATE_score
};

struct callback_entry
{
	EventHook* this_ptr;
};

typedef enum GameSpeedEnum
{
	GameSpeed_Normal,
	GameSpeed_Fast,
	GameSpeed_Extreme,
	GameSpeed_Slow
} GameSpeedType;

class MainObj
{
public:
	MainObj(void);
	virtual ~MainObj(void);
	void Setup(int ScrX, int ScrY);
	void TearDown();

	void Render();
	void SetOrientation(int orient);
	void IncreaseZoom(float rate);
	void Scroll(float horz, float vert);
	
	void PauseGame();
	void UnpauseGame();
	void SetGameSpeed(GameSpeedType gs) { m_game_speed = gs; }
	GameSpeedType GameSpeed() { return m_game_speed; }
	void Freeze();
	void Thaw();

	void Shutdown();
	void Restore();
	bool restore_flag;
	bool TryRestore();

	void LowMemory();

	void ReadTimer();

	Texture* m_texLifeBar;
	Sprite* m_overlay_boss1;
	Sprite* m_overlay_boss2;
	Sprite* m_overlay_fast;

	void TouchBegan(int finger);
	void TouchDragging(int finger);
	void TouchEnded(int finger);

	bool IsPaused() { return m_bPaused || m_bFrozen; }
	bool IsFrozen() { return m_bFrozen; }

	void SetState(enumState new_state) { delayedState = new_state; }
	void SetState(enumState new_state, int next_level) { delayedState = new_state; m_next_level = next_level; }

	void AddHook(EventHook* this_ptr);
	void RemoveHook(EventHook* this_ptr);
	void CallHooks(event_callback_message msg, int param1 );

	uint Transition(int direction);
	StarSprite* m_star;
	Sprite* m_back;

	uint m_star_ticks_left;

#ifdef _DEBUG
	void ShowDebugPoint(float x, float y, float size=10.0f);
	void ShowDebugNumber(float x, float y, int num);
	void SetTextSize(int memory);
#endif

protected:

	Sprite* sprLogo;
	TextSprite* sprResuming;
	uint logodelay;

	GameSpeedType m_game_speed;

	std::list<callback_entry*> m_callbacks;

	enumState state;
	enumState delayedState;
	void SetDelayedState(enumState new_state);
	bool m_bPaused;
	bool m_bFrozen;
	uint last_real_timer;
	int m_next_level;

#ifdef _DEBUG
	Sprite* m_touchspot;
//	TextSprite* m_memory;
#endif
	SpriteList sprlistTransition;
};
