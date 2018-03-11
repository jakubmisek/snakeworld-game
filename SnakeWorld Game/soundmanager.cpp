#include "soundmanager.h"
#include "errorlog.h"

#include <d3dx9math.h>

//////////////////////////////////////////////////////////////////////////
// Global defines
#define NUM_PRESETS 30
#define	UNIT_SCALE	(8.0f)
//////////////////////////////////////////////////////////////////////////
CSoundManager::CSoundManager()
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	InitVariables();
}
CSoundManager::~CSoundManager()
{
	FreeSoundManager();

}
//////////////////////////////////////////////////////////////////////////
void			CSoundManager::InitVariables()
{
	bInitialized = false;

	// XAudio2
	pXAudio2 = NULL;
	pMasteringVoice = NULL;


	// 3D
	ZeroMemory(x3DInstance, sizeof(x3DInstance));
	nFrameToApply3DAudio = 0;

	dwChannelMask = 0;
	nChannels = 0;

	ZeroMemory(&listener, sizeof(listener));
	ZeroMemory(&vListenerPos, sizeof(vListenerPos));
	fListenerAngle = 0;
}
void			CSoundManager::FreeSoundManager()
{
	Cleanup();

	CoUninitialize();
}
void			CSoundManager::Cleanup()
{
	// stop and disconnect all used audio paths
	locked(

		while (m_audiopaths.Count() > 0)
			m_audiopaths[0]->FreeAudioPath();

	);

	if (bInitialized)
	{
		if (pMasteringVoice)
		{
			pMasteringVoice->DestroyVoice();
			pMasteringVoice = NULL;
		}

		pXAudio2->StopEngine();
		SAFE_RELEASE(pXAudio2);

		bInitialized = false;

	}
}
bool			CSoundManager::InitSoundManager()
{
	UINT32 flags = 0;
#ifdef _DEBUG
	flags |= XAUDIO2_DEBUG_ENGINE;
#endif

	HRESULT hr;

	if (FAILED(hr = XAudio2Create(&pXAudio2, flags)))
	{
		AddLog(L"Error:XAudio2Create");
		return false;
	}

	//
	// Create a mastering voice
	//
	if (FAILED(hr = pXAudio2->CreateMasteringVoice(&pMasteringVoice)))
	{
		SAFE_RELEASE(pXAudio2);
		AddLog(L"Error:CreateMasteringVoice");
		return false;
	}

#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)

	pMasteringVoice->GetChannelMask(&dwChannelMask);

	XAUDIO2_VOICE_DETAILS vdetails;
	pMasteringVoice->GetVoiceDetails(&vdetails);
	nChannels = vdetails.InputChannels;

#else

	// Check device details to make sure it's within our sample supported parameters
	XAUDIO2_DEVICE_DETAILS details;
	if (FAILED(hr = pXAudio2->GetDeviceDetails(0, &details)))
	{
		AddLog(L"Error:GetDeviceDetails");
		SAFE_RELEASE(pXAudio2);
		return false;
	}

	if (details.OutputFormat.Format.nChannels > OUTPUTCHANNELS)
	{
		AddLog(L"Error:details.OutputFormat.Format.nChannels > OUTPUTCHANNELS");
		SAFE_RELEASE(pXAudio2);
		return false;
	}

	dwChannelMask = details.OutputFormat.dwChannelMask;
	nChannels = details.OutputFormat.Format.nChannels;

#endif

	//
	// Initialize X3DAudio
	//  Speaker geometry configuration on the final mix, specifies assignment of channels
	//  to speaker positions, defined as per WAVEFORMATEXTENSIBLE.dwChannelMask
	//
	//  SpeedOfSound - speed of sound in user-defined world units/second, used
	//  only for doppler calculations, it must be >= FLT_MIN
	//
	const float SPEEDOFSOUND = X3DAUDIO_SPEED_OF_SOUND;

	X3DAudioInitialize(dwChannelMask, SPEEDOFSOUND, x3DInstance);

	//
	// Setup 3D audio structs
	//
	listener.OrientFront.z = 1;
	listener.OrientTop.y = 1;


	//
	// Done
	//
	bInitialized = true;



	LoadSoundSegments();

	return true;
}


