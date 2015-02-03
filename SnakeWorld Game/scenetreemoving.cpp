#include "scenetreemoving.h"


//////////////////////////////////////////////////////////////////////////
// scene tree node dtor
ISceneTree_Moving::~ISceneTree_Moving()
{
	// remove all objects from the tree node, do not delete them
	for (MovableObjectsList::iterator	it = m_movableobjects.GetIterator(); !it.EndOfList(); ++it )
	{
		ISceneTree_MovableObject*p = it;

		p->m_pNode = 0;	// remove from the tree
	}

	m_movableobjects.Clear(false);	// clear the list, do not delete objects
}


//////////////////////////////////////////////////////////////////////////
// scene tree node
// removes given object from this node
void	ISceneTree_Moving::RemoveMovableObject( ISceneTree_MovableObject*pObject )
{
	// remove the connection between this node and the object
	// object must be in THIS node
	m_movableobjects.Remove( pObject, false );
	pObject->m_pNode = 0;
}


//////////////////////////////////////////////////////////////////////////
// scene tree node
// add given object into this node
void	ISceneTree_Moving::AddMovableObject( ISceneTree_MovableObject*pObject )
{
	// add the object into this node if not yet
	if (pObject && pObject->m_pNode != this)
	{
		pObject->RemoveFromTreeNode();	// first remove object from the old node

		// now add the object into this node
		pObject->m_pNode = this;
		m_movableobjects.Add( pObject );
	}
}


//////////////////////////////////////////////////////////////////////////
// render the objects in this node
void	ISceneTree_Moving::RenderObjects( CDXDevice&dev, CCamera&camera )
{
	for ( MovableObjectsList::iterator it = m_movableobjects.GetIterator();!it.EndOfList();++it )
	{
		ISceneTree_MovableObject*pObject = it;

		// test if the model is visible in the view frustum
		if ( camera.CullBlock( &pObject->GetBoundingBox() ) )
		{
			pObject->RenderObject( dev, camera );	// render the object
		}
		
	}
}


//////////////////////////////////////////////////////////////////////////
// render the objects in this node
void	ISceneTree_Moving::RenderObjectsShadowVolume( CDXDevice&dev, CCamera&camera )
{
	for ( MovableObjectsList::iterator it = m_movableobjects.GetIterator();!it.EndOfList();++it )
	{
		ISceneTree_MovableObject*pObject = it;

		// test if the model is visible in the view frustum
		if ( camera.CullBlock( &pObject->GetBoundingBox() ) )
		{
			pObject->RenderShadowVolume( dev );
		}

	}
}


//////////////////////////////////////////////////////////////////////////
// movable object ctor
ISceneTree_MovableObject::ISceneTree_MovableObject()
{
	m_pNode = 0;	// is not in any node yet
}


//////////////////////////////////////////////////////////////////////////
// movable object dtor
// remove this from the tree node
ISceneTree_MovableObject::~ISceneTree_MovableObject()
{
	RemoveFromTreeNode();	// remove from the tree
}


//////////////////////////////////////////////////////////////////////////
// update the current tree node to fits in
void	ISceneTree_MovableObject::MoveToTreeNode( ISceneTree_Moving*pRootNode )
{
	if (m_pNode)
	{
		m_pNode->MoveMovableObject( this );	// move up from the current node and down into the lowest node that fits
	}
	else if (pRootNode)
	{
		pRootNode->AddMovableObject( this );// move down throw the root
	}
}


//////////////////////////////////////////////////////////////////////////
// remove the object from his tree node
void	ISceneTree_MovableObject::RemoveFromTreeNode()
{
	if (m_pNode)
		m_pNode->RemoveMovableObject( this );	// remove from his tree node
}