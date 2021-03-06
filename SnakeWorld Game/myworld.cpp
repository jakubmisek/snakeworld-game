#include "myworld.h"
#include "texts.h"


#define VIEW_HEIGHT		(1.0)	// height factor // [1.0 - 3.0]
#define VIEW_DISTANCE	(2.0)	// camera distance factor [1.0 - 2.5]

#define VIEW_TOP		(180.0)	// top view height

#define CAMERA_SPEED	(1.8)

#define TURN_SPEED		(3.0)	// user snake turning factor

#define	MESSAGE_TIME		(8.0f)	// time to message be shown
#define	MESSAGE_FADEIN_TIME	(0.8f)	// message roll in/out animation time

#define MESSAGE_LENGTH_INTERVAL	(50.0f)
#define MESSAGE_BEST_INTERVAL	(90.0f)

#define	BUTTERFLIES_COUNT	(30)	// butterflies near the users snake

static D3DVECTOR vecNamesColors[] = {{1,1,1},{1,0,0},{0,1,0},{0,0,1},{1,1,0}/*,{1,0,1}*/,{0,1,1},{0,0,0}};
static UINT nNamesColors = sizeof(vecNamesColors) / sizeof(D3DVECTOR);

//////////////////////////////////////////////////////////////////////////
// my world constructor
CMyGameWorld::CMyGameWorld()
	:m_vecCameraEye(-20,5,0)
	,m_vecCameraAt(0,0,0)
	,m_vecCameraAtSpeed(0,0,0)
	,m_vecCameraEyeSpeed(0,0,0)
	,m_vecCameraUp(0,1,0)
	,m_vecCameraUpSpeed(0,0,0)
{
	
	m_dViewDistance = 0.75;

	m_pSceneTree = NULL;

	m_flSnakingAnimation = 0.0f;

	m_pShadowVB = NULL;

	m_bShadowsEnabled = true;

	m_pDownloader = NULL;

	m_flMessageAnim = 0.0f;
	m_flShowLengthAfter = m_flShowBestAfter = 0.0f;
}


//////////////////////////////////////////////////////////////////////////
// my world destructor
CMyGameWorld::~CMyGameWorld()
{
	SAFE_DELETE( m_pSceneTree );
	SAFE_RELEASE( m_pShadowVB );
}


//////////////////////////////////////////////////////////////////////////
// create the world
// init the objects
// load graphics
bool	CMyGameWorld::CreateWorld( CDXDevice&dev, CDownloader*pDownloader, CMyGameWorldEvents*pEvents )
{
	
	this->m_pDownloader = pDownloader;
	this->m_pEvents = pEvents;

	// segments model container
	if ( !m_segmentsmodels.CreateContainer( &dev ) )
		return false;

	// sky
	if ( !m_skybox.CreateSkyBox( dev ) )
		return false;

	// terrain rendering
	if ( !m_terrainmodel.CreateTerrainModel( dev ) )
		return false;

	// grass
	if ( !m_grassmodels.CreateContainer( &dev ) )
		return false;

	// apple
	if ( !m_applemodel.CreateContainer( dev ) )
		return false;

	// butterfly
	if ( !CreateButterflies(dev) )
		return false;

	// stone wall
	if ( !m_stonewallmodels.CreateContainer( &dev ) )
		return false;

	//
	ResetProjection(dev);

	if (dev.pDX10)
	{
		// shadow rendering
		if ( !CreateShadow10( *dev.pDX10 ) )
			return false;
	}
	else if (dev.pDX9)
	{
		// TODO
	}

	// scene tree (will be recreated after new connection)
	ResetSceneTree();

	// done
	return true;
}


