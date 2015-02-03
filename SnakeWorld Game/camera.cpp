#include "camera.h"


//////////////////////////////////////////////////////////////////////////
// camera constructor
CCamera::CCamera()
{
	D3DXMatrixIdentity( &m_matView );
	D3DXMatrixIdentity( &m_matProjection );

	m_flLastAspectRatio = 4.0f / 3.0f;
	m_flViewDistance = 100.0f;

	OnViewProjChanged();
}


//////////////////////////////////////////////////////////////////////////
// camera destructor
CCamera::~CCamera()
{

}


//////////////////////////////////////////////////////////////////////////
// set the final transform into the device
void	CCamera::ApplyTransform( CDXDevice&dev )
{
	if ( dev.pDX10 )
	{
		// set the transform into the device
		dev.pDX10->pVarMatViewProj->SetMatrix( (float*)&m_matViewProjection );
		dev.pDX10->pVarViewDir->SetFloatVector( (float*)&m_vecViewDir );
		dev.pDX10->pVarViewEye->SetFloatVector( (float*)&m_vecViewEye );
	}
	else if (dev.pDX9)
	{
		dev.pDX9->pEffect->SetMatrix( dev.pDX9->pVarViewProjTransform, &m_matViewProjection  );
		dev.pDX9->pEffect->SetVector( dev.pDX9->pVarViewDirectionVector, &D3DXVECTOR4(m_vecViewDir, 1.0f) );
	}
}
//////////////////////////////////////////////////////////////////////////
// set the final transform into the device
void	CCamera::ApplyTransform( CDXDevice&dev, CSoundManager&snd )
{
	ApplyTransform( dev );
	
	// sound position
	snd.SetListenerPosition( m_vecViewEye.x, m_vecViewEye.y, m_vecViewEye.z, m_vecViewDir.x, m_vecViewDir.y, m_vecViewDir.z, 0,1,0 );
}


//////////////////////////////////////////////////////////////////////////
// set the current projection matrix
void	CCamera::SetProjection( float flAspectRatio, float flViewDistance )
{
	m_flLastAspectRatio = flAspectRatio;

	SetProjection( flViewDistance );
}


//////////////////////////////////////////////////////////////////////////
// set the current projection matrix
void	CCamera::SetProjection( float flViewDistance )
{
	m_flViewDistance = flViewDistance;

	D3DXMatrixPerspectiveFovLH( &m_matProjection, (float)(D3DX_PI / 3.75), m_flLastAspectRatio, 1.0, m_flViewDistance );

	// update view×proj matrix and view frustum
	OnViewProjChanged();
}

//////////////////////////////////////////////////////////////////////////
// set the current view matrix
void	CCamera::SetView( D3DXVECTOR3&vecEye, D3DXVECTOR3&vecAt, D3DXVECTOR3&vecUp )
{
	D3DXMatrixLookAtLH( &m_matView, &vecEye, &vecAt, &vecUp );

	m_vecViewEye = vecEye;
	m_vecViewDir = vecAt - vecEye;
	m_vecViewUp = vecUp;

	D3DXVec3Normalize( &m_vecViewDir, &m_vecViewDir );

	// update view×proj matrix and view frustum
	OnViewProjChanged();
}


//////////////////////////////////////////////////////////////////////////
// update precomputed matrices
// update view frustum
void	CCamera::OnViewProjChanged()
{
	// build the view*projection transform matrix
	D3DXMatrixMultiply( &m_matViewProjection, &m_matView, &m_matProjection );

	// build the view frustum
	UpdateCullInfo();
}


//////////////////////////////////////////////////////////////////////////
//
void	CCamera::UpdateCullInfo()
{
	D3DXMATRIX		matInvViewProj;
	D3DXVECTOR3		vecFrustum[8];
	
	D3DXMatrixInverse( &matInvViewProj, NULL, &m_matViewProjection );

	vecFrustum[0] = D3DXVECTOR3(-1.0f, -1.0f,  0.0f); // xyz
	vecFrustum[1] = D3DXVECTOR3( 1.0f, -1.0f,  0.0f); // Xyz
	vecFrustum[2] = D3DXVECTOR3(-1.0f,  1.0f,  0.0f); // xYz
	vecFrustum[3] = D3DXVECTOR3( 1.0f,  1.0f,  0.0f); // XYz
	vecFrustum[4] = D3DXVECTOR3(-1.0f, -1.0f,  1.0f); // xyZ
	vecFrustum[5] = D3DXVECTOR3( 1.0f, -1.0f,  1.0f); // XyZ
	vecFrustum[6] = D3DXVECTOR3(-1.0f,  1.0f,  1.0f); // xYZ
	vecFrustum[7] = D3DXVECTOR3( 1.0f,  1.0f,  1.0f); // XYZ
	
	for( byte i = 0; i < 8; i++ )
	{
		D3DXVec3TransformCoord( vecFrustum+i, vecFrustum+i, &matInvViewProj );
	}

	// world cull frustum
	D3DXPlaneFromPoints( m_planeFrustum+0, vecFrustum+0, vecFrustum+1, vecFrustum+2 ); // Near
	D3DXPlaneFromPoints( m_planeFrustum+1, vecFrustum+6, vecFrustum+7, vecFrustum+5 ); // Far
	D3DXPlaneFromPoints( m_planeFrustum+2, vecFrustum+2, vecFrustum+6, vecFrustum+4 ); // Left
	D3DXPlaneFromPoints( m_planeFrustum+3, vecFrustum+7, vecFrustum+3, vecFrustum+5 ); // Right
	D3DXPlaneFromPoints( m_planeFrustum+4, vecFrustum+2, vecFrustum+3, vecFrustum+6 ); // Top
	D3DXPlaneFromPoints( m_planeFrustum+5, vecFrustum+1, vecFrustum+0, vecFrustum+4 ); // Bottom
}


