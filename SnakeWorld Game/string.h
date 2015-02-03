#ifndef _STRING_H_
#define _STRING_H_

#include "elasticarray.h"

template<class T>
class CStringBase: public CArrayBase<T>
{
public:

	// default ctor
	CStringBase()
	{
		SetString(0);
	}

	// copy ctor from the array of T
	CStringBase(const T*sz)
	{
		SetString(sz);
	}

	// copy ctor
	CStringBase(const CStringBase<T>&str)
	{
		operator = (str);
	}

	// dtor
	virtual ~CStringBase()
	{
		
	}

	// get element by index
	T operator [] (int index)
	{
		if (index < nStr && index >= 0)
			return data[index];
		else
			return 0;	// out of range			
	}

	// insert character on the specified position
	void	Insert( int position, T c )
	{
		// fix insert position
		if ( position < 0 )	position = 0;
		if ( position > length() ) position = length();

		// expand size
		++nStr;
		ExpandSize( nStr, true );

		// insert character
		memmove( data + position + 1, data + position, (nStr - 1 - position)*sizeof(T) );
		data[position] = c;
	}

	// set the string
	CStringBase<T>& operator = ( const CStringBase<T> &str )
	{
		SetString(str.data);

		return *this;
	}

	// set the string
	CStringBase<T>& operator = ( const char*sz )
	{
		SetString(sz);

		return *this;
	}

	// ==
	bool operator == (const CStringBase<T>&str) const
	{
		if (length() == str.length())
		{
			T *p1 = (T*)this->str(), *p2 = (T*)str.str();

			for (int i = 0; i < length(); ++i)
				if (p1[i] != p2[i])
					return false;

			return true;
		}
		else
			return false;
	}

	// ==
	bool operator == (const T*str) const
	{
		T *p1 = (T*)data, *p2 = (T*)str;

		if ( !p1 || !p2 )
			return ( !p1 && !p2 );

		// test string elements
		while (*p1 && *p2)
		{
			if (*p1 != *p2)	return false;
			++p1;
			++p2;
		}

		return (*p1 == 0 && *p2 == 0);	// both strings has same length
	}

	// !=
	bool operator != (const CStringBase<T>&str)	const
	{
		return !( operator== (str) );
	}

	// +=
	CStringBase<T>& operator += (const CStringBase<T>&str)
	{
		if ( str.length() > 0 )
		{
			int newlength = length() + str.length() + 1;
			ExpandSize( newlength, true );

			memcpy( data + length(), str.str(), (str.length() + 1)*sizeof(T) );

			nStr = newlength;
		}

		return *this;
	}

	// +=
	CStringBase<T>& operator += (const T*str)
	{
		int strLength = StrLength(str);

		if ( strLength > 0 )
		{
			int newlength = length() + strLength + 1;
			ExpandSize( newlength, true );

			memcpy( data + length(), str, (strLength + 1)*sizeof(T) );

			nStr = newlength;
		}

		return *this;
	}

	// T*
	operator T* ()
	{
		return data;
	}

	// get array of T
	T* str() const
	{
		return data;
	}

	// string length
	int length() const
	{
		return nStr - 1;
	}

	// StartsWith
	bool	StartsWith( CStringBase<T>&start )
	{
		if ( start.length() > length() )
			return false;

		T*	p1 = start.str();
		T*	p2 = str();

		return Compare( p1,p2, start.length() ) == 0;
	}


	// EndsWith
	bool	EndsWith( CStringBase<T>&end )
	{
		if (end.length() > length())
			return false;

		T*	p1 = end.str();
		T*	p2 = str() + (length() - end.length());

		return Compare( p1,p2, end.length() ) == 0;
	}
	

protected:

	int		nStr;	// string length (including \0)

	// set new string
	// thread unsafe
	void	SetString( const T*sz )
	{
		if (sz)
		{
			nStr = StrLength(sz) + 1;
			ExpandSize( nStr, false );

			memcpy(data, sz, nStr*sizeof(T));
		}
		else
		{
			nStr = 1;
			ExpandSize( nStr, false );

			data[0] = 0;
		}
	}

	// null-terminated string length (not including \0)
	static int StrLength( const T*sz )
	{
		if ( !sz )
			return 0;

		T*p = (T*)sz;

		while (*p)
			++p;

		return (p - sz);
	}

	// compare two arrays of same length
	// 0: same  -1: p1<p2  +1: p1>p2
	static int	Compare( T*p1, T*p2, int length )
	{
		for ( int i = 0; i < length; ++i )
		{
			if ( p1[i] != p2[i] )
			{
				if ( p1[i] < p2[i] )	return -1;
				else return +1;
			}
		}

		return 0;
	}
};

template <class T>
CStringBase<T> operator + (CStringBase<T>&str1, CStringBase<T>&str2)
{
	CStringBase<T> str = str1;
	str += str2;

	return str;
}

template <class T>
CStringBase<T> operator + (T*str1, CStringBase<T>&str2)
{
	CStringBase<T> str = str1;
	str += str2;

	return str;
}


// char string
typedef CStringBase<wchar_t>	CString;

#endif//_STRING_H_
