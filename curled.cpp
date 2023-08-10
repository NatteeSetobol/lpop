#include "curled.h"
extern Map *tmpHeaders = NULL;
extern int oldsize = 0;

char errorBuffer[CURL_ERROR_SIZE];

CURLED::CURLED()
{
	respHeaders = NULL;
	errorCount = 0;
}

CURLED::~CURLED()
{

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

size_t header_callback(char *buffer, size_t size, size_t nmemb, void *userdata) {
    // Calculate the total size of the header
    size_t totalSize = size * nmemb;
	s32* stats = NULL;

	if (strlen(buffer) == 2)
	{

		if (StrCmp(buffer,"\r\n"))
		{
			_Array *pUserData = (_Array*) userdata;
			pUserData->Add(tmpHeaders);

			tmpHeaders = NULL;
		}
	}
	if (strlen(buffer) > 5)
	{
		stats = MidString(buffer,0,4);
		if (stats)
		{
			if (StrCmp(stats,"HTTP"))
			{
				
				Array *split = NULL;


				tmpHeaders = new Map();

				split = S32Split(buffer, " ");

				if (split)
				{

					if (split->length > 0)
					{
						s32 *status = NULL;
						s32 *statusCode = NULL;

						status = (s32*) split->Get(1);

						statusCode = S32(status);

						tmpHeaders->Add("status", (void*) statusCode);
						//respStatus = SToI(status);
					}
				}

				delete split;
				split=NULL;
				
			}
		} else {
			/*
			Array *colSplit = NULL;

			split = S32Split(buffer, ":");

			if (split->length > 0)
			{
				
			}
			*/
		}

		Free(stats);
		stats=NULL;

	} else {
	}


    return totalSize;
}

static int errorHandler(CURL* handle, curl_infotype type, char* data, size_t size, void* userptr) {
    (void)handle; // Unused parameter
    (void)type; // Unused parameter
    (void)userptr; // Unused parameter

	CURLED *errorCurl =  NULL;

	errorCurl =  (CURLED*) userptr;

    return 0;
}

s32* CURLED::Request(s32 *url, s32* postData)
{
    char *response_body=NULL;
    CURLcode res;

    curl = curl_easy_init();

	respHeaders = new _Array();

    if (curl) 
    {
        //curl_slist_append(headers, "Accept-Encoding: zip, deflate, br");
        if (postData)
        {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData);
        }
 		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_func);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);
    	curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEHEADER, respHeaders);
        curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, errorHandler);

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

bool IsHttp(s32 *url)
{
	bool isHttp = false;
	s32 *http=NULL;
	s32 *https=NULL;

	isHttp = false;

	if (Strlen(url) < 7) return false;

	http = MidString(url,0,7);
	https = MidString(url,0,8);

	if (StrCmp(http, "http://"))
	{
		isHttp = true;
	} else 
	if (StrCmp(https, "https://"))
	{
		isHttp = true;
	}

	if (http)
	{
		Free(http);
		http=NULL;
	}
	if (https)
	{
		Free(https);
		https=NULL;
	}
	return isHttp;
}