//////////////////////////////////////////////////////////////////////////
// set the projection matrix
void	CMyGameWorld::ResetProjection( CDXDevice&dev )
{
	if (dev.pDX10)
	{
		// camera projection
		if ( dev.pDX10->bWindowed )
			m_camera.SetProjection( (float)dev.pDX10->uWidth / (float)dev.pDX10->uHeight, (float)( m_dWorldRefreshDistance * m_dViewDistance ) );
		else
			m_camera.SetProjection( dev.pDX10->flFullScreenAspectRatio, (float)( m_dWorldRefreshDistance * m_dViewDistance ) );
	}
	else if (dev.pDX9)
	{
		// camera projection
		if ( dev.pDX9->bWindowed )
			m_camera.SetProjection( (float)dev.pDX9->uWidth / (float)dev.pDX9->uHeight, (float)( m_dWorldRefreshDistance * m_dViewDistance ) );
		else
			m_camera.SetProjection( dev.pDX9->flFullScreenAspectRatio, (float)( m_dWorldRefreshDistance * m_dViewDistance ) );
	}
	
}


//////////////////////////////////////////////////////////////////////////
// create new scene tree
void	CMyGameWorld::ResetSceneTree()
{
	//
	// update world quad-tree
	//

	// delete the old tree if any
	SAFE_DELETE( m_pSceneTree );

	// size of the visible scene
	CBoundRect rcWorldVisible( m_boundWorld );
	rcWorldVisible += m_dWorldRefreshDistance;

	// create new scene tree with the new scene size and position
	m_pSceneTree = new CSceneTreeNode(
		0,	// no parent node (this is root)

		(int)(rcWorldVisible.topleft.x / TERRAIN_SIZE), (int)(rcWorldVisible.topleft.y / TERRAIN_SIZE),	// visible terrain position (in TERRAIN_SIZE units)
		(int)(rcWorldVisible.Cx() / TERRAIN_SIZE), (int)(rcWorldVisible.Cy() / TERRAIN_SIZE),	// visible terrain size (in TERRAIN_SIZE units)

		(int)(m_boundWorld.topleft.x / TERRAIN_SIZE), (int)(m_boundWorld.topleft.y / TERRAIN_SIZE),	// scene position (in TERRAIN_SIZE units)
		(int)(m_boundWorld.Cx() / TERRAIN_SIZE), (int)(m_boundWorld.Cy() / TERRAIN_SIZE)	// scene size (in TERRAIN_SIZE units));

		);
}


//////////////////////////////////////////////////////////////////////////
// scene size and refresh distance was initialized by the server
// new scene size and refresh distance
// new view distance
void	CMyGameWorld::OnSceneDefReceived()
{
	locked(

		// update the projection matrix (view distance changed)
		m_camera.SetProjection( (float)( m_dWorldRefreshDistance * m_dViewDistance ) );

		// update scene tree
		ResetSceneTree();

	);
}


//////////////////////////////////////////////////////////////////////////
// users snake is initialized now
// update the camera (skip the camera move, jump to the new position)
void	CMyGameWorld::OnUserSnakeInitialized()
{
	locked(

		UpdateCameraMove( 0.0, true );
	);
}


//////////////////////////////////////////////////////////////////////////
// snake killed by you
void	CMyGameWorld::OnDead( CSnake*pKilledSnake, CSnake*pKilledBy )
{
	if ( pKilledSnake && pKilledBy )
	{
		if ( pKilledBy == m_pUserSnake && pKilledSnake == m_pUserSnake )
		{	// you killed yourself
			AddMessage( g_gameTexts[tSelfkill] );
		}

		if ( pKilledBy == m_pUserSnake && pKilledSnake != m_pUserSnake )
		{	// you killed somebody
			wchar_t buff[512];
			swprintf_s( buff, g_gameTexts[tYouKilled].str(), pKilledSnake->Name.str() );

			AddMessage( CString(buff) );
		}

		if ( pKilledBy != m_pUserSnake && pKilledSnake == m_pUserSnake )
		{	// somebody killed you
			wchar_t buff[512];
			swprintf_s( buff, g_gameTexts[tKilledBy].str(), pKilledBy->Name.str() );

			AddMessage( CString(buff) );
		}
	}

}


//////////////////////////////////////////////////////////////////////////
// message from server
void	CMyGameWorld::OnMessage( CString&str )
{
	AddMessage( str );
}


