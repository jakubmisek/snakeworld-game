#include "boundrect.h"

// min / max
#define min(a,b)	( ((a)<(b)) ? (a) : (b) )
#define max(a,b)	( ((a)>(b)) ? (a) : (b) )


//////////////////////////////////////////////////////////////////////////
// bound rect ctor
CBoundRect::CBoundRect( CVector&pt )
: topleft( pt)
, bottomright( pt )
{
	
}


//////////////////////////////////////////////////////////////////////////
// bound rect ctor
CBoundRect::CBoundRect( CVector&pt1, CVector&pt2 )
	: topleft( min( pt1.x, pt2.x ),min( pt1.y, pt2.y ) )
	, bottomright( max( pt1.x, pt2.x ),max( pt1.y, pt2.y ) )
{

}


//////////////////////////////////////////////////////////////////////////
// rectangle ctor
CBoundRect::CBoundRect( double x1, double y1, double x2, double y2 )
	: topleft( min( x1, x2 ),min( y1,y2 ) )
	, bottomright( max( x1, x2 ),max( y1,y2 ) )
{

}


//////////////////////////////////////////////////////////////////////////
// bound rect ctor
CBoundRect::CBoundRect( CBoundRect&rect )
	: topleft( rect.topleft)
	, bottomright( rect.bottomright )
{

}


//////////////////////////////////////////////////////////////////////////
// bound rect ctor
CBoundRect::CBoundRect( CVector&center, double dHalfSide )
	: topleft( center.x - dHalfSide, center.y - dHalfSide )
	, bottomright( center.x + dHalfSide, center.y + dHalfSide )
{

}

//////////////////////////////////////////////////////////////////////////
// bound expand
void	CBoundRect::operator +=(CVector &pt)
{
	if (pt.x < topleft.x)			topleft.x = pt.x;
	else if (pt.x > bottomright.x)	bottomright.x = pt.x;

	if (pt.y < topleft.y)			topleft.y = pt.y;
	else if (pt.y > bottomright.y)	bottomright.y = pt.y;
}


//////////////////////////////////////////////////////////////////////////
// bound expand
void	CBoundRect::operator +=(CBoundRect &rc)
{
	operator += (rc.topleft);
	operator += (rc.bottomright);
}


//////////////////////////////////////////////////////////////////////////
// bound expand
void	CBoundRect::operator += (double expand)
{
	topleft.x -= expand;
	topleft.y -= expand;

	bottomright.x += expand;
	bottomright.y += expand;
}


//////////////////////////////////////////////////////////////////////////
// bound expand
void	CBoundRect::operator +=(CList<CVector> &positions)
{
	for (CList<CVector>::iterator it = positions.GetIterator(); !it.EndOfList(); ++it)
		operator += (*it);
}

//////////////////////////////////////////////////////////////////////////
// bound set
void	CBoundRect::operator =(CVector &pt)
{
	topleft = bottomright = pt;
}


//////////////////////////////////////////////////////////////////////////
// bound set
void	CBoundRect::operator =(CBoundRect &rc)
{
	topleft = rc.topleft;
	bottomright = rc.bottomright;
}


//////////////////////////////////////////////////////////////////////////
// set rect
void	CBoundRect::operator = (CList<CVector> &positions)
{
	if (positions.Count() == 0)
	{
		topleft.x =
		topleft.y =

		bottomright.x =
		bottomright.y = 0.0;
	}
	else
	{
		bool bfirst = true;
		for (CList<CVector>::iterator it = positions.GetIterator(); !it.EndOfList(); ++it)
		{
			if ( bfirst )
			{
				operator = ( *it );
				bfirst = false;
			}
			else
			{
				operator += ( *it );
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////////
// returns true if given point is in the rectangle
// excluding the rectangle edges
bool	CBoundRect::IsIn( CVector &pt )
{
	return ( pt.x > topleft.x && pt.x < bottomright.x && pt.y > topleft.y && pt.y < bottomright.y );
}


//////////////////////////////////////////////////////////////////////////
// return true if one rectangle intersect the second
// including the rectangle edges
bool	CBoundRect::IsIntersect( CBoundRect &rc )
{
	return !( topleft.x > rc.bottomright.x || topleft.y > rc.bottomright.y || bottomright.x < rc.topleft.x || bottomright.y < rc.topleft.y );
}


//////////////////////////////////////////////////////////////////////////
// return true if line intersects the rectangle
bool	CBoundRect::IsIntersect( CVector&a0, CVector&a1, CVector*phit )
{
	// basic bound test
	if (a0.x < topleft.x && a1.x < topleft.x)
		return false;
	if (a0.y < topleft.y && a1.y < topleft.y)
		return false;
	if (a0.x > bottomright.x && a1.x > bottomright.x)
		return false;
	if (a0.y > bottomright.y && a1.y > bottomright.y)
		return false;

	// find intersection
	if ( IsIn(a0) )
	{
		if (phit)	*phit = a0;
		return true;
	}
	else if ( IsIn(a1) )
	{
		if (phit)	*phit = a1;
		return true;
	}
	else
	{
		CVector hit(0,0);

		if (
		(intersection( a0.x-topleft.x, a1.x-topleft.x, a0, a1, hit) && (hit.y > topleft.y && hit.y < bottomright.y) ) ||
		(intersection( a0.y-topleft.y, a1.y-topleft.y, a0, a1, hit) && (hit.x > topleft.x && hit.x < bottomright.x) ) ||
		(intersection( a0.x-bottomright.x, a1.x-bottomright.x, a0, a1, hit) && (hit.y > topleft.y && hit.y < bottomright.y) ) ||
		(intersection( a0.y-bottomright.y, a1.y-bottomright.y, a0, a1, hit) && (hit.x > topleft.x && hit.x < bottomright.x) )
			)
		{
			if (phit)	*phit = hit;
			return true;
		}
		else
		{
			return false;
		}
	}
}
