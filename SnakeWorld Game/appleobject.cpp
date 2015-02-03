#include "appleobject.h"

// new apple initialization
CApple::CApple( double x, double y )
:m_position(x,y)
,m_boundrect(m_position,APPLE_RADIUS)
,m_bValid(true)
{
	
}


// destroy the apple
CApple::~CApple()
{

}


// new position retrieved
// if position is different from the previous position, apple should be restarted
void	CApple::OnUpdatePosition( double x, double y )
{
	m_position.x = x;
	m_position.y = y;

	m_boundrect = CBoundRect(m_position,APPLE_RADIUS);

	m_bValid = true;
}


// apple was eaten
// invalidate him
// some animation should be nice
void	CApple::OnEaten()
{
	m_bValid = false;
}