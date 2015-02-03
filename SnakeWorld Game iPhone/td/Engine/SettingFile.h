#pragma once
#include <map>
#include <string>

//struct setting_entry_struct
//{
//	int m_val;
//	std::string m_str;
//	int m_




class SettingFile
{
public:
	SettingFile();
	virtual ~SettingFile();

	bool LoadSettings(std::string fname);
	bool SaveSettings(std::string fname, bool create_backup = true);

	void SetInt(int key, int val)
	{
		m_ints[key] = val;
	}
;
	void SetString(int key, std::string val);

	void Clear() { m_ints.clear(); m_strings.clear(); }

	int GetInt(int key, int defaultval);
	std::string GetString(int key, std::string defaultval);

protected:
	std::map<int, int> m_ints;
	std::map<int, std::string> m_strings;
//	std::map<float, std::string> m_floats;
};