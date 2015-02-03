#include "soundmanager.h"


CSoundSegment::CSoundSegment()
{
	m_pManager = NULL;

	pbSampleData = NULL;
	cbWaveSize = 0;


}
CSoundSegment::~CSoundSegment()
{
	FreeSegment();
}
//////////////////////////////////////////////////////////////////////////
void		CSoundSegment::FreeSegment()
{
	m_pManager = NULL;

	SAFE_DELETE_ARRAY( pbSampleData );
	cbWaveSize = 0;
}
bool		CSoundSegment::LoadSegmentFromFile( CSoundManager*pManager, CString&szFileName )
{
	if ( !pManager || !pManager->IsInitialized()  )
		return false;

	m_pManager = pManager;

	//
    // Read in the wave file
    //
    if (FAILED( wav.Open( szFileName, NULL, WAVEFILE_READ ) ))
		return false;

    // Get format of wave file
    WAVEFORMATEX* pwfx = wav.GetFormat();

    // Calculate how many bytes and samples are in the wave
    cbWaveSize = wav.GetSize();

    // Read the sample data into memory
    SAFE_DELETE_ARRAY( pbSampleData );

    pbSampleData = new BYTE[ cbWaveSize ];

    if (FAILED( wav.Read( pbSampleData, cbWaveSize, &cbWaveSize ) ))
		return false;

    return true;
}