#ifndef _BOUNDRECT_H_
#define _BOUNDRECT_H_

#include "vector.h"
#include "list.h"

class CBoundRect
{
public:
	// ctor
	CBoundRect( CVector&pt );
	CBoundRect( CVector&pt1, CVector&pt2 );
	CBoundRect( double x1, double y1, double x2, double y2 );
	CBoundRect( CBoundRect&rect );
	CBoundRect( CVector&center, double dHalfSide );

	// modifying
	void operator += ( CList<CVector>&positions );
	void operator += (CVector&pt);
	void operator += (CBoundRect&rc);
	void operator += (double expand);

	void operator = (CVector&pt);
	void operator = (CBoundRect&rc);

	void operator = ( CList<CVector>&positions );
	
	// the rectangle
	CVector topleft, bottomright;

	//
	bool IsIn( CVector&pt );
	bool IsIntersect( CBoundRect&rc );
	bool IsIntersect( CVector&a0, CVector&a1, CVector*phit );

	inline double Cx(){ return bottomright.x - topleft.x; }
	inline double Cy(){ return bottomright.y - topleft.y; }

private:

	inline bool intersection(double dDst1, double dDst2, CVector&P1, CVector&P2, CVector &Hit)
	{
		if ( (dDst1 * dDst2) >= 0.0 || dDst1 == dDst2 ) return false;
		
		Hit = P1 + (P2-P1) * ( -dDst1/(dDst2-dDst1) );

		return true;
	}

};

#endif//_BOUNDRECT_H_