//////////////////////////////////////////////////////////////////////////
// chat message received
void	CMyGameWorld::OnChatMessage( CString&strUserName, CString&strMessage )
{
	if ( m_pEvents )
		m_pEvents->OnChatMessageReceived(strUserName, strMessage );
}


//////////////////////////////////////////////////////////////////////////
// move with the world objects, animate
void	CMyGameWorld::FrameMove( CDXDevice&dev, CSoundManager&snd, double dStep )
{
	/**/
	Lock();

	// user control
	if ( m_pUserSnake && m_pUserSnake->IsInitialized() && m_pUserSnake->IsAlive() )
	{
		bool bUserTurning = false;
		// turn the users snake
		if ( GetAsyncKeyState( VK_LEFT ) )
		{
			m_pUserSnake->Turn( + TURN_SPEED * dStep );
			m_flSnakingAnimation = +1.0f;
			bUserTurning = true;
		}

		if ( GetAsyncKeyState( VK_RIGHT ) )
		{
			m_pUserSnake->Turn( - TURN_SPEED * dStep );
			m_flSnakingAnimation = -1.0f;
			bUserTurning = true;
		}

		// snaking animation
		if ( !bUserTurning )
		{
			float fTurn = 0.75f;

			if ( GetAsyncKeyState( VK_UP ) )	fTurn = 0.3f;
			else if ( GetAsyncKeyState( VK_DOWN ) )	fTurn = 3.0f;

			if ( m_flSnakingAnimation > 0.0f )
			{
				m_flSnakingAnimation += (float)dStep;
				if (m_flSnakingAnimation > 1.0f )m_flSnakingAnimation = -0.01f;
			}
			else
			{
				m_flSnakingAnimation -= (float)dStep;
				if (m_flSnakingAnimation < -1.0f )m_flSnakingAnimation = 0.01f;
			}			

			m_pUserSnake->Turn( m_flSnakingAnimation * 0.25f * fTurn * TURN_SPEED * dStep );
		}
		
	}

	// keep-alive box
	CBoundBox boxKeepAlive(
		m_pUserSnake?
			D3DXVECTOR3( (float)m_pUserSnake->GetHeadPosition().x, 0, (float)m_pUserSnake->GetHeadPosition().y ):
			m_vecCameraEye );

	boxKeepAlive += (float)m_dWorldRefreshDistance + (TERRAIN_SIZE*0.5f);	// keep alive radius + some tolerance

	// snakes move
	DoSnakesMove( dev, snd, dStep );

	// apples move
	DoApplesMove( dev, dStep, boxKeepAlive, snd );

	// butterflies move
	ButterfliesFrameMove( dStep, boxKeepAlive );

	// remove old snakes, collisions, send data to server
	CGameWorld::FrameMove( dStep );

	// camera view & frame length
	UpdateCameraMove( dStep, false );
	UpdateCamera( dev, snd );
	
	UpdateFrameLength( dev, dStep );

	HUDFrameMove( dStep );

	// terrain animation
	if (m_pSceneTree)
	{
		// animate the scene (grass, ...)
		MovableObjectsList* ppTmpSnakeLists[ 64 ];
		m_pSceneTree->FrameMove( dev, dStep, m_camera, boxKeepAlive, m_grassmodels, m_stonewallmodels, ppTmpSnakeLists );
	}

	/**/
	Unlock();
}


