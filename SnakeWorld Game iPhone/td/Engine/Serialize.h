#pragma once
#include <string>

class TargetValue;

class MemFile
{
public:
	MemFile();
	void StartSave(const char* fname);
	bool StartLoad(const char* fname);	// true if opened
	virtual ~MemFile();
	void WriteInt(int val);
	void WriteUInt(uint val);
	void WriteFloat(float val);
	void WriteString(std::string s);
	int ReadInt();
	uint ReadUInt();
	float ReadFloat();
	void ReadInt(int &val);
	void ReadUInt(uint &val);
	void ReadFloat(float &val);
	void ReadString(std::string &str);

	bool Saving;
protected:
	fpos_t m_filesize;
	byte* mem;
	FILE * m_file_handle;
};

class Serialize
{
protected:
	Serialize() { m_mf = NULL; }
	virtual ~Serialize() { }
//	virtual void WriteState() { m_mf = new MemFile(); }
//	virtual void ReadState() { m_mf = new MemFile(); }
public:
	virtual bool SyncState(MemFile* mf) {m_mf = mf; return TRUE;}
protected:
	MemFile* m_mf;

	void SyncInt(int &val);
	void SyncBool(bool &val);
	void SyncUInt(uint &val);
	void SyncString(std::string &val);
	void SyncFloat(float &val);

	void SyncTarget(TargetValue &val);

	bool SyncHeader(uint h);	// returns TRUE if header is correct

	//void WriteInt(int val) { m_mf->WriteInt(val); }
	//void WriteUInt(uint val) { m_mf->WriteUInt(val); }
	//void WriteFloat(float val) { m_mf->WriteFloat(val); }
	//void WriteString(std::string s) { m_mf->WriteString(s); }
	//int ReadInt() { return m_mf->ReadInt(); }
	//uint ReadUInt() { return m_mf->ReadUInt(); }
	//float ReadFloat() { return m_mf->ReadFloat(); }
	//void ReadInt(int &val) { m_mf->ReadInt(val); }
	//void ReadUInt(uint  &val) { m_mf->ReadUInt(val); }
	//void ReadFloat(float &val) { m_mf->ReadFloat(val); }
	//void ReadString(std::string &str){ m_mf->ReadString(str); }
};
