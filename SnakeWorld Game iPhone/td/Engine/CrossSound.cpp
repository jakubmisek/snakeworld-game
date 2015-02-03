#include "stdafx.h"

#include "CrossSound.h"
#include "MainObj.h"
//#include "TextSprite.h"
//#include "Tower.h"
//#include "Game.h"


CrossSound theSoundEngine;

extern td_timer last_main_timer;


//void TDPlayGameSound( const char* fname, float reduce_rate, float increase_rate )
//{
//	theSoundEngine.PlaySound(std::string(fname), TheSettings.SoundVolume(), reduce_rate, increase_rate);
//}
//void TDPlayTowerSound(Tower* tower, const char* fname, float reduce_rate, float increase_rate )
//{
//	if (TheSettings.TowerMode() == TOWER_MODE_OFF) return;
//	if ((tower != NULL && tower == theGame->SelectedTower()) || TheSettings.TowerMode() != TOWER_MODE_PERIODIC)
//	{
//		theSoundEngine.PlaySound(std::string(fname), TheSettings.TowerVolume(), reduce_rate, increase_rate);
//	}
//	else
//	{
//		theSoundEngine.PlayOccasionalSound(std::string(fname), TheSettings.TowerVolume(), reduce_rate, increase_rate);
//	}
//}
//void TDStopSound(const char* fname, bool flush)
//{
//	theSoundEngine.StopSounds(std::string(fname), flush);
//}

CrossSound::CrossSound()
{
	m_device = NULL;
	m_context = NULL;
	m_bOpen = false;
	m_device = alcOpenDevice(NULL);
	if (m_device == NULL) return;
	m_context = alcCreateContext(m_device, NULL);
	if (m_context == NULL) return;
	alcMakeContextCurrent(m_context);

	m_bOpen = true;
}

CrossSound::~CrossSound()
{
	std::list<sound_buffer*>::iterator i;
	for ( i = m_buffers.begin(); i != m_buffers.end(); i++ )
	{
		sound_buffer* buffer = (*i);
		std::list<soundplay*>::iterator ii;
		for (; buffer->m_plays.size() > 0; )
		{
			ii = buffer->m_plays.begin();
			soundplay* play = (*ii);
			alSourceStop(play->alsource);
			alDeleteSources(1, &(play->alsource));
			buffer->m_plays.remove(play);
			delete play;
		}
	}
	std::list<sound_buffer*>::iterator t;
	for (; m_buffers.size() > 0; )
	{
		t = m_buffers.begin();
		sound_buffer* buffer = (*t);
		alDeleteBuffers(1, &(buffer->albuffer));
		m_buffers.remove(buffer);
		delete buffer;
	}
	alcMakeContextCurrent(NULL);
	if (m_context) alcDestroyContext(m_context);
	if (m_device) alcCloseDevice(m_device);
}

void CrossSound::PlayOccasionalSound( std::string fname, int volum, float reduce_pitch /*= 0.0f*/, float increase_pitch /*= 0.0f*/ )
{
	FlushPlays();
	FlushBuffers();
	if (volum == 0) return;
	if (!m_bOpen) return;
	sound_buffer* buffer = LocateOrLoad(fname);
	if (buffer == NULL) return;
	if (buffer->last_playstart + 1000 < last_main_timer.last_timer)
	{
		PlaySound(fname, volum, reduce_pitch, increase_pitch);
	}
}

void CrossSound::PlaySound( std::string fname, int volum, float reduce_pitch , float increase_pitch, bool looping )
{
	FlushPlays();
	FlushBuffers();
	if (volum == 0) return;
	if (!m_bOpen) return;
	sound_buffer* buffer = LocateOrLoad(fname);
	if (buffer == NULL) return;
	soundplay* play = new soundplay();

	alGenSources( 1, &(play->alsource ));
	if (play->alsource == 0) return;

	// Attach Source to Buffer
	alSourcei( (play->alsource), AL_BUFFER, (buffer->albuffer) );

	if (reduce_pitch != 0 || increase_pitch != 0)
	{
		float b = reduce_pitch;
		float c = (float)rand() / (float)RAND_MAX;
		float d = (increase_pitch - reduce_pitch) * c + b;
		alSourcef( play->alsource, AL_PITCH, 1 + d);
	}
	alSourcef( play->alsource, AL_GAIN, (float)(volum) / 256.0f);
	if (looping) alSourcei( play->alsource, AL_LOOPING, AL_TRUE);
	alSourcePlay( play->alsource );

	buffer->m_plays.push_back(play);
	buffer->last_playstart = last_main_timer.last_timer;
}

