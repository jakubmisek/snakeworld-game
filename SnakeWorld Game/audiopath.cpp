#include "soundmanager.h"
#include "errorlog.h"

CAudioPath::CAudioPath()
{
	m_pManager = NULL;

	m_pSegment = NULL;

	bIs3D = false;

	// XAudio2
	pSourceVoice = NULL;
    
    ZeroMemory( &dspSettings, sizeof(dspSettings) );
    ZeroMemory( &emitter, sizeof(emitter) );
    ZeroMemory( &emitterCone, sizeof(emitterCone) );

    ZeroMemory( &vEmitterPos, sizeof(vEmitterPos) );
    
    ZeroMemory( emitterAzimuths, sizeof(emitterAzimuths) );
    ZeroMemory( matrixCoefficients, sizeof(matrixCoefficients) );
}
CAudioPath::~CAudioPath()
{
	FreeAudioPath();
}
bool		CAudioPath::InitAudioPath()
{
	locked(

		emitter.OrientFront.z = 1;
		emitter.OrientTop.y = 1;
		emitter.ChannelCount           = INPUTCHANNELS;
		emitter.ChannelRadius          = 1.0f;
		emitter.CurveDistanceScaler    = 1.0f;
		emitter.DopplerScaler          = 1.0f;
		emitter.pCone                  = &emitterCone;
		emitter.pCone->InnerAngle      = 0.0f;
								   // Setting the inner cone angles to X3DAUDIO_2PI and
								   // outer cone other than 0 causes
								   // the emitter to act like a point emitter using the
								   // INNER cone settings only.
		emitter.pCone->OuterAngle      = 0.0f;
								   // Setting the outer cone angles to zero causes
								   // the emitter to act like a point emitter using the
								   // OUTER cone settings only.
		emitter.pCone->InnerVolume     = 0.0f;
		emitter.pCone->OuterVolume     = 1.0f;
		emitter.pCone->InnerLPF        = 0.0f;
		emitter.pCone->OuterLPF        = 1.0f;
		emitter.pCone->InnerReverb     = 0.0f;
		emitter.pCone->OuterReverb     = 1.0f;

		emitter.ChannelCount           = INPUTCHANNELS;
		emitter.CurveDistanceScaler    = 14.0f;
		emitter.DopplerScaler          = 1.0f;
		emitter.pChannelAzimuths       = emitterAzimuths;

		dspSettings.SrcChannelCount    = INPUTCHANNELS;
		dspSettings.DstChannelCount    = m_pManager->GetChannels();
		dspSettings.pMatrixCoefficients = matrixCoefficients;

		if ( pSourceVoice )
		{
			pSourceVoice->Stop( 0 );
			pSourceVoice->DestroyVoice();
			pSourceVoice = 0;
		}

	);
	
	return true;
}
void		CAudioPath::FreeAudioPath()
{
	// disconnect from the manager
	if ( m_pManager )
	{
		m_pManager->ReleaseAudioPath( this );
		m_pManager = NULL;
	}

	locked(
	
	// release voice
	m_pSegment = NULL;

	if (pSourceVoice)
    {
        pSourceVoice->DestroyVoice();
        pSourceVoice = NULL;
    }

	);
}
//////////////////////////////////////////////////////////////////////////
bool		CAudioPath::PlaySegment( CSoundSegment*pSegment, bool bRepeat, bool b3D )
{
	if ( !pSegment || !pSegment->m_pManager )
		return false;

	if ( !m_pManager )
	{
		// first use, add to manager
		pSegment->m_pManager->AddAudioPath( this );
	}

	m_pManager = pSegment->m_pManager;

	if ( !pSegment || !m_pManager->IsInitialized() )
		return false;

	if ( !InitAudioPath() )
		return false;

	bIs3D = b3D;

	//
    // Play the wave using a XAudio2SourceVoice
    //
    const IXAudio2Voice* voices[] = { m_pManager->GetMasteringVoice() };
    const XAUDIO2_VOICE_SENDS sendList = { 1, (IXAudio2Voice**)voices };

    // Create the source voice
    if (FAILED( m_pManager->GetXAudio2()->CreateSourceVoice( &pSourceVoice, pSegment->GetWaveFormat(), 0,
                          XAUDIO2_DEFAULT_FREQ_RATIO, NULL, &sendList ) ))
		  return false;

	XAUDIO2_BUFFER buffer = {0};
    buffer.pAudioData = pSegment->GetWaveData( &buffer.AudioBytes );
    buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount = bRepeat?XAUDIO2_LOOP_INFINITE:0;

	
	 if (FAILED( pSourceVoice->SubmitSourceBuffer( &buffer ) ))
		return false;

	m_pManager->UpdateAudioPath( this );

    if (FAILED( pSourceVoice->Start( 0 ) ))
		return false;

	return true;
}
bool		CAudioPath::Stop()
{
	locked(

		if ( pSourceVoice )
		{
			pSourceVoice->Stop( 0 );
		}

	);

	return false;
}
bool		CAudioPath::SetPosition( float x, float y, float z )
{
	vEmitterPos.x = x;
	vEmitterPos.y = y;
	vEmitterPos.z = z;

	return true;
}