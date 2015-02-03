#pragma once

#ifdef _WINDOWS
#include"al.h"
#include"alc.h"
#else
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <string>
#include "LoadTexture.h"
#endif

class Tower;

typedef struct soundplay_struct
{
	ALuint alsource;
} soundplay;

typedef struct sound_buffer_struct
{
	std::string fname;
	ALuint albuffer;
	std::list<soundplay*> m_plays;
	uint last_playend;
	uint last_playstart;
} sound_buffer;


class CrossSound
{
public:
	CrossSound();
	~CrossSound();
	void PlaySound(std::string fname, int volum, float reduce_pitch = 0.0f, float increase_pitch = 0.0f, bool looping = false);
	void PlayOccasionalSound(std::string fname, int volum, float reduce_pitch = 0.0f, float increase_pitch = 0.0f);
	void StopSounds(std::string fname, bool flush = false);
	void SetSoundVolume(std::string fname, int volum);
	bool IsPlaying(std::string fname);
	void FlushBuffers();
	void FlushPlays();
	void SetRunFlag(bool flag) { m_bOpen = flag; }
protected:
	bool m_bOpen;
	bool flag;
	ALCdevice* m_device;
	ALCcontext* m_context;

	sound_buffer* LocateOrLoad(std::string fname);

	std::list<sound_buffer*> m_buffers;
};

extern CrossSound theSoundEngine;

//void TDPlayGameSound( const char* fname, float reduce_rate = 0, float increase_rate = 0 );
//void TDPlayTowerSound(Tower* tower, const char* fname, float reduce_rate = 0, float increase_rate = 0 );
//void TDStopSound(const char* fname, bool flush = false);

