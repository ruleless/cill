#include "core/CillCore.h"

#include "asyncnet/AsyncNet.h"
#include "asyncnet/TraceLog.h"
#include "asyncnet/system.h"
#include "asyncnet/inetnot.h"

int main()
{
	core::createTrace();
	core::output2Console();
	core::output2Html("log.html");

	AsyncCore *asncore = asn_core_new();
	long hidListen = asn_core_new_listen(asncore, "127.0.0.1", 8001, 0);

	char buf[MAX_BUF];
	int evt = 0;
	long wparam = 0, lparam = 0, sz = 0;
	while (true)
	{
		asn_core_wait(asncore, 16);
		sz = asn_core_read(asncore, &evt, &wparam, &lparam, buf, MAX_BUF);
		switch (evt)
		{
		case ASYNC_CORE_EVT_NEW:		/* new: (hid, tag)   */
			{
				// logTrace("");
			}
			break;
		case ASYNC_CORE_EVT_LEAVE:		/* leave: (hid, tag) */
			break;
		case ASYNC_CORE_EVT_ESTAB:		/* estab: (hid, tag) */
			break;
		case ASYNC_CORE_EVT_DATA:		/* data: (hid, tag)  */
			break;
		case ASYNC_CORE_EVT_PROGRESS:	/* output progress: (hid, tag) */
			break;
		case ASYNC_CORE_EVT_PUSH:		/* msg from async_core_push */
			break;
		}
	}

	core::closeTrace();
	return 0;
}