#include "core/CillCore.h"

#include "asyncnet/AsyncNet.h"
#include "asyncnet/TraceLog.h"
#include "asyncnet/system.h"

using namespace core;

int main(int argc, char *argv[])
{
	core::createTrace();
	core::output2Console();
	core::output2Html("EchoClient.html");

	std::string ip;
	int port = -1;
	if (argc > 1)
	{
		std::vector<std::string> v = core::splits(argv[1], ":");
		if (v.size() != 2)
		{
			logError("socket adress format err!");
			closeTrace();
			return 1;
		}
		port = atoi(v[1].c_str());
		if (port < 0)
		{
			logError("Invalid port!");
			closeTrace();
			return 1;
		}
		ip = v[0];
	}
	else
	{
		logWarningLn("no remote address assgined!use 127.0.0.0:8001");
		ip = "127.0.0.1";
		port = 8001;
	}

	AsyncCore *asncore = asn_core_new();
	long hid = asn_core_new_connect(asncore, ip.c_str(), port, 0);
	if (hid < 0)
	{
		logError("create connection err:"<<coreStrError(coreLastError()));
		closeTrace();
		return 1;
	}

	static const char *s_echomsg = "Hello World!";

	ostrbuf osb;
	char buf[MAX_BUF], peername[MAX_BUF];
	int evt = 0;
	long wparam = 0, lparam = 0, hr = 0;
	ulong exsendtime = 0;
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
		case ASYNC_CORE_EVT_NEW:		/* new: (hid, tag)   */
			{
				logTraceLn("new! hid="<<wparam);
			}
			break;
		case ASYNC_CORE_EVT_LEAVE:		/* leave: (hid, tag) */
			{
				logWarningLn("leave! hid="<<wparam);
				bloop = false;
			}
			break;
		case ASYNC_CORE_EVT_ESTAB:		/* estab: (hid, tag) */
			{
				logTraceLn("estabed! hid="<<wparam);
				asn_core_send(asncore, wparam, s_echomsg, strlen(s_echomsg));
				exsendtime = getTickCount();
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

	logWarningLn("End of EchoClient!");
	core::closeTrace();
	getchar();
	return 0;
}