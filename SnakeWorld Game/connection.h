#ifndef _CONNECTION_H_
#define _CONNECTION_H_

//

#pragma comment (lib,"wsock32.lib")

#include <WinSock.h>

#define CONNECTION_BUFF_SIZE	(2048)
//

#include "binaryio.h"
#include "locker.h"

#include "vector.h"
#include "boundrect.h"

//

class CConnection;
class CCommandsObserver;

DWORD WINAPI ThreadedConnectionReceiveStartRoutine( LPVOID param );

//////////////////////////////////////////////////////////////////////////
// commands observer

class CCommandsObserver
{
public:
	virtual void OnConnectionCommand( const unsigned char uCmdId, CBinaryReader*pbr ) = 0;	// new command received
	virtual void OnConnectionConnected( CConnection*pConnection, CString*message ) = 0;		// connection connected / closed
};


//////////////////////////////////////////////////////////////////////////
// connection

class CConnection: public CBinaryReader, public CBinaryWriter, public ILocker
{
public:
	CConnection();
	virtual ~CConnection();

	//
	// Connecting
	//

	virtual bool	Connect( char*szAddress, int iPort, /*HWND hReceiveWnd = 0, UINT uReceiveMsg = 0,*/ CCommandsObserver*pObserver = 0, bool bThreaded = false );
	virtual void	Disconnect( CString*message = 0 );

	// parse the commands and process them
	virtual int		RefillBuffer();
			int		ParseCommands();

	inline char* GetReceiveBuffer( int*pn ){ *pn = m_receivebufferlength; return m_receivebuffer; }
	inline void ClearReceiveBuffer( int n ){ moveReceiveBuffer(n); }


	//
	// CBinaryReader
	//
	virtual unsigned char	ReadByte(int*ierr);		// read 1 byte
	virtual unsigned int	ReadUInt32(int*ierr);	// read 4 bytes
	virtual double			ReadDouble(int*ierr);	// read 8 bytes
	virtual float			ReadFloat(int*ierr);	// read 4 bytes
	virtual signed int		ReadInt(int*ierr);		// read 4 bytes
	virtual void			Read( char*buff, int len, int*ierr );	// read "len" bytes into the input buffer "buff"
	virtual void			ReadString( CString&str, int*ierr );	// read zero terminated string


	//
	// CBinaryWriter
	//
	virtual int Write( char*buff, int len );	// writes the input buffer "buff" of size "len"
	virtual int Write( unsigned char val );// write 1 byte
	virtual int Write( unsigned int val );	// write 4 bytes
	virtual int Write( double val );		// write 8 bytes
	virtual int Write( float val );		// write 4 bytes
	virtual int Write( signed int val );	// write 4 bytes
	virtual int Write( wchar_t*str );	// write zero terminated string

	virtual void Flush();	// flushes the write buffer

protected:

			SOCKET	m_sock;			// connection

			SOCKADDR_IN SockAddrIn;
	virtual bool	DoConnect();	// socket.connect() using SockAddrIn

			bool	m_bIsConnected;	// connection is established
	
			char	m_receivebuffer[CONNECTION_BUFF_SIZE];
			int		m_receivebufferlength;
			void	moveReceiveBuffer(int bytes);

			char	m_sendbuffer[CONNECTION_BUFF_SIZE];
			int		m_sendbufferlength;
	virtual int		WriteToBuff( char*p, int n );

			CCommandsObserver*m_pObserver;

private:

	friend DWORD WINAPI ThreadedConnectionReceiveStartRoutine( LPVOID param );
	HANDLE	m_thread;

};

#endif//_CONNECTION_H_