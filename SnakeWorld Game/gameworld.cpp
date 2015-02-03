#include "gameworld.h"
#include "connectiondata.h"

//////////////////////////////////////////////////////////////////////////
//

#define SNAKE_LENGTH_ADD_FOR_KILL	(4.0)
#define	SNAKE_LENGTH_ADD_FOR_APPLE	(3.0)

#define WORLD_DEFAULT_SIZE	(10000.0)
#define WORLD_DEFAULT_REFRESH_DISTANCE	(200.0)

//////////////////////////////////////////////////////////////////////////
// world ctor
CGameWorld::CGameWorld()
	: m_boundWorld( -WORLD_DEFAULT_SIZE/2.0, -WORLD_DEFAULT_SIZE/2.0, WORLD_DEFAULT_SIZE/2.0, WORLD_DEFAULT_SIZE/2.0 )
	, m_dWorldRefreshDistance(WORLD_DEFAULT_REFRESH_DISTANCE)
{
	m_pConnection = 0;

	m_pUserSnake = 0;

	m_pCurReceivingSnake = 0;
	m_iCurReceivingSnakeId = -1;

	m_iLanguageId = 0;

	m_dBestSnakeLength = 0.0;
}


//////////////////////////////////////////////////////////////////////////
// world dtor
CGameWorld::~CGameWorld()
{
	if (m_pConnection)
		m_pConnection->Disconnect();

	locked(
		// remove all snakes and delete them
		m_snakes.Clear( true );

		// remove all apples and delete them
		m_apples.Clear( true );
	);
}


//////////////////////////////////////////////////////////////////////////
// move with world objects
void	CGameWorld::FrameMove( double dStep )
{
	if (!m_pConnection)
		return;	// do not move without existing connection ! Snakes are not controlled ...

	locked(

		// move with the snakes
		DoSnakesMove(dStep);

		// remove to far objects
		RemoveFarObjects();

		// only with existing users snake
		if (m_pUserSnake && m_pConnection)
		{
			// try to find collision
			TestCollisions();

			// send modifications to server
			m_pUserSnake->SendDirtyValues( m_pConnection );
		}

	);
}


//////////////////////////////////////////////////////////////////////////
// creates new snake object for this world
CSnake*	CGameWorld::NewSnake( bool bUserControlled )
{
	return new CSnake(bUserControlled);
}


//////////////////////////////////////////////////////////////////////////
// creates new apple object for this world
CApple*	CGameWorld::NewApple( double x, double y )
{
	return new CApple(x,y);
}



//////////////////////////////////////////////////////////////////////////
// commands observer
// command name + parameters
void	CGameWorld::OnConnectionCommand( const unsigned char uCmdId, CBinaryReader*pbr )
{
#ifdef DEBUG
	wchar_t debugbuff[128];
	swprintf_s(debugbuff,L"received:(%u) ", (unsigned int)uCmdId);
	OutputDebugStringW( debugbuff );
#endif

	if (pbr)
	switch (uCmdId)
	{
	case CMD_RECEIVING_SNAKE:// snake data will be received
		Receive_ReceivingSnakeId(pbr);
		return;

	case CMD_SNAKE_DIR:// receiving snake head direction
		Receive_Dir(pbr);
		return;

	case CMD_SNAKE_SEG:// receiving snake new segment ( = new head position )
		Receive_Seg(pbr);
		return;

	case CMD_SNAKE_HEAD:// receiving snake head position
		Receive_Head(pbr);
		return;

	case CMD_SNAKE_LENGTH:// receiving snake length
		Receive_Length(pbr);
		return;

	case CMD_SNAKE_SPEED:// receiving snake speed
		Receive_Speed(pbr);
		return;

	case CMD_SNAKE_DEAD:// receiving snake is dead
		Receive_Dead(pbr);
		return;

	case CMD_NAME:
		Receive_Name(pbr);
		return;

	case CMD_DESCRIPTION:
		Receive_Description(pbr);
		return;

	case CMD_LANGUAGE:
		Receive_Language(pbr);
		return;

	case CMD_TEXTURE:
		Receive_Texture(pbr);
		return;

	case CMD_TYPE:
		Receive_Type(pbr);
		return;

	case CMD_BLOCKEND:// snake initializing done
		Receive_BlockEnd(pbr);
		return;

	case CMD_NEWSNAKE:// initializing unknown snake in the world
		Receive_NewSnake(pbr);
		return;

	case CMD_YOURSNAKEID:// initializing my snake
		Receive_YourSnake(pbr);
		return;

	case CMD_SERVER:// server name
		Receive_Server(pbr);
		return;

	case CMD_BESTSNAKE:// best snake info
		Receive_BestSnake(pbr);
		return;

	case CMD_SCENE:// scene size and refresh distance
		Receive_Scene(pbr);
		return;

	case CMD_ERROR:
		Receive_Error(pbr);
		return;

	case CMD_APPLE:
		Receive_Apple(pbr);
		return;

	case CMD_CHATMESSAGE:
		Receive_ChatMessage(pbr);
		return;

	default:	// error, unknown command, terminate the connection
		//if (m_pConnection)
		//	m_pConnection->Disconnect();

		m_pConnection = 0;
		return;
	}
	
}