//////////////////////////////////////////////////////////////////////////
// HUD frame move
void	CMyGameWorld::HUDFrameMove( double dStep )
{
	if ( m_flMessageAnim <= 0.0f )
	{
		// message time out
		// show next message

		m_messages.Remove( 0, true );

		if ( m_pUserSnake && m_pUserSnake->IsInitialized() && m_pUserSnake->IsAlive() )
		{
			// your length
			if ( m_messages.Count() == 0 && m_flShowLengthAfter < 0.0f )
			{
				wchar_t buff[128];
				swprintf_s( buff, L"%s: %u", g_gameTexts[tLength].str(), (unsigned int)m_pUserSnake->GetLength() );

				AddMessage( CString(buff) );

				m_flShowLengthAfter = MESSAGE_LENGTH_INTERVAL;
			}

			// best score
			if ( m_messages.Count() == 0 && m_flShowBestAfter < 0.0f )
			{
				if ( m_dBestSnakeLength > 0.0 )
				{
					wchar_t buff[256];
					swprintf_s( buff, L"%s: %s (%s: %u)", g_gameTexts[tBestScore].str(), m_strBestSnakeName.str(), g_gameTexts[tLength].str(), (unsigned int)m_dBestSnakeLength );

					AddMessage( CString(buff) );
				}

				m_flShowBestAfter = MESSAGE_LENGTH_INTERVAL;
			}
		}
	}

	m_flMessageAnim -= (float)dStep;

	m_flShowLengthAfter -= (float)dStep;
	m_flShowBestAfter -= (float)dStep;
}


//////////////////////////////////////////////////////////////////////////
// add message
void	CMyGameWorld::AddMessage( CString&str )
{
	if ( m_messages.Count() == 0 ) m_flMessageAnim = MESSAGE_TIME;
	m_messages.Add( new CString(str) );
}


//////////////////////////////////////////////////////////////////////////
// move with the snakes
// do not use the old CMySnake::Move method (obsolete), we need to use the new one
void	CMyGameWorld::DoSnakesMove( CDXDevice&dev, CSoundManager&snd, double dStep )
{
	if ( !m_pConnection ) return;	// no connection, no moves

	// snakes move
	SnakeList::CIterator	it = m_snakes.GetIterator();

	while ( !it.EndOfList() )
	{
		CMySnake *pSnake = (CMySnake*)it.Next();

		pSnake->Move(dev,m_segmentsmodels,dStep,m_pSceneTree,snd);
	}
}


//////////////////////////////////////////////////////////////////////////
// apples move
void	CMyGameWorld::DoApplesMove( CDXDevice&dev, double dStep, CBoundBox&keepAliveBox, CSoundManager&snd )
{
	// apples move
	AppleList::CIterator	it = m_apples.GetIterator();

	while ( !it.EndOfList() )
	{
		CMyApple *pApple = (CMyApple*)it.Next();

		pApple->FrameMove(dStep,m_pSceneTree,keepAliveBox,snd);
	}
}


//////////////////////////////////////////////////////////////////////////
// render the world
void	CMyGameWorld::Render( CDXDevice&dev )
{
	//
	// clear render target
	//
	ClearRenderTarget(dev);

	//
	// render the sky
	//
	m_skybox.Render( dev, m_camera );


	//
	// render the scene
	//
	if (m_pSceneTree)
	{
		locked(

			UpdateCamera(dev);	// update transform again (because DX9 must set variables betwenn BeginScene/EndScene

			// normal render
			m_pSceneTree->RenderVisible( dev, m_camera, m_terrainmodel );

			// grass (dx9)
			if ( dev.pDX9 )
				m_grassmodels.FinalizeGrassRender9( *dev.pDX9 );

			// render shadows
			if (m_bShadowsEnabled)
			{
				RenderShadows(dev);
			}

			// render snakes name
			RenderSnakesName(dev);
			// render top10 snakes name
			RenderTop10Snakes(dev);
		);
	}

	//
	// HUD
	//
	HUDRender( dev );

}


//////////////////////////////////////////////////////////////////////////
// HUD render
// snake length, ...
void	CMyGameWorld::HUDRender( CDXDevice &dev )
{
	// bottom line messages
	if ( m_messages.Count() > 0 && m_flMessageAnim > 0.0f )
	{
		float xPos = 0.01f;	// default pos
		float flAnim = 0.0f;

		if ( m_flMessageAnim < MESSAGE_FADEIN_TIME )
			flAnim = (m_flMessageAnim - MESSAGE_FADEIN_TIME) / MESSAGE_FADEIN_TIME;
		else if (m_flMessageAnim > (MESSAGE_TIME - MESSAGE_FADEIN_TIME))
			flAnim = ( (m_flMessageAnim - (MESSAGE_TIME - MESSAGE_FADEIN_TIME))) / MESSAGE_FADEIN_TIME;

		dev.RenderText( *m_messages[0], D3DXCOLOR(1,1,1,(1.0f-abs(flAnim))*0.5f), D3DXVECTOR2(xPos+flAnim, 0.95f) );	
	}
	
}


