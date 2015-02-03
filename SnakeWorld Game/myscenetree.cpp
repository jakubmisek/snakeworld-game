#include "myscenetree.h"


//////////////////////////////////////////////////////////////////////////
// base tree node ctor
CSceneBaseTree::CSceneBaseTree( CSceneBaseTree*pParentNode, int x, int y, int cx, int cy, int scenex, int sceney, int scenecx, int scenecy )
{
	// size
	m_x = x;
	m_y = y;
	m_cx = cx;
	m_cy = cy;

	// bound box
	m_bound = D3DXVECTOR3(
		(float)x * TERRAIN_SIZE,
		-5.0f,	// tollerance because of ground spheric shape
		(float)y * TERRAIN_SIZE );

	m_bound.m_vecMax += D3DXVECTOR3(
		(float)cx * TERRAIN_SIZE,
		10.0f,	// tollerance because of grass and snakes
		(float)cy * TERRAIN_SIZE );

	// scene size
	m_scenex = scenex;
	m_sceney = sceney;
	m_scenecx = scenecx;
	m_scenecy = scenecy;

	// parent node
	m_pParentNode = pParentNode;
}


//////////////////////////////////////////////////////////////////////////
// moves the given object from this node into the right node
// object mustn't be in any tree node (m_pNode should be NULL)
// object will be moved into the lowest node that overlaps object
void	CSceneBaseTree::MoveMovableObject( ISceneTree_MovableObject*pObject )
{
	CSceneBaseTree	*p = this;

	// move up, until object does not fit
	while ( p->m_pParentNode )
	{
		if ( p->Encloses( pObject->GetBoundingBox() ) )
			break;	// pObject fits in "p"

		// move up
		p = p->m_pParentNode;
	}

	// move down, until object fits and add into the lowest node
	p->AddMovableObject( pObject );
}


//////////////////////////////////////////////////////////////////////////
// inner tree node ctor
CSceneTreeNode::CSceneTreeNode( CSceneBaseTree*pParentNode, int x, int y, int cx, int cy, int scenex, int sceney, int scenecx, int scenecy )
:CSceneBaseTree(pParentNode,x,y,cx,cy,scenex,sceney,scenecx,scenecy)
{
	ZeroMemory( m_pChilds, sizeof(m_pChilds) );
}


//////////////////////////////////////////////////////////////////////////
// inner tree node dtor
CSceneTreeNode::~CSceneTreeNode()
{
	// delete child nodes
	for ( int i = 0; i < 4; ++i )
		SAFE_DELETE( m_pChilds[i] );
}


//////////////////////////////////////////////////////////////////////////
// create tree nodes in the view frustum and not created yet
// delete tree nodes out of boxKeepAlive
// animate tree nodes in the view frustum (grass, ...)
void	CSceneTreeNode::FrameMove(
								  CDXDevice&dev, double dStep, CCamera&camera,
								  CBoundBox&boxKeepAlive,
								  CGrassContainer&grassmodels, CStoneWallContainer&stonewallmodels,
								  MovableObjectsList**ppTmpArray, unsigned int nTmpArray )
{
	// add my movable objects list
	if ( m_movableobjects.Count() )
		ppTmpArray[nTmpArray++] = &m_movableobjects;

	// moves the child nodes
	for (int i = 0; i < 4; ++i )
	{
		if ( m_pChilds[i] )
		{
			// delete the child node, if its out of the "boxKeepAlive"
			if ( !m_pChilds[i]->GetBoundBox()->Intersects( boxKeepAlive ) )
			{	// m_pChilds[i] is not in the refreshable area now, delete it
				delete m_pChilds[i];
				m_pChilds[i] = NULL;

				continue;	// m_pChilds[i] is NULL
			}
		}
		else
		{
			// create child node
			if ( !(m_pChilds[i] = CreateChildNode( i )) )	// create new child
				continue;	// child was not created (too small probably)
		}

		// animate the child (m_pChilds[i] != NULL)
		// and create child-child nodes if the child node is in the view frustum
		if ( camera.CullBlock( m_pChilds[i]->GetBoundBox() ) )
		{
			m_pChilds[i]->FrameMove( dev, dStep, camera, boxKeepAlive, grassmodels, stonewallmodels, ppTmpArray, nTmpArray );
		}
	}
}


