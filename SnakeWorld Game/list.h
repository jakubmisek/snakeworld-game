#ifndef _LIST_H_
#define _LIST_H_

#include "string.h"

//////////////////////////////////////////////////////////////////////////
// generic list element
template<class T>
class CListNode
{
public:
	CListNode(T *pValue);
	CListNode(T *pValue, CString&Name);

	T				*pValue;
	CListNode<T>	*pNext;
	CString			Name;
};


//////////////////////////////////////////////////////////////////////////
// Generic list object
template<class T>
class CList
{
public:

	CList();
	~CList();

	bool Add( T *pValue );	// insert new item on the list end
	bool Insert( T*pValue, int index );	// insert new item on the specified position into the list

	bool Add( T *pValue,CString&Name );	// insert new named item on the list end
	bool Insert( T*pValue, CString&Name, int index );	// insert new named item on the specified position into the list
	bool InsertAfter( T*pValue, T*pItemBefore );	// insert new named item after specified item

	bool Remove( T *pValue, bool bDeleteValue );	// remove the item from the list
	bool Remove( int index, bool bDeleteValue );	// remove the item[index] from the list
	void Clear( bool bDeleteValues );	// remove all items from the list

	bool Exist(T *pValue);	// returns true if the specified value exists in the list

	T*	operator [] (int index);	// returns the item by index, or null if index >= count or index < 0
	T*	operator [] (wchar_t*name);	// returns the item by name, or null if that item does not exist
	T*	Last();						// returns the last item pointer or null if the list is empty

	T**	ToArray();	// create array of elements (T*)

	inline int Count()	// returns nodes count
	{
		return nNodes;
	}
	
	/// List iterator class
	class iterator
	{
	public:
		iterator( CList<T>&iList );

		T*	operator ++ ();
		operator	T* ()
		{
			return value();
		}
		T*	operator -> ()
		{
			return value();
		}
		T*	value()
		{
			if ( pCurrentNode )
				return pCurrentNode->pValue;
			else
				return 0;
		}

		bool	EndOfList()
		{
			return (pCurrentNode == 0);
		}
	private:
		CListNode<T>	*pCurrentNode;
	};

	// create iterator
	iterator GetIterator()
	{
		return iterator(*this);
	}

private:

	bool	Add( CListNode<T>*p );
	bool	Insert( CListNode<T>*p, int index );

	CListNode<T>	*pNodes, *pLastNode;
	int				nNodes;

	void	CutItem( CListNode<T>* pPrevNode, CListNode<T>* p, bool bDeleteValue );
};


//////////////////////////////////////////////////////////////////////////
// functions should be in the header file with the template classes declaration
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CListNode ctor
template<class T>
CListNode<T>::CListNode(T *pValue)
{
	this->pValue = pValue;
	this->pNext = 0;
}

//////////////////////////////////////////////////////////////////////////
// CListNode ctor
template<class T>
CListNode<T>::CListNode(T *pValue, CString&Name)
{
	this->pValue = pValue;
	this->pNext = 0;
	this->Name = Name;
}

//////////////////////////////////////////////////////////////////////////
// CList ctor
template<class T>
CList<T>::CList()
{
	pNodes = pLastNode = 0;
	nNodes = 0;
}


//////////////////////////////////////////////////////////////////////////
// CList dtor
// !! all values in the list will be deleted !!
template<class T>
CList<T>::~CList()
{
	Clear(true);
}


//////////////////////////////////////////////////////////////////////////
// remove all elements from CList
template <class T>
void	CList<T>::Clear( bool bDeleteValues )
{
	CListNode<T> *pNext;

	// remove nodes
	while (pNodes)
	{
		// save the next pointer
		pNext = pNodes->pNext;

		// delete the value
		if (bDeleteValues)
			delete pNodes->pValue;

		// delete the  list node
		delete pNodes;

		// update the pointer to the first node
		pNodes = pNext;
	}

	//
	pLastNode = 0;

	nNodes = 0;
}


//////////////////////////////////////////////////////////////////////////
// add new element to the CList end

template<class T>
bool	CList<T>::Add(T *pValue)
{
	return Add( new CListNode<T>( pValue ) );
}

template<class T>
bool	CList<T>::Add(T *pValue, CString&Name)
{
	return Add( new CListNode<T>( pValue, Name ) );
}

template <class T>
bool	CList<T>::Add( CListNode<T> *p )
{
	if (!p)	return false;

	// insert on the list end
	if (pLastNode)
		pLastNode->pNext = p;
	else
		pNodes = p;

	// increment nodes count
	++nNodes;

	// update pLastNode
	pLastNode = p;

	return true;
}


//////////////////////////////////////////////////////////////////////////
// insert new element on the specified position into the list

template<class T>
bool	CList<T>::Insert(T *pValue, int index)
{
	return Insert( new CListNode<T>( pValue ), index );
}

template<class T>
bool	CList<T>::Insert(T *pValue, CString&Name, int index)
{
	return Insert( new CListNode<T>( pValue, Name ), index );
}

template<class T>
bool	CList<T>::InsertAfter( T*pValue, T*pItemBefore )
{
	if ( !pValue || !pItemBefore)
		return false;

	for (CListNode<T>*p = pNodes;p;p = p->pNext)
	{
		if (p->pValue == pItemBefore)
		{
			CListNode<T> *newNode = new CListNode<T>( pValue );

			newNode->pNext = p->pNext;
			p->pNext = newNode;

			if (!newNode->pNext)
				pLastNode = newNode;

			++ nNodes;
			return true;
		}
	}

	return false;
}

