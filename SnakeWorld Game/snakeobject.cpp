#include "snakeobject.h"
#include "constants.h"
#include "connectiondata.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////////
//
#define ADD_SEGMENT_MININTERVAL	(0.15)		// only one segment can be created in this time
#define ADD_SEGMENT_MAXINTERVAL	(10.0)		// after this time, new segment have to be created (to keep positions updated)

#define	TEST_COLLISION_MIN_LENGTH	(10.0)	// collision is tested only with snakes with length greater or equal to this


//////////////////////////////////////////////////////////////////////////
// snake segment ctor
CSnakeSegment::CSnakeSegment( unsigned int id, double x, double y )
: m_position(x,y)
, m_id(id)
{

}


//////////////////////////////////////////////////////////////////////////
// snake super segment ctor
CSnakeSuperSegment::CSnakeSuperSegment()
	:m_bound(0,0,0,0)
{
	m_curSegmentsLength = 0.0;
}


//////////////////////////////////////////////////////////////////////////
// snake super segment destructor
CSnakeSuperSegment::~CSnakeSuperSegment()
{
	m_segments.Clear( false );	// DO NOT DELETE SEGMENTS IN THIS LIST (they are duplicated in the CSnake::m_segments)
}


//////////////////////////////////////////////////////////////////////////
// add new segment on the end
void	CSnakeSuperSegment::Add( CSnakeSegment*pSegment )
{
	if (m_segments.Count() > 0)
	{
		m_bound += pSegment->Position();

		CSnakeSegment	*pLastSegment = m_segments.Last();

		if (pLastSegment)	// always true
		{
			m_curSegmentsLength += pLastSegment->Position().Distance( pSegment->Position() );
		}
	}
	else
	{
		m_bound = pSegment->Position();
		m_curSegmentsLength = 0.0;
	}

	m_segments.Add( pSegment );
}


//////////////////////////////////////////////////////////////////////////
// remove the first segment
void	CSnakeSuperSegment::RemoveFirst()
{
	CSnakeSegment	*firstSegment = m_segments[0];

	if (firstSegment)
	{
		bool bUpdateBounds = !m_bound.IsIn( firstSegment->Position() );

		m_segments.Remove( 0, false );	// remove the first segment from the list, but DO NOT DELETE HIM

		if (bUpdateBounds)
		{
			UpdateBoundsAndLength();
		}
	}
}


//////////////////////////////////////////////////////////////////////////
// update super-segment segments bounding rectangle
// update distances between segments in this super-segment
void	CSnakeSuperSegment::UpdateBoundsAndLength()
{
	CSnakeSegment *pPrevSeg = 0, *pCurSeg;

	m_curSegmentsLength = 0.0;
	
	for ( SegmentList::iterator it = m_segments.GetIterator(); !it.EndOfList(); ++it )
	{
		pCurSeg = it;

		if (pPrevSeg)
		{
			m_bound += pCurSeg->Position();
			m_curSegmentsLength += pPrevSeg->Position().Distance( pCurSeg->Position() );
		}
		else
		{
			m_bound = pCurSeg->Position();			
		}

		pPrevSeg = pCurSeg;
	}
}


//////////////////////////////////////////////////////////////////////////
// uninitialized snake ctor
CSnake::CSnake( bool bUserControlled )
: m_bUserControlled( bUserControlled )
, m_posHead( 0,0 )
, m_posHeadPrevious( 0,0 )
, m_curDirection( 0,0 )
, m_curSegmentsLength( 0.0 )
, m_boundrect( m_posHead )
{
	m_bInitialized = false;	// uninitialized
	m_bAlive = true;		// alive
	m_dLastUpdateTime = 0.0;//
	m_iUpdateBoundsCountdown = 0;	// update bounds interval

	// user controlled snake will add new segment in the first call of Move
	m_dAddSegmentAfter = 0.0;	
	m_bHeadDirectionChanged = true;
	
	// reset dirty values bits
	SetDirtyValue( DirtyNothing );

	//
	m_dLength = m_dSpeed = -1.0;	// invalid values
}


//////////////////////////////////////////////////////////////////////////
// dtor
CSnake::~CSnake()
{
	
}


//////////////////////////////////////////////////////////////////////////
// initialization done
void	CSnake::InitializationDone()
{
	locked(

		m_bInitialized = true;

		// head position in the last frame
		m_posHeadPrevious = m_posHead;

		// update snake bound rect
		UpdateBoundRectAndSegmentsLength();
		m_iUpdateBoundsCountdown = 0;

	);
}

