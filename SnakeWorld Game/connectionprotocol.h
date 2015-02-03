#ifndef _CONNECTIONPROTOCOL_H_
#define _CONNECTIONPROTOCOL_H_

#include "binaryio.h"
#include "snakeobject.h"
#include "locker.h"


class CConnectionProtocol: public ILocker
{
public:
	CConnectionProtocol(){}
	virtual ~CConnectionProtocol(){}

	//////////////////////////////////////////////////////////////////////////
	// data packets
	//

	
	void Send_Request(CBinaryWriter*pbw,wchar_t*name, wchar_t*desc, wchar_t*type, wchar_t*texture, unsigned int lng);
	void Send_IDontKnow(CBinaryWriter*pbw, unsigned int unknownSnakeId);
	void Send_AppleEaten(CBinaryWriter*pbw, unsigned int appleId);
	void Send_ChatMesasage(CBinaryWriter*pbw, CString&targetuser, CString&message );

protected:

	
};


#endif//_CONNECTIONPROTOCOL_H_