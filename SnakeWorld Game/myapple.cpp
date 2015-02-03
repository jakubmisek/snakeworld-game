#include "myapple.h"


#define		THROW_DISTANCE	(25.0f)
#define		THROW_HEIGHT	(16.0f)
#define		DISAPEAR_HEIGHT	(10.0f)


// my apple initialization
CMyApple::CMyApple( double x, double y, CAppleModel*pModel )
: CApple(x,y)
, m_pModel(pModel)
, m_iAnimPhase( asNothing )
{
	ThrowApple();
}


// my apple destructor
CMyApple::~CMyApple()
{
	
}


// animate apple
void	CMyApple::FrameMove( double dStep, ISceneTree_Moving*pSceneTreeRoot, CBoundBox&keepAliveBox, CSoundManager&snd )
{
	// update the apple if its in the keepAliveBox (near the users snake)
	if ( keepAliveBox.Intersects( m_boundbox ) && m_iAnimPhase != asNothing )
	{
		// set sound position
		// update transform matrix
		if ( m_iAnimPhase != asOnPlace )
		{
			// on animation phase begin
			if ( m_dAnimState == 0.0 )
			{
				if ( m_iAnimPhase == asDisapearing )
				{
					m_audioPath.PlaySegment( (rand()&1) ? (&snd.sndBite1) : (&snd.sndBite2), false, false );
				}
			}


			// animation move
			m_dAnimState += dStep;
			if ( m_dAnimState > 1.0 )
			{
				if ( m_iAnimPhase == asDisapearing )
				{
					// apple just disapeared, we should throw new apple
					if ( m_bValid )
						ThrowApple();
					else
						m_iAnimPhase = asNothing;
				}
				else
				{
					m_dAnimState = 0.0;
					m_iAnimPhase = (EAnimationPhase)( (int)m_iAnimPhase + 1 );

					if ( m_iAnimPhase == asBowling )
					{	// just felt down => play the sound
						m_audioPath.PlaySegment( &snd.sndHit, false, true );
					}
				}				
			}

			// transform
			UpdateTransform();
		}

		// move into the best scene tree node
		MoveToTreeNode( pSceneTreeRoot );
	}
}


// new position retrieved
// if position is different from the previous position, apple should be restarted
void	CMyApple::OnUpdatePosition( double x, double y )
{
	CApple::OnUpdatePosition(x,y);

	if ( m_iAnimPhase != asDisapearing )	// disapearing animation not running
	{
		ThrowApple();
	}
}

// apple was eaten
// invalidate him
// some animation should be nice
void	CMyApple::OnEaten()
{
	CApple::OnEaten();

	// animation
	m_iAnimPhase = asDisapearing;
	m_dAnimState = 0.0;
}


// initialite animation
void	CMyApple::ThrowApple()
{
	// initial position (random, near the target position)
	D3DXVECTOR3	vecRnd( (float)(rand()&0xff) / (float)0xff, 0.0f, (float)(rand()&0xff) / (float)0xff);
	D3DXVec3Normalize( &vecRnd, &vecRnd );

	m_vecFromMove = vecRnd*THROW_DISTANCE*( (float)(rand()&0xff) / (float)0x7f - 1.0f );

	m_vecMoveOrtho.x = -m_vecFromMove.y;
	m_vecMoveOrtho.y = 0.0f;
	m_vecMoveOrtho.z = m_vecFromMove.x;

	// init animation
	m_iAnimPhase = asThrown;
	m_dAnimState = 0.0;

	// update apple transform
	UpdateTransform();
}


