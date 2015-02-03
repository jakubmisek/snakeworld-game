#ifndef _CONNECTIONLOCAL_H_
#define _CONNECTIONLOCAL_H_

#include "connection.h"

//////////////////////////////////////////////////////////////////////////
// "local" server connection
// looks like a connection to remote server,
// but this connection only keeps one snake working
class CConnectionLocal: public CConnection
{
public:
	CConnectionLocal();
	virtual ~CConnectionLocal();

	virtual bool Connect( char*szAddress, int iPort, CCommandsObserver*pObserver /* = 0 */, bool bThreaded /* = false */ );
	virtual void Disconnect( CString*message /* = 0 */ );

	virtual int  RefillBuffer();

	virtual void Flush();	// flushes the write buffer

protected:

	virtual int	 WriteToBuff( char*p, int n );
};

#endif//_CONNECTIONLOCAL_H_