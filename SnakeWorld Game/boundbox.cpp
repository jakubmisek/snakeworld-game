#include "boundbox.h"

CBoundBox::CBoundBox()	// uninitialized
{

}
CBoundBox::CBoundBox( D3DXVECTOR3&vecMin, D3DXVECTOR3&vecMax )
	:m_vecMin( vecMin )
	,m_vecMax( vecMax )
{

}
CBoundBox::CBoundBox( D3DXVECTOR3&vec )
	:m_vecMin( vec )
	,m_vecMax( vec )
{

}
CBoundBox::CBoundBox( CBoundBox&box )
	:m_vecMin( box.m_vecMin )
	,m_vecMax( box.m_vecMax )
{

}

void CBoundBox::operator += (D3DXVECTOR3&pt)
{
	if ( pt.x < m_vecMin.x )	m_vecMin.x = pt.x;
	else if ( pt.x > m_vecMax.x )	m_vecMax.x = pt.x;

	if ( pt.y < m_vecMin.y )	m_vecMin.y = pt.y;
	else if ( pt.y > m_vecMax.y )	m_vecMax.y = pt.y;

	if ( pt.z < m_vecMin.z )	m_vecMin.z = pt.z;
	else if ( pt.z > m_vecMax.z )	m_vecMax.z = pt.z;
}

void CBoundBox::operator += (CBoundBox&box)
{
	if ( box.m_vecMin.x < m_vecMin.x )	m_vecMin.x = box.m_vecMin.x;
	if ( box.m_vecMax.x > m_vecMax.x )	m_vecMax.x = box.m_vecMax.x;

	if ( box.m_vecMin.y < m_vecMin.y )	m_vecMin.y = box.m_vecMin.y;
	if ( box.m_vecMax.y > m_vecMax.y )	m_vecMax.y = box.m_vecMax.y;

	if ( box.m_vecMin.z < m_vecMin.z )	m_vecMin.z = box.m_vecMin.z;
	if ( box.m_vecMax.z > m_vecMax.z )	m_vecMax.z = box.m_vecMax.z;
}

void CBoundBox::operator += (float expand)
{
	m_vecMin.x -= expand;
	m_vecMin.y -= expand;
	m_vecMin.z -= expand;

	m_vecMax.x += expand;
	m_vecMax.y += expand;
	m_vecMax.z += expand;
}

void CBoundBox::operator = (D3DXVECTOR3&pt)
{
	m_vecMin = m_vecMax = pt;
}

void CBoundBox::operator = (CBoundBox&box)
{
	m_vecMin = box.m_vecMin;
	m_vecMax = box.m_vecMax;
}

D3DXVECTOR3 CBoundBox::SizeVector()
{
	return D3DXVECTOR3( Cx(), Cy(), Cz() );
}

bool	CBoundBox::Intersects( CBoundBox&box )
{
	return !( box.m_vecMin.x > m_vecMax.x || box.m_vecMin.y > m_vecMax.y || box.m_vecMin.z > m_vecMax.z ||
			  box.m_vecMax.x < m_vecMin.x || box.m_vecMax.y < m_vecMin.y || box.m_vecMax.z < m_vecMin.z );
}

bool	CBoundBox::IsIn( CBoundBox&box )
{
	return ( m_vecMin.x >= box.m_vecMin.x && m_vecMin.y >= box.m_vecMin.y && m_vecMin.z >= box.m_vecMin.z &&
			 m_vecMax.x <= box.m_vecMax.x && m_vecMax.y <= box.m_vecMax.y && m_vecMax.z <= box.m_vecMax.z);
}