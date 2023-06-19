#include <stdio.h>
#include "httpprotocol.hpp"

Http_Request::Http_Request()
{
	SetVersion(1.1);
}

Http_Request::~Http_Request()
{
	
	if (responseHeader.version)
	{
		Free(responseHeader.version);
		responseHeader.version = NULL;
	}
	if (responseHeader.status)
	{
		Free(responseHeader.status);
		responseHeader.status = NULL;
	}

	if (responseHeader.statusText)
	{
		Free(responseHeader.statusText);
		responseHeader.statusText = NULL;
	}

	if (responseHeader.body)
	{
		Free(responseHeader.body);
		responseHeader.body= NULL;
	}

	responseHeader.headers.DeleteAll();

	#if USE_SSL
	if (this->isSSL)
	{
		if (secureSocketLayer)
		{
			delete secureSocketLayer;
		}
	}
	#endif
}

void Http_Request::SetVersion(double version)
{
	this->version=version;
}

bool Http_Request::IsInHeader(s32 *key)
{
	i8 *object = headers.Get(key);

	if (object)
	{
		return true;
	}

	return false;
}

void Http_Request::AddHeader(s32* key, s32* value)
{
	s32* newKey = NULL;
	s32* newValue = NULL;

	//newKey = S32(key);
	value = S32(value);

	headers.Add((i8*) key, (i8*) value);
}

s32* Http_Request::Get(s32* url)
{
	response = HttpRequest(url,GET,NULL);
	return response;
}

s32* Http_Request::Post(s32* url, s32* body)
{
	return HttpRequest(url,POST,body);
}

s32 *Http_Request::GetHostStringName(s32* localIP)
{
	s32 *httpString = NULL;

	httpString = S32Replace(localIP, "https://","");

	if (StrCmp(httpString, localIP))
	{
		if (httpString)
		{
			Free(httpString);
			httpString = NULL;
		}
		httpString = S32Replace(localIP, "http://","");
	}

	return httpString;
}

bool IsSSL(char* url)
{
	bool isSSL = false;
	struct marray *findHttp = NULL;

	findHttp = FindString("https://", url);

	if (findHttp->count == 1)
	{
		int *httpPos = NULL;

		httpPos = GetFromMArray(findHttp,0);

		if (*httpPos == 0)
		{
			isSSL = true;
		}
	} else {
		int *httpPos = NULL;

		FreeMArray(findHttp);

		if (findHttp)
		{
			Free(findHttp);
			findHttp = NULL;
		}

		findHttp = FindString("http://", url);

		if (findHttp->count == 1)
		{
			httpPos = GetFromMArray(findHttp,0);
			if (*httpPos == 0)
			{
				isSSL = false;
			} 
		}
	}


	FreeMArray(findHttp);

	if (findHttp)
	{
		Free(findHttp);
		findHttp=NULL;
	}

	return isSSL;
}

inline s32*  GetMethod(Method method)
{
	s32* methodString = NULL;

	if (method == GET)
	{
		methodString = S32((char*) "GET");
	} else 
	if (method == OPTIONS)
	{
		methodString = S32((char*) "OPTIONS");
	} else 
	if (method == POST)
	{
		methodString = S32((char*) "POST");
	}

	return methodString;
}


