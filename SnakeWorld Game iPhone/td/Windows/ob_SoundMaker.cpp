#include "stdafx.h"
#include "SoundMaker.h"
#include <string>
#include <mmsystem.h>
#include "OpenAL\Framework.h"
#include "Settings.h"
#include "wininet.h"

//ALuint      uiBuffer[50];
//ALuint      uiSource[50];
ALint       iState;

ALuint      uiFlyBuffer[50];
ALuint      uiFlySource[50];
int on_sound = 0;

void Load( int s_no, char* fname )
{
	// Generate an AL Buffer
	alGenBuffers( 1, &(uiFlyBuffer[s_no]) );

	// Load Wave file into OpenAL Buffer
	if (ALFWLoadWaveToBuffer(fname, uiFlyBuffer[s_no]))
	{
		// Generate a Source to playback the Buffer
		alGenSources( 1, &(uiFlySource[s_no]) );

		// Attach Source to Buffer
		alSourcei( (uiFlySource[s_no]), AL_BUFFER, (uiFlyBuffer[s_no]) );
	}
}


void TDStartEngine()
{
	return;
	for (int i = 0; i < 50; i++)
	{
		uiFlyBuffer[i] = NULL;
		uiFlySource[i] = NULL;
	}
	// Initialize Framework
	ALFWInit();

	if (!ALFWInitOpenAL())
	{
		ALFWShutdown();
		return ;
	}

	//Load(SOUND_TWANG, "..\\Sounds\\twang.wav");
	//Load(SOUND_ZAP, "..\\Sounds\\zap.wav");
	//Load(SOUND_MENUCLICK, "..\\Sounds\\menuclick.wav");
	//Load(SOUND_TOCK, "..\\Sounds\\tock.wav");


	//ALFWprintf("Playing Source ");

	//do
	//{
	//	Sleep(100);
	//	ALFWprintf(".");
	//	// Get Source State
	//	alGetSourcei( uiSource, AL_SOURCE_STATE, &iState);
	//} while (iState == AL_PLAYING);

	//ALFWprintf("\n");
}

void TDSetMusicGain()
{
	return;
	alSourcef( uiFlySource[MUSIC_TITLE], AL_GAIN, (float)TheSettings.MusicVolume() / 256.0f);
}

void TDStartMusic( int music_number )
{
	return;
	Load(music_number, "..\\Sounds\\title-loop.wav");
	alSourcef( uiFlySource[music_number], AL_GAIN, (float)TheSettings.MusicVolume() / 256.0f);
	alSourcei( uiFlySource[music_number], AL_LOOPING, AL_TRUE);
	alSourcePlay( uiFlySource[music_number] );
}

void TDStopMusic(int music_number)
{
	return;
	if (uiFlySource[music_number] != NULL)
	{
		alSourceStop( uiFlySource[music_number] );
		alSourceStop(uiFlySource[on_sound]);
		alDeleteSources(1, &(uiFlySource[on_sound]));
		uiFlySource[on_sound] = NULL;
		alDeleteBuffers(1, &(uiFlyBuffer[on_sound]));
		uiFlyBuffer[on_sound] = NULL;
	}
}

//void TDPlaySound( int sound_number, float reduce_rate, float increase_rate )
//{
//	if (reduce_rate != 0 && increase_rate != 0)
//	{
//		float b = reduce_rate;
//		float c = (float)rand() / (float)RAND_MAX;
//		float d = (increase_rate - reduce_rate) * c + b;
//		alSourcef( uiFlySource[sound_number], AL_PITCH, 1 + d);
//	}
//	// Play Source
//	alSourcef( uiFlySource[sound_number], AL_GAIN, (float)TheSettings.SoundVolume() / 256.0f);
//	alSourcePlay( uiFlySource[sound_number] );
//	//if (sound_number == SOUND_TWANG)
//	//	PlaySound("..\\Sounds\\Twang.wav", NULL, SND_FILENAME | SND_NOWAIT | SND_ASYNC );
//	//if (sound_number == SOUND_ZAP)
//	//	PlaySound("..\\Sounds\\Zap.wav", NULL, SND_FILENAME | SND_NOWAIT | SND_ASYNC );
//}

void TDPlaySound( const char* fname, int volum, float reduce_rate, float increase_rate )
{
	return;
	if (uiFlySource[on_sound] != NULL)
	{
		alSourceStop(uiFlySource[on_sound]);
		alDeleteSources(1, &(uiFlySource[on_sound]));
		uiFlySource[on_sound] = NULL;
	}
	if (uiFlyBuffer[on_sound] != NULL)
	{
		alDeleteBuffers(1, &(uiFlyBuffer[on_sound]));
		uiFlyBuffer[on_sound] = NULL;
	}
	std::string pname = Format("..\\Sounds\\%s.wav", fname);
	char buf[512];
	size_t p = pname._Copy_s(buf, 511, 511); buf[p] = 0;
	Load(on_sound, buf);

	if (reduce_rate != 0 || increase_rate != 0)
	{
		float b = reduce_rate;
		float c = (float)rand() / (float)RAND_MAX;
		float d = (increase_rate - reduce_rate) * c + b;
		alSourcef( uiFlySource[on_sound], AL_PITCH, 1 + d);
	}
	// Play Source
	alSourcef( uiFlySource[on_sound], AL_GAIN, (float)(volum) / 256.0f);
	alSourcePlay( uiFlySource[on_sound] );


	on_sound++;
	if (on_sound >= 12) on_sound = 0;
}

void TDStopEngine()
{
	return;
	for (int i = 0; i < 50; i++)
	{
		if (uiFlySource[i] != NULL)
		{
			alSourceStop(uiFlySource[i]);
			alDeleteSources(1, &(uiFlySource[i]));
		}
		if (uiFlyBuffer[i] != NULL)
		{
			alDeleteBuffers(1, &(uiFlyBuffer[i]));
		}
	}

	// Clean up by deleting Source(s) and Buffer(s)
	ALFWShutdownOpenAL();

	ALFWShutdown();
}

