#ifndef _DOWNLOADER_H_
#define _DOWNLOADER_H_

#include "threadedloader.h"
#include "list.h"
#include "locker.h"

class	CDownloader:ILocker
{
public:
	CDownloader();
	virtual ~CDownloader();

	void	SetAddress( char*szaddress, int iport );
	
	bool	DownloadFile( LPCWSTR szFileName );

protected:

	char	m_szaddress[16];
	int		m_iport;

	struct	SDownloadingItem
	{
		LPCWSTR szFileName;
		CList<bool>	waiting;
	};

	CList<SDownloadingItem>	m_downloading;

	bool	FileExists( LPCWSTR szFileName );

};


#endif//_DOWNLOADER_H_
