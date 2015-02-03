#ifndef _BUTTERFLY_H_
#define _BUTTERFLY_H_


#include "scenetreemoving.h"
#include "butterflymodel.h"


// butterfly scene object
class CButterfly: public ISceneTree_MovableObject
{
public:
	CButterfly( CTextureLoader*pTexture, CButterflyModel*pModel );
	virtual ~CButterfly();

	// object frame move
	virtual void FrameMove( double dStep, ISceneTree_Moving*pSceneTreeRoot, CBoundBox&keepAliveBox );

	// object bounding box
	virtual CBoundBox&	GetBoundingBox(){ return m_boundbox; }

	// render the object
	virtual void RenderObject( CDXDevice&dev, CCamera&camera );

	virtual void RenderGrassMove( CDXDevice&dev, D3DXVECTOR3&vecGrassTranslation ){}	// do nothing
	virtual void RenderShadowVolume( CDXDevice&dev ){}	// do nothing

protected:

	bool	m_bValid;	// object is alive

	// object bounding box
	CBoundBox	m_boundbox;

	// transformation
	D3DXMATRIX	m_matWorld;
	double		m_dAnimState, m_dAnimSpeed;

	void	MoveButterfly( double dStep );

	// used texture
	CTextureLoader	*m_pTexture;

	// butterfly 3D model
	CButterflyModel	*m_pModel;
};


// list of butterflies
typedef CList<CButterfly>	ButterflyList;



#endif//_BUTTERFLY_H_