//////////////////////////////////////////////////////////////////////////
// turn users snake head
void	CSnake::Turn( double dAngleAdd )
{
	if (dAngleAdd != 0.0 && m_bInitialized)
	{
		locked(
			m_bHeadDirectionChanged = true;

			m_dHeadDirection += dAngleAdd;
		);
	}
}


//////////////////////////////////////////////////////////////////////////
// snake move
void	CSnake::Move( double dStep )
{
	if (!m_bInitialized)
		return;	// not initialized yet or snake is dead -> do not move

	Lock();

	if ( m_bAlive )
	{
		/// check direction
		if ( m_bUserControlled &&	// snake must be user controlled to allow him add segments by himself
				(
				((m_dAddSegmentAfter -= dStep) <= 0.0 && m_bHeadDirectionChanged) ||	// direction was changed and "minimal add segment time" elapsed
				m_dAddSegmentAfter < -ADD_SEGMENT_MAXINTERVAL							// new segment was not created for "maximum add segment time"
				)
			)
		{
			//
			// only users snake should reach this
			//
			
			// align the direction angle
			while (m_dHeadDirection < 0.0 )	m_dHeadDirection += PI2;
			while (m_dHeadDirection >= PI2) m_dHeadDirection -= PI2;
			// compute direction vector
			UpdateDirectionVector();

			/// create new segment
			// get the last segment (first after the head)
			CSnakeSegment *pLastSegment = m_segments.Last();	// O( 1 ), only returns last node in the list

			// generate new segment id
			unsigned int newSegmentId;

			if (pLastSegment)
				newSegmentId = pLastSegment->Id() + 1;
			else
				newSegmentId = 0;

			// add new segment to the list end
			AddNewSegment( new CSnakeSegment(newSegmentId, m_posHead.x, m_posHead.y) );

			// head direction state checked
			m_bHeadDirectionChanged = false;
			m_dAddSegmentAfter = ADD_SEGMENT_MININTERVAL;
			m_dLastUpdateTime = 0.0;

			AddDirtyValue( DirtySegment );
		}

		// last update time
		m_dLastUpdateTime += dStep;

		/// move the head
		m_posHeadPrevious = m_posHead;	// save previous position
		m_posHead.Add( m_curDirection * (m_dSpeed * dStep) );	// head move
		m_boundrect += m_posHead;	// update bound rect

	}

	/// cut old segments (after tail)
	CutOldSegments();

	/**/
	Unlock();
}


//////////////////////////////////////////////////////////////////////////
// tests the collision of my head with <pSnakeBodyWith
bool	CSnake::TestCollision( CSnake*pSnakeBodyWith )
{
	bool bCollision = false;

	if ( m_bInitialized && m_bAlive &&
		 pSnakeBodyWith && pSnakeBodyWith->IsInitialized() && pSnakeBodyWith->IsAlive() &&
		 m_boundrect.IsIntersect( pSnakeBodyWith->m_boundrect ) &&
		 m_curSegmentsLength >= TEST_COLLISION_MIN_LENGTH && pSnakeBodyWith->m_curSegmentsLength >= TEST_COLLISION_MIN_LENGTH
		 )
	{	// bound boxes of this two snakes intersects -> collision is possible

		CSnakeSegment	*pLastSegment = m_segments.Last();

		if (pLastSegment)	// snake has body
		{
			// this snake head segment [the last segment - head]
			CVector		vecHeadNose = m_posHead + m_curDirection*2.0;
			CVector		vecHeadNosePrevious = m_posHeadPrevious;// + m_curDirection*2.0;
			CBoundRect		headSegRect( vecHeadNosePrevious, vecHeadNose );

			headSegRect += 1.5;	// head radius

			// try to find collision with the given snake body and my head segment
			bCollision = pSnakeBodyWith->TestCollision(
				vecHeadNosePrevious, vecHeadNose, headSegRect, 1.5,	// head segment, bound rect, head segment width
				(pSnakeBodyWith == this)?
					(m_dSpeed*1.0 + pLastSegment->Position().Distance(m_posHead)):	// do not test body created in the last 1.0 second when testing collision with myself
					(0.0) );		
		}
		
	}

	//
	return bCollision;
	
}