//////////////////////////////////////////////////////////////////////////
// render snakes name
// not thread safe
void	CMyGameWorld::RenderSnakesName( CDXDevice &dev )
{
	D3DXMATRIX matViewProj;
	m_camera.GetViewProj(matViewProj);

	// snakes iterator
	SnakeList::CIterator	it = m_snakes.GetIterator();

	while ( !it.EndOfList() )
	{
		unsigned int uSnakeId;
		CMySnake *pSnake = (CMySnake*)it.Next(&uSnakeId);

		if (pSnake == m_pUserSnake || pSnake == 0)
			continue;

		// snake head screen-coord position
		CVector headposition = pSnake->GetHeadPosition();
		D3DXVECTOR3 vecScreenHeadPos;
		D3DXVec3TransformCoord( &vecScreenHeadPos, &D3DXVECTOR3((float)headposition.x,0.0f,(float)headposition.y), &matViewProj );

		// render the snake name if the snake head is visible
		if (vecScreenHeadPos.x > -1.0f && vecScreenHeadPos.x < 1.0f &&
			vecScreenHeadPos.y > -1.0f && vecScreenHeadPos.y < 1.0f &&
			vecScreenHeadPos.z > 0.0f && vecScreenHeadPos.z < 1.0f)
		{
			float flAlpha = 0.5f * vecScreenHeadPos.z * (float)(1.0f - abs(vecScreenHeadPos.x));
			float px48 = 48.0f / (float)dev.ScreenHeight();

			CString strName = pSnake->Name + CString(L"\n↓");
			UINT iColor = uSnakeId % nNamesColors;
			D3DVECTOR vecColor = vecNamesColors[iColor];

			dev.RenderText( strName, D3DXCOLOR(vecColor.x,vecColor.y,vecColor.z,flAlpha), D3DXVECTOR2((vecScreenHeadPos.x + 1.0f)*0.5f,(1.0f - vecScreenHeadPos.y)*0.5f - px48), true );
		}
		
	}
}



//////////////////////////////////////////////////////////////////////////
// render top10 snakes name
// not thread safe
void	CMyGameWorld::RenderTop10Snakes( CDXDevice &dev )
{
	const int topnum = 10;
	CMySnake *topsnakes[topnum];
	unsigned int topsnakesid[topnum];
	int topsnakescount = 0;
	
	// snakes iterator
	SnakeList::CIterator	it = m_snakes.GetIterator();

	while ( !it.EndOfList() )
	{
		unsigned int uSnakeId;
		CMySnake *pSnake = (CMySnake*)it.Next(&uSnakeId);

		if (pSnake == 0)
			continue;

		// add snake into the array
		int i = 0;
		for (;i<topsnakescount;++i)
		{
			if (pSnake->GetLength() > topsnakes[i]->GetLength())
				break;
		}

		if (i < topnum)
		{
			// move array
			for (int j = topsnakescount; j > i; --j)
			{
				if ( j < topnum && j > 0 )
				{
					topsnakes[j] = topsnakes[j-1];
					topsnakesid[j] = topsnakesid[j-1];
				}
			}
			topsnakes[i] = pSnake;
			topsnakesid[i] = uSnakeId;
			
			topsnakescount = min(topsnakescount+1, topnum);
		}
	}

	// write snakes
	float ypos = 0.0f;
	for (int i = 0; i < topsnakescount; ++i)
	{
		CMySnake *pSnake = topsnakes[i];

		wchar_t buff[1024];
		swprintf_s( buff, L"%s: %u", pSnake->Name.str(), (unsigned int)pSnake->GetLength() );

		UINT iColor = topsnakesid[i] % nNamesColors;
		D3DVECTOR vecColor = vecNamesColors[iColor];

		dev.RenderText( CString(buff), D3DXCOLOR(vecColor.x,vecColor.y,vecColor.z,0.5f), D3DXVECTOR2(1.0f, ypos), false, true);

		ypos += (float)FONT_HEIGHT / (float)dev.ScreenHeight();
	}
}

