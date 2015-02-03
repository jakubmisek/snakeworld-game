#include "webdownloader.h"

CWebDownloader::CWebDownloader()
{
	m_pObserver = 0;
}


CWebDownloader::~CWebDownloader()
{
	OnDestruct(true);
}


void	CWebDownloader::StartDownload( CWebDownloaderObserver*pObserver, const char*szHost, const char*szScript )
{
	m_web.setHost( szHost );
	m_web.setScriptFile( szScript );

	m_pObserver = pObserver;

	StartThread();
}


DWORD	CWebDownloader::DoLoading()
{
	if ( !m_pObserver )
		return 0;

	try
	{
		m_web.sendRequest();

		char responsebuffer[65536];

		if ( m_web.getResponse( responsebuffer, 65535 ) )
		{
			m_pObserver->OnDownloaded( responsebuffer, m_web.getBytesRead() );

			return 0;
		}
	}
	catch( WebFormException ex )
	{
		
	}

	// else
	m_pObserver->OnDownloaded( 0, 0 );

	return -1;
}