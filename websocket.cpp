#include "websocket.hpp"

WSocket::WSocket()
{
    host=NULL;
    port=-1;
}

WSocket::~WSocket()
{
    if (host)
    {
        Free(host);
        host=NULL;
    }
}

bool WSocket::Connect(s32* wurl)
{
    bool isConnected = false;

    GetUrl(wurl);
    
    if (port && host)
    {
        s32 *headers = NULL;


        DebugLog("[i] Connecting to %s on %i\n", host,port);

        socket = CreateSocket(host,port);

        if (socket.connected)
        {
            DebugLog("[+] Connected to %s on port %i", host, port);

            headers = CreateHttpHeaders();

			SendToSocket(&socket,headers,Strlen(headers));

            if (IsProtocolSwitched())
            {
                DebugLog("[+] Connected to Protocol\n");
                isConnected = true;
            } else {
                DebugLog("[+] Did not recieve header.\n");
            }

            Free(headers);
            headers=NULL;

        } else {
            DebugLog("[-] Unable to connect to %s on port %i\n", host, port);          
        }
    }

    return isConnected;
}

bool WSocket::IsProtocolSwitched()
{
    bool didConnected = false;
    int count = -1;

    do
    {
        char temp[1028] = {};

        count = RecvFromSocket(&socket, (s32*) temp, 512);

        if (count > 1)
        {
            Array *headers = NULL;
            s32* respHeader = NULL;

            headers = S32Split(temp,"\r\n");

            respHeader = (s32*) headers->Get(0);

            if (StrCmp(respHeader,"HTTP/1.1 101 Switching Protocols"))
            {
                didConnected = true;
                break;
            }

            delete headers;
        }
    } while (count > 0);

    return didConnected;
}


s32 *WSocket::Recv()
{
    bool didConnected = false;
    int count = -1;
    s32 *dataRecv = NULL;

    do
    {
        char temp[1028] = {};

        count = RecvFromSocket(&socket, (s32*) temp, 512);

        if (dataRecv == NULL)
        {
            dataRecv = S32(temp);
        } else {
            s32 *lastDataRecv = NULL;
            lastDataRecv = dataRecv;

            dataRecv = CS32Cat(2,lastDataRecv, temp );

            if (lastDataRecv)
            {
                Free(lastDataRecv);
                lastDataRecv = NULL;
            }
        }
    } while(count != 0);

    return dataRecv;
}

s32 *WSocket::CreateHttpHeaders()
{
    Map *headers = NULL;
    s32* strHeaders = NULL;
    s32 *tmpFinalHeader = NULL;
    s32 *origin = NULL;

    headers = new Map();

    origin = CS32Cat(2,"http://", host);

    headers->Add("Host", host); 
    headers->Add("User-Agent", (void*) "mozilla/5.0 (macintosh; intel mac os x 10.14; rv:66.0) gecko/20100101 firefox/66.0");   
    headers->Add( "Accept",(void*) "*/*");
    headers->Add( "Accept-Language",(void*) "en-US,en;q=0.5");
    headers->Add( "Accept-Encoding",(void*) "gzip");
    headers->Add( "Origin",(void*) origin);
    headers->Add( "Connection",(void*) "keep-alive, Upgrade");
    headers->Add( "Pragma",(void*) "no-cache");
    headers->Add( "Cache-Control",(void*) "no-cache");
    headers->Add( "Sec-Fetch-Dest",(void*) "websocket");
    headers->Add( "Sec-Fetch-Mode",(void*) "websocket");
    headers->Add( "Sec-Fetch-Site",(void*) "game-origin");
    headers->Add( "Sec-WebSocket-Extensions",(void*) "permessage-deflate");
    headers->Add( "Sec-WebSocket-Key",(void*) "gDLpjYAKUWzdQLf/xTFhKQ==");
    headers->Add( "Sec-WebSocket-Version",(void*) "13");
    headers->Add( "Upgrade",(void*) "websocket");

    strHeaders = CS32Cat(1,"GET /service HTTP/1.1");

    for (int keyIndex = 0; keyIndex < headers->keys->count; keyIndex++)
    {
        s32 *key = NULL;
        s32 *value = NULL;
        s32* temp = NULL;

        key = (s32*) headers->keys->Get(keyIndex);
        value = (s32*) headers->Get(key);

        temp = strHeaders;

        strHeaders = CS32Cat(5,temp,"\r\n", key, ":", value);

        if (temp)
        {
            Free(temp);
            temp = NULL;
        }
    }

    tmpFinalHeader = CS32Cat(2,strHeaders,"\r\n\r\n");

    if (strHeaders)
    {
        Free(strHeaders);
        strHeaders = NULL;
    }

    delete headers;
    headers = NULL;

    return tmpFinalHeader;

}

