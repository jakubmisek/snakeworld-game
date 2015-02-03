#ifndef _HASHEDLIST_H_
#define _HASHEDLIST_H_


//////////////////////////////////////////////////////////////////////////
//
template<class T>
class CHashedListNode
{
public:
	CHashedListNode(unsigned int key, T *pValue, CHashedListNode*pNext)
	{
		this->key = key;
		this->pValue = pValue;
		this->pNext = pNext;
	}

	unsigned int	key;
	T				*pValue;

	CHashedListNode<T>	*pNext;
};


//////////////////////////////////////////////////////////////////////////
//
template<class T, int table_size>
class CHashedList
{
public:
	CHashedList()
	{
		for (int i = 0; i < table_size; ++i)
			nodes[i] = 0;
	}

	~CHashedList()
	{
		Clear(true);
	}

	// insert new item
	void Add( unsigned int key, T *pValue )
	{
		unsigned int ihash = GetHash(key);

		nodes[ ihash ] = new CHashedListNode<T>( key, pValue, nodes[ihash] );
	}
	

	// remove the item by the key from the list
	bool Remove( unsigned int key, bool bDeleteValue )
	{
		unsigned int ihash = GetHash(key);

		CHashedListNode<T>	*pPrev = 0;
		for ( CHashedListNode<T>*p = nodes[ihash]; p; p = p->pNext )
		{
			if (p->key == key)
			{
				// remove from the list
				if (pPrev)
					pPrev->pNext = p->pNext;
				else
					nodes[ihash] = p->pNext;

				// delete p
				if (bDeleteValue)
					delete p->pValue;
				delete p;

				return true;
			}

			pPrev = p;
		}

		return false;
	}


	// remove all items from the list
	void Clear( bool bDeleteValues )
	{
		for (int i = 0; i < table_size; ++i)
		{
			while (nodes[i])
			{
				CHashedListNode<T>*pNext = nodes[i]->pNext;

				if (bDeleteValues)
					delete nodes[i]->pValue;
				delete nodes[i];

				nodes[i] = pNext;
			}
		}
	}


	// items count
	inline int Count()
	{
		int count = 0;

		for (int i = 0; i < table_size; ++i)
			for ( CHashedListNode<T>*p = nodes[i]; p; p = p->pNext )
				++count;

		return count;
	}

	// indexing by the key
	T* operator [] (int key)
	{
		unsigned int ihash = GetHash(key);

		for ( CHashedListNode<T>*p = nodes[ihash]; p; p = p->pNext )
			if (p->key == key)
				return p->pValue;

		return 0;
	}

	/// Hashed list iterator class
	class CIterator
	{
	public:
		CIterator( CHashedList<T,table_size>&iList )
			: m_iList(iList)
		{
			// find first not empty line, set first node pointer
			m_ihash = 0;
			FindNotEmpty();
		}

		T*		Next( unsigned int *pKeyOut )
		{
			if (pNode)
			{
				// output value
				T*pRet = pNode->pValue;

				// output node key
				if (pKeyOut)
					*pKeyOut = pNode->key;

				// find next
				if ( !(pNode = pNode->pNext) )
				{
					++ m_ihash;	// next line
					FindNotEmpty();// find next not empty
				}

				// return
				return pRet;
			}
			else	// end of list, exception should be thrown
				return 0;
		}

		T*	Next()
		{
			return Next(0);
		}

		bool	EndOfList()
		{
			return (m_ihash >= table_size);
		}

	private:

		CHashedList<T,table_size>	&m_iList;
		CHashedListNode<T>			*pNode;
		unsigned int				m_ihash;
		
		void	FindNotEmpty()
		{
			while ( m_ihash < table_size && !(pNode = m_iList.nodes[m_ihash]) )
				++m_ihash;
		}
	};

	// create iterator
	CIterator GetIterator()
	{
		return CIterator(*this);
	}


private:

	CHashedListNode<T>	*nodes[table_size];

	inline unsigned int GetHash( unsigned int key )
	{
		return (key % table_size);
	}
};

#endif//_HASHEDLIST_H_