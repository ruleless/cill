#include "corebase.h"
#include "str_helper.h"

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
// networking api
CORE_API bool str2Ipv4(const char *straddr, sockaddr_in &sockaddr)
{
    std::vector<std::string> v;
    split<char>(straddr, ':', v);
    if (v.size() != 2)
        return false;

    int port = atoi(v[1].c_str());
    if (port < 0)
        return false;

    memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(port);
    if (inet_pton(AF_INET, v[0].c_str(), &sockaddr.sin_addr) != 1)
        return false;

    return true;
}

CORE_API bool isValidIpv4(const char *ip)
{
    std::vector<std::string> dotList;
    split<char>(ip, '.', dotList);
    if (dotList.size() != 4)
        return false;

    for (int i = 0; i < 4; ++i)
    {
        int num = atoi(dotList[i].c_str());
        if (num < 0 || num > 255)
            return false;
    }
    return true;
}

CORE_API bool setNonblocking(int fd)
{
    int fstatus = fcntl(fd, F_GETFL);
    if (fstatus < 0)
        return false;
    
    if (fcntl(fd, F_SETFL, fstatus|O_NONBLOCK) < 0)
        return false;

    return true;
}
//--------------------------------------------------------------------------

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
