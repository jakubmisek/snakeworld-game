#include "stdafx.h"
#include "PlatformSpecific.h"
#include "wininet.h"


void TDBuzz()
{
	// void
}


std::string TDSubmitURL( std::string url )
{
	std::string ret = std::string("FAIL");
	HINTERNET hINet, hFile;
	hINet = InternetOpen("InetURL/1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
	if ( !hINet )
	{
		return ret;
	}
	hFile = InternetOpenUrl( hINet, url.data(), NULL, 0, 0, 0 );
	if ( hFile )
	{
		CHAR buffer[1024];
		DWORD dwRead;
		ret = "";
		while ( InternetReadFile( hFile, buffer, 1023, &dwRead ) )
		{
			if ( dwRead == 0 )
				break;
			buffer[dwRead] = 0;
			ret += buffer;
		}
		InternetCloseHandle( hFile );
	}
	InternetCloseHandle( hINet );
	return ret;
}

int AlertResult = 0;

void TDAlert( std::string title, std::string msg, std::string btn1, int btn1_val, std::string btn2, int btn2_val, std::string btn3, int btn3_val )
{
	AlertResult = 0;
	std::string body = msg;
	body += "\n\n";
	body += btn1;
	body += "\n";
	body += btn2;
	body += "\n";
	body += btn3;
	int res = MessageBox(NULL, body.data(), title.data(), MB_ABORTRETRYIGNORE);
	if (res == IDABORT) AlertResult = btn1_val;
	if (res == IDRETRY) AlertResult = btn2_val;
	if (res == IDIGNORE) AlertResult = btn3_val;
}

int TDAlertResult()
{
	int res = AlertResult;
	AlertResult = 0;
	return res;

}

void TDGotoWebSite( std::string site )
{

}