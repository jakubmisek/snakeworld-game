#ifndef _AUDIOPATH_H_
#define _AUDIOPATH_H_

#include "soundsegment.h"
#include "list.h"
#include "locker.h"


class CSoundManager;

//
// audio path
// specify sound to play and position of the sound
//
class CAudioPath: public ILocker
{
public:
	CAudioPath();
	virtual ~CAudioPath();

	void					FreeAudioPath();

	bool		PlaySegment( CSoundSegment*pSegment, bool bRepeat, bool b3D = true );
	bool		Stop();
	bool		SetPosition( float x, float y, float z );
	
	CSoundManager			*m_pManager;

protected:

	
	bool					InitAudioPath();
	
	CSoundSegment			*m_pSegment;

	bool	bIs3D;

	// XAudio2
    IXAudio2SourceVoice*    pSourceVoice;
    
    X3DAUDIO_DSP_SETTINGS   dspSettings;
    X3DAUDIO_EMITTER        emitter;
    X3DAUDIO_CONE           emitterCone;

    D3DXVECTOR3             vEmitterPos;
    
    FLOAT32 emitterAzimuths[INPUTCHANNELS];
    FLOAT32 matrixCoefficients[INPUTCHANNELS * OUTPUTCHANNELS];

	friend class CSoundManager;
};


typedef CList<CAudioPath>	AudioPathList;

#endif//_AUDIOPATH_H_