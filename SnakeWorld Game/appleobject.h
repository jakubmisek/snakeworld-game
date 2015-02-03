#ifndef _APPLEOBJECT_H_
#define _APPLEOBJECT_H_

#include "vector.h"
#include "boundrect.h"

#include "appleparam.h"

// base apple object
class CApple
{
public:
	// new apple initialization
	CApple( double x, double y );

	// destroy the apple
	virtual ~CApple();

	// new position retrieved
	// if position is different from the previous position, apple should be restarted
	virtual void OnUpdatePosition( double x, double y );

	// apple was eaten
	// invalidate him
	// some animation should be nice
	virtual void OnEaten();

	// apple bounding rect
	CBoundRect&GetBoundingRect(){ return m_boundrect; }

	// apple position
	CVector&	GetPosition(){ return m_position; }

	// apple is valid (enabled)
	bool		IsValid(){ return m_bValid; }

protected:

	// apple position
	CVector	m_position;

	// apple bounding rectangle
	CBoundRect m_boundrect;

	// apple validity, gets true on the initialization and position update
	// is false after apple is eaten by the users snake
	bool	m_bValid;

};

#endif//_APPLEOBJECT_H_