bool WSocket::CheckProtocol(s32* wurl)
{
    s32 *webProto = NULL;
    bool isProtoFound = false;
    
    if (Strlen(wurl) > 5)
    {
        
        webProto = MidString(wurl, 0,5);
               
        if (StrCmp(webProto, "ws://")) 
        {
            isProtoFound = true;
        }

        if (webProto) 
        {
            Free(webProto);
            webProto = NULL;
        } 
        
    }
    
    return isProtoFound;
}

void WSocket::GetUrl(char* wurl)
{
    
    bool success = false;
    
    if (CheckProtocol(wurl))
    {
        Array *colons = NULL;
        s32* fullURL = NULL;

        fullURL = MidString(wurl,5,Strlen(wurl));
        
        colons = S32Find(":",fullURL);


        if (colons->length == 1)
        {
            i32 *colonPosition = NULL;
            s32* portStr = NULL;

            colonPosition = colons->Get(0);

            host = MidString(fullURL,0,*colonPosition);
            portStr = MidString(fullURL, *colonPosition + 1,Strlen(fullURL));

            if (portStr)
            {
                Array *slash = NULL;

                slash = S32Find("/", portStr);
                
                if (slash->length > 0)
                {
                    i32 *slashPos = NULL;
                    s32* newPortString = NULL;

                    slashPos = slash->Get(0);
                    newPortString = MidString(portStr,0,*slashPos);
                    request = MidString(portStr,*slashPos,strlen(portStr));

                    port = SToI(newPortString);

                    Free(newPortString);
                    newPortString = NULL;

                    Free(request);
                    request=NULL;
                } else {
                    port = SToI(portStr);
                }
                Free(portStr);
                portStr = NULL;

                if (slash)
                {
                    delete slash;
                    slash = NULL;
                }
            }

        } else {
            port = 80;
        }
        
        delete colons;
        colons = NULL;

        if (fullURL)
        {
            Free(fullURL);
            fullURL = NULL;
        }
    }
    
    return ;
}


bool WSocket::Send(s32 *sentmessage)
{
	struct WEBPACKET webpacket = {};
	bool sent=false;
	us32 *packet = NULL;
	char *message = NULL;

	message = S32(sentmessage);

	//send the fin to true
	webpacket.fin = 1;
	webpacket.fin = webpacket.fin << 7;
	//set op code to 1
	webpacket.fin = webpacket.fin | 1;

	webpacket.payloadLen = 1;
	webpacket.payloadLen = webpacket.payloadLen << 7;
	webpacket.payloadLen = webpacket.payloadLen | (uchar) Strlen(message);
	memcpy(webpacket.key, "\x81\xfb\xc4\x3f", 4);
	
	packet = (us32*) MemoryRaw(sizeof(WEBPACKET) + Strlen(message)+3);

	memcpy(packet, (void*) &webpacket,sizeof(WEBPACKET));
		
	for (int i=0, xorCount=0; i < Strlen(message); i++)
	{
		message[i] = message[i] ^ webpacket.key[xorCount];

		if (xorCount == 3)
		{
			xorCount = 0;
		} else {
			xorCount++;
		}
	}
	
	memcpy(packet+sizeof(WEBPACKET)-1, message,Strlen(message));

	if  (SendToSocket(&socket,(char*) packet,sizeof(WEBPACKET) + (Strlen(message)-1)) > 0)
	{
		sent = true;
	} else {
		DebugLog("[!] Send failed.\n");
	}
	
	Free(packet);
	Free(message);

	return sent;
}
