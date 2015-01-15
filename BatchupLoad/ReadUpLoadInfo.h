#ifndef _READ_UPLOAD_INFO_H_
#define _READ_UPLOAD_INFO_H_
void ReadUploadInfo(char *FileName, char **pHead);

void ReadUploadInfo(char *FileName, char **pHead)
{
	CIniConfig iniconfig(FileName);
	iniconfig.GetString("Information","AccessKey",pHead[0],256);
	iniconfig.GetString("Information","SecretKey",pHead[1],256);
	iniconfig.GetString("Information","Space",pHead[2],256);
	iniconfig.GetString("Information","Path",pHead[3],256);
	iniconfig.GetString("Information","Http",pHead[4],256);

}
#endif