//////////////////////////////////////////////////////////////////////////
// render tree nodes visible in the view frustum
void	CSceneTreeNode::RenderVisible( CDXDevice&dev, CCamera&camera, CTerrainContainer&terrainmodel )
{
	// render objects in this node
	ISceneTree_Moving::RenderObjects( dev, camera );

	// ender the child nodes
	for (int i = 0; i < 4; ++i )
	{
		if ( m_pChilds[i] && camera.CullBlock( m_pChilds[i]->GetBoundBox() ) )
			m_pChilds[i]->RenderVisible( dev, camera, terrainmodel );
	}
}


/*
- render shadow volume models of objects visible in the given view frustum
*/
void	CSceneTreeNode::RenderShadowVolumes( CDXDevice&dev, CCamera&camera )
{
	// render objects in this node
	ISceneTree_Moving::RenderObjectsShadowVolume( dev, camera );

	// ender the child nodes
	for (int i = 0; i < 4; ++i )
	{
		if ( m_pChilds[i] && camera.CullBlock( m_pChilds[i]->GetBoundBox() ) )
			m_pChilds[i]->RenderShadowVolumes( dev, camera );
	}
}


//////////////////////////////////////////////////////////////////////////
// add the object into this node or child nodes
// new node overlaps the given object but childs not or childs are NULL
void	CSceneTreeNode::AddMovableObject( ISceneTree_MovableObject*pObject )
{
	for (int i = 0; i < 4; ++i )
	{
		// find the child that overlaps given object
		if ( m_pChilds[i] && m_pChilds[i]->Encloses( pObject->GetBoundingBox() ) )
		{
			m_pChilds[i]->AddMovableObject( pObject );	// add into the child node
			return;
		}
	}

	// else, add into this node. Given object does not fit into any child node.
	CSceneBaseTree::AddMovableObject( pObject );
}


//////////////////////////////////////////////////////////////////////////
// create new child node with new size specified by given child index
CSceneBaseTree*	CSceneTreeNode::CreateChildNode( int index )
{
	// left-top child size
	int cx = m_cx / 2;
	int cy = m_cy / 2;

	// child position
	int x,y;

	// specify the child size and position
	switch (index)
	{
	case 0:	// left top
		x = m_x;
		y = m_y;
		break;
	case 1:	// right top
		x = m_x + cx;
		y = m_y;

		cx = m_cx - cx;
		break;
	case 2:	// left bottom
		x = m_x;
		y = m_y + cy;

		cy = m_cy - cy;
		break;
	case 3:	// right bottom
		x = m_x + cx;
		y = m_y + cy;

		cx = m_cx - cx;
		cy = m_cy - cy;
		break;
	default:
		return NULL;
	}

	// create the child node with specified size and position
	if (cx >= 1 && cy >= 1)
	{
		if (cx == 1 && cy == 1)
			return new CSceneTreeLeaf( this,x,y,cx,cy,m_scenex,m_sceney,m_scenecx,m_scenecy );	// child will be leaf
		else //if (cx >= 2 || cy >= 2)
			return new CSceneTreeNode( this,x,y,cx,cy,m_scenex,m_sceney,m_scenecx,m_scenecy );	// child will be inner node
	}
	
	// cx or cy are zero
	return 0;
}


//////////////////////////////////////////////////////////////////////////
// scene tree leaf ctor
CSceneTreeLeaf::CSceneTreeLeaf( CSceneBaseTree*pParentNode, int x, int y, int cx, int cy, int scenex, int sceney, int scenecx, int scenecy )
:CSceneBaseTree(pParentNode,x,y,cx,cy,scenex,sceney,scenecx,scenecy)
{
	m_pGrassModel = NULL;
	m_pStoneWallModel = NULL;

	m_bStoneWallChecked = false;
}


//////////////////////////////////////////////////////////////////////////
// scene tree leaf dtor
CSceneTreeLeaf::~CSceneTreeLeaf()
{
	SAFE_RELEASE( m_pGrassModel );
	SAFE_RELEASE( m_pStoneWallModel );
}


//////////////////////////////////////////////////////////////////////////
// create tree nodes in the view frustum and not created yet
// animate the world terrain (grass, ...)
void	CSceneTreeLeaf::FrameMove(
								  CDXDevice&dev, double dStep, CCamera&camera,
								  CBoundBox&boxKeepAlive,
								  CGrassContainer&grassmodels, CStoneWallContainer&stonewallmodels,
								  MovableObjectsList**ppTmpArray, unsigned int nTmpArray )
{
	// add my movable objects list
	if ( m_movableobjects.Count() )
		ppTmpArray[nTmpArray++] = &m_movableobjects;

	// animate the grass
	GrassFrameMove( dev,dStep,camera,grassmodels,ppTmpArray,nTmpArray );

	// create the stone wall if not yet
	StoneWallFrameMove( dev, stonewallmodels );
}


