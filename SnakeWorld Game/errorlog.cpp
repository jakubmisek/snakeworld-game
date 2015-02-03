#include <windows.h>

#include "errorlog.h"
#include "list.h"
#include "locker.h"

static CList<CString>	g_log;
static ILocker locker;


//////////////////////////////////////////////////////////////////////////
// add log text
void	AddLog( wchar_t*str )
{
	locker.Lock();

	OutputDebugStringW(str);

	g_log.Add( new CString( str ) );

	locker.Unlock();
}

void	AddLog( char*str )
{
	wchar_t buff[512];

	int i = 0;
	while (*str != 0 && i < 511)
	{
		buff[i] = *str;

		++str;
		++i;
	}

	buff[i] = 0;

	AddLog(buff);
}

//////////////////////////////////////////////////////////////////////////
// get log texts
void	GetLog( CString&strOut )
{
	strOut = L"";

	locker.Lock();

	for (CList<CString>::iterator it = g_log.GetIterator(); !it.EndOfList(); ++it)
	{
		strOut += *it;
		strOut += L"\n";
	}

	locker.Unlock();
}