sound_buffer* CrossSound::LocateOrLoad( std::string fname)
{
	std::list<sound_buffer*>::iterator i;
	for ( i = m_buffers.begin(); i != m_buffers.end(); i++ )
	{
		sound_buffer* buffer = (*i);
		if (buffer->fname.compare(fname) == 0)
			return buffer;
	}
	sound_buffer* buffer = new sound_buffer();
	buffer->fname = fname;

	alGenBuffers( 1, &(buffer->albuffer) );
	if (buffer->albuffer == 0)
	{
		delete buffer;
		return NULL;
	}

#ifdef _WINDOWS
	std::string fullfname = "..\\Sounds\\";
	fullfname += fname;
	fullfname += ".wav";
	FILE* f = fopen(fullfname.data(), "rb");
	if (f == NULL) MessageBox(NULL, fullfname.data(), "could not load sound", MB_OK);
	if (fname.compare("gem") == 0)
	{
		int a = 1 + 2;
	}
#else
	const char* pathname = BundlePathName(fname.data(), "wav");
	FILE* f = fopen(pathname, "rb");
#endif
	if (f == NULL)
	{
		delete buffer;
		return NULL;
	}
	uint header;
	uint size;
	fread(&header, 1, 4, f);
	if (header != 'FFIR')	// "RIFF"
	{
		delete buffer;
		fclose(f);
		return NULL;
	}
	byte fmt[16];
	fread(&size, 1, 4, f);	// full file size
	fread(&header, 1, 4, f);	// "WAVE"
	while (header != 'atad')
	{
		fread(&header, 1, 4, f);
		fread(&size, 1, 4, f);
		if (header == ' tmf')
		{
			fread(&fmt, 1, 16, f);
		}
		else
		{
			if (header != 'atad')
				fseek(f, size, SEEK_CUR);
		}
	}

	byte* b = new byte[size];
	fread(b, 1, size, f);
	fclose(f);

	alBufferData(buffer->albuffer, AL_FORMAT_MONO16, b, (ALsizei)size, 22050 );

	delete b;
	m_buffers.push_back(buffer);
	buffer->last_playstart = 0;
	buffer->last_playend = 0;
	return buffer;
}

void CrossSound::FlushBuffers()
{
	std::list<sound_buffer*>::iterator i;
//#ifdef _WINDOWS
//	std::string dbg;
//	for ( i = m_buffers.begin(); i != m_buffers.end(); i++ )
//	{
//		sound_buffer* buffer = (*i);
//		dbg += buffer->fname;
//		dbg += TextHandler::FormatNumber("(%d), ", (int)buffer->m_plays.size());
//	}
//	dbg += "\n";
//	Trace(dbg.data());
//#endif
	for ( i = m_buffers.begin(); i != m_buffers.end(); i++ )
	{
		sound_buffer* buffer = (*i);
		if (buffer->m_plays.size() == 0)
		{
			if (last_main_timer.last_timer > buffer->last_playend + 5000)
			{
				m_buffers.remove(buffer);
				alDeleteBuffers(1, &(buffer->albuffer));
				delete buffer;
				return;
			}
		}
	}
}

void CrossSound::FlushPlays()
{
	std::list<sound_buffer*>::iterator ii;
	for ( ii = m_buffers.begin(); ii != m_buffers.end(); ii++ )
	{
		sound_buffer* buffer = (*ii);
		std::list<soundplay*>::iterator i;
		for ( i = buffer->m_plays.begin(); i != buffer->m_plays.end(); i++ )
		{
			soundplay* play = (*i);
			ALint       iState;
			alGetSourcei( play->alsource, AL_SOURCE_STATE, &iState);
			if (iState == AL_STOPPED)
			{
				buffer->m_plays.remove(play);
				buffer->last_playend = last_main_timer.last_timer;
				alDeleteSources(1, &(play->alsource));
				delete play;
				return;
			}
		}
	}
}

void CrossSound::StopSounds( std::string fname, bool flush )
{
	std::list<sound_buffer*>::iterator ii;
	for ( ii = m_buffers.begin(); ii != m_buffers.end(); ii++ )
	{
		sound_buffer* buffer = (*ii);
		if (buffer->fname.compare(fname) == 0)
		{
			std::list<soundplay*>::iterator i;
			bool repeat = true;
			do
			{
				repeat = false;
				for ( i = buffer->m_plays.begin(); i != buffer->m_plays.end(); i++ )
				{
					soundplay* play = (*i);
					alSourceStop(play->alsource);
					if (flush)
					{
						buffer->m_plays.remove(play);
						buffer->last_playend = last_main_timer.last_timer;
						alDeleteSources(1, &(play->alsource));
						delete play;
						repeat = true;
						break;
					}
				}
			} while (repeat);
			if (flush)
			{
				m_buffers.remove(buffer);
				alDeleteBuffers(1, &(buffer->albuffer));
				delete buffer;
				return;
			}
		}
	}
}

void CrossSound::SetSoundVolume( std::string fname, int volum )
{
	std::list<sound_buffer*>::iterator ii;
	for ( ii = m_buffers.begin(); ii != m_buffers.end(); ii++ )
	{
		sound_buffer* buffer = (*ii);
		if (buffer->fname.compare(fname) == 0)
		{
			std::list<soundplay*>::iterator i;
			for ( i = buffer->m_plays.begin(); i != buffer->m_plays.end(); i++ )
			{
				soundplay* play = (*i);
				alSourcef( play->alsource, AL_GAIN, (float)volum / 256.0f);
			}
		}
	}

}

bool CrossSound::IsPlaying( std::string fname )
{
	std::list<sound_buffer*>::iterator ii;
	for ( ii = m_buffers.begin(); ii != m_buffers.end(); ii++ )
	{
		sound_buffer* buffer = (*ii);
		if (buffer->fname.compare(fname) == 0)
		{
			std::list<soundplay*>::iterator i;
			for ( i = buffer->m_plays.begin(); i != buffer->m_plays.end(); i++ )
			{
				soundplay* play = (*i);
				ALint       iState;
				alGetSourcei( play->alsource, AL_SOURCE_STATE, &iState);
				if (iState == AL_PLAYING) return true;
			}
		}
	}
	return false;
}
