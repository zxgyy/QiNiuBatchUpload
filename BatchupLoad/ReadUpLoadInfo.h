#ifndef _READ_UPLOAD_INFO_H_
#define _READ_UPLOAD_INFO_H_
void ReadUploadInfo(char *FileName, char *AccessKey, char *SecretKey, char *Space, char *Path);

void ReadUploadInfo(char *FileName, char *AccessKey, char *SecretKey, char *Space, char *Path)
{
	CIniConfig iniconfig(FileName);
	iniconfig.GetString("Information","AccessKey",AccessKey,256);
	iniconfig.GetString("Information","SecretKey",SecretKey,256);
	iniconfig.GetString("Information","Space",Space,256);
	iniconfig.GetString("Information","Path",Path,256);

}
#endif