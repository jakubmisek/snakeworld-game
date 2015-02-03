#ifndef _SCENETREEMOVING_H_
#define _SCENETREEMOVING_H_


#include "list.h"
#include "boundbox.h"

#include "device.h"
#include "camera.h"


//////////////////////////////////////////////////////////////////////////
//
class ISceneTree_Moving;
class ISceneTree_MovableObject;


typedef CList<ISceneTree_MovableObject>	MovableObjectsList;

//////////////////////////////////////////////////////////////////////////
// scene tree node
// functionality for managing the movable objects
class ISceneTree_Moving
{
public:

	virtual ~ISceneTree_Moving();

	/*
		- moves the given object from this node into the right node
		- object mustn't be in any tree node (m_pNode should be NULL)
		- object will be moved into the lowest node that overlaps object
	*/
	virtual void MoveMovableObject( ISceneTree_MovableObject*pObject ) = 0;	// moves the given object into the right node

	/*
		- add the object into this node or child nodes
		- the lowest node that object fits in is selected
	*/
	virtual void AddMovableObject( ISceneTree_MovableObject*pObject );	// add the object into this node or child nodes


	void RemoveMovableObject( ISceneTree_MovableObject*pObject );		// remove the given object from the node


	void RenderObjects( CDXDevice&dev, CCamera&camera );				// render the objects in this node
	void RenderObjectsShadowVolume( CDXDevice&dev, CCamera&camera );	// render the shadow volume models of objects in this node

protected:

	MovableObjectsList	m_movableobjects;						// list of objects in this node
	
};


//////////////////////////////////////////////////////////////////////////
// object in the scene tree
// that should dynamically change his own bounding box
class ISceneTree_MovableObject
{
public:

	ISceneTree_MovableObject();
	virtual ~ISceneTree_MovableObject();

	void	MoveToTreeNode( ISceneTree_Moving*pRootNode );	// moves into right tree node after bound-box change. Root node will be used if the object isn't in any tree node yet.
	void	RemoveFromTreeNode();				// removes the object from his node

	// object bounding box
	virtual CBoundBox&	GetBoundingBox() = 0;

	// render the object
	virtual void RenderObject( CDXDevice&dev, CCamera&camera ) = 0;

	virtual void RenderGrassMove( CDXDevice&dev, D3DXVECTOR3&vecGrassTranslation ) = 0;

	virtual void RenderShadowVolume( CDXDevice&dev ) = 0;

protected:
	ISceneTree_Moving	*m_pNode;				// current tree node containing this object

	friend ISceneTree_Moving;					// scene tree node can modify  m_pNode
};


#endif//_SCENETREEMOVING_H_