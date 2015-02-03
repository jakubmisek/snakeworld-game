#ifndef _ELASTICARRAY_H_
#define _ELASTICARRAY_H_

#include <memory>

template<class T>
class CArrayBase
{
public:

	CArrayBase()
	{
		data = 0;
		datasize = 0;
	}


	virtual	~CArrayBase()
	{
		if ( data )
		{
			delete[] data;
			data = 0;
		}

		datasize = 0;
	}

protected:

	// set new array size if newsize is larger
	void	ExpandSize( int newsize, bool bSavePreviousData = true )
	{
		// align newsize up to 0xf
		newsize |= 0xf;

		// check array size
		if ( newsize > datasize )
		{
			T*	newdata = new T[newsize];

			if ( data )
			{
				if ( bSavePreviousData )
					memcpy( newdata, data, datasize*sizeof(T) );

				delete[] data;
			}

			data = newdata;
			datasize = newsize;
		}
	}

	T*		data;		// allocated array
	int		datasize;	// allocated array size
};


#endif//_ELASTICARRAY_H_
