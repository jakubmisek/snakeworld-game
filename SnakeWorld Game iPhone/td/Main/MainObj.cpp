#include "stdafx.h"

#include "EventHook.h"

#include "Sprite.h"
#include "MainObj.h"
#include "Touches.h"
#include "StarSprite.h"
#include "CrossSound.h"
#include "TextSprite.h"

#ifdef _WINDOWS
#include <iostream>
#include <io.h>
#include <fstream>
#endif

#ifndef _WINDOWS
#include "LoadTexture.h"
#endif

#ifdef _DEBUG
//Sprite* vertex1;
//Sprite* vertex2;
//Sprite* vertex3;
//Sprite* vertex4;
#endif

td_timer last_main_timer;
td_timer last_game_timer;

int screen_orientation;

void ReadTimer();

//Sprite* s;

//const GLfloat spriteVertices[] = {
//	-50.0f, -50.0f,
//	50.0f, -50.0f,
//	-50.0f,  50.0f,
//	50.0f,  50.0f,
//};
//
//// Sets up an array of values for the texture coordinates.
//const GLshort spriteTexcoords[] = {
//	0, 0,
//	1, 0,
//	0, 1,
//	1, 1,
//};

bool closing = false;

float absf(float val)
{
	if (val > 0) return val;
	return 0.0f - val;
}

bool fexist(std::string fname)
{
#ifdef _WINDOWS
	int feexist = _access(fname.data(), 0);
#else
	int feexist = access(fname.data(), 0);
#endif
	if (feexist == 0) return true;
	return false;
}

MainObj::MainObj(void)
{
	srand(GetTickCount() % RAND_MAX);
	m_bFrozen = false;
	m_bPaused = false;
	last_real_timer = GetTickCount();
	ReadTimer();
	ReadTimer();	// correctly set elapsed ticks
	SetState(STATE_logo);
	//TheSettings.LoadSettings();
	//TheSettings.SetActiveProfile(TheSettings.ActiveProfile());
	//TheProfile.SetProfileNo(TheSettings.ActiveProfile());
	//TheProfile.LoadProfile();
	//TheProfile.Increment(STAT_LAUNCHED);
	//m_texLifeBar = TextureHandler::OpenTexture(tex_lifebar);
	//m_overlay_boss1 = new Sprite(tex_mob_overlays, 16, 16);
	//m_overlay_boss1->GetAnim()->SetFrame(0, 0);
	//m_overlay_boss2 = new Sprite(tex_mob_overlays, 16, 16);
	//m_overlay_boss2->GetAnim()->SetFrame(0, 1);
	//m_overlay_fast  = new Sprite(tex_mob_overlays, 16, 16);
	//m_overlay_fast ->GetAnim()->SetFrame(0, 2);
	sprLogo = NULL;
	//theScore = NULL;
	restore_flag = false;
	m_star = NULL;
	m_back = NULL;

	//	Restore();
}

MainObj::~MainObj(void)
{
//	delete s;
	closing = true;
	//delete m_star;	sprlist will delete
	//delete theGame;
	//delete theTitle;
	//delete theScore;
	TextureHandler::CloseTexture(m_texLifeBar);
}

float angle = 0.0f;

