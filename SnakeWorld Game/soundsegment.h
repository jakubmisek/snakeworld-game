#ifndef _SOUNDSEGMENT_H_
#define _SOUNDSEGMENT_H_

class CSoundManager;

#include "list.h"
#include "memtool.h"
#include "string.h"

#include <xaudio2.h>
#include <xaudio2fx.h>
#include <x3daudio.h>
#include <d3dx9math.h>


#include "wavefile.h"


//////////////////////////////////////////////////////////////////////////
// Global defines
#define INPUTCHANNELS 1
#define OUTPUTCHANNELS 8

class CAudioPath;

//////////////////////////////////////////////////////////////////////////
//
// sound object
// the sound to be played
//
class CSoundSegment
{
public:
	CSoundSegment();
	virtual ~CSoundSegment();

	void	FreeSegment();

	bool					LoadSegmentFromFile( CSoundManager*pManager, CString&szFileName );

	BYTE		*	GetWaveData( UINT32*pdwWaveSize  ){ *pdwWaveSize = cbWaveSize; return pbSampleData; }
	WAVEFORMATEX*	GetWaveFormat(){ return wav.GetFormat(); }
	CSoundManager*	GetSoundManager(){ return m_pManager; }

protected:
	

	CSoundManager			*m_pManager;

	CWaveFile				wav;

	BYTE*                   pbSampleData;
	DWORD					cbWaveSize;

	friend class CAudioPath;

};

typedef CList<CSoundSegment>	SoundSegmentList;

#endif//_SOUNDSEGMENT_H_