#include "CoreBase.h"

NAMESPACE_BEG(core)

CORE_API void sleepms(ulong ms)
{
#if PLATFORM == PLATFORM_WIN32
	::Sleep(ms);
#else
	struct timeval tval;
	tval.tv_sec= ms / 1000;
	tval.tv_usec = (ms * 1000) % 1000000;
	select(0, NULL, NULL, NULL, &tval);
#endif
}

CORE_API char* coreStrError(int ierrorno)
{
#if PLATFORM == PLATFORM_WIN32
	if(ierrorno == 0)
		ierrorno = GetLastError();

	static char lpMsgBuf[256] = {0};
	
	__snprintf(lpMsgBuf, 256, "errorno=%d",  ierrorno);
	return lpMsgBuf;
#else
	if(ierrorno != 0)
		return strerror(ierrorno);
	return strerror(errno);
#endif
}

CORE_API int coreLastError()
{
#if PLATFORM == PLATFORM_WIN32
	return GetLastError();
#else
	return errno;
#endif
}

//--------------------------------------------------------------------------
// console color set
CORE_API void consoleSetColor(int color)
{
#ifdef _WIN32
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	WORD result = 0;
	if (color & 1) result |= FOREGROUND_RED;
	if (color & 2) result |= FOREGROUND_GREEN;
	if (color & 4) result |= FOREGROUND_BLUE;
	if (color & 8) result |= FOREGROUND_INTENSITY;
	if (color & 16) result |= BACKGROUND_RED;
	if (color & 32) result |= BACKGROUND_GREEN;
	if (color & 64) result |= BACKGROUND_BLUE;
	if (color & 128) result |= BACKGROUND_INTENSITY;
	SetConsoleTextAttribute(hConsole, (WORD)result);
#else
	int foreground = color & 7;
	int background = (color >> 4) & 7;
	int bold = color & 8;
	printf("\033[%s3%d;4%dm", bold? "01;" : "", foreground, background);
#endif
}

CORE_API void consoleResetColor()
{
#ifdef _WIN32
	consoleSetColor(7);
#else
	printf("\033[0m");
#endif
}

CORE_API void consoleCursor(int row, int col)
{
#ifdef _WIN32
	COORD point; 
	point.X = col - 1;
	point.Y = row - 1;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), point); 
#else
	printf("\033[%d;%dH", row, col);
#endif
}

CORE_API void consoleClear(int color)
{
#ifdef _WIN32
	COORD coordScreen = { 0, 0 };
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD dwConSize;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
	FillConsoleOutputCharacter(hConsole, TEXT(' '),
		dwConSize,
		coordScreen,
		&cCharsWritten);
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	FillConsoleOutputAttribute(hConsole,
		csbi.wAttributes,
		dwConSize,
		coordScreen,
		&cCharsWritten);
	SetConsoleCursorPosition(hConsole, coordScreen); 
#else
	printf("\033[2J");
#endif
}
//--------------------------------------------------------------------------

NAMESPACE_END // namespace core