template<class T>
bool	CList<T>::Insert(CListNode<T> *p, int index)
{
	if (!p)	return false;

	// align the index to the possible range
	if (index < 0)	index = 0;
	else if (index > nNodes) index = nNodes;

	// check for special most used cases
	if (index == 0)
	{
		p->pNext = pNodes;
		pNodes = p;

		if (!p->pNext)	pLastNode = pNodes;
	}
	else if (index == nNodes)
	{
		// insert on the list end (there is at least 1 node in the list)
		pLastNode->pNext = p;
		pLastNode = p;
	}
	else
	{
		int i = 1;
		CListNode<T>*pPrevNode = pNodes;
		for (	CListNode<T>*p = pNodes->pNext;	// skip the first node (index 0)
				p;
				p = p->pNext, ++i)
		{
			if (i == index)
			{
				//
				// skip cases for insertion before the list and on the list end
				// pNodes and pLastNode cannot be modified
				//

				p->pNext = pPrevNode->pNext;
				pPrevNode->pNext = p;				

				break;
			}

			// update previous node pointer
			pPrevNode = p;
		}

#ifdef DEBUG
		if (i != index)	// index was not found !!! ???
			DebugBreak();
#endif
	}

	// increment nodes count
	++nNodes;

	// ok
	return true;
}


//////////////////////////////////////////////////////////////////////////
// test if exists node with specified value
template<class T>
bool	CList<T>::Exist(T *pValue)
{
	for (CListNode<T>*p = pNodes; p; p = p->pNext)
		if (p->pValue == pValue)
			return true;

	return false;
}


//////////////////////////////////////////////////////////////////////////
// remove node specified by the value
template<class T>
bool	CList<T>::Remove(T *pValue, bool bDeleteValue )
{
	CListNode<T>*pPrevNode = 0;

	for (CListNode<T>*p = pNodes; p; p = p->pNext)
	{
		if (p->pValue == pValue)
		{
			CutItem(pPrevNode, p, bDeleteValue);

			// done
			return true;
		}

		pPrevNode = p;
	}

	return false;
}


//////////////////////////////////////////////////////////////////////////
// remove the item by the specified index
template<class T>
bool	CList<T>::Remove( int index, bool bDeleteValue )
{
	if ( index < 0 || index >= nNodes )
		return false;

	int i = 0;
	CListNode<T>*pPrevNode = 0;
	for (CListNode<T>*p = pNodes; p; p = p->pNext, ++i)
	{
		if (i == index)
		{
			CutItem(pPrevNode, p, bDeleteValue);

			// done
			return true;
		}

		// update previous node pointer
		pPrevNode = p;
	}

	// this code cannot be reached
	return false;
}


//////////////////////////////////////////////////////////////////////////
// cut off the specified item
template<class T>
void	CList<T>::CutItem( CListNode<T>* pPrevNode, CListNode<T>* p, bool bDeleteValue )
{
	// reorganize the the list
	if (pPrevNode)
	{
		pPrevNode->pNext = p->pNext;
	}
	else
	{
		pNodes = p->pNext;
	}

	// update pLastNode pointer
	if (p == pLastNode)
	{
		pLastNode = pPrevNode;
	}

	// update count
	-- nNodes;

	// delete p
	if (bDeleteValue)
		delete p->pValue;

	delete p;
}


//////////////////////////////////////////////////////////////////////////
// get the item by index
template<class T>
T*		CList<T>::operator [](int index)
{
	if (index < 0 || index >= nNodes)
		return 0;	// out of range

	// find the item

	if (index == 0)
	{	// first node
		return pNodes->pValue;
	}
	else if (index == nNodes - 1)
	{	// last node
		return pLastNode->pValue;
	}
	else
	{	// another node
		int i = 0;

		for (	CListNode<T>*p = pNodes;
				p;
				p = p->pNext, ++ i)
			{
				if ( i == index )
					return p->pValue;
			}
	}

	// unreachable
	return 0;
}


//////////////////////////////////////////////////////////////////////////
// get the item by name
template<class T>
T*		CList<T>::operator [](wchar_t*name)
{
	CString strName = name;
	// another node
	for (	CListNode<T>*p = pNodes;
			p;
			p = p->pNext)
		{
			if ( p->Name == strName )
				return p->pValue;
		}

	// not found
	return 0;
}


//////////////////////////////////////////////////////////////////////////
// returns the last item in the list
template<class T>
T*		CList<T>::Last()
{
	return pLastNode ? pLastNode->pValue : 0;
}


//////////////////////////////////////////////////////////////////////////
// create array  T** [Count] and fill it with elements of the list
template<class T>
T**		CList<T>::ToArray()
{
	if (!nNodes)	return 0;

	T** newArray = new T*[ nNodes ];

	int i = 0;

	for ( CListNode<T>*p = pNodes; p; p = p->pNext, ++i )
	{
		newArray[i] = p->pValue;
	}

	return newArray;
}



//////////////////////////////////////////////////////////////////////////
// LIST ITERATOR
//////////////////////////////////////////////////////////////////////////
// List Iterator ctor
template<class T>
CList<T>::iterator::iterator(CList<T> &iList)
{
	pCurrentNode = iList.pNodes;
}


//////////////////////////////////////////////////////////////////////////
// Get item on iterator position and move position
template<class T>
T* CList<T>::iterator::operator ++()
{
	if (pCurrentNode)
	{
		pCurrentNode = pCurrentNode->pNext;
	}
	
	if (pCurrentNode)
	{
		return pCurrentNode->pValue;
	}
	else
		return 0;
}


#endif//_LIST_H_
