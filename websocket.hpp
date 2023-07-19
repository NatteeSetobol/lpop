#ifndef __WEBSOCKET__
#define __WEBSOCKET__

#include "../lpop/required/nix.hpp"
#include "../lpop/required/memory.hpp"
#include "../lpop/required/platform.hpp"
#include "../lpop/required/assert.hpp"
#include "../lpop/bucket.hpp"
#include "../lpop/array.hpp"
#include "../lpop/strings.hpp"
#include "../lpop/httpprotocol.hpp"
#include "../lpop/curled.h"

class WSocket
{
    public:
        WSocket();
        ~WSocket();
        bool Connect(s32* wurl);
        bool Send(s32 *message);
        bool CheckProtocol(s32* wurl);
        void GetUrl(char *wurl);
        s32 *CreateHttpHeaders();
        bool IsProtocolSwitched();
    private:
        struct platform_socket socket;
        s32* host;
        i32 port;
        s32* request;
};

#endif