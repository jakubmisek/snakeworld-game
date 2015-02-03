#include "xml.h"

//////////////////////////////////////////////////////////////////////////
// find end of name
wchar_t*	EndOfName( wchar_t*p )
{
	bool bq = false;

	if ( p )
	{
		while ( *p && *p != L' ' && *p != L'\t' && *p != L'\n' && *p != L'>' && *p != L'=' )
			++p;
	}

	return p;
}

//////////////////////////////////////////////////////////////////////////
// xml ducument ctor
// load xml document from file
XmlDocument::XmlDocument()
{
	m_pRootNode = 0;
}


//////////////////////////////////////////////////////////////////////////
// xml ducument dtor
XmlDocument::~XmlDocument()
{
	if (m_pRootNode)
		delete m_pRootNode;
	m_pRootNode = 0;
}



//////////////////////////////////////////////////////////////////////////
// load xml document from file
bool	XmlDocument::LoadFromFile(wchar_t *strFile)
{
	if (m_pRootNode)
		delete m_pRootNode;
	m_pRootNode = 0;

	//
	// read the file
	//
	HANDLE hFile;

	// create the file
	hFile = CreateFile( strFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if (hFile == INVALID_HANDLE_VALUE) 
	{
		return false;
	}

	DWORD uSize = GetFileSize( hFile, NULL );

	char*	pBuff = new char[uSize];
	
	// write data
	DWORD dataread;
	if ( !ReadFile( hFile, pBuff, uSize, &dataread, NULL ) )
		return false;

	// close the file
	CloseHandle(hFile);

	if (dataread != uSize)
		return false;

	//
	// convert to UNICODE
	//
	int nBuffW = MultiByteToWideChar( CP_UTF8, 0, pBuff, uSize, NULL, 0 );
	wchar_t	*pBuffW = new wchar_t[nBuffW + 1];
	MultiByteToWideChar( CP_UTF8, 0, pBuff, uSize, pBuffW, nBuffW );
	pBuffW[nBuffW] = 0;	// terminate the string

	delete pBuff;

	//
	// parse elements
	//

	// find document root
	for (wchar_t *p = pBuffW;p; ++p)
		if ( p[0] == L'<' && p[1] != '?' )
		{
			// root element found
			m_pRootNode = new XmlNode( p );
			break;
		}
	
	delete pBuffW;

	return true;
}


//////////////////////////////////////////////////////////////////////////
// xml node constructor
// from source xml buffer
XmlNode::XmlNode(wchar_t *&pSrc)
{
	ParseNode( pSrc );
}


//////////////////////////////////////////////////////////////////////////
// parse xml node and child nodes and attributes
bool	XmlNode::ParseNode(wchar_t *&pSrc)
{
	if ( !pSrc )
		return false;

	if ( *pSrc != L'<' )
		return false;

	++pSrc;

	wchar_t	*pNameBegin = pSrc;
	wchar_t	*pNameEnd;

	// find end of the name
	pNameEnd = pSrc = EndOfName( pSrc );
	
	// parse attributes (until '>')
	bool bClosed = false;
	while (*pSrc && *pSrc != L'>')
	{
		if ( pSrc[0] == L'/' && pSrc[1] == L'>' )
			bClosed = true;

		if (*pSrc != L' ' && *pSrc != L'\t' && *pSrc != L'\n')
		{
			XmlAttribute *att = new XmlAttribute( pSrc );
			Attributes.Add( att, att->Name );
		}
		else
			++pSrc;
	}

	++pSrc;


	// copy name
	*pNameEnd = 0;
	Name = pNameBegin;

	// child nodes
	if ( !bClosed )
	{
		wchar_t*pInnerTextBegin = pSrc;
		wchar_t*pInnerTextEnd = pSrc;
		bool bNodes = false;

		// find <
		while ( *pSrc )
		{
			if ( pSrc[0] == L'<' )
			{
				if ( pSrc[1] == L'/' )	// ending node
				{
					pInnerTextEnd = pSrc;

					// find >
					while (*pSrc && *pSrc != L'>')
					{
						++pSrc;
					}
					++pSrc;
					break;
				}
				else
				{
					XmlNode *node = new XmlNode( pSrc );
					ChildNodes.Add( node, node->Name );

					bNodes = true;
				}
			}
			else
				++pSrc;
		}

		if ( !bNodes )
		{
			*pInnerTextEnd = 0;
			InnerText = pInnerTextBegin;
		}
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////
// select nodes with specified name
void	XmlNode::SelectNodes( CString&strName, XmlNodeList&ListOut )
{
	for (XmlNodeList::iterator it = ChildNodes.GetIterator(); !it.EndOfList(); ++it)
	{
		if ( (*it).Name == strName )
			ListOut.Add( it );
	}
}


//////////////////////////////////////////////////////////////////////////
// attribute ctor
XmlAttribute::XmlAttribute(wchar_t *&pSrc)
{
	wchar_t*pNameBegin = pSrc, *pNameEnd = EndOfName( pSrc ); 
	pSrc = pNameEnd;

	// find =
	while (*pSrc && *pSrc != L'=')
		++pSrc;

	pSrc ++;

	// find "
	while (*pSrc && *pSrc != L'\"' && *pSrc != L'\'')
		++pSrc;

	++pSrc;
	wchar_t	*pValBegin = pSrc;
	wchar_t	*pValEnd;

	// find ending "
	bool bl = false;
	while ( *pSrc && (bl || (*pSrc != L'\"' && *pSrc != L'\'')) )
	{
		if ( *pSrc == L'\\' )
			bl = true;
		else
			bl = false;

		++pSrc;
	}

	pValEnd = pSrc;
	++pSrc;

	*pValEnd = 0;
	Value = pValBegin;

	*pNameEnd = 0;
	Name = pNameBegin;

}