s32 *Http_Request::BuildHttpHeader(s32 *url, Method method, s32* postData)
{
	s32* fullHeader = NULL;
	s32* temp = NULL;
	s32 htmlVersion[256]={};
	s32 *httpPath=NULL;
		
	fullHeader = GetMethod(method);

	//TODO(): Don't forget to do the POST method 
	temp = fullHeader;

	gcvt(this->version,2,htmlVersion);

	DebugLog("%s\n", url);

	httpPath = GetHttpPath(url);

	fullHeader = S32Cat(temp, 6, " ",httpPath, " ", "HTTP/", htmlVersion, "\r\n");

	if (temp)
	{
		Free(temp);
		temp = NULL;
	}
	
	for (int headerIndex = 0; headerIndex < headers.count; headerIndex++)
	{
		char*  index = NULL;
		int breakhere = 0;
		char* buck = NULL;// GetFromBucket(&headers,index); //(bucket*)& headers->buckets[index];
	
		index = (char*) headers.keys.Get(headerIndex);
		buck = (char*) headers.Get(index);
		if (buck)
		{
			temp = fullHeader;
			fullHeader = S32Cat(temp, 4, index, ": ", buck, "\r\n");

			if (temp)
			{
				Free(temp);
				temp = NULL;
			}

		} 
	}

	if (temp)
	{
		Free(temp);
		temp = NULL;
	}

	if (method == GET || method == OPTIONS)
	{
		temp = fullHeader;
		fullHeader = S32Cat(temp, 1, "\r\n\r\n");

		if (temp)
		{
			Free(temp);
			temp = NULL;
		}
	} else 
	if (method == POST)
	{
		temp = fullHeader;

		if (postData)
		{
			fullHeader = S32Cat(temp, 3, "\r\n", postData, "\r\n\r\n");
		} else {
			fullHeader = S32Cat(temp, 1, "\r\n\r\n");
		}

		if (temp)
		{
			Free(temp);
			temp = NULL;
		}

	} else {
		DebugLog("Bad Method\n");
	}

	Free(httpPath);
	return fullHeader;
}

s32* Http_Request::GetHttpPath(s32 *url)
{
	s32 *httpPath = NULL;
	s32 *hostname = NULL;
	s32 *noHttp = NULL;

	hostname = StripToHostName(url);

	noHttp = GetHostStringName(url);

	
	DebugLog("replace %s with %s\n", noHttp, hostname);
	httpPath = S32Replace(noHttp, hostname, "");

	if (httpPath == NULL)
	{
		httpPath = S32("/");
	}

	Free(hostname);
	Free(noHttp);
	return httpPath;
}

char* Http_Request::HttpRequest(char* url, Method method, char* postData)
{
	bool isEncrypted = false;
	bool connected = false;
	int count = 0;
	int totalSize= 0;
	us32 *tempBuffer = NULL;
	bool foundHeader = false;
	us32 lastChar=0;
	int retCarCount=0;
	s32 *response=NULL;
	i32 touts=0;
    us32 *results=NULL;
	us32* body = NULL;
	s32 *hostName=NULL;
#if USE_SSL
#endif
	s32* fullHeader = NULL;

	this->isSSL = IsSSL(url);

	if (this->isSSL)
	{
#if USE_SSL
		/*
		if (!InitSSL(&sslInfo, url))
		{
			DebugLog("can't not init SSL\n");
			connected = false;
		} else {
			connected = true;
		}
		*/
		
		hostName = StripToHostName(url);
		secureSocketLayer = new SecureSocketLayer(hostName);

		connected = true;
#else
		DebugLog("SSL has not been compiled! Please include the SSL library with the compile flag -lssl -lcrypto and -DUSE_SSL\n")
#endif
	} else {
		psocket = CreateSocket(url,80);
		connected = psocket.connected;
	}

	if (connected )
	{
		i32 headerSize = 0;
		CheckHeaders(url);

		fullHeader = this->BuildHttpHeader(url, method, postData);

		this->Send(fullHeader);

	//	DebugLog("%s\n", fullHeader);
		do
		{
			tempBuffer = this->Read(&count);

			if (results == NULL)
			{
				results = US32(tempBuffer);
				totalSize = count;
			} else 
			{
				us32 *temp = NULL;
				int oldCount = 0;

				temp = results;
				
				oldCount = totalSize;
				totalSize += count;

				results = (us32*) MemoryRaw(totalSize+1);

				memcpy(results,temp,oldCount);
				memcpy(results+oldCount,tempBuffer,count);

				if (temp)
				{
					Free(temp);
				}
			}

			if (tempBuffer)
			{
				Free(tempBuffer);
			}

		} while (count > 0);


		body = ParseHeaders( results, &headerSize);

		if (body)
		{
			if (isCompressed)
			{
				#if 1
				i32 chunkCount = 0;
				i32 chunkAv = 0;
				z_stream zStream = {}; 
				i32 zStatus = 0;
				i32 bodySize = 0;

				//i32 bodySize = totalSize - headerSize;

				if (inflateInit2(&zStream, (15+32)) != Z_OK)
				{
					printf("error init inflation\n");
				}

				bodySize = totalSize - headerSize;

				DebugLog("total size: %i\n", totalSize);
				DebugLog("header size: %i\n", headerSize);
				DebugLog("payload size: %i\n", totalSize  - headerSize);


				//HexPrint((char*) (body+9), 32);

				zStream.next_in = (Bytef*) body+9;
				zStream.avail_in = CHUNK;
				chunkAv = CHUNK;

				s32 *decompressed = NULL;

				decompressed = (char*) MemoryRaw(bodySize+300);
				zStream.avail_out = chunkAv;
				zStream.next_out = (Bytef*) (decompressed);
				zStatus = inflate(&zStream, Z_SYNC_FLUSH);
				chunkAv =CHUNK - zStream.avail_out;

				if (zStatus == Z_STREAM_END || zStatus == Z_OK)
				{
					DebugLog("depressed: %s\n", decompressed);
				} else
				if (zStatus != Z_OK)
				{
					DebugLog("Error: can't not depressed data\n");
				}
				#endif

			} else {
				responseHeader.body = S32((s32*) body);
			}
		} else {
			responseHeader.body = NULL;
		}

		if (fullHeader)
		{
			Free(fullHeader);
		}
	} 

	FreeSSL(&sslInfo);

	if (results)
	{
		Free(results);
	}

	if (hostName)
	{
		Free(hostName);
	}

	return NULL;
}


