#ifndef _SOUNDMANAGER_H_
#define _SOUNDMANAGER_H_

#include "soundsegment.h"
#include "audiopath.h"
#include "locker.h"


#include "string.h"

//
// sound manager
// create/destroy sound segments and audio paths
//
class CSoundManager: public ILocker
{
public:
	CSoundManager();
	virtual ~CSoundManager();

	bool			InitSoundManager();
	void			Cleanup();

	inline bool		IsInitialized(){ return bInitialized; }

	inline IXAudio2* GetXAudio2(){ nFrameToApply3DAudio = 0;return pXAudio2; }
	inline IXAudio2MasteringVoice* GetMasteringVoice(){ return pMasteringVoice; }
	inline DWORD	GetChannels(){ return nChannels; }

	void			SetListenerPosition( float x, float y, float z, float xFront, float yFront, float zFront, float xTop, float yTop, float zTop );
	
	void			UpdateAudio( float fElapsedTime );

	void			UpdateAudioPath( CAudioPath*pPath );

	void			AddAudioPath(CAudioPath*pPath){ locked(m_audiopaths.Add(pPath);); }
	void			ReleaseAudioPath(CAudioPath*pPath){ locked(m_audiopaths.Remove(pPath,false);) }

	void			SetVolume( float flVolume ){ m_flVolume = flVolume; }

	//
	// used sound segments
	//
	CSoundSegment		sndClick, sndSnaking, sndHit, sndStart, sndBite1, sndBite2;

protected:
	void		InitVariables();

	void		LoadSoundSegments();
	
	void		FreeSoundManager();

	bool bInitialized;

	// XAudio2
    IXAudio2*               pXAudio2;
	IXAudio2MasteringVoice* pMasteringVoice;
    
    // 3D
    X3DAUDIO_HANDLE         x3DInstance;
    int                     nFrameToApply3DAudio;

    DWORD                   dwChannelMask;
    UINT32                  nChannels;

    X3DAUDIO_LISTENER       listener;
    D3DXVECTOR3		        vListenerPos;
    float                   fListenerAngle;

	AudioPathList			m_audiopaths;

	float			m_flVolume;

};


#endif//_SOUNDMANAGER_H_