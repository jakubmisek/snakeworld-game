#include "connectionlocal.h"

//////////////////////////////////////////////////////////////////////////
#define WORLD_SIZE		(100.0)
#define APPLES_COUNT	(30)


//////////////////////////////////////////////////////////////////////////
// ctor
CConnectionLocal::CConnectionLocal()
{

}


//////////////////////////////////////////////////////////////////////////
// dtor
CConnectionLocal::~CConnectionLocal()
{

}


//////////////////////////////////////////////////////////////////////////
// init
bool CConnectionLocal::Connect(char *szAddress, int iPort, CCommandsObserver *pObserver, bool bThreaded)
{
	// address and port are ignored

	Disconnect(0);	// close the connection if its open

	// update connection state
	m_pObserver = pObserver;
	m_bIsConnected = true;

	// zero buffers
	m_receivebufferlength = 0;
	m_sendbufferlength = 0;

	// create the socket (TCP)
	m_sock = -1;

	// done
	if (m_pObserver)
		m_pObserver->OnConnectionConnected(this, 0);

	return true;
}


//////////////////////////////////////////////////////////////////////////
// close
void CConnectionLocal::Disconnect( CString*message )
{
	if (m_bIsConnected)
	{
		if (m_pObserver)	// disconnect event
			m_pObserver->OnConnectionConnected(0, message);

		m_sock = -1;
		m_bIsConnected = false;	// update the state
	}
}


//////////////////////////////////////////////////////////////////////////
//
int	CConnectionLocal::RefillBuffer()
{
	// do nothing

	return 0;
}


//////////////////////////////////////////////////////////////////////////
// called after data block send
void CConnectionLocal::Flush()
{
	// process & clear send data (locked)
	// [init world on new game, add apple on apple eaten, close connection on snake dead]

	// fill receive buffer

	// call the observer
	ParseCommands();
}


//////////////////////////////////////////////////////////////////////////
// send data through the connection
int	CConnectionLocal::WriteToBuff( char*p, int n )
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
			DebugBreak();// buffer too small
		}

	);


	return n;
}