#include "mysnake.h"
#include "constants.h"

//////////////////////////////////////////////////////////////////////////
// my snake constructor
CMySnake::CMySnake( bool bUserControlled, CDownloader*pDownloader )
	:CSnake(bUserControlled)
{
	m_dSmoothHeadDirection = 0.0;

	m_pDownloader = pDownloader;

	m_pSndHit = m_pSndSnaking = NULL;
}


//////////////////////////////////////////////////////////////////////////
// my snake destructor
CMySnake::~CMySnake()
{
	
}


//////////////////////////////////////////////////////////////////////////
// snake object move - OIBSOLETE - DO NOT USE
void	CMySnake::Move( double dStep )
{
	throw "this method is obsolete ! Do not use it.";
}


//////////////////////////////////////////////////////////////////////////
// snake move
// update the snake model
void	CMySnake::Move( CDXDevice&dev, CSegmentsModelContainer&segmentsmodels, double dStep, ISceneTree_Moving*pSceneTreeRoot, CSoundManager&snd )
{
	if (!m_bInitialized)
		return;

	// dead animation
	if ( !m_bAlive )
	{
		// decrease length

		double dDecreaseSpeed = max( m_dLength * 0.1, 3.0 );

		m_dLength -= dStep * m_dSpeed * dDecreaseSpeed;
		if ( m_dLength < 0.0 ) m_dLength = 0.0;
	}

	// base snake object move
	CSnake::Move( dStep );

	// smooth head direction move (fix the remote snakes dirty head direction change)
	HeadDirectionSmoothUpdate( dStep );

	// update model
	UpdateSegmentsModels( dev, segmentsmodels, pSceneTreeRoot );

	// update sounds	
	UpdateSounds( snd );	
}


//////////////////////////////////////////////////////////////////////////
// update sounds
void	CMySnake::UpdateSounds( CSoundManager&snd )
{
	if ( !m_pSndSnaking && m_bInitialized && m_bAlive )
	{
		m_pSndSnaking = &snd.sndSnaking;
		m_pSndHit = &snd.sndHit;
		
		m_audioPath.PlaySegment( m_pSndSnaking, true );
	}

	// update sound position
	m_audioPath.SetPosition( (float)m_posHead.x, 0.0f, (float)m_posHead.y );
}


//////////////////////////////////////////////////////////////////////////
// on texture update
void	CMySnake::OnTextureUpdate( CString&szTexture )
{
	locked(
		CSnake::OnTextureUpdate( szTexture );

		//
		m_texturename = szTexture;
	);
}


//////////////////////////////////////////////////////////////////////////
// snake was killed
void	CMySnake::OnSnakeKilled()
{
	CSnake::OnSnakeKilled();

	m_audioPath.PlaySegment( m_pSndHit, false );
}


//////////////////////////////////////////////////////////////////////////
// snake is dead and animation is over
bool	CMySnake::WantToDelete()
{
	return ( !m_bAlive && m_dLength < 1.0 );
}


//////////////////////////////////////////////////////////////////////////
// updates the head direction smoothly
void	CMySnake::HeadDirectionSmoothUpdate(double dStep)
{
	double dHeadDir = m_dHeadDirection - m_dSmoothHeadDirection;

	if (dHeadDir == 0.0)
		return;

	if ( abs(dHeadDir) <= PI )
	{
		
	}
	else
	{
		if (dHeadDir < 0.0)
			dHeadDir = PI2 + dHeadDir;
		else
			dHeadDir = dHeadDir - PI2;
	}

	if (dHeadDir > 0.0)
		dHeadDir = min(dHeadDir,dStep*PI2);
	else
		dHeadDir = max(dHeadDir,-dStep*PI2);

	m_dSmoothHeadDirection += dHeadDir;

	while (m_dSmoothHeadDirection < 0.0)	m_dSmoothHeadDirection += PI2;
	while (m_dSmoothHeadDirection >= PI2)	m_dSmoothHeadDirection -= PI2;
}


