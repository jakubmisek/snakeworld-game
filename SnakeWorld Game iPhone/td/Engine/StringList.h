#pragma once
#include <string>

class stringlist : public std::vector<std::string*>
{
protected:
public:
	stringlist()
	{
		clear();
	}
	virtual ~stringlist()
	{
		EraseAll();
	}
	void AddString(std::string str)
	{
		push_back(new std::string(str));
	}
	void AddString(const char* str)
	{
		push_back(new std::string(str));
	}
	void EraseAll()
	{
		if (empty()) return;
		for (uint i = 0; i < size(); i++)
		{
			std::string* str = this->at(i);
			delete str;
		}
		clear();
	}
};
