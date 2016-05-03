#include "core/CillCore.h"

#include "asyncnet/AsyncNet.h"
#include "asyncnet/TraceLog.h"
#include "asyncnet/system.h"

using namespace core;

int main()
{
	core::createTrace(levelTrace|levelWarning|levelError|levelEmphasis);
	core::output2Console();
	core::output2Html("EchoServer.html");

	AsyncCore *asncore = asn_core_new();
	long hidListen = asn_core_new_listen(asncore, "0.0.0.0", 8010, 0);
	if (hidListen < 0)
	{
		logError("create listen node err:"<<coreStrError(coreLastError()));
		closeTrace();
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
		case ASYNC_CORE_EVT_NEW:		/* new: (hid, tag)   */
			{
				logTraceLn("new! hid="<<wparam);
			}
			break;
		case ASYNC_CORE_EVT_LEAVE:		/* leave: (hid, tag) */
			{
				logWarningLn("leave! hid="<<wparam);
			}
			break;
		case ASYNC_CORE_EVT_ESTAB:		/* estab: (hid, tag) */
			{
				logTraceLn("estabed! hid="<<wparam);
			}
			break;
		case ASYNC_CORE_EVT_DATA:		/* data: (hid, tag)  */
			{
				osb.clear();
				if (0 == asn_core_peername(asncore, wparam, peername))
				{
					osb<<"from:"<<peername<<"  ";
				}
				buf[hr] = 0;
				osb<<"say:"<<buf;
				logInfoLn(osb.c_str());
				asn_core_send(asncore, wparam, buf, hr);
			}
			break;
		case ASYNC_CORE_EVT_PROGRESS:	/* output progress: (hid, tag) */
			{
				logTraceLn("progress! hid="<<wparam);
			}
			break;
		case ASYNC_CORE_EVT_PUSH:		/* msg from async_core_push */
			{
				logTraceLn("event push!");
			}
			break;
		}
	}

	core::closeTrace();
	return 0;
}