//////////////////////////////////////////////////////////////////////////
// on connected
void	CGameWorld::OnConnectionConnected( CConnection*pConnection, CString*message )
{
	m_pConnection = pConnection;	// should be NULL

	locked(

		if (m_pConnection)	// just connected
		{
			// remove all snakes
			m_pCurReceivingSnake = 0;
			m_pUserSnake = 0;

			m_snakes.Clear(true);
			
			// request for new game
			Send_Request( m_pConnection, m_strUsersName, m_strUsersEmail,m_strUsersPassword, m_strUsersDescription, L"default", m_strUsersDefaultTexture, m_iLanguageId, m_strWorldName );
		}
		else
		{
			// disconnected
			// kill all snakes (stop sounds)
			SnakeList::CIterator	it = m_snakes.GetIterator();

			while ( !it.EndOfList() )
			{
				CSnake *pSnake = it.Next();

				if ( pSnake )
					pSnake->KillSnake( 0, 0 );
			}
		}

	);
}


//////////////////////////////////////////////////////////////////////////
//
void	CGameWorld::DoSnakesMove(double dStep)
{
	// snakes move
	SnakeList::CIterator	it = m_snakes.GetIterator();

	while ( !it.EndOfList() )
	{
		CSnake *pSnake = it.Next();

		pSnake->Move(dStep);
	}
}


//////////////////////////////////////////////////////////////////////////
// try to find collisions between the users snake and the world
void	CGameWorld::TestCollisions()
{
	//
	// test collision with other snakes
	//
	SnakeList::CIterator	sit = m_snakes.GetIterator();

	while ( !sit.EndOfList() )
	{
		unsigned int snakeid;
		CSnake *pSnake = sit.Next(&snakeid);

		if ( m_pUserSnake->TestCollision( pSnake ) )
		{
			m_pUserSnake->KillSnake( snakeid, m_pConnection );
			OnDead( m_pUserSnake, pSnake );
			break;
		}
	}

	//
	// test collision with the world border
	//
	CVector vecHeadNose = m_pUserSnake->GetHeadPosition() + (m_pUserSnake->GetHeadDirection() * 2.0);
	if ( !m_boundWorld.IsIn( vecHeadNose ) )
	{
		m_pUserSnake->KillSnake( -1, m_pConnection );
		OnDead( m_pUserSnake, 0 );
	}

	//
	// test collision with the apples
	//
	AppleList::CIterator	ait = m_apples.GetIterator();

	while ( !ait.EndOfList() )
	{
		unsigned int appleid;
		CApple *pApple = ait.Next(&appleid);

		if ( m_pUserSnake->TestCollision(pApple) )
		{
			EatApple( appleid, pApple );
		}
	}
}


//////////////////////////////////////////////////////////////////////////
// eat the apple
// add points, invalidate the apple, send "apple eaten" to the server
void	CGameWorld::EatApple(unsigned int uAppleId, CApple *pApple)
{
	pApple->OnEaten();	// invalidate the apple

	m_pUserSnake->LengthAdd( SNAKE_LENGTH_ADD_FOR_APPLE );	// expand me

	Send_AppleEaten( m_pConnection, uAppleId );	// send info to the server
}