//////////////////////////////////////////////////////////////////////////
// check if the given point is inside the view frustum
bool		CCamera::IsPointInFrustum( D3DXVECTOR3*pVec )
{
	// Check vertex against all 6 frustum planes

	for( int iPlane = 0; iPlane < 6; iPlane++ )
	{
		if( m_planeFrustum[iPlane].a * pVec->x +
			m_planeFrustum[iPlane].b * pVec->y +
			m_planeFrustum[iPlane].c * pVec->z +
			m_planeFrustum[iPlane].d < 0.0f )
		{
			return false;	// outside the plane
		}
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////
// check if the given box is visible in the view frustum
bool		CCamera::CullBlock( CBoundBox*pBlock )
{
	BYTE bOutside[8];
	ZeroMemory( &bOutside, sizeof(bOutside) );

	int			iPlane;

	// Check boundary vertexes against all 6 frustum planes, 
	// and store result (1 if outside) in a bitfield

	for( iPlane = 0; iPlane < 6; ++iPlane )
	{
		// point 0 x1y2z1
		if( m_planeFrustum[iPlane].a * pBlock->m_vecMin.x +
			m_planeFrustum[iPlane].b * pBlock->m_vecMax.y +
			m_planeFrustum[iPlane].c * pBlock->m_vecMin.z +
			m_planeFrustum[iPlane].d < 0)
		{
			bOutside[0] |= (1 << iPlane);
		}
	}
	if ( bOutside[0] == 0 )	return true;

	for( iPlane = 0; iPlane < 6; ++iPlane )
	{
		// point 1 x2y1z2
		if( m_planeFrustum[iPlane].a * pBlock->m_vecMax.x +
			m_planeFrustum[iPlane].b * pBlock->m_vecMin.y +
			m_planeFrustum[iPlane].c * pBlock->m_vecMax.z +
			m_planeFrustum[iPlane].d < 0)
		{
			bOutside[1] |= (1 << iPlane);
		}
	}
	if ( bOutside[1] == 0 )	return true;

	for( iPlane = 0; iPlane < 6; ++iPlane )
	{
		// point 2 x1y2z2
		if( m_planeFrustum[iPlane].a * pBlock->m_vecMin.x +
			m_planeFrustum[iPlane].b * pBlock->m_vecMax.y +
			m_planeFrustum[iPlane].c * pBlock->m_vecMax.z +
			m_planeFrustum[iPlane].d < 0)
		{
			bOutside[2] |= (1 << iPlane);
		}
	}
	if ( bOutside[2] == 0 )	return true;

	for( iPlane = 0; iPlane < 6; ++iPlane )
	{
		// point 3 x2y1z1
		if( m_planeFrustum[iPlane].a * pBlock->m_vecMax.x +
			m_planeFrustum[iPlane].b * pBlock->m_vecMin.y +
			m_planeFrustum[iPlane].c * pBlock->m_vecMin.z +
			m_planeFrustum[iPlane].d < 0)
		{
			bOutside[3] |= (1 << iPlane);
		}
	}
	if ( bOutside[3] == 0 )	return true;

	for( iPlane = 0; iPlane < 6; ++iPlane )
	{
		// point 4 x2y2z2
		if( m_planeFrustum[iPlane].a * pBlock->m_vecMax.x +
			m_planeFrustum[iPlane].b * pBlock->m_vecMax.y +
			m_planeFrustum[iPlane].c * pBlock->m_vecMax.z +
			m_planeFrustum[iPlane].d < 0)
		{
			bOutside[4] |= (1 << iPlane);
		}
	}
	if ( bOutside[4] == 0 )	return true;

	for( iPlane = 0; iPlane < 6; ++iPlane )
	{
		// point 5 x1y1z1
		if( m_planeFrustum[iPlane].a * pBlock->m_vecMin.x +
			m_planeFrustum[iPlane].b * pBlock->m_vecMin.y +
			m_planeFrustum[iPlane].c * pBlock->m_vecMin.z +
			m_planeFrustum[iPlane].d < 0)
		{
			bOutside[5] |= (1 << iPlane);
		}
	}
	if ( bOutside[5] == 0 )	return true;

	for( iPlane = 0; iPlane < 6; ++iPlane )
	{
		// point 6 x2y2z1
		if( m_planeFrustum[iPlane].a * pBlock->m_vecMax.x +
			m_planeFrustum[iPlane].b * pBlock->m_vecMax.y +
			m_planeFrustum[iPlane].c * pBlock->m_vecMin.z +
			m_planeFrustum[iPlane].d < 0)
		{
			bOutside[6] |= (1 << iPlane);
		}
	}
	if ( bOutside[6] == 0 )	return true;

	for( iPlane = 0; iPlane < 6; ++iPlane )
	{
		// point 7 x1y1z2
		if( m_planeFrustum[iPlane].a * pBlock->m_vecMin.x +
			m_planeFrustum[iPlane].b * pBlock->m_vecMin.y +
			m_planeFrustum[iPlane].c * pBlock->m_vecMax.z +
			m_planeFrustum[iPlane].d < 0)
		{
			bOutside[7] |= (1 << iPlane);
		}
	}
	if ( bOutside[7] == 0 )	return true;

	// If all points are outside any single frustum plane, the object is
	// outside the frustum
	if( bOutside[0] & bOutside[1] & bOutside[2] & bOutside[3] & bOutside[4] & bOutside[5] & bOutside[6] & bOutside[7] )
	{
		return false;
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////
// moves the view frustum planes by the given values
void	CCamera::ExpandViewFrustum( float flAdd )
{
	for (int i = 0; i < 8; ++i)
		m_planeFrustum[i].d += flAdd;
}