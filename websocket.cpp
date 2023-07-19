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
                    printf("|%s| |%s|\n", request, newPortString);
                } else {
                    port = SToI(portStr);
                }
                Free(portStr);
                portStr = NULL;
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


bool WSocket::Send(s32 *message)
{

}