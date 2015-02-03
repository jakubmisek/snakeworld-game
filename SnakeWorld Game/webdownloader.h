#ifndef _WEBDOWNLOADER_H_
#define _WEBDOWNLOADER_H_

#include "threadedloader.h"
#include "web.h"

using namespace openutils;

class CWebDownloaderObserver
{
public:
	virtual void OnDownloaded( char*pdata, unsigned long ndata ) = 0;
};

class CWebDownloader: public IThreadedLoader
{
public:

	CWebDownloader();
	virtual~CWebDownloader();

	void StartDownload( CWebDownloaderObserver*pObserver, const char*szHost, const char*szScript );

protected:

	virtual	DWORD DoLoading();	// the main loading process

	WebForm	m_web;

	CWebDownloaderObserver*m_pObserver;
};

#endif//_WEBDOWNLOADER_H_