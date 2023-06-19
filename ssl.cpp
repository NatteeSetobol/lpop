#include "ssl.hpp"

void debug_callback(const SSL *ssl, int type, int val)
{
	if (type & SSL_CB_ALERT) {
		printf("SSL alert: %s: %s\n",
				(type & SSL_CB_READ ? "read" : "write"),
				SSL_alert_type_string_long(val));
	} else if (type & SSL_CB_LOOP) {
		printf("SSL error: %s: %s\n",
				(type & SSL_CB_READ ? "read" : "write"),
				ERR_error_string(val,NULL));
	} else {
		printf("SSL event: %s: %d\n",
				(type & SSL_CB_READ ? "read" : "write"),
				ERR_error_string(val,NULL));
	}
}

SecureSocketLayer::SecureSocketLayer(char *url)
{
	s32 *urlPort=NULL;
	SSL_library_init();
	SSL_load_error_strings();
	OpenSSL_add_all_algorithms();

	this->url = S32(url);

	ctx = SSL_CTX_new(TLS_method());

	if (!ctx)
	{
		DebugLog("SSL_CTX_new() failed.\n");
		exit(1);
	}

	//SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);

	#if 0
	SSL_CTX_set_info_callback(ctx, debug_callback);
	// Load the CA root certificate
	//Linux /etc/ssl/certs.pem
	if (SSL_CTX_load_verify_locations(ctx, "/etc/ssl/cert.pem", NULL) != 1)
	{
		DebugLog("SSL_CTX_load_verify_locations() failed.\n");
		exit(1);
	}

	//SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);
	// Verify the server certificate
	SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
	SSL_CTX_set_verify_depth(ctx, 4);
	#endif
	bio = BIO_new_ssl_connect(ctx);
	if (!bio)
	{
		DebugLog("BIO_new_ssl_connect() failed.\n");
		exit(1);
	}

	urlPort =  CS32Cat(3,this->url, ":", "443" );

	// Set the target host and port
	BIO_set_conn_hostname(bio, urlPort);

	BIO_get_ssl(bio, &ssl);

	//const char* version = SSL_get_version(ssl);

	//DebugLog("server is using %s\n", version);

	if (SSL_get_verify_result(ssl) != X509_V_OK)
	{
		DebugLog( "SSL certificate verification failed.\n");
		exit(1);
	}


	if (urlPort)
	{
		Free(urlPort);
	}

	if (this->url)
	{
		Free(this->url);
	}
	
	maxBuffer = 512;
}


SecureSocketLayer::~SecureSocketLayer()
{

	BIO_free_all(bio);
	SSL_CTX_free(ctx);
	//SSL_free(ssl);

}


bool SecureSocketLayer::Send(s32* message)
{
	i32 results = 0;
	results = BIO_write(bio, message, Strlen(message)-1);

	if (results == -2)
	{

		DebugLog("result: -2\n");
		return false;
	}  
	if (results == -1)
	{
		unsigned long err = ERR_get_error();
		char err_buf[256];
		ERR_error_string_n(err, err_buf, sizeof(err_buf));
		DebugLog("result: %s\n", err_buf);
		return false;
	}

	return true;
}


s32 * SecureSocketLayer::Recv(i32 *count)
{
	s32 *buffer =NULL;

	buffer = (s32*) MemoryRaw(maxBuffer);
	*count = BIO_read(bio, buffer, sizeof(buffer));

	return buffer;
}

static int VerifyCerf(X509_STORE_CTX *ctx, void *arg)
{

	return 1;
}



bool InitSSL(struct sslinfo *sslInfo, s32 *url)
{
	bool result = true;

	OpenSSL_add_all_algorithms();
	ERR_load_BIO_strings();
	ERR_load_crypto_strings();
	SSL_load_error_strings();

	sslInfo->certBio = BIO_new(BIO_s_file());
	sslInfo->outBio = BIO_new_fp(stdout,BIO_NOCLOSE);

	if (SSL_library_init() < 0)
	{
		printf("Error: Can not init ssl\n");
		result =  false;
	} else {

		sslInfo->method = (SSL_METHOD*) SSLv23_client_method();
		if ( (sslInfo->ctx = SSL_CTX_new(sslInfo->method)) == NULL)
		{
			printf("Unable to create a new SSL context structure.\n");
		}

		char *socketError = NULL;
		SSL_CTX_set_options(sslInfo->ctx,SSL_OP_NO_SSLv2);
		SSL_CTX_load_verify_locations (sslInfo->ctx, "certificate-authorities.pem", NULL);
		SSL_CTX_set_verify (sslInfo->ctx, SSL_VERIFY_PEER, NULL);
		SSL_CTX_set_cert_verify_callback(sslInfo->ctx,VerifyCerf,NULL );
		sslInfo->ssl = SSL_new(sslInfo->ctx);

		sslInfo->realURL = S32(url);
		sslInfo->host = StripToHostName(url);
		sslInfo->server  = CreateSocket(sslInfo->host, 443);

		if (sslInfo->server.connected)
		{
			//printf("not suppost to be here\n");
			SSL_set_fd(sslInfo->ssl,sslInfo->server.socket);

			//NOTES(): OpenSSL bug fix?.
			SSL_set_tlsext_host_name(sslInfo->ssl, sslInfo->host);

			int c = SSL_connect(sslInfo->ssl);
			if (c != 1)
			{
				int error = 0;
				error = SSL_get_error(sslInfo->ssl,c);

				switch(error)
				{
					case SSL_ERROR_NONE:
					{
						printf("here\n");
						break;
					}
					case SSL_ERROR_ZERO_RETURN:
					{
						printf("of here\n");
						break;
					}
					case SSL_ERROR_SSL:
					{
						printf("lib error\n");
						break;
					}
					case SSL_ERROR_WANT_READ:
					{
						printf("want read  error\n");
						break;
					}
					case SSL_ERROR_WANT_WRITE:
					{
						printf("want write error\n");
						break;
					}
					case SSL_ERROR_SYSCALL:
					{
						printf("system call error\n");
						break;
					}
					case SSL_ERROR_WANT_CONNECT:
					{
						printf("want to connect error\n");
						break;
					}
				}
				printf("can not connect to SSL port error type: %i\n", error);
			} else {
				sslInfo->cert = SSL_get_peer_certificate(sslInfo->ssl);

				if (sslInfo->cert == NULL)
				{
					printf("Can not get certification from peer\n");
				} else {
				}
			}
		} else {
			result = false;
		}
	}

	return result;
}

void FreeSSL(struct sslinfo *sslInfo)
{
	BIO_free(sslInfo->certBio);
	BIO_free(sslInfo->outBio);
	X509_free(sslInfo->cert);
	SSL_free(sslInfo->ssl);
	SSL_CTX_free(sslInfo->ctx);

	if (sslInfo->realURL)
	{
		Free(sslInfo->realURL);
		sslInfo->realURL=NULL;
	}
	if (sslInfo->host)
	{
		Free(sslInfo->host );
		sslInfo->host = NULL;
	}
}

void FreeSSL2(struct sslinfo *sslInfo)
{
	BIO_free(sslInfo->certBio);
	BIO_free(sslInfo->outBio);
	X509_free(sslInfo->cert);
	SSL_free(sslInfo->ssl);
	SSL_CTX_free(sslInfo->ctx);

}
