#pragma once
#import "SoundEngine.h"
#import <string.h>

#define MUSIC_TITLE 49
#define SOUND_FINISH 50

void TDStartSoundEngine();
void TDPlayGameSound(const char* fname, float min_freq = 0, float max_freq = 0);
void TDPlayTowerSound(const char* fname, float min_freq = 0, float max_freq = 0);
void TDStopSoundEngine();
void TDStartMusic(UInt32 music_number);
void TDStopMusic(UInt32 music_number);
void TDSetMusicGain();