//////////////////////////////////////////////////////////////////////////
// clear render target, depth buffer and stencil buffer
void	CMyGameWorld::ClearRenderTarget( CDXDevice&dev )
{
	if (dev.pDX10)
	{
		ID3D10RenderTargetView	*pOrigRT;
		ID3D10DepthStencilView	*pOrigDS;

		dev.pDX10->pd3dDevice->OMGetRenderTargets( 1, &pOrigRT, &pOrigDS );

		dev.pDX10->pd3dDevice->ClearDepthStencilView( pOrigDS, D3D10_CLEAR_DEPTH, 1.0, 0);
		dev.pDX10->pd3dDevice->ClearDepthStencilView( pOrigDS, D3D10_CLEAR_STENCIL, 1.0, 0);

		SAFE_RELEASE( pOrigRT );
		SAFE_RELEASE( pOrigDS );
	}
	else if(dev.pDX9)
	{
		dev.pDX9->pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 255), 1.0f, 0 );
	}
}


//////////////////////////////////////////////////////////////////////////
// render the shadows
void	CMyGameWorld::RenderShadows( CDXDevice&dev )
{
	const float flViewExp = 6.0f;

	// extend camera view frustum
	m_camera.ExpandViewFrustum( flViewExp );

	// "render shadows to stencil buffer"
	m_pSceneTree->RenderShadowVolumes( dev, m_camera );

	// render the shadows
	if (dev.pDX10)
	{
		RenderShadows10( *dev.pDX10 );
	}
	else if(dev.pDX9)
	{
		// TODO
	}

	// restore camera view frustum
	m_camera.ExpandViewFrustum( -flViewExp );
}


//////////////////////////////////////////////////////////////////////////
// render the shadows
void	CMyGameWorld::RenderShadows10( CDX10Device&dev )
{
	// blend the render target where shadow is visible (stencil > 0)
	D3D10_TECHNIQUE_DESC techDesc;

	// DX: init the input layout and primitive topology
	dev.pd3dDevice->IASetInputLayout( m_pShadowInputLayout );
	dev.pd3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

	UINT stride = sizeof(D3DXVECTOR3), offset = 0;

	// the vertex buffer
	dev.pd3dDevice->IASetVertexBuffers( 0, 1, &m_pShadowVB, &stride, &offset );

	// Render snake head
	m_pShadowTechnique->GetDesc( &techDesc );
	for( UINT p = 0; p < techDesc.Passes; ++p )
	{
		m_pShadowTechnique->GetPassByIndex( p )->Apply(0);
		dev.pd3dDevice->Draw( 4, 0 );
	}
	
}


//////////////////////////////////////////////////////////////////////////
// create the shadow mask model
bool	CMyGameWorld::CreateShadow10( CDX10Device &dev )
{
	// obtain techniques
	m_pShadowTechnique = dev.pEffect->GetTechniqueByName( "RenderShadows" );

	if ( !m_pShadowTechnique )
		return false;


	// input layout
	D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof(layout)/sizeof(layout[0]);

	D3D10_PASS_DESC PassDesc;
	m_pShadowTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );
	if ( FAILED(dev.pd3dDevice->CreateInputLayout( layout, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &m_pShadowInputLayout )) )
		return false;	// unable to create the input layout

	// vertex buffer
	D3DXVECTOR3 v[4];

	v[0] = D3DXVECTOR3( -1.0f, -1.0f, 0.5f );
	v[1] = D3DXVECTOR3( +1.0f, -1.0f, 0.5f );
	v[2] = D3DXVECTOR3( -1.0f, +1.0f, 0.5f );
	v[3] = D3DXVECTOR3( +1.0f, +1.0f, 0.5f );

	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( v );	// size
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;	// vertex buffer
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA	initdata;
	initdata.pSysMem = v;

	if( FAILED( dev.pd3dDevice->CreateBuffer( &bd, &initdata, &m_pShadowVB ) ) )
		return false;	// failed to create

	//
	return true;
}


