#include "qiniuopt.h"

/* @gist debug */
void debug(Qiniu_Client* client, Qiniu_Error err)
{
	printf("\nerror code: %d, message: %s\n", err.code, err.message);
	printf("respose header:\n%s", Qiniu_Buffer_CStr(&client->respHeader));
	printf("respose body:\n%s", Qiniu_Buffer_CStr(&client->b));
}
/* @endgist */

/* @gist stat */
void stat(Qiniu_Client* client, const char* bucket, const char* key)
{
	Qiniu_RS_StatRet ret;
	Qiniu_Error err = Qiniu_RS_Stat(client, &ret, bucket, key);
	if (err.code != 200) {
		debug(client, err);
		return;
	}
	printf("hash: %s, fsize: %lld, mimeType: %s\n", ret.hash, ret.fsize, ret.mimeType);
}
/* @endgist */
bool repeatcheck(Qiniu_Client* client, const char* bucket, const char* key,const char *hash)
{
	Qiniu_RS_StatRet ret;
	Qiniu_Error err = Qiniu_RS_Stat(client, &ret, bucket, key);
	if (err.code != 200) {
		debug(client, err);
		return false;
	}

	if(strcmp(hash,ret.hash)==0) 
	{
		printf("%s is already Uploaded!\n",key);
		printf("------Hash Code = %s\n",hash);
		return true;
	}
	return false;
}

/* @gist delete*/
void gist_delete(Qiniu_Client* client, const char* bucket, const char* key)
{
	Qiniu_Error err = Qiniu_RS_Delete(client, bucket, key);
	if (err.code != 200) {
		debug(client, err);
		return;
	}
	printf("%s:%s delete OK.\n", bucket, key);
}
/* @endgist */

/* @gist copy */
void copy(Qiniu_Client* client, 
		  const char* bucketSrc, const char* keySrc, 
		  const char* bucketDest, const char* keyDest)
{
	Qiniu_Error err = Qiniu_RS_Copy(client, bucketSrc, keySrc, bucketDest, keyDest);
	if (err.code != 200)
	{
		debug(client, err);
		return;
	}
	printf("Copy %s:%s -> %s:%s OK.\n", bucketSrc, keySrc, bucketDest, keyDest);
}
/* @endgist */

/* @gist move */
void move(Qiniu_Client* client, 
		  const char* bucketSrc, const char* keySrc, 
		  const char* bucketDest, const char* keyDest)
{
	Qiniu_Error err = Qiniu_RS_Move(client, bucketSrc, keySrc, bucketDest, keyDest);
	if (err.code != 200) {
		debug(client, err);
		return;
	}
	printf("Move %s:%s -> %s:%s OK.\n", bucketSrc, keySrc, bucketDest, keyDest);
}
/* @endgist */

/* @gist batchStat */
void batchStat(Qiniu_Client* client, 
			   Qiniu_RS_EntryPath* entries, Qiniu_ItemCount entryCount)
{
	Qiniu_RS_BatchStatRet* rets = (Qiniu_RS_BatchStatRet *)calloc(entryCount, sizeof(Qiniu_RS_BatchStatRet));
	Qiniu_Error err = Qiniu_RS_BatchStat(client, rets, entries, entryCount);

	int curr = 0;
	while (curr < entryCount) {
		printf("\ncode: %d\n", rets[curr].code);

		if (rets[curr].code != 200) {
			printf("error: %s\n", rets[curr].error);
		} else {
			printf("hash: %s\n", rets[curr].data.hash);
			printf("mimeType: %s\n", rets[curr].data.mimeType);
			printf("fsize: %lld\n", rets[curr].data.fsize);
			printf("putTime: %lld\n", rets[curr].data.putTime);
		}
		curr++;
	}

	free(rets);

	if (err.code != 200) {
		debug(client, err);
		return;
	}
}
/* @endgist */

/* @gist batchDelete */
void batchDelete(Qiniu_Client* client,
				 Qiniu_RS_EntryPath* entries, Qiniu_ItemCount entryCount)
{
	Qiniu_RS_BatchItemRet* rets = (Qiniu_RS_BatchItemRet*) calloc(entryCount, sizeof(Qiniu_RS_BatchStatRet));
	Qiniu_Error err = Qiniu_RS_BatchDelete(client, rets, entries, entryCount);

	int curr = 0;
	while (curr < entryCount) {
		printf("\ncode: %d\n", rets[curr].code);

		if (rets[curr].code != 200) {
			printf("error: %s\n", rets[curr].error);
		}
		curr++;
	}



	if (err.code != 200) {
		debug(client, err);
		return;
	}
	free(rets);
}
/* @endgist */

