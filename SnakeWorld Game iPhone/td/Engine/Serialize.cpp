#include "stdafx.h"
#include "Serialize.h"
#include "TargetValue.h"

MemFile::MemFile()
{
}
void MemFile::StartSave( const char* fname )
{
	Saving = true;
	m_file_handle = fopen(fname, "w+b");
}
bool MemFile::StartLoad( const char* fname )
{
	Saving = false;
	m_file_handle = fopen(fname, "r+b");
	fseek(m_file_handle, 0, SEEK_END);
	fgetpos(m_file_handle, &m_filesize);
	if (m_filesize == 0) return false;
	fclose(m_file_handle);
	m_file_handle = fopen(fname, "r+b");
	if (m_file_handle == 0)
		return false;
	return true;
}
MemFile::~MemFile()
{
	fclose(m_file_handle);
}

void MemFile::WriteInt( int val )
{
	fwrite(&val, sizeof(int), 1, m_file_handle);
}
void MemFile::WriteUInt( uint val )
{
	fwrite(&val, sizeof(uint), 1, m_file_handle);
}
int MemFile::ReadInt()
{
	fpos_t pos;
	fgetpos(m_file_handle, &pos);
	if (pos + sizeof(int) >= m_filesize) throw (0);
	int buf;
	fread(&buf, sizeof(int), 1, m_file_handle);
	return buf;
}
uint MemFile::ReadUInt()
{
	fpos_t pos;
	fgetpos(m_file_handle, &pos);
	if (pos + sizeof(uint) >= m_filesize) throw (0);
	int buf;
	fread(&buf, sizeof(uint), 1, m_file_handle);
	return buf;
}

void MemFile::ReadInt( int &val )
{
	fpos_t pos;
	fgetpos(m_file_handle, &pos);
	if (pos + sizeof(int) >= m_filesize) throw (0);
	fread(&val, sizeof(int), 1, m_file_handle);
}
void MemFile::ReadUInt( uint &val )
{
	fpos_t pos;
	fgetpos(m_file_handle, &pos);
	if (pos + sizeof(uint) >= m_filesize) throw (0);
	fread(&val, sizeof(uint), 1, m_file_handle);
}
void MemFile::WriteFloat( float val )
{
	fwrite(&val, sizeof(float), 1, m_file_handle);
}
float MemFile::ReadFloat( )
{
	fpos_t pos;
	fgetpos(m_file_handle, &pos);
	if (pos + sizeof(float) >= m_filesize) throw (0);
	float buf;
	fread(&buf, sizeof(float), 1, m_file_handle);
	return buf;
}

void MemFile::ReadFloat( float &val )
{
	fpos_t pos;
	fgetpos(m_file_handle, &pos);
	if (pos + sizeof(float) >= m_filesize) throw (0);
	fread(&val, sizeof(float), 1, m_file_handle);
}
void MemFile::WriteString( std::string s )
{
	size_t siz = s.size();
	fwrite(&siz, sizeof(size_t), 1, m_file_handle);
	fwrite(s.data(), sizeof(byte), siz, m_file_handle);
}

void MemFile::ReadString(std::string &str)
{
	fpos_t pos;
	fgetpos(m_file_handle, &pos);
	if (pos + sizeof(size_t) >= m_filesize) throw (0);
	size_t siz = 0;
	fread(&siz, sizeof(size_t), 1, m_file_handle);
	if (pos + siz >= m_filesize) throw (0);
	byte* buf = new byte[siz];
	fread(buf, sizeof(byte), 1, m_file_handle);
	str.append((const char*)buf);
	delete buf;
}


void Serialize::SyncInt( int &val )
{
	if (m_mf->Saving)
		m_mf->WriteInt(val);
	else
		m_mf->ReadInt(val);
}

void Serialize::SyncUInt( uint &val )
{
	if (m_mf->Saving)
		m_mf->WriteUInt(val);
	else
		m_mf->ReadUInt(val);
}

void Serialize::SyncString( std::string &val )
{
	if (m_mf->Saving)
		m_mf->WriteString(val);
	else
		m_mf->ReadString(val);

}

void Serialize::SyncFloat( float &val )
{
	if (m_mf->Saving)
		m_mf->WriteFloat(val);
	else
		m_mf->ReadFloat(val);

}

bool Serialize::SyncHeader( uint h )
{
	if (m_mf->Saving)
		m_mf->WriteUInt(h);
	else
	{
		uint a = m_mf->ReadUInt();
		return a == h;
	}
	return true;
}

void Serialize::SyncTarget( TargetValue &val )
{
	val.SyncState(m_mf);
}

void Serialize::SyncBool( bool &val )
{
	int b = val;
	SyncInt(b);
	val = (b != 0);
}