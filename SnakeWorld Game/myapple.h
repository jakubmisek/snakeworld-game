#ifndef _MYAPPLE_H_
#define _MYAPPLE_H_

#include "appleobject.h"
#include "scenetreemoving.h"
#include "applemodel.h"
#include "soundmanager.h"

class CMyApple: public CApple, public ISceneTree_MovableObject
{
public:
	CMyApple( double x, double y, CAppleModel*pModel );
	virtual ~CMyApple();

	// animate apple
	void	FrameMove( double dStep, ISceneTree_Moving*pSceneTreeRoot, CBoundBox&keepAliveBox, CSoundManager&snd );

	// new position retrieved
	// if position is different from the previous position, apple should be restarted
	virtual void OnUpdatePosition( double x, double y );

	// apple was eaten
	// invalidate him
	// some animation should be nice
	virtual void OnEaten();

	virtual CBoundBox&	GetBoundingBox(){ return m_boundbox; }

	virtual void	RenderObject( CDXDevice&dev, CCamera&camera );
	virtual void	RenderGrassMove( CDXDevice&dev, D3DXVECTOR3&vecGrassTranslation );
	virtual void	RenderShadowVolume( CDXDevice&dev );

protected:

	CBoundBox	m_boundbox;

	D3DXVECTOR3	m_vecFromMove, m_vecMoveOrtho;
	D3DXMATRIX m_matWorld;

	// initiate animation
	void	ThrowApple();

	enum	EAnimationPhase
	{
		asNothing = 0,	// no state defined, uninitialized
		asThrown = 1,	// on the air
		asBowling = 2,	// bowling to the place
		asOnPlace = 3,	// on the place, no moving
		asDisapearing = 4,//apple was eaten, disapear animation
	};

	EAnimationPhase	m_iAnimPhase;	// current animation state
	double			m_dAnimState;	// state of current animation phase [0;1]

	// set apple transform matrix depending on the current animation state
	void	UpdateTransform();

	// scale matrix in place
	void	ResizeTransform( D3DXMATRIX&mat, float s );

	CAppleModel	*m_pModel;

	CAudioPath		m_audioPath;
};


#endif//_MYAPPLE_H_