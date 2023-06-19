#ifndef __HTTP_PROTOCOL_H__
#define  __HTTP_PROTOCOL_H__

#include "socket.hpp"
#include "required/intrinsic.hpp"
#include "required/nix.hpp"
#include "required/memory.hpp"
#include "required/platform.hpp"
#include "token.hpp"
#include "bucket.hpp"
#include "pwn.hpp"
#include "zlib.h"
#if USE_SSL
#include "ssl.hpp"
#endif
#include "pwn.hpp"

#define CHUNK 16834

enum http_token_type 
{
	HTTP_NONE,
	HTTP_LINEBREAK,
	HTTP_WLINEBREAK,
	HTTP_DATA,
	HTTP_TOKEN_COUNT,
};

struct header
{
	s32 *key;
	s32 *value;
};

struct http_header
{
	struct bucket_list headers;
	s32 *body;
};

struct http_download_info
{
	us32 *data;
	int length;
};

typedef struct http_headers
{
	s32* status;	
	s32* statusText;	
	s32* version;
	HashBucket headers;
	s32 *body;
} http_headers;

class Http_Request
{
	public:
		HashBucket headers;
		struct http_headers responseHeader = {};
		s32 *response=NULL;
		double version;
		struct platform_socket psocket = {};
		bool isSSL = false;
		struct sslinfo sslInfo = {};
		bool isCompressed = false;

		SecureSocketLayer *secureSocketLayer;

		Http_Request();
		~Http_Request();
		void SetVersion(double version);
		s32* Get(s32* url);
		s32* Post(s32* url, s32* body);
		s32* HttpRequest(char* url,  Method method, char* postData);
		void AddHeader(s32* key, s32* value);
		s32 *GetHostStringName(s32* localIP);
		struct http_header HttpParseHeader();
		void *FreeHttpHeader();
		bool IsInHeader(s32 *key);
		int totalPacketSize;
		void CheckHeaders(s32 *url);
		s32* GetHttpPath(s32* url);
		s32 *BuildHttpHeader(s32 *url, Method method, s32* postData);
		bool Send(s32* message);
		us32 *Read(int *count);
		us32 *ParseHeaders(us32* text, i32* size);
};

http_download_info HttpSkipHeader(void *data,int length);
http_download_info  HttpRequestDownload(char* url, struct bucket_list headers, Method method, char* postData);
char* HttpRequest(char* url, struct bucket_list* headers, Method method, char* postData);
enum http_token_type GetHeaderToken(struct tokenizer *tokenizer);
struct http_header HttpParseHeader(char *data);
b32 AddToHeader(struct memory_arena *arena, struct http_headers *httpHeaders, s32* key, s32* value);
struct http_download_info HTTPSRequest(memory_arena *arena,s32* url, http_headers *httpHeaders, Method method, s32 *postData);
#endif
