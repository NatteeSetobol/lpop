#ifndef __CURLED__
#define __CURLED__

#include "../lpop/required/intrinsic.hpp"
#include "../lpop/required/nix.hpp"
#include "../lpop/required/memory.hpp"
#include "../lpop/required/platform.hpp"
#include "../lpop/required/assert.hpp"
#include "../lpop/bucket.hpp"
#include "../lpop/array.hpp"
#include "../lpop/strings.hpp"
#include <curl/curl.h>

class CURLED
{
    public:
        CURLED();
        ~CURLED();
        s32* Get(s32 *url);
        s32* Post(s32 *url, s32* postData);
        void AddHeader(s32* key,s32* value);
    private:
        HashBucket *headersBucket;
        CURL *curl=NULL;
	    struct curl_slist *headers = NULL;

        s32* Request(s32 *url, s32* postData);
};

bool IsHttp(s32 *url);
size_t curl_write_func(void *ptr, size_t size, size_t nmemb, void *userdata);
#endif