#include <atlconv.h>
#include <stdio.h>
#ifndef _INICONFIG_H
#define _INICONFIG_H

class CIniConfig
{
public:

	// struct
	CIniConfig(char *filename)
	{
		sprintf_s(file_name,"%s",filename);
	}

	// getter
	unsigned GetNumber(const char* section_name, const char* key_name)
	{
		return GetPrivateProfileIntA(section_name, key_name, 0, file_name);
	}
	double GetDData(const char* section_name, const char* key_name)
	{
		return atof(this->GetString(section_name, key_name));
	}

	char* GetString(const char* section_name, const char* key_name)
	{
		char* out_buf;
		GetPrivateProfileStringA(section_name, key_name, "", out_buf, 100, file_name);
		return out_buf;
	}
	void GetString(const char* section_name, const char* key_name, char* out_buf, int length)
	{
		GetPrivateProfileStringA(section_name, key_name, "", out_buf, length, file_name);
	}

	// setter
	void SetNumber(const char* section_name, const char* key_name, const unsigned val)
	{
		char buf[25] = { 0 };
		sprintf_s(buf, 25, "%d", val);
		WritePrivateProfileStringA(section_name, key_name, buf, file_name);
	}

	void SetString(const char* section_name, const char* key_name, const char* in_buf)
	{
		WritePrivateProfileStringA(section_name, key_name, in_buf, file_name);
	}
	void SetDData(const char* section_name, const char* key_name, double in_buf)
	{
		char s[25]={0};
		sprintf_s(s, 50, "%lf", in_buf);
		SetString(section_name, key_name, s);
	}
private:
	char file_name[512];
};
#endif // _INICONFIG_H
