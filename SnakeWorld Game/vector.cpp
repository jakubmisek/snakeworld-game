#include <math.h>
#include "vector.h"


inline double sqr( double x )
{
	return x*x;
}


CVector::CVector(double x, double y)
{
	this->x = x;
	this->y = y;
}


CVector::CVector(CVector&vec)
{
	x = vec.x;
	y = vec.y;
}


void	CVector::Add( CVector&vec )
{
	x += vec.x;
	y += vec.y;
}


void	CVector::Sub( CVector&vec )
{
	x -= vec.x;
	y -= vec.y;
}


double	CVector::Distance( CVector&vec )
{
	return sqrt( sqr(vec.x - x) + sqr(vec.y - y) );
}


double	CVector::Length()
{
	return sqrt( x*x + y*y );
}
void	CVector::Normalize()
{
	double dLength = Length();

	if (dLength > 0.0)
	{
		double d1Length = 1.0 / dLength;
		x *= d1Length;
		y *= d1Length;
	}
	else
	{
		x = 1.0;
		y = 0.0;
	}
}
CVector CVector::operator * ( double scalar )
{
	return CVector( x*scalar, y*scalar );
}


CVector CVector::operator + (CVector&vec)
{
	return CVector( x + vec.x, y + vec.y );
}


CVector CVector::operator - (CVector&vec)
{
	return CVector( x - vec.x, y - vec.y );
}


CVector CVector::Rotate( CVector&cossin )
{
	return CVector(	x * cossin.x - y * cossin.y,
					x * cossin.y + y * cossin.x );
}