//////////////////////////////////////////////////////////////////////////
// update the snake segments models
void	CMySnake::UpdateSegmentsModels( CDXDevice&dev, CSegmentsModelContainer&segmentsmodels, ISceneTree_Moving*pSceneTreeRoot )
{
	if (!m_bInitialized)
	{
		return;
	}

	// create the texture if not yet
	if ( m_texturename.length() > 0)
	//if ( m_texture.GetState() == m_texture.NotLoaded || m_texture.GetState() == m_texture.Loaded )
	{
		CString strFileName = (wchar_t*)L"media\\snakes\\" + m_texturename;
		m_texture.CreateTexture( dev, strFileName, m_pDownloader );
		//D3DX10CreateTextureFromFile( dev.pd3dDevice, L"media\\snake1.jpg", NULL, NULL, &pSnakeTexture, NULL );
		//dev.pd3dDevice->CreateShaderResourceView( pSnakeTexture, NULL, &pSnakeTextureView );
		m_texturename = L"";
	}

	//if ( m_normaltexture.GetState() == m_normaltexture.NotLoaded )
	if ( !m_normaltexture.IsLoaded() )
	{
		m_normaltexture.CreateTexture( dev, L"media\\normal.jpg", m_pDownloader );
		//D3DX10CreateTextureFromFile( dev.pd3dDevice, L"media\\normal.jpg", NULL, NULL, &pSnakeNormalTexture, NULL );
		//dev.pd3dDevice->CreateShaderResourceView( pSnakeNormalTexture, NULL, &pSnakeNormalTextureView );
	}

	// snake length and tail position
	double dBodyLength;
	CVector vecTail = GetTailPosition( &dBodyLength );

	//
	// update the super segments models
	//

	// update effect variables
	float	flCurDistFromTail = 0.0f,
			flCurDistFromHead = (float)dBodyLength;

	{	// add not created models
		// update models
		// inner super-segments are updated only once
		// - only first and last models vertex buffers are rewritten ( see UpdateModel(...) )

		CSnakeSuperSegmentEx*pPrevSS = 0;
		CSnakeSuperSegmentEx*pSS = 0;
		CSnakeSuperSegmentEx*pNextSS;

		SuperSegmentList::iterator sit = m_supersegments.GetIterator();

		pSS = (CSnakeSuperSegmentEx*)sit.value();

		while ( pSS )
		{
			++sit;
			pNextSS = (CSnakeSuperSegmentEx*)sit.value();
			
			if ( !pSS->pModel )	// create the model if not exists
			{
				if ( !segmentsmodels.CreateModel( &pSS->pModel ) )
				{
					continue;
				}
			}

			pSS->pModel->UpdateModel(
				vecTail,pPrevSS,pSS,pNextSS,m_posHead,	// model
				pSceneTreeRoot,							// scene tree
				flCurDistFromTail,flCurDistFromHead,	// render parameters
				m_texture, m_normaltexture );	// model textures
				//pSnakeTextureView, pSnakeNormalTextureView );	// model textures

			// update distances from the head and tail
			flCurDistFromHead -= pSS->pModel->GetLength();
			flCurDistFromTail += pSS->pModel->GetLength();

			// move the pointers, update pPrev and pSS
			pPrevSS = pSS;
			pSS = pNextSS;
		}

	}

	//
	// update the head
	//

	m_headmodel.UpdateHead(
		&segmentsmodels,
		D3DXVECTOR3( (float)m_posHead.x, 0.0f, (float)m_posHead.y ), (float)m_dSmoothHeadDirection, (float)dBodyLength,
		pSceneTreeRoot,
		m_texture, m_normaltexture );
		//pSnakeTextureView, pSnakeNormalTextureView );
}


//////////////////////////////////////////////////////////////////////////
// on init done
void	CMySnake::InitializationDone()
{
	CSnake::InitializationDone();

	//
	m_dSmoothHeadDirection = m_dHeadDirection;
}