void			CSoundManager::SetListenerPosition(float x, float y, float z, float xFront, float yFront, float zFront, float xTop, float yTop, float zTop)
{
	x /= UNIT_SCALE;
	y /= UNIT_SCALE;
	z /= UNIT_SCALE;

	vListenerPos.x = x;
	vListenerPos.y = y;
	vListenerPos.z = z;

	listener.OrientFront.x = xFront;
	listener.OrientFront.y = yFront;
	listener.OrientFront.z = zFront;

	listener.OrientTop.x = xTop;
	listener.OrientTop.y = yTop;
	listener.OrientTop.z = zTop;

}

void			CSoundManager::UpdateAudio(float fElapsedTime)
{
	if (!bInitialized)
		return;

	if (nFrameToApply3DAudio == 0)
	{
		D3DXVECTOR3 listenerPos = *(D3DXVECTOR3*)&(listener.Position);

		// Calculate listener orientation in x-z plane
		if (vListenerPos.x != listenerPos.x
			|| vListenerPos.y != listenerPos.y
			|| vListenerPos.z != listenerPos.z)
		{
			D3DXVECTOR3 vDelta = vListenerPos - listenerPos;

			fListenerAngle = float(atan2(vDelta.x, vDelta.z));

			vDelta.y = 0.0f;
			D3DXVec3Normalize(&vDelta, &vDelta);

			listener.OrientFront.x = vDelta.x;
			listener.OrientFront.y = 0.f;
			listener.OrientFront.z = vDelta.z;
		}

		if (fElapsedTime > 0)
		{
			D3DXVECTOR3 lVelocity = (vListenerPos - listenerPos) / fElapsedTime;
			listener.Position = *(X3DAUDIO_VECTOR*)&vListenerPos;
			listener.Velocity = *(X3DAUDIO_VECTOR*)&lVelocity;

			locked(

				for (AudioPathList::iterator it = m_audiopaths.GetIterator(); !it.EndOfList(); ++it)
				{
					CAudioPath*pPath = it;

					if (pPath && pPath->pSourceVoice && pPath->bIs3D)
					{
						D3DXVECTOR3 vEmiterPosScaled = pPath->vEmitterPos / UNIT_SCALE;
						D3DXVECTOR3 eVelocity = (vEmiterPosScaled - *(D3DXVECTOR3*)&pPath->emitter.Position) / fElapsedTime;
						pPath->emitter.Position = *(X3DAUDIO_VECTOR*)&vEmiterPosScaled;
						pPath->emitter.Velocity = *(X3DAUDIO_VECTOR*)&eVelocity;


						UpdateAudioPath(pPath);
					}
				}
			);
		}
	}

	++nFrameToApply3DAudio;
	nFrameToApply3DAudio %= 2;
}

void			CSoundManager::UpdateAudioPath(CAudioPath*pPath)
{
	lockit(pPath,

		IXAudio2SourceVoice* voice = pPath->pSourceVoice;

	if (voice)
	{
		voice->SetVolume(m_flVolume);

		if (pPath->bIs3D)
		{
			const DWORD dwCalcFlags = X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER
				| X3DAUDIO_CALCULATE_LPF_DIRECT | X3DAUDIO_CALCULATE_LPF_REVERB
				| X3DAUDIO_CALCULATE_REVERB;

			X3DAudioCalculate(x3DInstance, &listener, &pPath->emitter, dwCalcFlags,
				&pPath->dspSettings);

			voice->SetFrequencyRatio(pPath->dspSettings.DopplerFactor);
			voice->SetOutputMatrix(pMasteringVoice, INPUTCHANNELS, nChannels,
				pPath->matrixCoefficients);
		}
	}
	);
}

void			CSoundManager::LoadSoundSegments()
{
	sndClick.LoadSegmentFromFile(this, CString(L"media\\audio\\click.wav"));
	sndSnaking.LoadSegmentFromFile(this, CString(L"media\\audio\\snaking.wav"));
	sndHit.LoadSegmentFromFile(this, CString(L"media\\audio\\hit.wav"));
	sndStart.LoadSegmentFromFile(this, CString(L"media\\audio\\start.wav"));

	sndBite1.LoadSegmentFromFile(this, CString(L"media\\audio\\bite1.wav"));
	sndBite2.LoadSegmentFromFile(this, CString(L"media\\audio\\bite2.wav"));
}
