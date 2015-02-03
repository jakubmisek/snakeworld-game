#pragma once

//#define SOUND_TWANG 0
//#define SOUND_ZAP 1
//#define SOUND_MENUCLICK 2
//#define SOUND_TOCK 3

#define MUSIC_TITLE 49


void TDStartEngine();

//void TDPlaySound(int sound_number, float reduce_rate = 0, float increase_rate = 0);


void TDStopEngine();

void TDStartMusic(int music_no);
void TDStopMusic(int music_number);

void TDSetMusicGain();
