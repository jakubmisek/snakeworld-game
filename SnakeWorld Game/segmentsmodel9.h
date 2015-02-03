#ifndef _SEGMENTSMODEL9_H_
#define _SEGMENTSMODEL9_H_

#include "device.h"

static const int nBodyPoints = 7;			// snake body shape points count

static const D3DXVECTOR2 vecBodyPoints[7] =		// snake body shape
{
	D3DXVECTOR2( -1.0f, 0.0f ),
	D3DXVECTOR2( -0.866f, 0.5f ),
	D3DXVECTOR2( -0.5f, 0.866f ),
	D3DXVECTOR2( +0.0f, 1.0f ),
	D3DXVECTOR2( +0.5f, 0.866f ),
	D3DXVECTOR2( +0.866f, 0.5f ),
	D3DXVECTOR2( +1.0f, 0.0f ),
};

struct VertexSnakeBody9
{
	D3DXVECTOR3	PosCenter;	// ring center position
	D3DXVECTOR3	Dir;		// 
	D3DXVECTOR3	Tangent;	// 
	D3DXVECTOR3	Binormal;	// 
	D3DXVECTOR2	Distance;	// distance from the segments block begin and tex V coord
};


void	UpdateRing9( VertexSnakeBody9*pv, D3DXVECTOR3&vecCenter, D3DXVECTOR3&vecNorm, float flDist );

#endif//_SEGMENTSMODEL9_H_