void MainObj::Setup(int ScrX, int ScrY)
{
	glViewport(0, 0, ScrX, ScrY);
	//	glEnable(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(true);
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glAlphaFunc(GL_NEVER, 0.1f);
	glEnable(GL_BLEND);
	glClearColor(0, 0, 0, 0);

	//sprLogo = new Sprite(tex_logo, 512, 128);
	//sprLogo->GetAnim()->SetFrame(0, 0);
	//sprLogo->fade.a.Animate(0, 1, 250, Linear);
	//sprLogo->SetCoord(160, 240, FRONTZ-5, -90);
	//sprlistTransition.Add(sprLogo);

	Sprite* star = new Sprite(tex_misc_64x64, 128, 128);
	star->GetAnim()->SetFrame(0, 6);
	star->GetSize()->m_w.Animate(32, 128, 3000, EaseOutIn, Bounce);
	star->GetSize()->m_h.Animate(32, 128, 3000, EaseOutIn, Bounce);
	star->angle.Animate(0, 90, 500, Infinity);
	star->SetCoord(160, 240);
	sprlistTransition.Add(star);

	TextSprite* demo = TextHandler::CreateText(FontSize_sans_24s);
	demo->SetText("DEMO TEXT SPRITE");
	demo->SetWidth(650);
	demo->SetCoord(160,100);
	sprlistTransition.Add(demo);

	theSoundEngine.PlaySound("theft", 255);


	logodelay = last_main_timer.last_timer;
	//theSoundEngine.PlaySound("kiss", 255);
//	TDPlayGameSound("kiss");

	std::string fname = "state.bin";
#ifndef _WINDOWS
	const char* fuck = fname.data();
	fname = std::string(PathForFile(fuck));
#endif
	if (fexist(fname))
	{
		sprResuming = TextHandler::CreateText(FontSize_sans_24s);
		sprResuming->SetWidth(480);
		sprResuming->SetText("...Resuming Game...");
		sprResuming->SetCoord(30, 240, SCREENZ, -90);
		sprResuming->fade.a.Animate(1, 1, 1500);
		sprResuming->m_bDeleteWhenFadeDone = true;
		sprlistTransition.Add(sprResuming);
	}

	m_star = new StarSprite(tex_star);
	m_star->SetCoordi(160, 240);
	m_star->SetSize(32, 32);
	m_star->Visible(false);
	sprlistTransition.Add(m_star);
	m_star_ticks_left = last_main_timer.last_timer;

#ifdef _DEBUG
	m_touchspot= new Sprite(tex_misc_64x64);
	m_touchspot->GetAnim()->SetFrame(0, 2);
	sprlistTransition.Add(m_touchspot);

	//m_memory = TextHandler::CreateText(FontSize_sans_12o);
	//m_memory->SetText("memory");
	//m_memory->SetAlignment(AlignSE);
	//m_memory->SetCoord(0,0,FRONTZ-50, -90);
	//m_memory->SetWidth(160);
	//sprlistTransition.Add(m_memory);

	//vertex1 = new Sprite(tex_misc_64x64,64, 64);
	//vertex1->GetAnim()->SetFrame(0, 2);
	//vertex1->SetSize(10,10);
	//sprlistTransition.Add(vertex1);

	//vertex2 = new Sprite(tex_misc_64x64,64, 64);
	//vertex2->GetAnim()->SetFrame(0, 2);
	//vertex2->SetSize(10,10);
	//sprlistTransition.Add(vertex2);

	//vertex3 = new Sprite(tex_misc_64x64,64, 64);
	//vertex3->GetAnim()->SetFrame(0, 2);
	//vertex3->SetSize(10,10);
	//sprlistTransition.Add(vertex3);

	//vertex4 = new Sprite(tex_misc_64x64,64, 64);
	//vertex4->GetAnim()->SetFrame(0, 2);
	//vertex4->SetSize(10,10);
	//sprlistTransition.Add(vertex4);
#endif

	//	theGame->();
	//	theTitle->Start();
}

void MainObj::TearDown()
{
	//TheSettings.SaveSettings();
	//TheProfile.SaveProfile();
	TextureHandler::CloseAllTextures();
}

void MainObj::ReadTimer()
{
	uint ticks = GetTickCount();

	last_main_timer.elapsed_ticks = (uint)ticks - (uint)last_real_timer;
	last_game_timer.elapsed_ticks = (uint)ticks - (uint)last_real_timer;

	last_real_timer = ticks;

	if (last_main_timer.elapsed_ticks > 100)	last_main_timer.elapsed_ticks = 100;// clamp render rate to .5 seconds
	if (last_game_timer.elapsed_ticks > 100)	last_game_timer.elapsed_ticks = 100;// clamp render rate to .5 seconds

	if (m_bFrozen) last_main_timer.elapsed_ticks = 0;
	if (m_bPaused || m_bFrozen) last_game_timer.elapsed_ticks = 0;
	if (m_game_speed == GameSpeed_Extreme) last_game_timer.elapsed_ticks *= 3;
	if (m_game_speed == GameSpeed_Fast) last_game_timer.elapsed_ticks *= 2;
	if (m_game_speed == GameSpeed_Slow) last_game_timer.elapsed_ticks /= 2;

	last_main_timer.last_timer += last_main_timer.elapsed_ticks;
	last_game_timer.last_timer += last_game_timer.elapsed_ticks;
}