//////////////////////////////////////////////////////////////////////////
// create/animate the grass
void	CSceneTreeLeaf::GrassFrameMove(
									   CDXDevice&dev, double dStep, CCamera&camera,
									   CGrassContainer&grassmodels,
									   MovableObjectsList**ppTmpArray, unsigned int nTmpArray )
{
	//
	// create the grass if not created yet
	//
	if ( !m_pGrassModel )
	{
		if ( !grassmodels.CreateModel( &m_pGrassModel ) )
			return;
	}

	// do not animate grass too far from the camera
	if ( GetDistanceFromCamera(camera) < TERRAIN_SIZE * 3.0f )
	{
		//
		// create the grass move mask
		//

		grassmodels.ClearGrassMoveMap();

		// this leaf translation
		D3DXVECTOR3	vecPos( (float)m_x * TERRAIN_SIZE, 0, (float)m_y * TERRAIN_SIZE );

		//
		bool bRenderingGrassMove = false;

		// init the grass move texture map
		if ( nTmpArray )
		{
			// render the objects grass move map
			for (unsigned int i = 0; i < nTmpArray; ++i )
			{
				for (MovableObjectsList::iterator it = ppTmpArray[i]->GetIterator();!it.EndOfList();++it)
				{
					ISceneTree_MovableObject*pObject = it;

					// test if the model is visible in the view frustum
					if ( Intercests( pObject->GetBoundingBox() ) )
					{
						if (!bRenderingGrassMove)
						{
							bRenderingGrassMove = true;
							grassmodels.BeginRenderToGrassMoveMap();
						}

						pObject->RenderGrassMove( dev, vecPos );
						grassmodels.SetGrassMoveMapDirty();	// the grass move map have to be cleared next time
					}

				}
			}
		}

		// render the stone wall grass move map
		if (m_pStoneWallModel)
		{
			if (!bRenderingGrassMove)
			{
				bRenderingGrassMove = true;
				grassmodels.BeginRenderToGrassMoveMap();
			}

			m_pStoneWallModel->RenderStoneWallGrassMove( dev, vecPos );
			grassmodels.SetGrassMoveMapDirty();	// the grass move map have to be cleared next time
		}

		// end rendering the grass move
		if (bRenderingGrassMove)
		{
			grassmodels.EndRenderToGrassMoveMap();
		}


		//
		// animate the grass
		//
		m_pGrassModel->FrameMove( dStep );
	}
}



//////////////////////////////////////////////////////////////////////////
// create the stone wall
void	CSceneTreeLeaf::StoneWallFrameMove( CDXDevice&dev, CStoneWallContainer&stonewallmodels )
{
	if ( !m_bStoneWallChecked && !m_pStoneWallModel )
	{
		m_bStoneWallChecked = true;

		// check if there should be wall in this leaf
		if ( m_x < m_scenex-1 || m_y < m_sceney-1 || m_x > (m_scenex+m_scenecx) || m_y > (m_sceney+m_scenecy) )
			return;	// over the scene border
		if ( ( m_x == m_scenex-1 && (m_y == m_sceney-1 || m_y == m_sceney+m_scenecy) ) ||
			 ( m_x == m_scenex+m_scenecx && (m_y == m_sceney-1 || m_y == m_sceney+m_scenecy) )
			) return;	// in the border corner

		// set the wall transform matrix (position)
		D3DXMATRIX matRot, matPos;
		if ( m_x == m_scenex-1 )
		{	// on the left
			D3DXMatrixTranslation( &matPos, (m_x+1)*TERRAIN_SIZE, 0.0f, (m_y)*TERRAIN_SIZE );
			D3DXMatrixRotationY( &matRot, - (float)D3DX_PI * 0.5f );
		}
		else if ( m_x == m_scenex+m_scenecx )
		{	// on the right
			D3DXMatrixTranslation( &matPos, (m_x)*TERRAIN_SIZE, 0.0f, (m_y+1)*TERRAIN_SIZE );
			D3DXMatrixRotationY( &matRot, + (float)D3DX_PI * 0.5f );
		}
		else if ( m_y == m_sceney-1 )
		{	// near
			D3DXMatrixTranslation( &matPos, (m_x+1)*TERRAIN_SIZE, 0.0f, (m_y+1)*TERRAIN_SIZE );
			D3DXMatrixRotationY( &matRot, (float)D3DX_PI * 1.0f );
		}
		else if (m_y == m_sceney+m_scenecy)
		{	// far
			D3DXMatrixTranslation( &matPos, (m_x)*TERRAIN_SIZE, 0.0f, (m_y)*TERRAIN_SIZE );
			D3DXMatrixRotationY( &matRot, (float)D3DX_PI * 0.0f );
		}
		else
		{
			return;	// not in border
		}

		// create the wall object
		D3DXMATRIX matTransform;
		D3DXMatrixMultiply( &matTransform, &matRot, &matPos );

		if ( !stonewallmodels.CreateStoneWall( &m_pStoneWallModel, matTransform ) )
		{
			// m_bStoneWallChecked = false ?
		}
	}
}


