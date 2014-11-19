
#include <stdio.h>
#include "IniConfig.h"
#include "qiniuopt.h"
#include "ReadUpLoadInfo.h"
#include "ScanUploadFile.h"
#include "qiniu/cJSON/cJSON.h"
#include "qiniu/b64/urlsafe_b64.h"
#include "qiniu/include/qiniu/io.h"
#include "qiniu/include/qiniu/resumable_io.h"
#include "qiniu/include/qiniu/emu_posix.h"
#include "qiniu/include/openssl/hmac.h"


int main()
{
	char AccessKey[256]={0};
	char ScreteKey[256]={0};
	char Space[64]={0};
	char Path[256]={0};

	time_t deadline;

	ReadUploadInfo("./Information.ini", AccessKey, ScreteKey, Space, Path);
	std::vector<FileInfo> fi;
	ScanAllFile(Path,fi);

	Qiniu_Client client;
	QINIU_ACCESS_KEY=(const char *)AccessKey;
	QINIU_SECRET_KEY=(const char *)ScreteKey;

	Qiniu_Servend_Init(-1);
	Qiniu_Client_InitMacAuth(&client, 1024, NULL);

	printf("\n\nStart Uploading...\n");
	for(std::vector<FileInfo>::const_iterator it = fi.begin(); it < fi.end(); ++it)
	{

		char hash[29]="";
		memcpy(hash,it->Hash,28);
		bool ft=repeatcheck(&client, Space, it->FileLocalInfo,hash);
		if (ft) continue;

		char *output;
		char encodedPutPolicy[256]={0};
		time_t deadline;
		time(&deadline);
		deadline=deadline+7200;

		unsigned char hmac_sha1code[256]={0};
		unsigned int hmac_sha1code_length;
		char encodesign[512]={0};

		char scope[256]="";

		cJSON *fmt;
		fmt=cJSON_CreateObject();
		sprintf_s(scope,256,"%s:%s",Space,it->FileLocalInfo);
		cJSON_AddStringToObject(fmt,"scope",scope);
		cJSON_AddNumberToObject(fmt,"deadline",deadline);

		output=cJSON_Print(fmt);
		printf("%s\n",output);

		//Get Code 3£ºencodedPutPolicy
		size_t destlength=urlsafe_b64_encode(output,strlen(output),encodedPutPolicy,256);

		printf("encodedPutPolicy=%s\n",encodedPutPolicy);
		//Get Code 2£ºencodesign
		unsigned char *sign = HMAC(EVP_sha1(),ScreteKey,strlen(ScreteKey), 
									(const unsigned char *)encodedPutPolicy,destlength,
									(unsigned char *) hmac_sha1code,
									&hmac_sha1code_length);

		destlength=urlsafe_b64_encode(hmac_sha1code,strlen((const char *)hmac_sha1code),encodesign,256);

		char UpLoadToken[1024];
		sprintf_s(UpLoadToken,"%s:%s:%s",AccessKey,encodesign,encodedPutPolicy);
		
		char *LocalHashCode=upload(&client, UpLoadToken, it->FileLocalInfo, it->FileGlobalInfo);
		//if (LocalHashCode == NULL)
		//{
		//	int uploadflag=resumable_upload(&client, UpLoadToken, 
		//					it->FileLocalInfo, it->FileGlobalInfo,Space);
		//}
		free(output);

	}
	printf("\n\nStop Uploading...\n");
	Qiniu_Client_Cleanup(&client);
	Qiniu_Servend_Cleanup();
}