//////////////////////////////////////////////////////////////////////////
// remove objects to far from the users snake
// this removed objects must be removed, because server won't send updates to me
void	CGameWorld::RemoveFarObjects()
{
	if (!m_pUserSnake)	return;	// no world center

	// valid world area
	CBoundRect	rectVisible( m_pUserSnake->GetHeadPosition(), m_dWorldRefreshDistance );

	// snakes iterator
	SnakeList::CIterator	it = m_snakes.GetIterator();
	unsigned int iSnakeId;
	
	while ( !it.EndOfList() )
	{
		CSnake *pSnake = it.Next( &iSnakeId );	// next snake object

		if (
			(	pSnake->IsInitialized() &&									// snake is initialized
				!pSnake->GetBoundingRect().IsIntersect( rectVisible ) &&	// out of refreshable world area
				pSnake->GetLastUpdatetime() > 5.0 )							// not updated for 5 sec
			||	( pSnake->GetLastUpdatetime() > 30.0 )						// or snake is not updated for 30 seconds (small cleanup)
			||  ( pSnake->WantToDelete() )									// snake is dead and animation ended
		   )
		{
			// pSnake is out of the area which is updated by the server
			// or pSnake is not updated for 30 seconds
			// so remove pSnake from the world

			if (pSnake == m_pCurReceivingSnake)
			{	// zero the pointer to this snake
				m_pCurReceivingSnake = 0;
			}

			// check if we are not removing the users snake !!
			if (pSnake == m_pUserSnake)
			{	// in case of server did not disconnect the connection
				m_pUserSnake = 0;
			}

			// remove the snake from the world
			// iterator won't be invalidated
			m_snakes.Remove( iSnakeId, true );
			
		}
	}
}


//////////////////////////////////////////////////////////////////////////
// receiving from binary reader
//////////////////////////////////////////////////////////////////////////
void CGameWorld::Receive_ReceivingSnakeId(CBinaryReader*pbr)
{
	CmdSnakeId data;
	int err;
	pbr->Read( (char*)&data, 4, &err );
	if (err)
		return;

	locked(

		m_iCurReceivingSnakeId = data.iId;

		m_pCurReceivingSnake = m_snakes[m_iCurReceivingSnakeId];

		if ( !m_pCurReceivingSnake )
		{	// i don't know this snake !
			Send_IDontKnow( m_pConnection, m_iCurReceivingSnakeId );
			
			m_pCurReceivingSnake = 0;
			//m_iCurReceivingSnakeId = -1; // ID is kept (should be used in DEAD command)
		}

	);
}

void CGameWorld::Receive_Dir(CBinaryReader*pbr)
{
	CmdSnakeDirection data;
	int err;
	pbr->Read( (char*)&data, sizeof(data), &err );
	if ( err )
		return;

	if (m_pCurReceivingSnake)
	{
		m_pCurReceivingSnake->OnDirectionUpdate(data.dAngle);
	}
}

void CGameWorld::Receive_Seg(CBinaryReader*pbr)
{
	//CmdSnakeSegment data;
	//pbr->Read( (char*)&data, 20 /* !!!!!!! */ );

	int err;

	unsigned int segId = pbr->ReadUInt32();

/*#ifdef DEBUG
	wchar_t debugbuff[128];
	swprintf_s(debugbuff,L"\nSegID[%u] ", segId);
	OutputDebugStringW( debugbuff );
#endif*/

	double segX = pbr->ReadDouble();
	double segY = pbr->ReadDouble(&err);
	if (err)
		return;

	if (m_pCurReceivingSnake)
	{
		if ( !m_pCurReceivingSnake->OnSegmentAdd( segId, segX, segY ) )	// moves with head too
		{	// some segments are missing
			if (m_pConnection)
				Send_IDontKnow( m_pConnection, m_iCurReceivingSnakeId );
			
			m_pCurReceivingSnake = 0;	// ignore next info about this snake (wait for "NEWSNAKE" message)
		}
	}
}