/* @gist batchMove */
void batchMove(Qiniu_Client* client, 
			   Qiniu_RS_EntryPathPair* entryPairs, Qiniu_ItemCount entryCount)
{
	Qiniu_RS_BatchItemRet* rets =  (Qiniu_RS_BatchItemRet*)calloc(entryCount, sizeof(Qiniu_RS_BatchStatRet));
	Qiniu_Error err = Qiniu_RS_BatchMove(client, rets, entryPairs, entryCount);

	int curr = 0;
	while (curr < entryCount) {
		printf("\ncode: %d\n", rets[curr].code);

		if (rets[curr].code != 200) {
			printf("error: %s\n", rets[curr].error);
		}
		curr++;
	}

	free(rets);

	if (err.code != 200) {
		debug(client, err);
		return;
	}
}
/* @endgist */

/* @gist batchCopy */
void batchCopy(Qiniu_Client* client, 
			   Qiniu_RS_EntryPathPair* entryPairs, Qiniu_ItemCount entryCount)
{
	Qiniu_RS_BatchItemRet* rets = (Qiniu_RS_BatchItemRet*)calloc(entryCount, sizeof(Qiniu_RS_BatchItemRet));///zxg
	Qiniu_Error err = Qiniu_RS_BatchCopy(client, rets, entryPairs, entryCount);
	int curr = 0;

	while (curr < entryCount) {
		printf("\ncode: %d\n", rets[curr].code);

		if (rets[curr].code != 200) {
			printf("error: %s\n", rets[curr].error);
		}
		curr++;
	}
	free(rets);

	if (err.code != 200) {
		debug(client, err);
		return;
	}
}
/* @endgist */

/* @gist uptoken */
char* uptoken(Qiniu_Client* client, const char* bucket)
{
	Qiniu_RS_PutPolicy putPolicy;
	Qiniu_Zero(putPolicy);
	putPolicy.scope = bucket;
	return Qiniu_RS_PutPolicy_Token(&putPolicy, NULL);
}
/* @endgist */

/* @gist downloadUrl */
char* downloadUrl(Qiniu_Client* client, const char* domain, const char* key)
{
	char* url;
	char* baseUrl;
	Qiniu_RS_GetPolicy getPolicy;

	Qiniu_Zero(getPolicy);
	baseUrl = Qiniu_RS_MakeBaseUrl(domain, key); // baseUrl也可以自己拼接："http://"+domain+"/"+urlescape(key)
	url = Qiniu_RS_GetPolicy_MakeRequest(&getPolicy, baseUrl, NULL);

	Qiniu_Free(baseUrl);
	return url;                                  // When url is no longer being used, free it by Qiniu_Free.
}
/* @endgist */

/* @gist upload */
char* upload(Qiniu_Client* client, char* uptoken, const char* key, const char* localFile)
{
	Qiniu_Error err;
	Qiniu_Io_PutRet putRet;
	err = Qiniu_Io_PutFile(client, &putRet, uptoken, key, localFile, NULL);
	if (err.code != 200) {
		debug(client, err);
		return NULL;
	}
	return strdup(putRet.hash);
}
/* @endgist */

/* @gist simple-upload */
int simple_upload(Qiniu_Client* client, char* uptoken, const char* key, const char* localFile)
{
	Qiniu_Error err;
	err = Qiniu_Io_PutFile(client, NULL, uptoken, key, localFile, NULL);
	return err.code;
}
/* @endgist */

/* @gist resumable-upload */
int resumable_upload(Qiniu_Client* client, char* uptoken, const char* key, const char* localFile,char *bucket)
{
	Qiniu_Error err;
	Qiniu_Rio_PutExtra extra;
	Qiniu_Zero(extra);
	extra.bucket = bucket;
	err = Qiniu_Rio_PutFile(client, NULL, uptoken, key, localFile, &extra);
	return err.code;
}
/* @endgist */