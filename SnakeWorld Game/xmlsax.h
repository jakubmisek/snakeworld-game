#ifndef _XMLSAX_H_
#define _XMLSAX_H_
//
// TODO
//

#include <windows.h>
#include "string.h"



// xml loader event handler
class XmlSaxHandler
{
	virtual void DocumentBegin(){}
	virtual void DocumentEnd(){}

	virtual void ElementBegin( CString&name ){}
	virtual void ElementEnd(){}

	virtual void Attribute( CString&name, CString&value ){}

	virtual void InnerText( CString&text ){}
};



// xml event-based loader
class XmlSax
{
public:

	XmlDocument();
	virtual ~XmlDocument();

	bool	LoadFromFile( wchar_t *strFile );

protected:

};

#endif//_XMLSAX_H_