//////////////////////////////////////////////////////////////////////////
// test collision between the snake head and given apple
bool	CSnake::TestCollision(CApple *pApple)
{
	if ( m_boundrect.IsIntersect(pApple->GetBoundingRect()) && pApple->IsValid() )
	{
		// this snake head segment [the last segment - head]
		CVector		vecHeadNose = m_posHead + m_curDirection*2.0;
		CVector		vecHeadNosePrevious = m_posHeadPrevious;
		CBoundRect	headSegRect( vecHeadNosePrevious, vecHeadNose );

		headSegRect += 1.5;	// head radius

		// apple x head  collision
		if ( headSegRect.IsIntersect(pApple->GetBoundingRect()) )
			return true;
	}

	return false;
}


//////////////////////////////////////////////////////////////////////////
// test collision with the given line
// b1, b2 - line ends
// rectb - line bound rect
bool	CSnake::TestCollision(  CVector&b1, CVector&b2, CBoundRect&brect, double dWidth, double dIgnoreAfterHead )
{
	if (!m_bInitialized)
		return false;

	bool bCollision = false;

	Lock();


	CVector	*pLastPosition, *pCurrentPosition;
	double	dSegmentDistanceFromHead;

	SegmentList::iterator it = m_segments.GetIterator();
	// tail
	++it;	// skip the first segment
	pLastPosition = &GetTailPosition( &dSegmentDistanceFromHead );
	
	// body
	for ( ;!it.EndOfList();++it )
	{
		// next segment position
		pCurrentPosition = &(*it).Position();

		// update the segment distance from the head
		dSegmentDistanceFromHead -= pLastPosition->Distance( *pCurrentPosition );

		// test collision with line [ pLastPosition, pCurrentPosition ]
		if ( dSegmentDistanceFromHead >= dIgnoreAfterHead )
			if (TestIntersection( *pLastPosition, *pCurrentPosition, 1.0, b1, b2, dWidth, brect ))
			{
				bCollision = true;
				break;
			}

		// save the current segment position
		pLastPosition = pCurrentPosition;
	}

	// head segment (the last segment - head)
	if (!bCollision && dIgnoreAfterHead == 0.0)
	{
		CVector		vecHeadNose = m_posHead + m_curDirection*2.0;

		if (TestIntersection( *pLastPosition, vecHeadNose, 1.5, b1, b2, dWidth, brect ))
		{
			bCollision = true;
		}
	}

	Unlock();

	// no collision
	return bCollision;
}


//////////////////////////////////////////////////////////////////////////
// test intersection between line [a1,a2] x [b1,b2] with given width
bool	CSnake::TestIntersection( CVector&a1, CVector&a2, double aWidth, CVector&b1, CVector&b2, double bWidth, CBoundRect&brect )
{
	if ( brect.IsIntersect( CBoundRect(a1,a2) ) )
	{
		double dRad = (aWidth + bWidth) * 0.5;	// radius of first tested line + second

		/// transform the line B into the world where line A has coordinates [-aLength,0] - [0,0]

		CVector	aDir( a2 - a1 );			// line A direction
		double	aLength = aDir.Length();	// line A length

		if ( aLength > 0.0 )
		{
			CVector	aCosSin( aDir * ( 1.0 / aLength ) );	// line A normalized direction = direction angle sinus and cosines

			aCosSin.y *= -1.0;	// -angle

			// rotate (b1-a1) and (b2-a1) about -"aDir angle"
			CVector	b1Transformed = (b1 - a1).Rotate( aCosSin );	// 
			CVector	b2Transformed = (b2 - a1).Rotate( aCosSin );

			CBoundRect rc( 0.0, -dRad, aLength + dRad, +dRad);
			// there is a issue
			// testing only b2Transformed point works fine in most cases (for high fps)
			// testing line (b1Transformed, b2Transformed) intersect works good in all cases; but if somebody hit your head from the side, it will pass too
			return
				rc.IsIn( b2Transformed ) ||
				rc.IsIntersect( b1Transformed, b2Transformed, 0 );
		}		
	}

	return false;
}


