#include "downloader.h"
#include "connection.h"
#include "connectiondata.h"

//////////////////////////////////////////////////////////////////////////
// downloader ctor
CDownloader::CDownloader()
{
	m_iport = 0;
	m_szaddress[0] = 0;
}


//////////////////////////////////////////////////////////////////////////
// downloader dtor
CDownloader::~CDownloader()
{
	locked(
		m_downloading.Clear(false);
	);
}


//////////////////////////////////////////////////////////////////////////
// set new address to download from
void	CDownloader::SetAddress( char*szaddress, int iport )
{
	m_iport = iport;
	if (szaddress)
		strcpy_s( m_szaddress, 16, szaddress );
	else
		m_szaddress[0] = 0;
}


//////////////////////////////////////////////////////////////////////////
// download file from the specified address
bool	CDownloader::DownloadFile( LPCWSTR szFileName )
{
	// downloading item
	SDownloadingItem	item;
	item.szFileName = szFileName;

	bool	bWaiting = false;

	//
	bool bFileExists = false, bFileIsInQueue = false;

	//
	//
	//

	locked(

		// check if the file already exists
		if (FileExists(szFileName))
		{
			bFileExists = true;
		}
		else
		{
			// check if the file is already been downloading
			SDownloadingItem*pItem;

			for ( CList<SDownloadingItem>::iterator it = m_downloading.GetIterator();!it.EndOfList();++it )
			{
				pItem = it;

				if (pItem && pItem->szFileName)
				{
					if ( wcscmp( szFileName, pItem->szFileName ) == 0 )
					{	// file is downloading now, wait for download and return
						bFileIsInQueue = true;
						break;
					}
				}
			}

			if ( !bFileIsInQueue )
			{
				m_downloading.Add( &item );
			}
			else
			{
				bWaiting = true;
				pItem->waiting.Add( &bWaiting );
			}
		}

	);

	//
	//
	//

	// file exists
	if (bFileExists)
		return true;


	if (bFileIsInQueue)
	{
		// wait
		while (bWaiting);

		return true;
	}

	HANDLE hFile = NULL;
	int nWrittenBytes = 0;

	// create connection and download file
	CConnection	con;
	if ( con.Connect( m_szaddress, m_iport ) )
	{
		// "GET TEXTURE" COMMAND 
		con.Write( CMD_GETTEXTURECMD );
		con.Write( (wchar_t*)szFileName );
		// FLUSH !
		con.Flush();

		// create the file
		hFile = CreateFile( szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

		if (hFile != INVALID_HANDLE_VALUE) 
		{
			// receive the file content
			while (con.RefillBuffer() > 0)
			{
				int nBytes;
				char*pBytes;
			
				pBytes = con.GetReceiveBuffer( &nBytes );

				// write data
				DWORD written;
				if (!WriteFile( hFile, pBytes, nBytes, &written, NULL ) || (int)written < nBytes)
				{
					// error
					nWrittenBytes = -1;
					break;
				}

				nWrittenBytes += nBytes;

				con.ClearReceiveBuffer(nBytes);
			}
		}

		con.Disconnect();
	}
	

	//
	//
	//

	
	locked(

		// close the file
		if (hFile)
		{
			CloseHandle(hFile);// close the file
			if (nWrittenBytes <= 0)DeleteFile(szFileName);	// delete file if error
		}

		// remove from the queue
		if ( !bFileIsInQueue )
		{
			// signal end of downloading
			for ( CList<bool>::iterator it = item.waiting.GetIterator();!it.EndOfList();++it )
				*it = false;
			item.waiting.Clear(false);
			
			// remove downloading item from the queue
			m_downloading.Remove( &item, false );
		}

	);

	return (nWrittenBytes>0);
}


//////////////////////////////////////////////////////////////////////////
// file exists ?
bool	CDownloader::FileExists( LPCWSTR szFileName )
{
	HANDLE hFile;

	// try to open the file
	hFile = CreateFile( szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if (hFile == INVALID_HANDLE_VALUE) 
	{	// file does not exists or cannot be open for reading
		return false;
	}

	// close the file
	CloseHandle(hFile);

	return true;
}