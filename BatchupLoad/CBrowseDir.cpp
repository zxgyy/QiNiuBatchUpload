#include "CBrowseDir.h"

CBrowseDir::CBrowseDir()
{
	getcwd(m_szInitDir,_MAX_PATH);

	int len=strlen(m_szInitDir);
	if (m_szInitDir[len-1] != '\\')
		strcat(m_szInitDir,"\\");
}

bool CBrowseDir::SetInitDir(const char *dir)
{
	//获取绝对路径
	if (_fullpath(m_szInitDir,dir,_MAX_PATH) == NULL)
		return false;

	if (_chdir(m_szInitDir) != 0)
		return false;

	int len=strlen(m_szInitDir);
	if (m_szInitDir[len-1] != '\\')
		strcat(m_szInitDir,"\\");

	return true;
}

std::vector<std::string> CBrowseDir::BeginBrowseFilenames(const char *filespec)
{
	ProcessDir(m_szInitDir,NULL);
	return GetDirFilenames(m_szInitDir,filespec);
}

std::vector<std::string> CBrowseDir::GetDirFilenames(const char *dir,const char *filespec)
{
	_chdir(dir);
	std::vector<std::string>filename_vector;
	filename_vector.clear();

	//首先查找dir中符合要求的文件
	long hFile;
	_finddata_t fileinfo;
	if ((hFile=_findfirst(filespec,&fileinfo)) != -1)
	{
		do
		{
			//检查是不是目录
			//如果不是,则进行处理
			if (!(fileinfo.attrib & _A_SUBDIR))
			{
				char filename[_MAX_PATH];
				strcpy(filename,dir);
				strcat(filename,fileinfo.name);
				filename_vector.push_back(filename);
			}
		} while (_findnext(hFile,&fileinfo) == 0);
		_findclose(hFile);
	}
	//查找dir中的子目录
	//因为在处理dir中的文件时，派生类的ProcessFile有可能改变了
	//当前目录，因此还要重新设置当前目录为dir。
	//执行过_findfirst后，可能系统记录下了相关信息，因此改变目录
	//对_findnext没有影响。
	_chdir(dir);
	if ((hFile=_findfirst(filespec,&fileinfo)) != -1)
	{
		do
		{
			//检查是不是目录
			//如果是,再检查是不是 . 或 .. 
			//如果不是,进行迭代
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name,".") != 0 && strcmp
					(fileinfo.name,"..") != 0)
				{
					char subdir[_MAX_PATH];
					strcpy(subdir,dir);
					strcat(subdir,fileinfo.name);
					strcat(subdir,"\\");
					ProcessDir(subdir,dir);
					std::vector<std::string>tmp= GetDirFilenames(subdir,filespec);
					for (std::vector<std::string>::iterator it=tmp.begin();it<tmp.end();it++)
					{
						filename_vector.push_back(*it);
					}
				}
			}
		} while (_findnext(hFile,&fileinfo) == 0);
		_findclose(hFile);
	}
	return filename_vector;
}

std::vector<FileInfo> CBrowseDir::AssemblyFileInfo(std::vector<std::string> file_vec)
{
	std::vector<FileInfo> fi;
	int length=strlen(m_szInitDir);
	for(std::vector<std::string>::const_iterator it = file_vec.begin(); it < file_vec.end(); ++it)
	{
		FileInfo iit;

		int globallength=it->size();

		std::string oldstr="\\";
		std::string newstr="/";
		std::string source(*it);
		for (std::string::size_type pos(0); pos != std::string::npos; pos+=newstr.length())
		{
			
			if ((pos = it->find("\\", pos)) != std::string::npos)
			{
				source.replace(pos,oldstr.length(),"/",1);
			}
			else
			{
				break;
			}
		}
		
		source.copy(iit.FileGlobalInfo,source.size(),0);
		source.copy(iit.FileLocalInfo,(source.size()-length),length);
		SHA1File(*it,iit.Hash);
		fi.push_back(iit);
	}

	return fi;
}
void CBrowseDir::SHA1File(const std::string &filepath,char sha1_digest[SHA224_DIGEST_LENGTH])
{
	FILE * fp = fopen(filepath.c_str(), "rb");
	if (!fp) 
	{
		fprintf(stderr, "Could not open file: %s\n", filepath.c_str());
		return ;
	}
	char buffin[BUFF_SIZE];
	
	unsigned char SHACODE[21]="";

	fseek(fp,0,SEEK_END);     //定位到文件末   
	long nFileLen = ftell(fp);
	fseek(fp,0,SEEK_SET);     //定位到文件末  
	if (nFileLen <=(FILE_BLOCK_SIZE*512))
	{
		SHACODE[0]=0x16;
		SHA_CTX ctx;
		SHA1_Init(&ctx);
		size_t nread=0;
		while (nread = fread(buffin, 1, BUFF_SIZE, fp))
		{
			SHA1_Update(&ctx, buffin, nread);
		}
		SHA1_Final(&SHACODE[1],&ctx);
		
	}
	else
	{
		SHACODE[0]=0x96;
		//segment block
		int index=0;
		int BlockNum=nFileLen/(FILE_BLOCK_SIZE*512)+1;
		unsigned char *SHACODETEMP=new unsigned char[BlockNum*20];
		size_t nread=0;
		SHA_CTX ctx;
		for (int i=0; i<BlockNum; i++)
		{
			SHA1_Init(&ctx);
			int emptyflag=0; 
			while (nread = fread(buffin, 1, BUFF_SIZE, fp))
			{
				SHA1_Update(&ctx, buffin, nread);
				emptyflag++;
				if (emptyflag == FILE_BLOCK_SIZE) break;
			}
			SHA1_Final(&SHACODETEMP[index],&ctx);
			index+=20;
		}
		
		//Joint
		//SHA_CTX ctx;
		SHA1_Init(&ctx);
		SHA1_Update(&ctx, SHACODETEMP, BlockNum*20);

		delete [] SHACODETEMP;

		SHA1_Final(&SHACODE[1],&ctx);
	}


	
	urlsafe_b64_encode(SHACODE,21,sha1_digest,SHA224_DIGEST_LENGTH);
	fclose(fp);

}

bool CBrowseDir::ProcessFile(const char *filename)
{
	return true;
}

void CBrowseDir::ProcessDir(const char *currentdir,const char *parentdir)
{

}