//////////////////////////////////////////////////////////////////////////
// creates new snake object for this world
CSnake*	CMyGameWorld::NewSnake( bool bUserControlled )
{
	return new CMySnake( bUserControlled, m_pDownloader );
}


//////////////////////////////////////////////////////////////////////////
// creates new apple object for this world
CApple*	CMyGameWorld::NewApple( double x, double y )
{
	return new CMyApple( x,y, m_applemodel.GetModel() );
}


//////////////////////////////////////////////////////////////////////////
// update camera view
// dStep is in [0,1]
void	CMyGameWorld::UpdateCameraMove( double dStep, bool bSetImmediately )
{
	D3DXVECTOR3 vecCameraEyeTo, vecCameraAtTo, vecCameraUpTo;

	if (m_pUserSnake && m_pUserSnake->IsInitialized())
	{
		SetTargetCamera_3D( vecCameraEyeTo, vecCameraAtTo, vecCameraUpTo );

		//vecCameraEyeTo = m_vecCameraEye; // for testing&presentation purposes only
		//vecCameraEyeTo.y = 10.0f;		// for testing purposes only
	}
	else
	{
		vecCameraEyeTo = m_vecCameraEye;
		vecCameraAtTo = m_vecCameraAt;
		vecCameraUpTo = m_vecCameraUp;

		// speed down
		m_vecCameraAtSpeed *= (float)(1.0 - dStep);
		m_vecCameraEyeSpeed *= (float)(1.0 - dStep);
		m_vecCameraUpSpeed *= (float)(1.0 - dStep);
	}

	if (bSetImmediately)
	{
		m_vecCameraAt = vecCameraAtTo;
		m_vecCameraEye = vecCameraEyeTo;
		m_vecCameraUp = vecCameraUpTo;
	}
	else
	{
		// update the camera speed
		m_vecCameraEyeSpeed += ( (vecCameraEyeTo - m_vecCameraEye) - m_vecCameraEyeSpeed ) * (float)(dStep*CAMERA_SPEED);
		m_vecCameraAtSpeed += ( (vecCameraAtTo - m_vecCameraAt) - m_vecCameraAtSpeed ) * (float)(dStep*CAMERA_SPEED);
		m_vecCameraUpSpeed += ( (vecCameraUpTo - m_vecCameraUp) - m_vecCameraUpSpeed ) * (float)(dStep*CAMERA_SPEED);

		// interpolate from "m_vecCameraEye" and "m_vecCameraAt" do "...To"
		m_vecCameraEye += m_vecCameraEyeSpeed * (float)dStep;
		m_vecCameraAt += m_vecCameraAtSpeed * (float)dStep;
		m_vecCameraUp += m_vecCameraUpSpeed * (float)dStep;
	}

	// setup the transform
	m_camera.SetView( m_vecCameraEye, m_vecCameraAt, m_vecCameraUp );

	
}


//////////////////////////////////////////////////////////////////////////
// set camera for 3D view
void	CMyGameWorld::SetTargetCamera_3D(D3DXVECTOR3&vecCameraEyeTo, D3DXVECTOR3&vecCameraAtTo, D3DXVECTOR3&vecCameraUpTo)
{
	// view at
	vecCameraAtTo.x = (float)m_pUserSnake->GetHeadPosition().x;
	vecCameraAtTo.y = 0.0f;
	vecCameraAtTo.z = (float)m_pUserSnake->GetHeadPosition().y;

	// eye
	vecCameraEyeTo.x = (float)( m_pUserSnake->GetHeadPosition().x - m_pUserSnake->GetHeadDirection().x * m_pUserSnake->GetSpeed()*VIEW_DISTANCE );
	vecCameraEyeTo.y = (float)( m_pUserSnake->GetSpeed() * VIEW_HEIGHT );
	vecCameraEyeTo.z = (float)( m_pUserSnake->GetHeadPosition().y - m_pUserSnake->GetHeadDirection().y * m_pUserSnake->GetSpeed()*VIEW_DISTANCE );

	// up
	vecCameraUpTo = D3DXVECTOR3(0,1,0);
}


