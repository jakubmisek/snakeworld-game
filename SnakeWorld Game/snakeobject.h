#ifndef _SNAKEOBJECT_H_
#define _SNAKEOBJECT_H_

#include "list.h"
#include "vector.h"
#include "binaryio.h"
#include "string.h"
#include "boundrect.h"
#include "locker.h"
#include "appleobject.h"

//////////////////////////////////////////////////////////////////////////
//
#define SUPERSEGMENT_CAPACITY		(64)		// TODO: 128 ?

//////////////////////////////////////////////////////////////////////////
// Snake segment object
class CSnakeSegment
{
public:
	CSnakeSegment( unsigned int id, double x, double y );

	inline unsigned int Id()
	{
		return m_id;
	}

	CVector&	Position()
	{
		return m_position;
	}

private:
	
	CVector		m_position;

	unsigned int	m_id;

};

//////////////////////////////////////////////////////////////////////////
// list of segments
typedef CList<CSnakeSegment>		SegmentList;

//////////////////////////////////////////////////////////////////////////
// Snake super segment object
class CSnakeSuperSegment
{
public:

	CSnakeSuperSegment();
	virtual ~CSnakeSuperSegment();

	void	Add( CSnakeSegment*pSegment );
	void	RemoveFirst();

	inline SegmentList&	Segments(){ return m_segments; }

	inline CBoundRect&BoundRect(){ return m_bound; }
	inline double Length(){ return m_curSegmentsLength; }

protected:

	SegmentList		m_segments;
	CBoundRect		m_bound;

	double			m_curSegmentsLength;

	void	UpdateBoundsAndLength();
};


//////////////////////////////////////////////////////////////////////////
// list of super segments
typedef CList<CSnakeSuperSegment>	SuperSegmentList;

//////////////////////////////////////////////////////////////////////////
// Snake object
class CSnake: public ILocker
{
public:
	CSnake( bool bUserControlled );	// uninitialized snake
	virtual ~CSnake();

	//
	bool	IsInitialized(){ return m_bInitialized; }
	virtual void	InitializationDone();
	
	//
	virtual	void	Move( double dStep );
	
	void	Turn( double dAngleAdd );
	
	bool	TestCollision( CSnake*pSnakeBodyWith );	// tests the collision of my head with <pSnakeBodyWith>
	bool	TestCollision( CApple*pApple );	// tests the collision of my head with <pApple>

	// updates from server
	void	OnHeadUpdate( double x, double y );
	void	OnDirectionUpdate( double direction );
	bool	OnSegmentAdd( unsigned int id, double x, double y );

	virtual	void	OnTextureUpdate( CString&szTexture );
	virtual	void	OnTypeUpdate( CString&szType );
	virtual	void	OnSnakeKilled();

	virtual bool	WantToDelete(){ return !m_bAlive; }

	// parameters
	double	GetSegmentsLength(){ return m_curSegmentsLength; }
	double	GetLength(){ return m_dLength; }
	void	OnLengthUpdate( double dLength ){ m_dLength = dLength; }

	void	LengthAdd( double dAdd ){ if (dAdd != 0.0){ m_dLength += dAdd; AddDirtyValue(DirtyLength); } }

	double	GetSpeed(){ return m_dSpeed; }
	void	OnSpeedUpdate( double dSpeed ){ m_dSpeed = dSpeed; }

	CVector&GetHeadPosition(){ return m_posHead; }
	CVector&GetHeadDirection(){return m_curDirection; }
	CBoundRect&GetBoundingRect(){ return m_boundrect; }
	double	GetLastUpdatetime(){ return m_dLastUpdateTime; }

	CString	Name, Description;
	unsigned int iLanguage;

	bool	IsAlive(){ return m_bAlive; }
	void	KillSnake( unsigned int iKilledBy, CBinaryWriter*pbw );
	
	// send to server
	void	SendDirtyValues( CBinaryWriter*pbw );

protected:

	//
	double	m_dAddSegmentAfter;

	// parameters
	double	m_dLength, m_dSpeed;
	bool	m_bUserControlled;
	
	// state
	bool		m_bAlive;
	bool		m_bInitialized;

	double		m_dLastUpdateTime;
	int			m_iUpdateBoundsCountdown;

	// snake head
	double		m_dHeadDirection;	// in radians
	CVector		m_posHead;			// head position
	CVector		m_posHeadPrevious;	// head position in the previous frame

	bool		m_bHeadDirectionChanged;

	// snake body
	SegmentList			m_segments;		// snake body segments
	SuperSegmentList	m_supersegments;// snake body segments bucket, list of buckets with SUPERSEGMENT_CAPACITY segments

	CBoundRect	m_boundrect;
	void		UpdateBoundRectAndSegmentsLength();	// recompute bound rectangle (head+segments bound) and segments length (m_curSegmentsLength)

	bool		TestCollision( CVector&b1, CVector&b2, CBoundRect&brect, double dWidth, double dIgnoreAfterHead );
	bool		TestIntersection( CVector&a1, CVector&a2, double aWidth, CVector&b1, CVector&b2, double bWidth, CBoundRect&brect );	// find intersection between line [a1,a2] x [b1,b2] with given width

	// precomputed variables
	CVector		m_curDirection;	// == [ cos(direction), sin(direction) ] (its normalized)
	double		m_curSegmentsLength;	// == sum of distances between segments

	void		UpdateDirectionVector();	// m_curDirection = [ cos(direction), sin(direction) ] (normalized)

	void		CutOldSegments();	// cuts off old segments (after tail)
	CVector		GetTailPosition( double*pOutBodyLength );	// get the valid first segment position and his distance from the head ( == *pBodyLength )

	// add/remove segments
	void		AddNewSegment( CSnakeSegment*pNewSegment );
	void		CutFirstSegment();

	virtual CSnakeSuperSegment*NewSuperSegment(){ return new CSnakeSuperSegment(); }

	// modifications bitmap
	unsigned int		m_dirtyvalues;
	
	enum EDirtyValues
	{
		DirtyNothing = 0,

		DirtyHead = 1,		// head changed
		DirtyLength = 2,	// length changed
		DirtySpeed = 4,		// speed changed
		DirtyBounds = 8,	// send new bounding rectangle (to recompute this information is needed O( n ) time, so every client sends this to the server to save servers time)
		DirtySegment = 16,	// includes head direction, only this command implicates sending info to the other snakes by server

		DirtyEverything = 0xffff
	};

	inline	bool	IsDirty( EDirtyValues val ){ return (m_dirtyvalues & val) == val; }
	inline	void	AddDirtyValue( EDirtyValues val ){ m_dirtyvalues |= (unsigned int)(val); }
	inline	void	SetDirtyValue( EDirtyValues val ){ m_dirtyvalues = (unsigned int)(val); }

	//
	// sending state to the binary writer
	//
	void Send_HeadDirection( CBinaryWriter*pbw, double dir );
	void Send_HeadPosition( CBinaryWriter*pbw, CVector&pos );
	void Send_Speed( CBinaryWriter*pbw, double speed );
	void Send_Length( CBinaryWriter*pbw, double length );
	void Send_Bound( CBinaryWriter*pbw, CBoundRect&rect );
	void Send_Segment( CBinaryWriter*pbw, CSnakeSegment*pSegment );
	void Send_BlockEnd( CBinaryWriter*pbw );
	void Send_Dead( CBinaryWriter*pbw, unsigned int iKilledBy );
};

#endif//_SNAKEOBJECT_H_