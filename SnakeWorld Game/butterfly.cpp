#include "butterfly.h"

// moving parameters
#define	MIN_HEIGHT	(3.0f)
#define	MAX_HEIGHT	(12.0f)

#define	MAX_SPEED	(5.0)	// /sec
#define	MAX_TURN	(2.0)	// /sec

#define	ANIM_CYCLE_LENGTH	(0.2)	// butterfly anim cycle length, must be > 0.0


// butterfly initialization
CButterfly::CButterfly(CTextureLoader *pTexture, CButterflyModel *pModel)
{
	m_pTexture = pTexture;
	m_pModel = pModel;

	m_dAnimState = 0.0;
	m_dAnimSpeed = 1.0;

	m_bValid = false;
}


// butterfly dtor
CButterfly::~CButterfly()
{
	
}


// butterfly move
void	CButterfly::FrameMove(double dStep, ISceneTree_Moving *pSceneTreeRoot, CBoundBox &keepAliveBox)
{
	// check position
	if ( m_bValid )
	{
		if ( !keepAliveBox.Intersects( m_boundbox ) )
			m_bValid = false;
	}

	// respawn
	if ( !m_bValid )
	{
		// reset object
		
		// random rotation
		float fr = (float)(rand()&0xff) / (float)0xff;

		D3DXMatrixRotationY( &m_matWorld, (float)(fr * 2.0 * D3DX_PI) );

		// random xyz position
		float fx = (float)(rand()&0xff) / (float)0xff;
		float fy = (float)(rand()&0xff) / (float)0xff;

		m_matWorld._41 = fx * keepAliveBox.Cx() + keepAliveBox.m_vecMin.x;
		m_matWorld._42 = MAX_HEIGHT * 4.0f;
		m_matWorld._43 = fy * keepAliveBox.Cz() + keepAliveBox.m_vecMin.z;
		
		// random speed
		m_dAnimSpeed = 1.0 + ( (float)(rand()&0xff) / (float)0xff * 0.2 - 0.1 );

		//
		m_bValid = true;
	}

	// animate wings
	m_dAnimState += dStep*m_dAnimSpeed;
	if ( m_dAnimState > 60.0 )	m_dAnimState -= 60.0;	// normalize variable

	// move with transform matrix
	MoveButterfly( dStep );

	// set bounding box
	m_boundbox = CBoundBox( D3DXVECTOR3( m_matWorld._41, m_matWorld._42,m_matWorld._43 ) );
	m_boundbox += 1.0f;
	m_boundbox.m_vecMin.y -= 5.0f;	// ground spheric shape

	// update in the tree
	MoveToTreeNode( pSceneTreeRoot );
}


// move the object
void	CButterfly::MoveButterfly( double dStep )
{
	float	fToY;

	if ( rand()&1 )
		fToY = MAX_HEIGHT;
	else
		fToY = MIN_HEIGHT;

	// move matrix
	D3DXMATRIX	matMove;
	
	D3DXMatrixRotationY( &matMove, (float)( ((float)(rand()&0xff) / (float)0x7f - 1.0f) * dStep * MAX_TURN ) );
	matMove._41 = 0.0f;
	matMove._42 = (float)( (sin( m_dAnimState*D3DX_PI ) + sin( m_dAnimState*0.5 ) +  (fToY - m_matWorld._42))*dStep );
	matMove._43 = (float)( ((float)(rand()&0xff) / (float)0xff + 1.0f)*0.5f * dStep * MAX_SPEED );

	// multiply
	D3DXMatrixMultiply( &m_matWorld, &matMove, &m_matWorld );

}


// render butterfly
void	CButterfly::RenderObject(CDXDevice &dev, CCamera &camera)
{
	if ( m_bValid )
	{
		// wings animation state
		double f,n;
		f = modf( m_dAnimState / ANIM_CYCLE_LENGTH, &n );	// f: [0;1)
		f = abs( 2.0 * f - 1.0 );	// f: [1-0-1]

		// adjust height
		D3DXMATRIX matWorld = m_matWorld;
		matWorld._42 -= (float)f * 0.4f;

		// render model
		m_pModel->RenderModel( dev, matWorld, (float)(f), *m_pTexture );
	}
}