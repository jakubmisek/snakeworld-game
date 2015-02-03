#ifndef _GAMEWORLD_H_
#define _GAMEWORLD_H_

#include "hashedlist.h"
#include "snakeobject.h"
#include "appleobject.h"

#include "connection.h"
#include "connectionprotocol.h"



//
#define SNAKEID_HASH_SIZE	(0x20)
typedef CHashedList<CSnake,SNAKEID_HASH_SIZE>	SnakeList;

#define APPLEID_HASH_SIZE	(0x80)
typedef CHashedList<CApple,APPLEID_HASH_SIZE>	AppleList;

//
class CGameWorld: public CCommandsObserver, public CConnectionProtocol
{
public:
	CGameWorld();
	virtual ~CGameWorld();

	// one frame move
	void	FrameMove( double dStep );
	
	// create new snake object for this world
	virtual CSnake*	NewSnake( bool bUserControlled );

	// create new apple object for this world
	virtual CApple*	NewApple( double x, double y );

	//
	// connection observer
	//
	virtual	void	OnConnectionCommand( const unsigned char uCmdId, CBinaryReader*pbr );	// command received
	virtual	void	OnConnectionConnected( CConnection*pConnection, CString*message );					// connection established / closed

	//
	// users info
	//
	inline	void	SetUsersName( CString&strName )
	{
		m_strUsersName = strName;
	}
	inline	void	SetUsersEmail( CString&strEmail )
	{
		m_strUsersEmail = strEmail;
	}
	inline	void	SetUsersPassword( CString&strPassword )
	{
		m_strUsersPassword = strPassword;
	}
	inline	void	SetUsersDescription( CString&strDescription )
	{
		m_strUsersDescription = strDescription;
	}
	inline	void	SetUsersLanguage( unsigned int LngId )
	{
		m_iLanguageId = LngId;
	}
	inline	void	SetUsersDefaultTexture( CString&strTextureFile )
	{
		m_strUsersDefaultTexture = strTextureFile;
	}
	inline	void	SetWorldName( CString&strWorldName )
	{
		m_strWorldName = strWorldName;
	}

protected:

	//
	// world objects
	//
	CBoundRect	m_boundWorld;			// world size
	double		m_dWorldRefreshDistance;// objects in this distance will be received by server, this is the maximum view distance too

	SnakeList	m_snakes;		// snakes

	CSnake		*m_pUserSnake;	// pointer to the users snake

	// apples
	AppleList	m_apples;		// scene apples

	// current snake info
	CString		m_strUsersName, m_strUsersEmail, m_strUsersPassword, m_strUsersDescription, m_strUsersDefaultTexture;	// users info
	CString		m_strWorldName;
	unsigned int	m_iLanguageId;		// language id (LANGID)

	// best snake score
	CString		m_strBestSnakeName;	// current best snake name
	double		m_dBestSnakeLength;	// current best snake length


	// world frame move
	virtual void	DoSnakesMove( double dStep );	// make a move with the snakes

	void		TestCollisions();	// try to find collision between the users snake and the world

	void		RemoveFarObjects();	// remove all objects over the world-refresh-distance

	void		EatApple( unsigned int uAppleId, CApple*pApple );	// eat apple, add points, invalidate the apple, send "apple eaten"

	//
	// current connection to the server
	//
	CConnection *m_pConnection;
	
	//
	// snake just being received from the server
	//
	CSnake		*m_pCurReceivingSnake;
	unsigned int m_iCurReceivingSnakeId;

	//
	// events
	//
	virtual void OnSceneDefReceived(){}		// the scene size and refresh distance was initialized by the server
	virtual void OnUserSnakeInitialized(){}	// users snake is initialized
	virtual void OnDead( CSnake*pKilledSnake, CSnake*pKilledBy ){}	// snake killed by you
	virtual void OnMessage( CString&str ){}	// message from server
	virtual void OnChatMessage( CString&strUserName, CString&strMessage ){}	// chat message from the user

	//
	// on receive command
	//
	void Receive_ReceivingSnakeId(CBinaryReader*pbr);
	void Receive_Dir(CBinaryReader*pbr);
	void Receive_Seg(CBinaryReader*pbr);
	void Receive_Head(CBinaryReader*pbr);
	void Receive_Length(CBinaryReader*pbr);
	void Receive_Speed(CBinaryReader*pbr);
	void Receive_Dead(CBinaryReader*pbr);
	void Receive_Name(CBinaryReader*pbr);
	void Receive_Description(CBinaryReader*pbr);
	void Receive_Language(CBinaryReader*pbr);
	void Receive_Texture(CBinaryReader*pbr);
	void Receive_Type(CBinaryReader*pbr);
	void Receive_BlockEnd(CBinaryReader*pbr);
	void Receive_NewSnake(CBinaryReader*pbr);
	void Receive_YourSnake(CBinaryReader*pbr);
	void Receive_Server(CBinaryReader*pbr);
	void Receive_BestSnake(CBinaryReader*pbr);
	void Receive_Scene(CBinaryReader*pbr);
	void Receive_Error(CBinaryReader*pbr);
	void Receive_Apple(CBinaryReader*pbr);
	void Receive_ChatMessage(CBinaryReader*pbr);
};

#endif//_GAMEWORLD_H_