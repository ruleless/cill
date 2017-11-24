#include "core/cillcore.h"

#include "asyncnet/AsyncNet.h"
#include "asyncnet/TraceLog.h"
#include "asyncnet/system.h"

using namespace core;

int main(int argc, char *argv[])
{
    log_initialise(AllLog);
    log_reg_console();

    std::string ip;
    int port = -1;
    if (argc > 1)
    {
        std::vector<std::string> v = core::splits(argv[1], ":");
        if (v.size() != 2)
        {
            ErrorPrint("socket adress format err!");
            log_finalise();
            return 1;
        }
        port = atoi(v[1].c_str());
        if (port < 0)
        {
            ErrorPrint("Invalid port!");
            log_finalise();
            return 1;
        }
        ip = v[0];
    }
    else
    {
        WarningPrint("no remote address assgined!use 127.0.0.1:8010");
        ip = "127.0.0.1";
        port = 8010;
    }

    AsyncCore *asncore = asn_core_new();
    long hid = asn_core_new_connect(asncore, ip.c_str(), port, 0);
    if (hid < 0)
    {
        ErrorPrint("create connection err:%s", coreStrError(coreLastError()));
        log_finalise();
        return 1;
    }

    static const char *s_echomsg = "Hello World!";

    ostrbuf osb;
    char buf[MAX_BUF], peername[MAX_BUF];
    int evt = 0;
    long wparam = 0, lparam = 0, hr = 0;
    unsigned long exsendtime = 0;
    bool bloop = true;
    while (bloop)
    {
        if (exsendtime > 0 && getTickCount() > exsendtime+2000)
        {
            asn_core_send(asncore, hid, s_echomsg, strlen(s_echomsg));
            exsendtime = getTickCount();
        }
        asn_core_wait(asncore, 16);
        hr = asn_core_read(asncore, &evt, &wparam, &lparam, buf, MAX_BUF-1);
        if (hr < 0)
            continue;

        switch (evt)
        {
        case ASYNC_CORE_EVT_NEW:        /* new: (hid, tag)   */
            {
                InfoPrint("new! hid=%ld", wparam);
            }
            break;
        case ASYNC_CORE_EVT_LEAVE:      /* leave: (hid, tag) */
            {
                WarningPrint("leave! hid=%ld", wparam);
                bloop = false;
            }
            break;
        case ASYNC_CORE_EVT_ESTAB:      /* estab: (hid, tag) */
            {
                InfoPrint("estabed! hid=%ld", wparam);
                asn_core_send(asncore, wparam, s_echomsg, strlen(s_echomsg));
                exsendtime = getTickCount();
            }
            break;
        case ASYNC_CORE_EVT_DATA:       /* data: (hid, tag)  */
            {
                osb.clear();
                if (0 == asn_core_peername(asncore, wparam, peername))
                {
                    osb<<"from:"<<peername<<"  ";
                }
                buf[hr] = 0;
                osb<<"say:"<<buf;
                InfoPrint(osb.c_str());
            }
            break;
        case ASYNC_CORE_EVT_PROGRESS:   /* output progress: (hid, tag) */
            {
                InfoPrint("progress! hid=%ld", wparam);
            }
            break;
        case ASYNC_CORE_EVT_PUSH:       /* msg from async_core_push */
            {
                InfoPrint("event push!");
            }
            break;
        }
    }

    WarningPrint("End of EchoClient!");

    log_finalise();
    getchar();
    return 0;
}