//////////////////////////////////////////////////////////////////////////
// set camera for 2D top view
void	CMyGameWorld::SetTargetCamera_Top(D3DXVECTOR3&vecCameraEyeTo, D3DXVECTOR3&vecCameraAtTo, D3DXVECTOR3&vecCameraUpTo)
{
	// view at
	vecCameraAtTo.x = (float)m_pUserSnake->GetHeadPosition().x;
	vecCameraAtTo.y = 0.0f;
	vecCameraAtTo.z = (float)m_pUserSnake->GetHeadPosition().y;

	// eye
	vecCameraEyeTo.x = (float)m_pUserSnake->GetHeadPosition().x;
	vecCameraEyeTo.y = (float)( VIEW_TOP );
	vecCameraEyeTo.z = (float)m_pUserSnake->GetHeadPosition().y;

	// up
	vecCameraUpTo = D3DXVECTOR3(1,0,0);
}


//////////////////////////////////////////////////////////////////////////
// set the camera transform into the dx and audio device
void	CMyGameWorld::UpdateCamera( CDXDevice&dev, CSoundManager&snd )
{
	//
	// set the transform
	//
	m_camera.ApplyTransform( dev, snd );

}


//////////////////////////////////////////////////////////////////////////
// set the camera transform into the device
void	CMyGameWorld::UpdateCamera( CDXDevice&dev )
{
	//
	// set the transform
	//
	m_camera.ApplyTransform( dev );

}


//////////////////////////////////////////////////////////////////////////
// update the effect variable containing length of the last frame in seconds
void	CMyGameWorld::UpdateFrameLength( CDXDevice&dev, double dStep )
{
	if (dev.pDX10)
	{
		dev.pDX10->pVarFrameLength->SetFloat( (float)dStep );
	}
	else if(dev.pDX9)
	{
		// TODO
	}
}


//////////////////////////////////////////////////////////////////////////
// load butterflies textures
// create butterflies in the scene
bool	CMyGameWorld::CreateButterflies( CDXDevice&dev )
{
	// textures
	CreateButterfliesTextures( dev );

	// model
	if ( !m_butterflymodel.CreateContainer( dev ) )
		return false;

	// objects
	m_butterflies.Clear(true);

	for ( int i = 0; i < BUTTERFLIES_COUNT; ++i )
	{
		CButterfly *pb = new CButterfly( m_butterfliestextures[ i % m_butterfliestextures.Count() ], m_butterflymodel.GetModel() );

		m_butterflies.Add( pb );
	}

	//
	return true;
}


//////////////////////////////////////////////////////////////////////////
// load butterflies textures
void	CMyGameWorld::CreateButterfliesTextures( CDXDevice&dev )
{
	LPCTSTR ButterfliesTextureNames[] = 
	{
		L"media\\butterfly_v1.dds",
		L"media\\butterfly_v2.dds",
		L"media\\butterfly_v3.dds",
		L"media\\butterfly_v4.dds",
		L"media\\butterfly_v5.dds",
	};

	int nTextures = sizeof(ButterfliesTextureNames) / sizeof(ButterfliesTextureNames[0]);

	m_butterfliestextures.Clear(true);

	for ( int i = 0; i < nTextures; ++i )
	{
		CTextureLoader *pt = new CTextureLoader();

		pt->CreateTexture( dev, ButterfliesTextureNames[i], NULL );

		m_butterfliestextures.Add( pt );
	}
}


//////////////////////////////////////////////////////////////////////////
// animate butterflies
void	CMyGameWorld::ButterfliesFrameMove(double dStep, CBoundBox &keepAliveBox)
{
	for ( ButterflyList::iterator it = m_butterflies.GetIterator(); !it.EndOfList(); ++it )
	{
		CButterfly *pb = it;

		pb->FrameMove( dStep, m_pSceneTree, keepAliveBox );
	}
}