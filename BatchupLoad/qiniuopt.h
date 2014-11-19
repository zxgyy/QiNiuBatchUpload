#ifndef QINIU_OPT_H_
#define QINIU_OPT_H_
extern "C" {	
#include "./qiniu/include/qiniu/http.h"
#include "./qiniu/include/qiniu/base.h"
#include "./qiniu/include/qiniu/conf.h"
#include "./qiniu/include/qiniu/io.h"
#include "./qiniu/include/qiniu/resumable_io.h"
#include "./qiniu/include/qiniu/rs.h"
#include "./qiniu/b64/urlsafe_b64.h"
#pragma comment(lib, "qiniu/lib/qiniu.lib")
}

void debug(Qiniu_Client* client, Qiniu_Error err);
void stat(Qiniu_Client* client, const char* bucket, const char* key);
bool repeatcheck(Qiniu_Client* client, const char* bucket, const char* key,const char *hash);
void gist_delete(Qiniu_Client* client, const char* bucket, const char* key);
void copy(Qiniu_Client* client, 
		  const char* bucketSrc, const char* keySrc, 
		  const char* bucketDest, const char* keyDest);
void move(Qiniu_Client* client, 
		  const char* bucketSrc, const char* keySrc, 
		  const char* bucketDest, const char* keyDest);
void batchStat(Qiniu_Client* client, 
			   Qiniu_RS_EntryPath* entries, Qiniu_ItemCount entryCount);
void batchDelete(Qiniu_Client* client, 
				 Qiniu_RS_EntryPath* entries, Qiniu_ItemCount entryCount);
void batchMove(Qiniu_Client* client, 
			   Qiniu_RS_EntryPathPair* entryPairs, Qiniu_ItemCount entryCount);
void batchCopy(Qiniu_Client* client, 
			   Qiniu_RS_EntryPathPair* entryPairs, Qiniu_ItemCount entryCount);
char* uptoken(Qiniu_Client* client, const char* bucket);
char* downloadUrl(Qiniu_Client* client, const char* domain, const char* key);
void debug(Qiniu_Client* client, Qiniu_Error err);
char* upload(Qiniu_Client* client, char* uptoken, const char* key, const char* localFile);
int simple_upload(Qiniu_Client* client, char* uptoken, const char* key, const char* localFile);
int resumable_upload(Qiniu_Client* client, char* uptoken, const char* key, const char* localFile,char *bucket);
#endif