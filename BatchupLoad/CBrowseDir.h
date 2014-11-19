#ifndef _CBROWSEDIR_H_
#define _CBROWSEDIR_H_
#include "stdlib.h"
#include "direct.h"
#include "string.h"
#include "string"
#include "io.h"
#include "stdio.h" 
#include <vector>
#include "iostream"
#include "qiniu/include/openssl/sha.h"
#include "qiniu/include/openssl/bio.h"
#include "qiniu/include/openssl/hmac.h"
#include "qiniu/b64/urlsafe_b64.h"
#define FILE_BLOCK_SIZE 8192
#define BUFF_SIZE 512
struct FileInfo 
{
	char FileGlobalInfo[_MAX_PATH];
	char FileLocalInfo[_MAX_PATH];
	char Hash[SHA224_DIGEST_LENGTH];
	unsigned long FileSize;
	unsigned long UploadTime;
	FileInfo()
	{
		memset(FileGlobalInfo,0, _MAX_PATH);
		memset(FileLocalInfo,0, _MAX_PATH);
		memset(Hash,0, SHA224_DIGEST_LENGTH);
	}
};

class CBrowseDir
{
protected:
	char m_szInitDir[_MAX_PATH];

public:
	CBrowseDir();
	bool SetInitDir(const char *dir);


	std::vector<std::string> BeginBrowseFilenames(const char *filespec);
	std::vector<FileInfo> AssemblyFileInfo(std::vector<std::string> filepath);
	void SHA1File(const std::string & filepath,char sha1_digest[SHA224_DIGEST_LENGTH]);

protected:
	std::vector<std::string> GetDirFilenames(const char *dir,const char *filespec);
	virtual bool ProcessFile(const char *filename);
	virtual void ProcessDir(const char *currentdir,const char *parentdir);
};


class CStatDir:public CBrowseDir
{
protected:
	int m_nFileCount;
	int m_nSubdirCount;

public:

	CStatDir()
	{
		m_nFileCount=m_nSubdirCount=0;
	}

	int GetFileCount()
	{
		return m_nFileCount;
	}

	int GetSubdirCount()
	{
		//因为进入初始目录时，也会调用函数ProcessDir，
		//所以减1后才是真正的子目录个数。
		return m_nSubdirCount-1;
	}

protected:
	//覆写虚函数ProcessFile，每调用一次，文件个数加1
	virtual bool ProcessFile(const char *filename)
	{
		m_nFileCount++;
		return CBrowseDir::ProcessFile(filename);
	}

	//覆写虚函数ProcessDir，每调用一次，子目录个数加1
	virtual void ProcessDir(const char *currentdir,const char *parentdir)
	{
		m_nSubdirCount++;
		CBrowseDir::ProcessDir(currentdir,parentdir);
	}
};


#endif