void MainObj::SetOrientation(int orient)
{
	if (screen_orientation == orient) return;
	screen_orientation = orient;
}



void MainObj::Render(void)
{
	ReadTimer();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	SetFullScreen();

	CallHooks(EVENT_Render, 0);

	if (state == STATE_logo)
	{
		if (sprLogo != NULL)
		{
			if (last_main_timer.last_timer >= logodelay + 1000)
			{
				sprLogo->fade.a.Animate(1, 0, 500, Linear);
				sprLogo->m_bDeleteWhenFadeDone = true;
			}
			if (last_main_timer.last_timer >= logodelay + 1500)
			{
				if (restore_flag)
					if (!TryRestore())
					{
						SetDelayedState(STATE_title);
					}
			}
		}
	}

	//switch (state)
	//{
	//case STATE_game:
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//	if (theGame != NULL) theGame->Render();
	//	break;
	//case STATE_title:
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//	if (theTitle != NULL) theTitle->Render();
	//	break;
	//}

//	TDAlert("Welcome!", "Fuck off", "Get Lost");

	m_star->Visible(m_star_ticks_left > last_main_timer.last_timer);

	sprlistTransition.Render();

	if (state != delayedState)
	{
		SetDelayedState(delayedState);
	}
}

void MainObj::TouchBegan(int finger)
{
#ifdef _DEBUG
	m_touchspot->SetCoordi(theTouches.TouchSpot(0)->start.x, theTouches.TouchSpot(0)->start.y);
	m_touchspot->size.m_h.Animate(128, 10, 500);
	m_touchspot->size.m_w.Animate(128, 10, 500);
	m_touchspot->fade.a.Animate(1, 0, 500, EaseOut);
	
#endif

	CallHooks(EVENT_TouchBegan, finger);

	//switch (state)
	//{
	//case STATE_game:
	//	theGame->TouchBegan(finger);
	//	break;
	//case STATE_title:
	//	theTitle->TouchBegan(finger);
	//	break;
	//}
	theSoundEngine.PlaySound("tock", 255);
}

void MainObj::TouchDragging(int finger)
{
#ifdef _DEBUG
	m_touchspot->SetCoordi(theTouches.TouchSpot(0)->current.x, theTouches.TouchSpot(0)->current.y);
#endif
	CallHooks(EVENT_TouchDragging, finger);

	for (int i = 0; i < 9; i++)
	{
		if (theTouches.IsActive(i))
		{
			Sprite* sparkle = new Sprite(tex_misc_64x64, 64, 64);
			sparkle->GetAnim()->SetFrame(0, 6);
			sparkle->fade.a.Animate(1, 0, 3000, EaseOut);
			sparkle->SetCoord(theTouches.TouchSpot(i)->current.x, theTouches.TouchSpot(i)->current.y);
			sparkle->m_bDeleteWhenFadeDone = true;
			sprlistTransition.Add(sparkle);
		}
	}

	//switch (state)
	//{
	//case STATE_game:
	//	theGame->TouchDragging(finger);
	//	break;
	//case STATE_title:
	//	theTitle->TouchDragging(finger);
	//	break;
	//}
}

void MainObj::TouchEnded(int finger)
{
#ifdef _DEBUG
	m_touchspot->SetCoordi(theTouches.TouchSpot(0)->end.x, theTouches.TouchSpot(0)->end.y);
#endif	Trace("Ended: %d,%d\n", theTouches.TouchSpot(0)->end.x, theTouches.TouchSpot(0)->end.y);

	CallHooks(EVENT_TouchEnded, finger);

	theSoundEngine.PlaySound("tock", 255);

//	m_touchspot->size.m_h.Animate(1, 64, 500);
//	m_touchspot->size.m_w.Animate(1, 64, 500);
	//switch (state)
	//{
	//case STATE_game:
	//	theGame->TouchEnded(finger);
	//	break;
	//case STATE_title:
	//	theTitle->TouchEnded(finger);
	//	break;
	//}
}

void MainObj::Freeze()
{
	m_bFrozen = true;
	m_bPaused = true;
	//TheProfile.Increment(STAT_FROZEN);
}

void MainObj::Thaw()
{
//	if (m_bFrozen) TDPlayGameSound("kiss");
	m_bFrozen = false;
	//if (theGame != NULL)
	//{
	//	//theGame->PauseGame(false);
	//}
	//else
		m_bPaused = false;
}
void MainObj::PauseGame()
{
	m_bPaused = true;
	//TheProfile.Increment(STAT_PAUSED);
}

