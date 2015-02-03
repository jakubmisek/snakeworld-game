#ifndef _BOUNDBOX_H_
#define _BOUNDBOX_H_

#include "device.h"

//////////////////////////////////////////////////////////////////////////
// bounding box class
class CBoundBox
{
public:

	CBoundBox();	// uninitialized bound box
	CBoundBox( D3DXVECTOR3&vecMin, D3DXVECTOR3&vecMax );
	CBoundBox( D3DXVECTOR3&vec );
	CBoundBox( CBoundBox&box );

	void operator += (D3DXVECTOR3&pt);
	void operator += (CBoundBox&box);
	void operator += (float expand);

	void operator = (D3DXVECTOR3&pt);
	void operator = (CBoundBox&box);

	inline float	Cx(){ return m_vecMax.x - m_vecMin.x; }
	inline float	Cy(){ return m_vecMax.y - m_vecMin.y; }
	inline float	Cz(){ return m_vecMax.z - m_vecMin.z; }

	D3DXVECTOR3		SizeVector();

	bool	Intersects( CBoundBox&box );
	bool	IsIn( CBoundBox&box );

	// x1,y1,z1, x2, y2, z2
	D3DXVECTOR3	m_vecMin, m_vecMax;
};

#endif//_BOUNDBOX_H_