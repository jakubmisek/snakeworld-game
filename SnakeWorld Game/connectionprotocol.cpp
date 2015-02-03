#include "connectionprotocol.h"

#include "connectiondata.h"


//////////////////////////////////////////////////////////////////////////
// sending to binary writer
//////////////////////////////////////////////////////////////////////////
void CConnectionProtocol::Send_IDontKnow(CBinaryWriter*pbw, unsigned int unknownSnakeId)
{
	if ( !pbw )	return;

	locked(

	pbw->Write( (unsigned char)CMD_IDONTKNOW );

	CmdSnakeId data;
	data.iId = unknownSnakeId;
	pbw->Write( (char*)&data, 4 );

	// FLUSH !!
	pbw->Flush();

	);
}
void CConnectionProtocol::Send_Request(CBinaryWriter*pbw,wchar_t*name,wchar_t*email,wchar_t*password, wchar_t*desc, wchar_t*type, wchar_t*texture, unsigned int lng)
{
	if ( !pbw )	return;

	locked(
	// "NEW SNAKE" COMMAND 
	pbw->Write( CMD_NEWGAMECMD );

	if (email != 0 && password != 0 && wcslen(email) > 0 && wcslen(password) > 0)
	{	// authorized login
		pbw->Write( (unsigned char)CMD_LOGIN );
		pbw->Write( email );
		pbw->Write( password );
	}
	else
	{	// anonymous login
		// NAME
		pbw->Write( (unsigned char)CMD_NAME );
		pbw->Write( name );
	}

	// DESCIPTION
	pbw->Write( (unsigned char)CMD_DESCRIPTION );
	pbw->Write( desc );

	// TYPE
	pbw->Write( (unsigned char)CMD_TYPE );
	pbw->Write( type );

	// TEXTURE
	pbw->Write( (unsigned char)CMD_TEXTURE );
	pbw->Write( texture );

	// LANGUAGE
	pbw->Write( (unsigned char)CMD_LANGUAGE );
	pbw->Write( (unsigned int)lng );	// 4 bytes language id

	// done
	pbw->Write( (unsigned char)CMD_BLOCKEND );

	// FLUSH !!
	pbw->Flush();

	);
}
void CConnectionProtocol::Send_AppleEaten(CBinaryWriter*pbw, unsigned int appleId)
{
	if ( !pbw )	return;

	locked(

	// "APPLE EATEN" COMMAND
	pbw->Write( (unsigned char)CMD_APPLEEATEN );	// 1B

	// APPLE ID
	pbw->Write( (unsigned int)appleId );			// 4B

	// FLUSH !!
	pbw->Flush();

	);
}
void CConnectionProtocol::Send_ChatMesasage(CBinaryWriter *pbw, CString&targetuser, CString &message)
{
	if ( !pbw )	return;

	locked(

	// "CHAT MESSAGE" COMMAND
	pbw->Write( (unsigned char)CMD_CHATMESSAGE );	// 1B

	// target users
	pbw->Write( targetuser );		// string

	// message
	pbw->Write( message );			// string

	// FLUSH !!
	pbw->Flush();

	);
}