//////////////////////////////////////////////////////////////////////////
// update bounding rectangle
// head + segments bound
// update distances between snake segments
void	CSnake::UpdateBoundRectAndSegmentsLength()
{
	m_boundrect = m_posHead;
	m_curSegmentsLength = 0.0;

	CSnakeSuperSegment	*pCur, *pPrev = 0;

	for ( SuperSegmentList::iterator it = m_supersegments.GetIterator();!it.EndOfList();++it )
	{
		pCur = it;

		// update bound rect and segments length
		m_boundrect += pCur->BoundRect();
		m_curSegmentsLength += pCur->Length();

		// the edge between two super-segments
		if (pPrev &&
			pPrev->Segments().Count() > 0 && 
			pCur->Segments().Count() > 0)
			m_curSegmentsLength += pPrev->Segments().Last()->Position().Distance( pCur->Segments()[0]->Position() );

		//
		pPrev = pCur;
	}
}
//////////////////////////////////////////////////////////////////////////
// update direction vector
void	CSnake::UpdateDirectionVector()
{
	m_curDirection.x = cos(m_dHeadDirection);
	m_curDirection.y = sin(m_dHeadDirection);
}

//////////////////////////////////////////////////////////////////////////
// cuts off too old segments (after tail)
void	CSnake::CutOldSegments()
{
	CSnakeSegment *pLastSegment = m_segments.Last();	// O( 1 ), only returns last node in the list

	if (pLastSegment)
	{
		double dHeadDistance = m_posHead.Distance( pLastSegment->Position() );

		if (m_curSegmentsLength + dHeadDistance > m_dLength)
		{
			// remove the tail if its possible
			while (m_segments.Count() >= 2)
			{
				CSnakeSegment	*firstSegment = m_segments[0];
				double dlength = firstSegment->Position().Distance( m_segments[1]->Position() );

				// cut off too old segments (after snakeLength)
				if ((m_curSegmentsLength - dlength + dHeadDistance) >= m_dLength )
				{
					CutFirstSegment();
				}
				else
				{   // there are no old segments
					break;
				}   
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////////
// add new segment after the head
void	CSnake::AddNewSegment( CSnakeSegment*pNewSegment )
{
	//
	// add the new segment
	//
	CSnakeSegment *pLastSegment = m_segments.Last();	// O( 1 ), only returns last node in the list

	if (pLastSegment)
		m_curSegmentsLength += pNewSegment->Position().Distance( pLastSegment->Position() );
	else
		m_curSegmentsLength = 0.0;

	// add new segment on the list end
	m_segments.Add( pNewSegment );

	// update bound rect
	m_boundrect += pNewSegment->Position();

	//
	// update super-segment
	//
	CSnakeSuperSegment	*pLastSuperSegment = m_supersegments.Last();	// last super segment

	if ( !pLastSuperSegment || pLastSuperSegment->Segments().Count() >= SUPERSEGMENT_CAPACITY )
	{	// create new (there is no another super segment or the old one has reached the capacity)
		pLastSuperSegment = NewSuperSegment();
		m_supersegments.Add( pLastSuperSegment );
	}

	// insert new segment into the super segment object
	pLastSuperSegment->Add( pNewSegment );
}


//////////////////////////////////////////////////////////////////////////
// remove the last snake segment (tail segment)
void	CSnake::CutFirstSegment()
{
	//
	// remove the first segment from the first super segment object
	//
	CSnakeSuperSegment*pFirstSuperSegment = m_supersegments[0];

	if (pFirstSuperSegment)
	{
		pFirstSuperSegment->RemoveFirst();

		// remove the super segment if its empty
		if ( pFirstSuperSegment->Segments().Count() == 0 )
		{
			m_supersegments.Remove(0, true);
		}
		
	}

	//
	// remove the first segment from the m_segments list
	//
	CSnakeSegment	*firstSegment = m_segments[0];

	if (firstSegment)
	{
		CSnakeSegment	*secondSegment = m_segments[1];
		double dlength = secondSegment ? ( firstSegment->Position().Distance( m_segments[1]->Position() ) ) : 0.0;

		// 
		bool bUpdateBounds = !m_boundrect.IsIn( firstSegment->Position() );

		m_segments.Remove( 0, true );	// remove first segment from the list

		m_curSegmentsLength -= dlength;	// decrease the m_curSegmentsLength

		if (bUpdateBounds)
		{
			UpdateBoundRectAndSegmentsLength();

			if ( m_iUpdateBoundsCountdown <= 0 )
			{
				AddDirtyValue( DirtyBounds );
				m_iUpdateBoundsCountdown = 16;
			}
			else
			{
				--m_iUpdateBoundsCountdown;
			}
		}
	}

	
}


//////////////////////////////////////////////////////////////////////////
// get valid first segment position
// get tail distance from the head ( == *pBodyLength )
CVector	CSnake::GetTailPosition( double*pOutBodyLength )
{
	double dHeadDistance = 0.0;

	// tail
	if (m_segments.Count() > 0)
	{
		CSnakeSegment*pFirstSeg = m_segments[0];	// != NULL
		CSnakeSegment*pSecondSeg = m_segments[1];	// should be NULL
		CSnakeSegment *pLastSegment = m_segments.Last();	// get the last segment (first after the head)	// O( 1 ), only returns last node in the list

		dHeadDistance = pLastSegment->Position().Distance( m_posHead );

		if ( m_dLength < (m_curSegmentsLength + dHeadDistance) )
		{	// "snake length" is less than segments length
			// tail must be shorted

			double		dTailLength;	// tail length
			CVector	posSecondSeg = (pSecondSeg?pSecondSeg->Position():m_posHead);	// position of second segment

			dTailLength = pFirstSeg->Position().Distance( posSecondSeg );	// tail length
			
			double l2 = (m_curSegmentsLength + dHeadDistance) - m_dLength;	// == <tail length> - <requested tail length>
			double f = l2 / dTailLength;	// tail move factor

			CVector tailMove = (pFirstSeg->Position() - posSecondSeg) * (-f);	// tail move vector == (tail direction * move factor)

			if (pOutBodyLength)	*pOutBodyLength = m_dLength;	// body length = "snake length"
			return pFirstSeg->Position() + tailMove;	// real tail position
		}
		else
		{
			if (pOutBodyLength)	*pOutBodyLength = (m_curSegmentsLength + dHeadDistance);	// body length is less than "snake length"
			return pFirstSeg->Position();
		}

	}
	else
	{
		if (pOutBodyLength)	*pOutBodyLength = 0.0;	// no body -> no body length

		return m_posHead;
	}
}


//////////////////////////////////////////////////////////////////////////
// updates from server

//////////////////////////////////////////////////////////////////////////
// new head state
void	CSnake::OnHeadUpdate( double x, double y )
{
	locked(
		m_posHead.x = x;
		m_posHead.y = y;

		// update bound rect
		m_boundrect += m_posHead;

		m_dLastUpdateTime = 0.0;
	);
}


//////////////////////////////////////////////////////////////////////////
// new snake head direction
void	CSnake::OnDirectionUpdate( double direction )
{
	locked(

		m_dHeadDirection = direction;
		m_dLastUpdateTime = 0.0;

		UpdateDirectionVector();

	);
}


//////////////////////////////////////////////////////////////////////////
// new segment
// returns false if id doesn't match last segments id
bool	CSnake::OnSegmentAdd( unsigned int id, double x, double y )
{
	bool bOk = true, bAdd = false;

	// LOCK
	Lock();

	CSnakeSegment *pLastSegment = m_segments.Last();	// O( 1 ), only returns last node in the list

	if ( pLastSegment )
	{
		if (id == pLastSegment->Id() + 1)
		{
			// ok, add new segment
			bAdd = true;
		}
		else if ( id == pLastSegment->Id() )
		{
			// already inserted, do nothing
		}
		else if (id > pLastSegment->Id() + 1)
		{
			bOk = false;	// some segments missing, snake have to be reseted
		}
		else
		{
			// already inserted ? or segment id overflow
			bOk = false;	// in all cases - this segment is too old or too new :)
		}
	}
	else
	{
		bAdd = true;
	}

	if ( bAdd )
	{
		// add new segment (that follows the last segment)
		AddNewSegment( new CSnakeSegment(id, x,y) );

		// update head position
		OnHeadUpdate(x,y);	// -> update bound rect

		// cut segments after tail
		CutOldSegments();
	}

	// UNLOCK
	Unlock();

	return bOk;
}


//////////////////////////////////////////////////////////////////////////
// set new texture
void	CSnake::OnTextureUpdate( CString&szTexture )
{

	m_dLastUpdateTime = 0.0;
}


//////////////////////////////////////////////////////////////////////////
// snake was killed
void	CSnake::OnSnakeKilled()
{
	m_bAlive = false;
}


//////////////////////////////////////////////////////////////////////////
// set new type
void	CSnake::OnTypeUpdate( CString&szType )
{
	locked(
	m_dLastUpdateTime = 0.0;
	);
}


//////////////////////////////////////////////////////////////////////////
// sending commands to the server

//////////////////////////////////////////////////////////////////////////
// send values to the server
void	CSnake::SendDirtyValues( CBinaryWriter*pbw )
{
	Lock();

	if (m_bInitialized && m_bAlive && m_dirtyvalues != DirtyNothing && pbw)
	{
		if ( IsDirty(DirtyHead) )
		{
			Send_HeadDirection(pbw,m_dHeadDirection);
			Send_HeadPosition(pbw,m_posHead);
		}

		if ( IsDirty(DirtySpeed))
		{
			Send_Speed(pbw,m_dSpeed);
		}

		if ( IsDirty(DirtyLength) )
		{
			Send_Length(pbw,m_dLength);
		}

		if ( IsDirty(DirtyBounds) )
		{
			Send_Bound(pbw,m_boundrect);
		}

		if ( IsDirty(DirtySegment) )
		{
			Send_HeadDirection(pbw,m_dHeadDirection);
			
			CSnakeSegment*pLastSegment = m_segments.Last();
			if (pLastSegment)
				Send_Segment(pbw,pLastSegment);
		}

		SetDirtyValue( DirtyNothing );

		// end of messages block
		Send_BlockEnd(pbw);

		// FLUSH !!
		pbw->Flush();
	}

	Unlock();
	
}


//////////////////////////////////////////////////////////////////////////
// kill this snake and send it to the server
void	CSnake::KillSnake( unsigned int iKilledBy, CBinaryWriter*pbw )
{
	if ( m_bAlive )
	{
		// send info to the server
		// the connection will be closed (by server)
		// other snakes will be informed, i was killed by snake with id <iKilledBy>
		if (pbw)
		{
			Send_Dead(pbw, iKilledBy);
			// FLUSH !!
			pbw->Flush();
		}

		OnSnakeKilled();
	}
}


//
// sending state to the binary writer
//
void CSnake::Send_HeadDirection( CBinaryWriter*pbw, double dir )
{
	if ( !pbw )	return;

	pbw->Write( (unsigned char)CMD_SNAKE_DIR );

	CmdSnakeDirection data;
	data.dAngle = dir;
	pbw->Write( (char*)&data, sizeof(data) );
}
void CSnake::Send_HeadPosition( CBinaryWriter*pbw, CVector&pos )
{
	if ( !pbw )	return;

	pbw->Write( (unsigned char)CMD_SNAKE_HEAD );

	CmdSnakeHead data;
	data.x = pos.x;
	data.y = pos.y;

	pbw->Write( (char*)&data, sizeof(data) );
}
void CSnake::Send_Speed( CBinaryWriter*pbw, double speed )
{
	if ( !pbw )	return;

	pbw->Write( (unsigned char)CMD_SNAKE_SPEED );

	CmdSnakeSpeed data;
	data.dSpeed = speed;
	pbw->Write( (char*)&data, sizeof(data) );
}
void CSnake::Send_Length( CBinaryWriter*pbw, double length )
{
	if ( !pbw )	return;

	pbw->Write( (unsigned char)CMD_SNAKE_LENGTH );

	CmdSnakeLength data;
	data.dLength = length;
	pbw->Write( (char*)&data, sizeof(data) );
}
void CSnake::Send_Bound( CBinaryWriter*pbw, CBoundRect&rect )
{
	if ( !pbw )	return;

	pbw->Write( (unsigned char)CMD_SNAKE_BOUND );

	CmdSnakeBound data;
	data.x1 = rect.topleft.x;
	data.x2 = rect.bottomright.x;
	data.y1 = rect.topleft.y;
	data.y2 = rect.bottomright.y;
	pbw->Write( (char*)&data, sizeof(data) );
}
void CSnake::Send_Segment( CBinaryWriter*pbw, CSnakeSegment*pSegment )
{
	if ( !pbw )	return;

	pbw->Write( (unsigned char)CMD_SNAKE_SEG );

	pbw->Write( (unsigned int)pSegment->Id() );
	pbw->Write( (double)pSegment->Position().x );
	pbw->Write( (double)pSegment->Position().y );
}
void CSnake::Send_BlockEnd( CBinaryWriter*pbw )
{
	if ( !pbw )	return;

	pbw->Write( (unsigned char)CMD_BLOCKEND );
}
void CSnake::Send_Dead( CBinaryWriter*pbw, unsigned int iKilledBy )
{
	if ( !pbw )	return;

	pbw->Write( (unsigned char)CMD_SNAKE_DEAD );

	CmdSnakeId data;
	data.iId = iKilledBy;
	pbw->Write( (char*)&data, 4 /*!!!! */ );
}