// set apple transform matrix depending on the current animation state
// update apple transform matrix
// update bounding box
// update sound position
void	CMyApple::UpdateTransform()
{
	D3DXVECTOR3	vecPosition( (float)m_position.x, 0.0f, (float)m_position.y );

	const float	flMaxNumRotations = (D3DXVec3Length(&m_vecFromMove) * 0.5f) / (2.0f * D3DX_PI * APPLE_RADIUS);
	const float	flMaxAngle = flMaxNumRotations * (float)(2.0 * D3DX_PI);

	// position & rotation (depends on current animation phase&state)
	switch (m_iAnimPhase)
	{
	case asThrown:
		{
			float flRotMove = - (float)(1.0-m_dAnimState)*0.4f;	// ..0
			D3DXMatrixRotationAxis( &m_matWorld, &m_vecMoveOrtho, flMaxAngle * flRotMove );

			ResizeTransform( m_matWorld, (float)sqrt(m_dAnimState)*0.7f + 0.3f );

			vecPosition += m_vecFromMove * (0.7f + 0.3f * (float)(1.0 - m_dAnimState));	// only 0.3 of m_vecFromMove in the air
			vecPosition.y += THROW_HEIGHT * ( (float)(1.0 - m_dAnimState*m_dAnimState) );

		}

		break;
	case asBowling:
		{
			float flMove = (float)(m_dAnimState + sin(m_dAnimState*D3DX_PI)*0.3 );// 0;1	// on the begin, 5/3 times faster

			D3DXMatrixRotationAxis( &m_matWorld, &m_vecMoveOrtho, flMaxAngle * flMove );

			vecPosition += m_vecFromMove * ( 0.7f * (1.0f-flMove) );

			if ( flMove < 0.5f )
			{	// bump
				vecPosition.y += sinf( (flMove * 2.0f) * D3DX_PI ) * APPLE_RADIUS * 0.8f;	//
			}
		}

		break;
	case asOnPlace:
	
		D3DXMatrixRotationAxis( &m_matWorld, &m_vecMoveOrtho, flMaxAngle );

		// keep default position
		
		break;
	case asDisapearing:
		
		{
			vecPosition = D3DXVECTOR3( m_matWorld._41, 0.0f, m_matWorld._43 );	// copy xz-position from the last transform

			D3DXMatrixRotationAxis( &m_matWorld, &m_vecMoveOrtho, flMaxAngle );

			D3DXMATRIX matRotY;
			D3DXMatrixRotationY( &matRotY, (float)(m_dAnimState*m_dAnimState*D3DX_PI*6.0) );
			D3DXMatrixMultiply( &m_matWorld, &m_matWorld, &matRotY );

			ResizeTransform( m_matWorld, (float)(1.0-m_dAnimState*m_dAnimState)*0.7f + 0.3f );

			vecPosition.y += (float)(m_dAnimState*DISAPEAR_HEIGHT);	// move up
		}

		break;
	default:
		// unknown phase
		return;
	}
	
	// translation, todo rotation
	m_matWorld._41 = vecPosition.x;
	m_matWorld._42 = vecPosition.y + APPLE_RADIUS*0.75f;
	m_matWorld._43 = vecPosition.z;
	
	// bounding box
	m_boundbox = CBoundBox( D3DXVECTOR3( m_matWorld._41, m_matWorld._42, m_matWorld._43 ) );
	m_boundbox += APPLE_RADIUS;
	m_boundbox.m_vecMin.y -= 5.0f;	// ground spheric shape

	// audio position
	m_audioPath.SetPosition( m_matWorld._41, m_matWorld._42, m_matWorld._43 );
}


// rescale matrix in place
void	CMyApple::ResizeTransform(D3DXMATRIX&mat, float s)
{
	mat.m[0][0] *= s;
	mat.m[1][0] *= s;
	mat.m[2][0] *= s;
	mat.m[0][1] *= s;
	mat.m[1][1] *= s;
	mat.m[2][1] *= s;
	mat.m[0][2] *= s;
	mat.m[1][2] *= s;
	mat.m[2][2] *= s;
}


void	CMyApple::RenderObject( CDXDevice&dev, CCamera&camera )
{
	if ( m_iAnimPhase != asNothing )
		m_pModel->RenderApple( dev, m_matWorld );
}


void	CMyApple::RenderGrassMove( CDXDevice&dev, D3DXVECTOR3&vecGrassTranslation )
{
	// if apple is on the ground
	switch (m_iAnimPhase)
	{
	case asBowling:
	case asOnPlace:
		m_pModel->RenderAppleGrassMove( dev, m_matWorld, vecGrassTranslation );
		return;
	default:
		// apple is not on the ground, do not move with the grass
		return;
	}
	
}


void	CMyApple::RenderShadowVolume( CDXDevice&dev )
{
	if ( m_iAnimPhase != asNothing )
		m_pModel->RenderAppleShadowVolume( dev, m_matWorld );
}

