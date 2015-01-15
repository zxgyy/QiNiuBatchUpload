
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
#include "qiniu/include/curl/curl.h"


int main()
{
	char AccessKey[256]={0};
	char ScreteKey[256]={0};
	char Space[64]={0};
	char Path[256]={0};
	char Http[256]={0};
	time_t deadline;
	
	char *pHead[5]={AccessKey,ScreteKey,Space,Path,Http};

	ReadUploadInfo("./Information.ini", pHead);
	std::vector<FileInfo> fi;
	ScanAllFile(Path,fi);

	Qiniu_Client client;
	QINIU_ACCESS_KEY=(const char *)AccessKey;
	QINIU_SECRET_KEY=(const char *)ScreteKey;

	Qiniu_Servend_Init(-1);
	Qiniu_Client_InitMacAuth(&client, 1024, NULL);

	printf("\n\nStart Uploading...\n");
	CURL *curl;
	CURLcode res;
	res = curl_global_init(CURL_GLOBAL_DEFAULT);
	if(res != CURLE_OK) {
		fprintf(stderr, "curl_global_init() failed: %s\n",
			curl_easy_strerror(res));
		return 1;
	}

	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
	//curl_easy_setopt(curl,CURLOPT_HTTPGET,1L);
	//curl_easy_setopt(curl,CURLOPT_FRESH_CONNECT,1L);
	curl_easy_setopt(curl,CURLOPT_TIMEOUT,13);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
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
		struct tm *p=localtime(&deadline);
		

		
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
		//'D:\\Program Files\\Git\\bin\\curl.exe'
		char TempHttp[256];
		sprintf_s(TempHttp,"%s/%s?v=%4d%02d%02d",Http,it->FileLocalInfo,(1900+p->tm_year),1+p->tm_mon,p->tm_mday);
		curl_easy_setopt(curl, CURLOPT_URL, TempHttp);
		//int ret=system(TempHttp);
		res = curl_easy_perform(curl);
		/* Check for errors */
		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
			curl_easy_strerror(res));
		
		free(output);

	}
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	printf("\n\nStop Uploading...\n");
	Qiniu_Client_Cleanup(&client);
	Qiniu_Servend_Cleanup();
}
