#import "SoundMaker.h"
#import "SoundEngine.h"
#import "Settings.h"

//UInt32 _sounds[SOUND_FINISH];
UInt32 _sounds[SOUND_FINISH];

int on_sound = 0;

void TDStartSoundEngine()
{
//	NSBundle*				bundle = [NSBundle mainBundle];
	AudioSessionInitialize(NULL,NULL,NULL,NULL);
	UInt32 sessionCategory = kAudioSessionCategory_AmbientSound;
	AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(sessionCategory), &sessionCategory);
	
	SoundEngine_Initialize(44100);
	
	for (int i = 0; i < SOUND_FINISH; i++) _sounds[i] = 0;
/*
	const char* str = [[bundle pathForResource:@"twang" ofType:@"caf" ] UTF8String];
	SoundEngine_LoadEffect(str, &_sounds[SOUND_TWANG]);
	SoundEngine_LoadEffect(str, &_sounds[SOUND_TWANG+1]);
	SoundEngine_LoadEffect(str, &_sounds[SOUND_TWANG+2]);
	str = [[bundle pathForResource:@"zap" ofType:@"caf" ] UTF8String];
	SoundEngine_LoadEffect(str, &_sounds[SOUND_ZAP]);
	SoundEngine_LoadEffect(str, &_sounds[SOUND_ZAP+1]);
	SoundEngine_LoadEffect(str, &_sounds[SOUND_ZAP+2]);
	str = [[bundle pathForResource:@"menuclick" ofType:@"caf" ] UTF8String];
	SoundEngine_LoadEffect(str, &_sounds[SOUND_MENUCLICK]);
	SoundEngine_LoadEffect(str, &_sounds[SOUND_MENUCLICK+1]);
	SoundEngine_LoadEffect(str, &_sounds[SOUND_MENUCLICK+2]);
	const char* str = [[bundle pathForResource:@"tock" ofType:@"caf" ] UTF8String];
	SoundEngine_LoadEffect(str, &_sounds[SOUND_TOCK]);
	SoundEngine_LoadEffect(str, &_sounds[SOUND_TOCK+1]);
	SoundEngine_LoadEffect(str, &_sounds[SOUND_TOCK+2]);
 */
}
void TDPlaySound(const char* fname, int vol, float min_freq, float max_freq)
{
	NSBundle*				bundle = [NSBundle mainBundle];
	NSString* ns = [NSString stringWithCString:fname];

	if (_sounds[on_sound] != 0)
	{
		SoundEngine_StopEffect(_sounds[on_sound], false);
		SoundEngine_UnloadEffect(_sounds[on_sound]);
		_sounds[on_sound] = 0;
	}
	
	const char* str = [[bundle pathForResource:ns ofType:@"caf" ] UTF8String];
//	[ns release];
	if (str != nil)
	{
		SoundEngine_LoadEffect(str, &_sounds[on_sound]);

		SoundEngine_SetEffectLevel(_sounds[on_sound], (float)(TheSettings.SoundVolume() / 256.0f));
		SoundEngine_StartEffect(_sounds[on_sound]);
	}
	on_sound++;
	if (on_sound >= 12) on_sound = 0;
}
void TDPlayGameSound(const char* fname, float min_freq , float max_freq )
{
	TDPlaySound(fname, TheSettings.SoundVolume(), min_freq, max_freq);
}
void TDPlayTowerSound(const char* fname, float min_freq , float max_freq )
{
	TDPlaySound(fname, TheSettings.TowerVolume(), min_freq, max_freq);
}

void TDStopSoundEngine()
{
	SoundEngine_Teardown();
}


void TDStartMusic(UInt32 music_number)
{
	NSBundle*				bundle = [NSBundle mainBundle];
	const char* str;
	if (music_number == MUSIC_TITLE)
		str = [[bundle pathForResource:@"title-loop" ofType:@"caf" ] UTF8String];
	SoundEngine_LoadLoopingEffect(str, NULL, NULL, &_sounds[music_number]);
	SoundEngine_SetEffectLevel(_sounds[music_number], (float)(TheSettings.MusicVolume()/256.0f));
	SoundEngine_StartEffect(_sounds[music_number]);
}
void TDStopMusic(UInt32 music_number)
{
	SoundEngine_StopEffect(_sounds[music_number], false);
	SoundEngine_UnloadEffect(_sounds[music_number]);
}
void TDSetMusicGain()
{
	SoundEngine_SetEffectLevel(_sounds[MUSIC_TITLE], (float)(TheSettings.MusicVolume()/256.0f));
}

