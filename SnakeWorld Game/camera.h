#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "device.h"
#include "soundmanager.h"
#include "boundbox.h"

//////////////////////////////////////////////////////////////////////////
// camera object
class	CCamera
{
public:
	CCamera();
	~CCamera();

	void	ApplyTransform( CDXDevice&dev, CSoundManager&snd );
	void	ApplyTransform( CDXDevice&dev );

	void	SetProjection( float flAspectRatio, float flViewDistance );
	void	SetProjection( float flViewDistance );

	void	SetView( D3DXVECTOR3&vecEye, D3DXVECTOR3&vecAt, D3DXVECTOR3&vecUp );

	void	GetProjection( D3DXMATRIX&matOut ){ matOut = m_matProjection; }
	void	GetView( D3DXMATRIX&matOut ){ matOut = m_matView; }
	void	GetViewProj( D3DXMATRIX&matOut ){ matOut = m_matViewProjection; }

	// test for visibility
	bool			IsPointInFrustum( D3DXVECTOR3*pVec );				// is the point in the view frustum
	bool			CullBlock( CBoundBox*pBlock );						// is the block in the view frustum
	void			ExpandViewFrustum( float flAdd );					// moves the view frustum planes by the given value

	float			Distance( D3DXVECTOR3&vecPos ){ return D3DXVec3Length( &D3DXVECTOR3( m_vecViewEye - vecPos ) ); }
	float			GetViewDistance(){ return m_flViewDistance; }

protected:

	D3DXMATRIX	m_matView, m_matProjection;
	D3DXMATRIX	m_matViewProjection;

	D3DXVECTOR3	m_vecViewDir, m_vecViewEye, m_vecViewUp;
	float		m_flLastAspectRatio, m_flViewDistance;

	void		OnViewProjChanged();

	// view frustum
	D3DXPLANE		m_planeFrustum[6];
	void			UpdateCullInfo();			// update culling info ( recompute frustum planes )

};

#endif//_CAMERA_H_