void MainObj::UnpauseGame()
{
	m_bPaused = false;
}

void MainObj::Shutdown()
{
//	theProfile->Save();
	switch (state)
	{
	case STATE_game:
		{
			MemFile* mf = new MemFile();
			std::string fname = "state.bin";
#ifndef _WINDOWS
			const char* fuck = fname.data();
			fname = std::string(PathForFile(fuck));
#endif
			mf->StartSave(fname.data());
			//theGame->SyncState(mf);
			delete mf;
		}
		break;
	case STATE_title:
		//theTitle->SaveState();
		break;
	}
}

void MainObj::Restore()
{
	restore_flag = true;
}

bool MainObj::TryRestore()
{
	bool ASSret = false;
	std::string fname = "state.bin";
#ifndef _WINDOWS
	const char* fuck = fname.data();
	fname = std::string(PathForFile(fuck));
#endif
	if (fexist(fname))
	{
		sprLogo->fade.a.Animate(1, 0, 2500, Linear);
		sprLogo->m_bDeleteWhenFadeDone = true;

		MemFile* mf = new MemFile();
		bool ret = mf->StartLoad(fname.data());
		if (ret)
		{
			//if (theGame != NULL) { delete theGame; theGame = NULL; }
			//theGame = new Game();
			//theGame->Setup();
			//bool result = theGame->SyncState(mf);
			//if (result)
			//{
			//	state = STATE_game;
			//	delayedState = STATE_game;
			//	theGame->PauseGame(false);
			//	ASSret = true;
			//}
			//else
			//{
			//	delete theGame;
			//	theGame = NULL;
			//}
			delete mf;
			remove(fname.data());
		}
		else
		{
			delete mf;
		}
		//		_remove(fname.data();
	}
	//	theProfile->Load();
	return ASSret;
}

void MainObj::SetDelayedState( enumState new_state )
{
	if (state != new_state)
	{
		//if (theGame != NULL)
		//{
		//	delete theGame;
		//	theGame = NULL;
		//}
		//if (theTitle != NULL)
		//{
		//	delete theTitle;
		//	theTitle = NULL;
		//}
		//if (theScore != NULL)
		//{
		//	delete theScore;
		//	theScore = NULL;
		//}
	}
	state = new_state;
	delayedState = state;
	TextureHandler::FlushTextures(true);
	//if (state == STATE_game)
	//{
	//	theGame = new Game();
	//	theGame->Setup();
	//	theGame->LoadLevel(m_next_level);
	//}
	//if (state == STATE_title)
	//{
	//	theTitle = new Title();
	//	theTitle->Start();
	//}
	//if (state == STATE_score)
	//{
	//	theScore = new Score();
	//}
}

