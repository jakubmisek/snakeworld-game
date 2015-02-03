#ifndef _BINARYIO_H_
#define _BINARYIO_H_

#include "string.h"

//////////////////////////////////////////////////////////////////////////
// binary reader
class CBinaryReader
{
public:
	virtual unsigned char	ReadByte(int*ierr=0) = 0;		// read 1 byte
	virtual unsigned int	ReadUInt32(int*ierr=0) = 0;	// read 4 bytes
	virtual double			ReadDouble(int*ierr=0) = 0;	// read 8 bytes
	virtual float			ReadFloat(int*ierr=0) = 0;	// read 4 bytes
	virtual signed int		ReadInt(int*ierr=0) = 0;		// read 4 bytes
	virtual void			Read( char*buff, int len, int*ierr=0 ) = 0;	// read "len" bytes into the input buffer "buff"
	virtual void			ReadString( CString&str, int*ierr=0 ) = 0;	// read zero terminated string
};


//////////////////////////////////////////////////////////////////////////
// binary writer
class CBinaryWriter
{
public:
	virtual int Write( char*buff, int len ) = 0;	// writes the input buffer "buff" of size "len"
	virtual int Write( unsigned char val ) = 0;// write 1 byte
	virtual int Write( unsigned int val ) = 0;	// write 4 bytes
	virtual int Write( double val ) = 0;		// write 8 bytes
	virtual int Write( float val ) = 0;		// write 4 bytes
	virtual int Write( signed int val ) = 0;	// write 4 bytes
	virtual int Write( wchar_t*str ) = 0;	// write zero terminated string

	virtual void Flush() = 0;		// flushes the write buffer
};


#endif//_BINARYIO_H_