void CGameWorld::Receive_Head(CBinaryReader*pbr)
{
	CmdSnakeHead data;
	int err;
	pbr->Read( (char*)&data, sizeof(data), &err );
	if ( err )
		return;

	if (m_pCurReceivingSnake)
	{
		m_pCurReceivingSnake->OnHeadUpdate( data.x, data.y );
	}
}

void CGameWorld::Receive_Length(CBinaryReader*pbr)
{
	CmdSnakeLength data;
	int err;
	pbr->Read( (char*)&data, sizeof(data), &err );
	if (err)
		return;

	if (m_pCurReceivingSnake)
	{
		m_pCurReceivingSnake->OnLengthUpdate( data.dLength );
	}
}

void CGameWorld::Receive_Speed(CBinaryReader*pbr)
{
	CmdSnakeSpeed data;
	int err;
	pbr->Read( (char*)&data, sizeof(data), &err );
	if ( err )
		return;

	if (m_pCurReceivingSnake)
	{
		m_pCurReceivingSnake->OnSpeedUpdate( data.dSpeed );
	}
}

void CGameWorld::Receive_Dead(CBinaryReader*pbr)
{
	CmdSnakeDead data;
	int err;
	pbr->Read( (char*)&data, 4 /* !!!!! */, &err );
	if ( err )
		return;

	locked(

		CSnake *pKilledBy = m_snakes[data.uKilledBy];

		if (m_pCurReceivingSnake && m_pCurReceivingSnake == m_pUserSnake)
			m_pUserSnake = 0;	// at this time not reachable, server cannot kill me, but this option have to be handled
		else if ( m_pUserSnake && pKilledBy == m_pUserSnake )
		{
			double bonus = 0.0;
			if (m_pCurReceivingSnake){bonus += (m_pCurReceivingSnake->GetLength() - 30.0) * 0.2;}
			if (bonus < 0.0)bonus = 0.0;

			m_pUserSnake->LengthAdd( SNAKE_LENGTH_ADD_FOR_KILL + bonus );	// I killed this snake, so expand me
		}

		// event
		OnDead( m_pCurReceivingSnake, pKilledBy );

		// kill the snake, let the animation begin, after animation remove snake from the world
		if (m_pCurReceivingSnake)
		{
			m_pCurReceivingSnake->OnSnakeKilled();

			m_pCurReceivingSnake = 0;
		}

		// remove the dead snake from the world
		//m_snakes.Remove( m_iCurReceivingSnakeId, true ); // do not remove

	);
}

void CGameWorld::Receive_Name(CBinaryReader*pbr)
{
	CString str;
	int err;
	pbr->ReadString( str, &err );
	if  (err )
		return;

	if (m_pCurReceivingSnake)
	{
		lockit(
			m_pCurReceivingSnake,
			m_pCurReceivingSnake->Name = str;
		);
	}
}

void CGameWorld::Receive_Description(CBinaryReader*pbr)
{
	CString str;
	int err;
	pbr->ReadString( str, &err );
	if  (err )
		return;

	if (m_pCurReceivingSnake)
	{
		lockit(
			m_pCurReceivingSnake,
			m_pCurReceivingSnake->Description = str;
		);
	}
}

void CGameWorld::Receive_Language(CBinaryReader*pbr)
{
	int err;
	unsigned int ilng = pbr->ReadUInt32( &err );
	if (err )
		return;

	if (m_pCurReceivingSnake)
		m_pCurReceivingSnake->iLanguage = ilng;
}

void CGameWorld::Receive_Texture(CBinaryReader*pbr)
{
	CString str;
	int err;
	pbr->ReadString( str, &err );
	if  (err )
		return;

	if (m_pCurReceivingSnake)
	{
		m_pCurReceivingSnake->OnTextureUpdate( str );
	}
}

void CGameWorld::Receive_Type(CBinaryReader*pbr)
{
	CString str;
	int err;
	pbr->ReadString( str, &err );
	if  (err )
		return;

	if (m_pCurReceivingSnake)
	{
		m_pCurReceivingSnake->OnTypeUpdate( str );
	}
}