uint MainObj::Transition( int direction )
{
	uint ret = 0;
//	int i, f = 0;

	//Sprite* a = new Sprite(tex_transition_burn, 64, 64);
	//StarSprite* a = new StarSprite(tex_star);

	//a->SetCoordi(160, 240);
	//a->SetSize(32, 32);

	//f = (direction == -1) ? 15 : 0;
	//if (direction != -1)
	//	a->GetAnim()->SetFrame(0, 5, 5, 5, 5, 100);	// black
	//else
	//	a->GetAnim()->SetFrame(0, 225, 225, 5, 5, 100); // clear

	//f += direction;
	//for (i = 0; i < 15; i++)
	//{
	//	a->GetAnim()->AddFrame(f, 40); 
	//	f += direction;
	//}

	//if (direction == -1)
	//	a->GetAnim()->AddFrame(5, 5, 5, 5, 100);	// black
	//else
	//	a->GetAnim()->AddFrame(225, 225, 5, 5, 100); // clear

	//a->m_bDeleteWhenAnimDone = true;
	//a->GetAnim()->SetRepeat(AnimRepeat_None);
	//ret = a->GetAnim()->TotalAnimationTime();
	ret = 1000;
	if (direction == -1)
	{
		m_star->GetSize()->m_w.Animate(800, 2, ret);
		m_star->GetSize()->m_h.Animate(800, 2, ret);
	}
	else
	{
		m_star->GetSize()->m_w.Animate(2, 800, ret);
		m_star->GetSize()->m_h.Animate(2, 800, ret);
	}
//	m_star->fade.a.Animate(1, 1, ret, EaseOut);
//	m_star->m_bDeleteWhenFadeDone = true;
	float ang = (float)rand() / (float)RAND_MAX * 30;
	m_star->angle.Animate(ang, ang+90, 30, Infinity, Loop);

	m_star_ticks_left = last_main_timer.last_timer + ret + 200;

	//int x, y;
	//for (y = 0; y < 8; y++)
	//{
	//	for (x = 0; x < 5; x++)
	//	{
	//		f = (direction == -1) ? 15 : 0;
	//		Sprite* a = new Sprite(tex_transition_burn, 64, 64);
	//		a->SetAlignment(AlignSW);
	//		a->SetCoord((float)x * 64, (float)y * 64, FRONTZ - 10);
	//		if (direction == -1)
	//			a->GetAnim()->AddFrame(225, 225, 1, 1, 50); // clear
	//		else
	//			a->GetAnim()->AddFrame(1, 1, 1, 1, 50);	// black

	//		a->GetAnim()->AddFrame(f, 50);
	//		for (i = 0; i < 15; i++)
	//		{
	//			f += direction;
	//			a->GetAnim()->AddFrame(f, 50);
	//		}
	//		if (direction == -1)
	//			a->GetAnim()->AddFrame(1, 1, 1, 1, 50);	// black
	//		else
	//			a->GetAnim()->AddFrame(225, 225, 1, 1, 50); // clear
	//		a->m_bDeleteWhenAnimDone = true;
	//		a->GetAnim()->SetRepeat(AnimRepeat_None);
	//		sprlistTransition.Add(a);
	//		ret = a->GetAnim()->TotalAnimationTime();
	//	}
	//}
	return ret;
}

void MainObj::AddHook( EventHook* this_ptr )
{
	callback_entry* cbe = new callback_entry();
	cbe->this_ptr = this_ptr;
	m_callbacks.push_back(cbe);
}

void MainObj::RemoveHook( EventHook* this_ptr )
{
	std::list<callback_entry*>::iterator i;
	for (i = m_callbacks.begin(); i != m_callbacks.end(); i++)
	{
		callback_entry* cbe = (*i);
		if (cbe->this_ptr == this_ptr)
		{
			m_callbacks.remove(cbe);
			delete cbe;
			return;
		}
	}
}

void MainObj::CallHooks(event_callback_message msg, int param1 )
{
	std::list<callback_entry*>::iterator i;
	for (i = m_callbacks.begin(); i != m_callbacks.end(); i++)
	{
		callback_entry* cbe = (*i);
		if (msg == EVENT_Render) cbe->this_ptr->Render();
		if (msg == EVENT_TouchBegan) cbe->this_ptr->TouchBegan(param1);
		if (msg == EVENT_TouchDragging) cbe->this_ptr->TouchDragging(param1);
		if (msg == EVENT_TouchEnded) cbe->this_ptr->TouchEnded(param1);
	}
}

#ifdef _DEBUG
void MainObj::ShowDebugPoint( float x, float y, float size )
{
	Sprite* v = new Sprite(tex_misc_64x64,64, 64);
	v->GetAnim()->SetFrame(0, 2);
	v->SetSize(size,size);
	v->fade.a.Animate(1, 0, 3000);
	v->m_bDeleteWhenFadeDone = true;
	v->SetCoord(x, y);
	sprlistTransition.Add(v);
}
void MainObj::ShowDebugNumber( float x, float y, int num )
{
	TextSprite* v = TextHandler::CreateText(FontSize_sans_12o);
	v->SetTextFormatted("%d", num);
	v->fade.a.Animate(1, 0, 3000);
	v->SetIdealHeight();
	v->SetWidth(30);
	v->m_bDeleteWhenFadeDone = true;
	v->SetCoord(x, y);
	sprlistTransition.Add(v);
}
void MainObj::SetTextSize( int memory )
{
	if (closing) return;
//	m_memory->SetTextI(memory);
}


#endif

void MainObj::LowMemory()
{
	TextureHandler::FlushTextures(true);
	theSoundEngine.FlushBuffers();
	//theSoundEngine.SetRunFlag(false);
}