us32 *Http_Request::ParseHeaders( us32* text, int *size)
{
	us32* tempBufferPointer = NULL;
	i32* retCarCount = 0;
	bool foundHeader = false;
	i32 chunkCount = 0;
	us32 *startingPointer = NULL;
	us32 *tempBuffer = NULL;
	us32 *newBuffer=NULL;

	tempBufferPointer = text;
	startingPointer = text;

	while(*tempBufferPointer != '\0' && foundHeader==false)
	{
		if (*tempBufferPointer == 0xd ) 
		{
			retCarCount++;
			if (*(tempBufferPointer+1) != 0x00 )
			{
				if (*(tempBufferPointer+1) == 0xa )
				{
					i32 length  = 0;
					s32 *text = NULL;
					Array* colonSearch = NULL; 

					length =  tempBufferPointer - startingPointer ;
					
					text = (s32*) MemoryRaw(length+1);

					memcpy(text,startingPointer,length);

					colonSearch = S32Find(":",text);

					if (colonSearch->length != 0)
					{
						i32 *colonPosition = NULL;
						s32 *key = NULL;
						s32 *value = NULL;

						colonPosition = colonSearch->Get(0);

						key = MidString(text,2, *(colonPosition));
						value = MidString(text,*(colonPosition)+2, Strlen(text));

						
						if (StrCmpCS(key, "content-encoding"))
						{
							//DebugLog("compressed: %s\n", value);
							if (StrCmpCS(value, "gzip"))
							{
								isCompressed = true;
							}
						}

						responseHeader.headers.Add((i8*) key, (i8*) value);

						Free(key);

					} else {
						Array *spacesSearch = NULL;
						i32 *spacesPosition = NULL;
						i32 *spacesPosition2 = NULL;
						i32 *spacesPosition3 = NULL;

						spacesSearch = S32Find(" ", text);

						if (spacesSearch->length != 0)
						{
							spacesPosition = spacesSearch->Get(0);
							spacesPosition2 = spacesSearch->Get(1);

							responseHeader.version = MidString(text,0,*(spacesPosition)); 
							responseHeader.status =  MidString(text,*(spacesPosition)+1,*(spacesPosition2));
							responseHeader.statusText = MidString(text,*(spacesPosition2)+1,Strlen(text));
						}

						delete spacesSearch;
						spacesSearch = NULL;
					}


					delete colonSearch;
					colonSearch=NULL;

					startingPointer =  tempBufferPointer;

					if (*(tempBufferPointer+2) != 0x0)
					{
						if (*(tempBufferPointer+2) == 0xd)
						{
							if (*(tempBufferPointer+3) != 0x00)
							{
								if (*(tempBufferPointer+3) == 0xa)
								{
									foundHeader= true;
									chunkCount = (tempBuffer+chunkCount) -  tempBufferPointer-4;
									newBuffer = tempBufferPointer+4; 
									*size += 4;
									if (text)
									{
										Free(text);
										text=NULL;
									}
									break;
								} 
							} else {
								//lastChar = 0xE;
							}
						} 

					}

					if (text)
					{
						Free(text);
						text=NULL;
					}
				}
				retCarCount++;
			} else {
				//lastChar = 0xd;
			}
		}
		if (*tempBufferPointer == 0xa)
		{
			//lastChar = 0xa;
			retCarCount++;
		}
		*size += 1;
		tempBufferPointer++;
	}


	return newBuffer;
}

