#include "stdafx.h"

#include "stdarg.h"
#include <stdio.h>
//#include <io.h>
#include <string>
#include <istream>
#include <fstream>
#include <sstream>
#include "SettingFile.h"
#ifndef _WINDOWS
	#include "LoadTexture.h"
#endif

using namespace std;


SettingFile::SettingFile()
{

}

SettingFile::~SettingFile()
{

}

bool SettingFile::LoadSettings( std::string fname )
{
	fstream fs;
#ifndef _WINDOWS
	const char* fuck = fname.data();
	fname = string(PathForFile(fuck));
#endif
	fs.open(fname.data(), ios_base::in);
	if (fs.fail()) return false;

	string s;
	while( getline(fs, s) )
	{
		stringstream Str;
		char equal;
		Str << s;
		int d, v_int;
		string v_str;
		Str >> d;
		Str >> equal;
		if (equal == '=')
		{
			Str >> v_int;
			SetInt(d, v_int);
		}
		if (equal == '"')
		{
			char buf[257];
			Str.getline(buf, 256);
			v_str = string(buf);
			SetString(d, v_str);
		}
	}
	fs.close();
	return true;
}

//string FormatString(const char* szFormat, ...)
//{
//	char szBuff[1024];
//	va_list arg;
//	va_start(arg, szFormat);
//	_vsnprintf(szBuff, sizeof(szBuff), szFormat, arg);
//	va_end(arg);
//	return string(szBuff);
//}

bool SettingFile::SaveSettings( std::string fname, bool create_backup /*= true*/ )
{
	char buf[257];
	std::string backname = fname;
	backname += ".bak";
#ifndef _WINDOWS
	fname = string(PathForFile(fname.data()));
#endif
	FILE* f = fopen(fname.data(), "w");
	if (f == 0) return false;

	std::map<int, int>::iterator i1;
	std::map<int, std::string>::iterator i2;
	std::string fmt;
	char szBuff[1024];
	for (i1 = m_ints.begin(); i1 != m_ints.end(); i1++)
	{
		memset(szBuff, 0, 1024);
		int iKey = (*i1).first;
		int iVal = (*i1).second;
//		string s = FormatString("%d=%d\n", iKey, iVal);
#ifdef _WINDOWS
		sprintf_s(buf, 256, "%d=%d\n", iKey, iVal);
#else
		snprintf(buf, 256, "%d=%d\n", iKey, iVal);
#endif
		string s(buf);
		fwrite(s.data(), sizeof(char), s.size(), f);
	}
	for (i2 = m_strings.begin(); i2 != m_strings.end(); i2++)
	{
		memset(szBuff, 0, 1024);
		int iKey = (*i2).first;
		string iVal = (string)((*i2).second);
//		string s = FormatString("%d\"%s\n", iKey, iVal.data());
#ifdef _WINDOWS
		sprintf_s(buf, 256, "%d\"%s\n", iKey, iVal.data());
#else
		snprintf(buf, 256, "%d\"%s\n", iKey, iVal.data());
#endif
		string s(buf);
		fwrite(s.data(), sizeof(char), s.size(), f);
	}
	fclose(f);
	
	return true;
}


void SettingFile::SetString( int key, std::string val )
{
	m_strings[key] = val;
}

int SettingFile::GetInt( int key, int defaultval )
{
	map<int, int>::iterator i = m_ints.find(key);
	if (i == m_ints.end()) return defaultval;
	return m_ints[key];
}

std::string SettingFile::GetString( int key, std::string defaultval )
{
	map<int, string>::iterator i = m_strings.find(key);
	if (i == m_strings.end()) return defaultval;
	return m_strings[key];
}