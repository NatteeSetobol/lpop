#ifndef __SSL_H__
#define __SSL_H__

/*
	NOTE(): To install openssl on windows, install and compile vcpkg and type vcpkg install openssl --triplet x64-windows.
*/


#include "socket.hpp"
#include <string.h>
#include <openssl/rand.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include "required/intrinsic.hpp"
#include "required/nix.hpp"
#include "required/memory.hpp"
#include "required/platform.hpp"
#include "bucket.hpp"
#include "memory_arena.hpp"
#include "strings.hpp"



class SecureSocketLayer
{
	private:
		BIO *bio;
		SSL_CTX *ctx;
		SSL *ssl;
	public:
		s32* url;
		i32 maxBuffer;
		SecureSocketLayer(s32 *url);
		~SecureSocketLayer();
		bool Send(s32 *message);
		s32 *Recv(i32 *count);
};

struct sslinfo
{
	float httpVer;
	char* url;
	char* host;
	char* realURL;
	BIO *certBio;
	X509 *cert;
	//X509_NAME *certName;
	SSL_METHOD *method;
	SSL_CTX *ctx;
	SSL *ssl;
	BIO *outBio;
	//int server;
	struct platform_socket server;
	char* httpVersion;
	int packetLen;
};


void FreeSSL2(struct sslinfo *sslInfo);
void FreeSSL(struct sslinfo *sslInfo);
bool InitSSL(struct sslinfo *sslInfo, s32 *url);
bool InitSSL(memory_arena *arena, struct sslinfo *sslInfo, s32 *url);
#endif