us32 *Http_Request::Read(int *count)
{
	us32 *tempBuffer = NULL;

	if (this->isSSL)
	{
#if USE_SSL
		tempBuffer = (us32*) secureSocketLayer->Recv(count);

		//*count = SSL_read(sslInfo.ssl, tempBuffer,512);
#endif

	} else {
		tempBuffer = (us32*) MemoryA(us32*,535);
		*count = RecvFromSocket(&psocket, (s32*) tempBuffer,512);
	}

	return tempBuffer;
}

bool Http_Request::Send(s32 *message)
{
	if (this->isSSL)
	{
#if USE_SSL
		if (!secureSocketLayer->Send(message))
		{
			DebugLog("[!] Not sent\n");
		}
		//SSL_write(sslInfo.ssl,message,Strlen(message)-1);
#endif
	} else {
		SendToSocket(&psocket,message,Strlen(message));
	}

	return true;
}

void Http_Request::CheckHeaders(s32 *url)
{
	
	Array usualHeader;
	s32* host = NULL;
	s32* userAgent = NULL;
	s32* accept=NULL;
	s32* connection=NULL;

	host = S32("host");
	userAgent = S32("user-agent");
	accept = S32("accept");
	connection= S32("connection");

	usualHeader.Add((i32*) host);
	usualHeader.Add((i32*) userAgent);
	usualHeader.Add((i32*) accept);
	usualHeader.Add((i32*) connection);
	
	for (int headerIndex = 0; headerIndex < headers.keys.length; headerIndex++)
	{
		//if (StrCmpCS()
		for (int i=0;i<usualHeader.length;i++)
		{
			s32* usualHeaderStr = NULL;
			s32* header = NULL;

			usualHeaderStr = (s32*) usualHeader.Get(i);
			header = (s32*) headers.keys.Get(headerIndex);
			
			if (StrCmpCS(usualHeaderStr, header))
			{
				usualHeader.Remove(i);
				break;
			}
		}
	}

	for (int j=0; j < usualHeader.length; j++)
	{
		s32* usualHeaderStr = NULL;

		usualHeaderStr = (s32*) usualHeader.Get(j);

		if (StrCmpCS(usualHeaderStr, "host"))
		{
			s32* hostName = NULL;
			s32* host= NULL;

			hostName = StripToHostName(url);
			
			host = S32("Host");

			headers.Add((i8*) host, (i8*) hostName);

			Free(host);
		}

		if (StrCmpCS(usualHeaderStr, "user-agent"))
		{
			s32 *userAgent = NULL;
			s32 *browser = NULL;

			userAgent = S32("User-Agent");
			browser = S32("Mozilla/5.0 (Macintosh; Intel Mac OS X 10.15; rv:109.0) Gecko/20100101 Firefox/109.0");

			headers.Add((i8*) userAgent, (i8*) browser);

			Free(userAgent);
		}

		if (StrCmpCS(usualHeaderStr, "accept"))
		{
			s32 *accept = NULL;
			s32 *all = NULL;

			accept = S32("Accept");
			all = S32("*/*");

			headers.Add((i8*) accept, (i8*) all);

			Free(accept);
		}
		
		if (StrCmpCS(usualHeaderStr, "connection"))
		{
			s32 *connection = NULL;
			s32 *closed = NULL;

			connection = S32("Connection");
			closed = S32("close");

			headers.Add((i8*) connection, (i8*) closed);

			Free(connection);
		}
		
	}
}
