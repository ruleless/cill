#include "core/cillcore.h"

#include "asyncnet/AsyncNet.h"
#include "asyncnet/TraceLog.h"
#include "asyncnet/system.h"

using namespace core;

int main()
{
    log_initialise(AllLog);
    log_reg_console();

    AsyncCore *asncore = asn_core_new();
    long hidListen = asn_core_new_listen(asncore, "0.0.0.0", 8010, 0);
    if (hidListen < 0)
    {
        ErrorPrint("create listen node err:%s", coreStrError(coreLastError()));
        log_finalise();
        return 1;
    }

    ostrbuf osb;
    char buf[MAX_BUF], peername[MAX_BUF];
    int evt = 0;
    long wparam = 0, lparam = 0, hr = 0;
    while (true)
    {
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
            }
            break;
        case ASYNC_CORE_EVT_ESTAB:      /* estab: (hid, tag) */
            {
                InfoPrint("estabed! hid=%ld", wparam);
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
                asn_core_send(asncore, wparam, buf, hr);
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

    log_finalise();
    return 0;
}
