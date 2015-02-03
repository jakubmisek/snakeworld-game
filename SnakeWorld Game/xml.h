#ifndef _XML_H_
#define _XML_H_

#include <windows.h>
#include "list.h"
#include "string.h"

//////////////////////////////////////////////////////////////////////////
class XmlAttribute;
class XmlNode;
class XmlDocument;


//////////////////////////////////////////////////////////////////////////
typedef	CList<XmlNode>	XmlNodeList;
typedef	CList<XmlAttribute>	XmlAttributeList;



//////////////////////////////////////////////////////////////////////////
// xml attribute
class XmlAttribute
{
public:

	XmlAttribute( wchar_t*&pSrc );

	CString	Name;
	CString	Value;

protected:

};



//////////////////////////////////////////////////////////////////////////
// xml node
class XmlNode
{
public:

	XmlNode( wchar_t*&pSrc );

	CString		Name;
	CString		InnerText;

	XmlNodeList			ChildNodes;
	XmlAttributeList	Attributes;

	void	SelectNodes( CString&strName, XmlNodeList&ListOut );

protected:

	bool	ParseNode( wchar_t*&pSrc );

};



//////////////////////////////////////////////////////////////////////////
// xml tree
class XmlDocument
{
public:

	XmlDocument();
	virtual ~XmlDocument();

	bool	LoadFromFile( wchar_t *strFile );

	inline	XmlNode*	DocumentElement(){ return m_pRootNode; }

protected:
	
	XmlNode	*m_pRootNode;

};

#endif//_XML_H_