#include "connection.h"
#include <stdio.h>


//////////////////////////////////////////////////////////////////////////
// data receiving thread start routine
DWORD WINAPI ThreadedConnectionReceiveStartRoutine( LPVOID param )
{
	// connection
	CConnection*me = (CConnection*)param;

	if ( !me )
		return -1;

	// connect
	if ( me->DoConnect() )
	{
		// receive data until connection is closed
		while ( me->RefillBuffer() > 0 )
		{
			me->ParseCommands();
		}
	}

	// thread end (!!!)
	me->m_thread = NULL;

	// connection is closed
	me->Disconnect();

	// ok
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// connection ctor
CConnection::CConnection()
{
	m_bIsConnected = false;
	
	m_receivebufferlength = 0;
	m_sendbufferlength = 0;

	m_thread = NULL;

}


//////////////////////////////////////////////////////////////////////////
// connection dtor
CConnection::~CConnection()
{
	Disconnect();
}


//////////////////////////////////////////////////////////////////////////
// connect
bool	CConnection::Connect( char*szAddress, int iPort, /*HWND hReceiveWnd, UINT uReceiveMsg,*/ CCommandsObserver*pObserver, bool bThreaded )
{
	Disconnect();	// close the connection if its open

	if ( !szAddress )
		return false;

	// update connection state
	m_pObserver = pObserver;
	m_bIsConnected = true;
	
	// zero buffers
	m_receivebufferlength = 0;
	m_sendbufferlength = 0;

	// target address
	SockAddrIn.sin_family = AF_INET;
	SockAddrIn.sin_addr.s_addr = inet_addr( szAddress );
	SockAddrIn.sin_port = htons(iPort);

	// create the socket (TCP)
	m_sock = socket( AF_INET, SOCK_STREAM, 0 );

	if ( bThreaded )
	{
		m_thread = CreateThread( NULL, 0, ThreadedConnectionReceiveStartRoutine, this, 0, NULL );

		if ( !m_thread )
			return false;
	}
	else
	{
		if ( !DoConnect() )
			return false;
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////
// connect
bool	CConnection::DoConnect()
{
	// connect
	int iState = connect( m_sock, (sockaddr*)&SockAddrIn, sizeof(SockAddrIn) );

	if (iState != 0)
	{	// connecting error
		closesocket(m_sock);
		return false;
	}

	// send data immediately
	BOOL bTcpNoDelayVal = TRUE;
	int rr = setsockopt( m_sock, IPPROTO_TCP, TCP_NODELAY, (char*)&bTcpNoDelayVal, sizeof(bTcpNoDelayVal) );

	// done
	if (m_pObserver)
			m_pObserver->OnConnectionConnected(this, 0);

	return true;
}


//////////////////////////////////////////////////////////////////////////
// disconnect and pass some message to the observer
void	CConnection::Disconnect( CString*message )
{
	if (m_bIsConnected)
	{
		if (m_pObserver)	// disconnect event
			m_pObserver->OnConnectionConnected(0, message);

		closesocket( m_sock );	// close the socket
		m_bIsConnected = false;	// update the state
	}

	// wait for thread end
	while (m_thread)
	{
		Sleep( 10 );
	}
}


//////////////////////////////////////////////////////////////////////////
// read waiting data into the buffer
int		CConnection::RefillBuffer()
{
	if ( m_bIsConnected )
	{
		// blocking receive
		int cnt = recv( m_sock, m_receivebuffer + m_receivebufferlength, CONNECTION_BUFF_SIZE - m_receivebufferlength, 0 );

		if (cnt > 0)
		{
			m_receivebufferlength += cnt;
		}

		return cnt;
	}
	else
	{
		return 0;
	}
	
}


//////////////////////////////////////////////////////////////////////////
// process data from the connected socket
int	CConnection::ParseCommands()
{
	if (m_pObserver && m_bIsConnected)
	while(m_receivebufferlength>0)
	{
		int err;
		unsigned char uCmdId = ReadByte(&err);

		if ( !err )
			m_pObserver->OnConnectionCommand( uCmdId, this );
	}
	
	return 0;
}


//////////////////////////////////////////////////////////////////////////
// loose the first bytes of the receive buffer
void	CConnection::moveReceiveBuffer(int bytes)
{
	if (bytes == m_receivebufferlength)
	{
		m_receivebufferlength = 0;
	}
	else if (bytes > m_receivebufferlength)
	{
		DebugBreak();
	}
	else
	{
		memmove( m_receivebuffer, m_receivebuffer + bytes, m_receivebufferlength - bytes );

		m_receivebufferlength -= bytes;
	}
}


//////////////////////////////////////////////////////////////////////////
// CBinaryReader
//////////////////////////////////////////////////////////////////////////


unsigned char	CConnection::ReadByte(int*ierr)		// read 1 byte
{
	if (ierr)	*ierr = 0;

	while ( m_receivebufferlength < 1 )
	{
		if ( RefillBuffer() <= 0 )
		{
			if ( ierr )	*ierr = -1;
			return 0;
		}
	}

	unsigned char val = *(unsigned char*)m_receivebuffer;

	moveReceiveBuffer(1);

	return val;
}
unsigned int	CConnection::ReadUInt32(int*ierr)	// read 4 bytes
{
	if (ierr)	*ierr = 0;

	while ( m_receivebufferlength < 4 )
	{
		if ( RefillBuffer() <= 0 )
		{
			if ( ierr )	*ierr = -1;
			return 0;
		}
	}

	unsigned int val = *(unsigned int*)m_receivebuffer;

	moveReceiveBuffer(4);

	return val;
}
double			CConnection::ReadDouble(int*ierr)	// read 8 bytes
{
	if (ierr)	*ierr = 0;

	while ( m_receivebufferlength < 8 )
	{
		if ( RefillBuffer() <= 0 )
		{
			if ( ierr )	*ierr = -1;
			return 0.0;
		}
	}

	double val = *(double*)m_receivebuffer;

	moveReceiveBuffer(8);

	return val;
}
float			CConnection::ReadFloat(int*ierr)	// read 4 bytes
{
	if (ierr)	*ierr = 0;

	while ( m_receivebufferlength < 4 )
	{
		if ( RefillBuffer() <= 0 )
		{
			if ( ierr )	*ierr = -1;
			return 0.0f;
		}
	}

	float val = *(float*)m_receivebuffer;

	moveReceiveBuffer(4);

	return val;
}
signed int		CConnection::ReadInt(int*ierr)		// read 4 bytes
{
	if (ierr)	*ierr = 0;

	while ( m_receivebufferlength < 4 )
	{
		if ( RefillBuffer() <= 0 )
		{
			if ( ierr )	*ierr = -1;
			return 0;
		}
	}

	int val = *(int*)m_receivebuffer;

	moveReceiveBuffer(4);

	return val;
}
void			CConnection::Read( char*buff, int len, int*ierr )	// read "len" bytes into the input buffer "buff"
{
	if (ierr)	*ierr = 0;

	while ( m_receivebufferlength < len )
	{
		if ( RefillBuffer() <= 0 )
		{
			if ( ierr )	*ierr = -1;
			return;
		}
	}

	memcpy( buff, m_receivebuffer, len );
	moveReceiveBuffer(len);
}


void			CConnection::ReadString( CString&str, int*ierr )
{
	if (ierr)	*ierr = 0;

	int i = 0;	// char position (1 byte step)

	// find the terminating wchar_t character
	while(true)
	{
		while ( (int)(i + sizeof(wchar_t)) > m_receivebufferlength  )
		{
			if ( RefillBuffer() <= 0 )
			{
				if ( ierr )	*ierr = -1;
				return;
			}
		}

		while ( (int)(i + sizeof(wchar_t)) <= m_receivebufferlength )
		{
			if ( *(wchar_t*)(m_receivebuffer+i) == 0 )
				break;

			i += sizeof(wchar_t);
		}

		//
		if ( (int)(i + sizeof(wchar_t)) <= m_receivebufferlength )
			break;
	};

	// read the string
	str = (wchar_t*)m_receivebuffer;
	moveReceiveBuffer( i + sizeof(wchar_t) );
}


//////////////////////////////////////////////////////////////////////////
// CBinaryWriter
//////////////////////////////////////////////////////////////////////////


int	 CConnection::Write( char*buff, int len )	// writes the input buffer "buff" of size "len"
{
	//return ( m_bIsConnected ? send(m_sock,buff,len,0) : (-1) );
	return WriteToBuff( buff,len );
}
int	 CConnection::Write( unsigned char val )// write 1 byte
{
	//return ( m_bIsConnected ? send(m_sock,(char*)&val,1,0) : (-1) );
	return WriteToBuff( (char*)&val,1 );
}
int	 CConnection::Write( unsigned int val )	// write 4 bytes
{
	//return ( m_bIsConnected ? send(m_sock,(char*)&val,4,0) : (-1) );
	return WriteToBuff( (char*)&val,4 );
}
int	 CConnection::Write( double val )		// write 8 bytes
{
	//return ( m_bIsConnected ? send(m_sock,(char*)&val,8,0) : (-1) );
	return WriteToBuff( (char*)&val,8 );
}
int	 CConnection::Write( float val )		// write 4 bytes
{
	//return ( m_bIsConnected ? send(m_sock,(char*)&val,4,0) : (-1) );
	return WriteToBuff( (char*)&val,4 );
}
int	 CConnection::Write( signed int val )	// write 4 bytes
{
	//return ( m_bIsConnected ? send(m_sock,(char*)&val,4,0) : (-1) );
	return WriteToBuff( (char*)&val,4 );

}
int  CConnection::Write( wchar_t*str )		// write 2 bytes * (string length + 1)
{
	if ( !str)
		str = L"";

	wchar_t *p = str;

	while (true)
	{
		WriteToBuff( (char*)p, sizeof(wchar_t) );
		//send(m_sock,(char*)p,sizeof(wchar_t),0);

		if (*p == 0)
			break;

		++p;
	};

	return (int)p - (int)str;
}
void	CConnection::Flush()	// flushes the send buffer
{
	if ( m_bIsConnected && m_sendbufferlength > 0 )
	{
		locked(

			send(m_sock,m_sendbuffer,m_sendbufferlength,0);
			m_sendbufferlength = 0;

		);
	}
}
int	CConnection::WriteToBuff( char*p, int n )
{
	if ( !m_bIsConnected || n < 0 || !p )
		return -1;

	locked(

		if ( m_sendbufferlength + n <= CONNECTION_BUFF_SIZE )
		{
			memcpy( m_sendbuffer + m_sendbufferlength, p, n );
			m_sendbufferlength += n;
		}
		else
		{
			Flush();

			if ( n > CONNECTION_BUFF_SIZE )
			{
				send(m_sock,p,n,0);
			}
			else
			{
				memcpy( m_sendbuffer, p, n );
				m_sendbufferlength = n;
			}
		}

	);
	

	return n;
}