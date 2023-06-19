#include "curled.h"

extern int oldsize = 0;
CURLED::CURLED()
{
    headersBucket = new HashBucket();
}

CURLED::~CURLED()
{
   delete headersBucket;
}


s32* CURLED::Get(s32 *url)
{
    return Request(url, NULL);
}

s32* CURLED::Post(s32 *url, s32* postData)
{
    return Request(url, postData);
}

void CURLED::AddHeader(s32* key,s32* value)
{
    /*
    s32 *newValue = NULL;

    newValue = S32(value);
    headersBucket->Add((i8*) key,(i8*) value);
    */

    s32* newValue = NULL;

    newValue = CS32Cat(3, key, ": ", value);

    headers = curl_slist_append(headers, newValue);

    Free(newValue);

}

s32* CURLED::Request(s32 *url, s32* postData)
{
    char *response_body=NULL;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) 
    {
        #if 0
        for (int i=0; i < headersBucket->keys.length;i++)
        {
            s32 *value = NULL;
            s32 *key = NULL;
            s32 *comb = NULL;

            key = (s32*) headersBucket->keys.Get(i);
            value = (s32*) headersBucket->Get(key);

            comb = CS32Cat(3, key, ": ", value);

            headers = curl_slist_append(headers, comb);

        }
        #endif

        curl_slist_append(headers, "Accept-Encoding: zip, deflate, br");
        if (postData)
        {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData);
        }

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_func);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
        }
    }

	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);


    oldsize = 0;

    return response_body;
}

size_t curl_write_func(void *ptr, size_t size, size_t nmemb, void *userdata)
{
	char **response_body_ptr = (char **) userdata;
    size_t response_body_size = (oldsize + nmemb);
	/*
	DebugLog("------------------------\n");
	DebugLog("nmemb: %i\n", nmemb);
	DebugLog("body_size: %i\n", response_body_size);
	DebugLog("size: %i\n", size);
	DebugLog("ptr: %s\n", (char*) ptr);
	DebugLog("userdata: %s\n", (char*) userdata);
	DebugLog("old size: %i\n", oldsize);
	DebugLog("------------------------\n");
	*/
	
	if (*response_body_ptr == NULL)
	{
		*response_body_ptr =  (s32*) MemoryRaw(response_body_size+1);
		memcpy(*response_body_ptr, ptr, response_body_size);
		oldsize = nmemb;
	} else {
		char *temp =  NULL;

		temp = (char*) *response_body_ptr;

		*response_body_ptr=  (s32*) MemoryRaw(response_body_size+1);

		memcpy(*response_body_ptr, temp, oldsize);
		memcpy((*response_body_ptr)+oldsize, (char*) ptr, nmemb);
		oldsize += nmemb;

        Free(temp);
	}
	
    return size*nmemb;
}
