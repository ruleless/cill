#ifndef __CORE_COREBASE_H__
#define __CORE_COREBASE_H__

#include "CoreBase.h"

NAMESPACE_BEG(core)

CORE_API void sleepms(ulong ms);

CORE_API char* coreStrError(int ierrorno = 0);
CORE_API int coreLastError();

//--------------------------------------------------------------------------
// networking api
CORE_API bool getSocketAddress(const char *straddr, sockaddr_in &sockaddr);
CORE_API char* sockaddr2Str(const sockaddr_in &sockaddr, char *addr, int len);
CORE_API char* sockaddr2IpStr(const sockaddr_in &sockaddr, char *addr, int len);

CORE_API bool setNonblocking(int fd);
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// console color set
// font color
#define CONSOLE_TEXT_BLACK			0
#define CONSOLE_TEXT_RED			1
#define CONSOLE_TEXT_GREEN			2
#define CONSOLE_TEXT_YELLOW			3
#define CONSOLE_TEXT_BLUE			4
#define CONSOLE_TEXT_MAGENTA		5
#define CONSOLE_TEXT_CYAN			6
#define CONSOLE_TEXT_WHITE			7
#define CONSOLE_TEXT_BOLD			8
#define CONSOLE_TEXT_BOLD_RED		9
#define CONSOLE_TEXT_BOLD_GREEN		10
#define CONSOLE_TEXT_BOLD_YELLO		11
#define CONSOLE_TEXT_BOLD_BLUE		12
#define CONSOLE_TEXT_BOLD_MAGENTA	13
#define CONSOLE_TEXT_BOLD_CYAN		14
#define CONSOLE_TEXT_BOLD_WHITE		15

// background color
#define CONSOLE_BG_BLACK			0
#define CONSOLE_BG_RED				(1 << 4)
#define CONSOLE_BG_GREEN			(2 << 4)
#define CONSOLE_BG_YELLO			(3 << 4)
#define CONSOLE_BG_BLUE				(4 << 4)
#define CONSOLE_BG_MAGENTA			(5 << 4)
#define CONSOLE_BG_CYAN				(6 << 4)
#define CONSOLE_BG_WHITE			(7 << 4)

CORE_API void consoleSetColor(int color);
CORE_API void consoleResetColor();
CORE_API void consoleCursor(int row, int col);
CORE_API void consoleClear(int color);
//--------------------------------------------------------------------------

NAMESPACE_END // namespace core

#endif // __CORE_COREBASE_H__
