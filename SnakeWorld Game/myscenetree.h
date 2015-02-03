#ifndef _MYSCENETREE_H_
#define _MYSCENETREE_H_

//////////////////////////////////////////////////////////////////////////

#include "boundrect.h"
#include "boundbox.h"
#include "camera.h"
#include "device.h"

#include "gameworld.h"
#include "mysnake.h"

#include "terrain.h"
#include "grass.h"
#include "stonewall.h"


//////////////////////////////////////////////////////////////////////////
// base tree node
// abstract class for tree nodes with basic functionality
class CSceneBaseTree: public ISceneTree_Moving
{
public:
	CSceneBaseTree( CSceneBaseTree*pParentNode, int x, int y, int cx, int cy, int scenex, int sceney, int scenecx, int scenecy );
	virtual ~CSceneBaseTree(){/*nothing to be released*/}

	/*
		- create tree nodes in the view frustum and not created yet
		- delete tree nodes out of boxKeepAlive (boxKeepAlive must be greater than View Frustum)
		- animate the world terrain (grass, ...)
	*/
	virtual void	FrameMove(
		CDXDevice&dev, double dStep, CCamera&camera,
		CBoundBox&boxKeepAlive,
		CGrassContainer&grassmodels, CStoneWallContainer&stonewallmodels,
		MovableObjectsList**ppTmpArray, unsigned int nTmpArray = 0	// array to be used to store visible snakes in the current tree path
		) = 0;


	/*
		- render tree nodes visible in the view frustum
	*/
	virtual void	RenderVisible( CDXDevice&dev, CCamera&camera, CTerrainContainer&terrainmodel ) = 0;


	/*
		 - render shadow volume models of objects visible in the given view frustum
	*/
	virtual void	RenderShadowVolumes( CDXDevice&dev, CCamera&camera ) = 0;


	/*
		- moves the given object from this node into the right node
		- object mustn't be in any tree node (m_pNode should be NULL)
		- object will be moved into the lowest node that overlaps object
	*/
	virtual void MoveMovableObject( ISceneTree_MovableObject*pObject );

	// get the tree node bounding box
	inline CBoundBox*	GetBoundBox(){ return &m_bound; }

	inline bool Encloses( CBoundBox&box )
	{
		return	m_bound.m_vecMin.x <= box.m_vecMin.x && m_bound.m_vecMax.x >= box.m_vecMax.x &&
				m_bound.m_vecMin.z <= box.m_vecMin.z && m_bound.m_vecMax.z >= box.m_vecMax.z;
	}

	inline bool Intercests( CBoundBox&box )
	{
		return !( box.m_vecMin.x > m_bound.m_vecMax.x || box.m_vecMin.z > m_bound.m_vecMax.z ||
				  box.m_vecMax.x < m_bound.m_vecMin.x || box.m_vecMax.z < m_bound.m_vecMin.z );
	}

protected:

	int					m_x, m_y, m_cx, m_cy;	// size of the node in the TERRAIN_SIZE units
	CBoundBox			m_bound;	// the tree node bounding box
	
	int					m_scenex, m_sceney, m_scenecx, m_scenecy;	// size of the scene in the TERRAIN_SIZE units

	CSceneBaseTree		*m_pParentNode;	// parent node, NULL for the root
};


//////////////////////////////////////////////////////////////////////////
// inner nodes
// contains pointers to other nodes or leaves
class CSceneTreeNode: public CSceneBaseTree
{
public:
	CSceneTreeNode( CSceneBaseTree*pParentNode, int x, int y, int cx, int cy, int scenex, int sceney, int scenecx, int scenecy );
	virtual ~CSceneTreeNode();

	/*
		- create tree nodes in the view frustum and not created yet
		- delete tree nodes out of boxKeepAlive
		- animate tree nodes in the view frustum (grass, ...)
	*/
	virtual void	FrameMove(
		CDXDevice&dev, double dStep, CCamera&camera,
		CBoundBox&boxKeepAlive,
		CGrassContainer&grassmodels, CStoneWallContainer&stonewallmodels,
		MovableObjectsList**ppTmpArray, unsigned int nTmpArray );


	/*
		- render tree nodes visible in the view frustum
	*/
	virtual void	RenderVisible( CDXDevice&dev, CCamera&camera, CTerrainContainer&terrainmodel );

	/*
		- render shadow volume models of objects visible in the given view frustum
	*/
	virtual void	RenderShadowVolumes( CDXDevice&dev, CCamera&camera );

	/*
		- add the object into this node or child nodes
		- the lowest node that object fits in is selected
	*/
	virtual void AddMovableObject( ISceneTree_MovableObject*pObject );

protected:

	CSceneBaseTree	*m_pChilds[4];

	CSceneBaseTree*	CreateChildNode( int index );
};


//////////////////////////////////////////////////////////////////////////
// leaves
// contains terrain objects
class CSceneTreeLeaf: public CSceneBaseTree
{
public:
	CSceneTreeLeaf( CSceneBaseTree*pParentNode, int x, int y, int cx, int cy, int scenex, int sceney, int scenecx, int scenecy );
	virtual ~CSceneTreeLeaf();

	/*
	- create tree nodes in the view frustum and not created yet
	- animate the world terrain (grass, ...) if visible
	*/
	virtual void	FrameMove(
		CDXDevice&dev, double dStep, CCamera&camera,
		CBoundBox&boxKeepAlive,
		CGrassContainer&grassmodels, CStoneWallContainer&stonewallmodels,
		MovableObjectsList**ppTmpArray, unsigned int nTmpArray );


	/*
	- render if visible in the view frustum
	*/
	virtual void	RenderVisible( CDXDevice&dev, CCamera&camera, CTerrainContainer&terrainmodel );

	/*
		- render shadow volume models of objects visible in the given view frustum
	*/
	virtual void	RenderShadowVolumes( CDXDevice&dev, CCamera&camera );

protected:

	void	GrassFrameMove(
		CDXDevice&dev, double dStep, CCamera&camera,
		CGrassContainer&grassmodels, MovableObjectsList**ppTmpArray, unsigned int nTmpArray );

	void	StoneWallFrameMove( CDXDevice&dev, CStoneWallContainer&stonewallmodels );

	void	UpdateDeviceTranslation9(CDX9Device&dev, D3DXMATRIX&matWorld);
	void	UpdateDeviceTranslation10(CDX10Device&dev, D3DXMATRIX&matWorld);

	float	GetDistanceFromCamera( CCamera&camera );

	CGrassModel		*m_pGrassModel;

	CStoneWallModel	*m_pStoneWallModel;
	bool			m_bStoneWallChecked;
};


#endif//_MYSCENETREE_H_