//////////////////////////////////////////////////////////////////////////
// render the terrain and grass
// if its called, we should be sure this node is visible (see CSceneTreeNode::RenderTerrain - called nodes are already culled)
void	CSceneTreeLeaf::RenderVisible( CDXDevice&dev, CCamera&camera, CTerrainContainer&terrainmodel )
{
	//
	// render objects in this node
	//
	ISceneTree_Moving::RenderObjects( dev, camera );

	
	//
	// render the wall
	//
	if (m_pStoneWallModel)
	{
		// render the stone wall
		m_pStoneWallModel->RenderStoneWall( dev );
	}

	//
	// this leaf grass&terrain translation
	//
	D3DXMATRIX matWorld;

	if (dev.pDX10)
		UpdateDeviceTranslation10(*dev.pDX10,matWorld);
	else if (dev.pDX9)
		UpdateDeviceTranslation9(*dev.pDX9,matWorld);
	else
		return;


	//
	// render the grass
	//

	if (m_pGrassModel)
	{
		float flMinDistance = GetDistanceFromCamera(camera);
		float flDetail;

		// get the requested render detail
		if (flMinDistance < TERRAIN_SIZE)
			flDetail = 1.0f;
		else
			flDetail = 1.0f - (flMinDistance - TERRAIN_SIZE) / (camera.GetViewDistance() - TERRAIN_SIZE);
			

		m_pGrassModel->RenderGrass( dev, flDetail*flDetail, matWorld );
	}

	//
	// render the terrain
	//

	terrainmodel.RenderTerrain( dev );

	
}


//////////////////////////////////////////////////////////////////////////
// render objects shadow volume
void	CSceneTreeLeaf::RenderShadowVolumes( CDXDevice&dev, CCamera&camera )
{
	// render objects in this node
	ISceneTree_Moving::RenderObjectsShadowVolume( dev, camera );

	// render stone wall shadow
	if (m_pStoneWallModel)
	{
		m_pStoneWallModel->RenderStoneWallShadowVolume( dev );
	}
}


//////////////////////////////////////////////////////////////////////////
// return the distance of this leaf from the camera
float	CSceneTreeLeaf::GetDistanceFromCamera( CCamera&camera )
{
	D3DXVECTOR3	vecCorner[4];
	float	flMinDistance = camera.GetViewDistance();

	// find the nearest grass model corner
	vecCorner[0] = m_bound.m_vecMin;
	vecCorner[1] = m_bound.m_vecMin + D3DXVECTOR3( TERRAIN_SIZE,0,0 );
	vecCorner[2] = m_bound.m_vecMin + D3DXVECTOR3( 0,0,TERRAIN_SIZE );
	vecCorner[3] = m_bound.m_vecMin + D3DXVECTOR3( TERRAIN_SIZE,0,TERRAIN_SIZE );

	for (int i = 0; i < 4; ++i)
	{
		float fl = camera.Distance( vecCorner[i] );

		if (fl < flMinDistance)
			flMinDistance = fl;
	}

	return flMinDistance;
}


//////////////////////////////////////////////////////////////////////////
// update translation matrix
void	CSceneTreeLeaf::UpdateDeviceTranslation10(CDX10Device&dev, D3DXMATRIX&matWorld)
{
	D3DXVECTOR3	vecPos( (float)m_x * TERRAIN_SIZE, 0, (float)m_y * TERRAIN_SIZE );

	D3DXMatrixTranslation( &matWorld, vecPos.x, vecPos.y, vecPos.z );
	dev.pVarMatWorld->SetMatrix( (float*)&matWorld );	// set the HLSL translation
}
void	CSceneTreeLeaf::UpdateDeviceTranslation9(CDX9Device&dev, D3DXMATRIX&matWorld)
{
	D3DXVECTOR3	vecPos( (float)m_x * TERRAIN_SIZE, 0, (float)m_y * TERRAIN_SIZE );

	D3DXMatrixTranslation( &matWorld, vecPos.x, vecPos.y, vecPos.z );
	dev.pEffect->SetMatrix( dev.pVarWorldTransform, &matWorld  );
}