void CGameWorld::Receive_BlockEnd(CBinaryReader*pbr)
{
	// end of command block
	if (m_pCurReceivingSnake && !m_pCurReceivingSnake->IsInitialized())
	{	// new snake initialization done
		m_pCurReceivingSnake->InitializationDone();

		// users snake is initialized callback
		if (m_pCurReceivingSnake == m_pUserSnake)
			OnUserSnakeInitialized();
	}
}

void CGameWorld::Receive_NewSnake(CBinaryReader*pbr)
{
	CmdSnakeId data;
	int err;
	pbr->Read( (char*)&data, 4, &err );

	if ( err )
		return;

	locked(

		// TODO: do not receive info about me
		m_iCurReceivingSnakeId = data.iId;
		
		m_snakes.Remove( m_iCurReceivingSnakeId, true );

		m_pCurReceivingSnake = NewSnake(false);	// new uninitialized snake
		m_snakes.Add( m_iCurReceivingSnakeId, m_pCurReceivingSnake );	// insert to the list of snakes

	);
}

void CGameWorld::Receive_YourSnake(CBinaryReader*pbr)
{
	CmdSnakeId data;
	int err;
	pbr->Read( (char*)&data, 4, &err );
	if (err)
		return;

	locked(

		if (m_pUserSnake)
		{	// i already have a snake
			// TODO
		}

		m_iCurReceivingSnakeId = data.iId;

		m_pCurReceivingSnake = m_pUserSnake = NewSnake(true);	// new uninitialized user controlled snake
		m_snakes.Add( m_iCurReceivingSnakeId, m_pCurReceivingSnake );	// insert to the list of snakes

	);
}

void CGameWorld::Receive_Server(CBinaryReader*pbr)
{
	CString str;
	int err;
	pbr->ReadString( str, &err );
	if  (err )
		return;
}

void CGameWorld::Receive_BestSnake(CBinaryReader*pbr)
{
	CString str;
	int err;
	pbr->ReadString( str, &err );
	if  (err )
		return;

	double d = pbr->ReadDouble( &err );

	if ( err )
		return;

	locked(
		m_strBestSnakeName = str;
		m_dBestSnakeLength = d;
	);
}
void CGameWorld::Receive_Scene(CBinaryReader*pbr)
{
	CmdScene data;
	int err;
	pbr->Read( (char*)&data, sizeof(data), &err );
	if ( err )
		return;

	locked(
		//
		m_boundWorld.topleft.x = - data.dCx / 2.0;
		m_boundWorld.topleft.y = - data.dCy / 2.0;

		m_boundWorld.bottomright.x = data.dCx / 2.0;
		m_boundWorld.bottomright.y = data.dCy / 2.0;

		m_dWorldRefreshDistance = data.dRefreshDistance;

	);
	// 
	OnSceneDefReceived();
}

void CGameWorld::Receive_Error(CBinaryReader*pbr)
{
	CString str;
	int err;
	pbr->ReadString( str, &err );

	if  ( !err )
	{
		OutputDebugString( str );

		// throw a message
		OnMessage( str );
	}
}
void CGameWorld::Receive_Apple(CBinaryReader*pbr)
{
	CmdAppleState apple;
	int err;

	apple.uAppleId = pbr->ReadUInt32(&err);
	if ( err )	return;

	apple.x = pbr->ReadDouble(&err);
	if ( err )	return;

	apple.y = pbr->ReadDouble(&err);
	if ( err )	return;

	// process apple state
	locked(

	CApple *pApple = m_apples[apple.uAppleId];	// look for this apple

	if ( pApple )
	{
		pApple->OnUpdatePosition( apple.x, apple.y );	// apple with this ID exists, update his position
	}
	else
	{
		m_apples.Add( apple.uAppleId, NewApple(apple.x,apple.y) );	// apple with this ID does not exist, create it
	}
	
	);

}
void CGameWorld::Receive_ChatMessage(CBinaryReader *pbr)
{
	CString username, message;
	int err;

	pbr->ReadString( username, &err );
	if ( err )	return;

	pbr->ReadString( message, &err );
	if ( err )	return;

	locked(
	
		OnChatMessage( username, message );
		
	);
}



