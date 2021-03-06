#ifndef __CORE_BASE_H__
#define __CORE_BASE_H__

//--------------------------------------------------------------------------
// common include   
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h> 
#include <math.h>
#include <assert.h> 
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>  
#include <cstring>  
#include <vector>
#include <map>
#include <list>
#include <set>
#include <deque>
#include <queue>
#include <limits>
#include <algorithm>
#include <utility>
#include <functional>
#include <cctype>
#include <iterator>

#include "log/log_inc.h"
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// windows include  
#if defined( __WIN32__ ) || defined( WIN32 ) || defined( _WIN32 )

#ifndef WINVER
#   define WINVER 0x0501
#endif
#ifndef _WIN32_WINNT
#   define _WIN32_WINNT 0x0501
#endif
#ifndef _WIN32_WINDOWS
#   define _WIN32_WINDOWS 0x0410
#endif

#pragma warning(disable:4996)
#pragma warning(disable:4819)
#pragma warning(disable:4049)
#pragma warning(disable:4217)

#include <io.h>
#include <time.h> 
// # define FD_SETSIZE 1024
#ifndef WIN32_LEAN_AND_MEAN 
#   include <winsock2.h> // 必须在windows.h之前包含， 否则网络模块编译会出错
#   include <mswsock.h> 
#   define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <process.h>

#if _MSC_VER >= 1500
#   include <unordered_map>
#endif

#include <functional>
#include <memory>
#define _SCL_SECURE_NO_WARNINGS

#else // #if defined( __WIN32__ ) || defined( WIN32 ) || defined( _WIN32 )

// linux include
#include <errno.h>
#include <float.h>
#include <pthread.h>    
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <iconv.h>
#include <langinfo.h>
#include <stdint.h>
#include <signal.h>
#include <dirent.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/tcp.h> 
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <tr1/unordered_map>
#include <tr1/functional>
#include <tr1/memory>
#include <linux/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/resource.h>
#include <linux/errqueue.h>

#endif // #if defined( __WIN32__ ) || defined( WIN32 ) || defined( _WIN32 )

#include <signal.h>

#if !defined( _WIN32 )
# include <pwd.h>
#else
# define SIGHUP 1
# define SIGINT 2
# define SIGQUIT 3
# define SIGUSR1 10
# define SIGPIPE 13
# define SIGSYS 32
#endif
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// 字节序
#ifndef LITTLE_ENDIAN
# define LITTLE_ENDIAN  0
#endif
#ifndef BIG_ENDIAN
# define BIG_ENDIAN     1
#endif
#if !defined(ENDIAN)
# if defined (USE_BIG_ENDIAN)
#  define ENDIAN BIG_ENDIAN
# else 
#  define ENDIAN LITTLE_ENDIAN
# endif 
#endif
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// 平台定义
#define PLATFORM_WIN32 0
#define PLATFORM_UNIX  1
#define PLATFORM_APPLE 2
#define PLATFORM_INTEL 3

#define UNIX_FLAVOUR_LINUX  1
#define UNIX_FLAVOUR_BSD    2
#define UNIX_FLAVOUR_OTHER  3
#define UNIX_FLAVOUR_OSX    4

#if defined( __WIN32__ ) || defined( WIN32 ) || defined( _WIN32 )
# define PLATFORM PLATFORM_WIN32
#elif defined( __INTEL_COMPILER )
# define PLATFORM PLATFORM_INTEL
#elif defined( __APPLE_CC__ )
# define PLATFORM PLATFORM_APPLE
#else
# define PLATFORM PLATFORM_UNIX
#endif

#if PLATFORM == PLATFORM_UNIX || PLATFORM == PLATFORM_APPLE
# ifdef HAVE_DARWIN
#  define PLATFORM_TEXT "MacOSX"
#  define UNIX_FLAVOUR UNIX_FLAVOUR_OSX
# else
#  ifdef USE_KQUEUE
#   define PLATFORM_TEXT "FreeBSD"
#   define UNIX_FLAVOUR UNIX_FLAVOUR_BSD
#  else
#   ifdef USE_KQUEUE_DFLY
#    define PLATFORM_TEXT "DragonFlyBSD"
#    define UNIX_FLAVOUR UNIX_FLAVOUR_BSD
#   else
#    define PLATFORM_TEXT "Linux"
#    define UNIX_FLAVOUR UNIX_FLAVOUR_LINUX
#   endif
#  endif
# endif
#elif PLATFORM == PLATFORM_WIN32
# define PLATFORM_TEXT "Win32"
#endif
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// 编译器定义
#define COMPILER_MICROSOFT 0
#define COMPILER_GNU       1
#define COMPILER_BORLAND   2
#define COMPILER_INTEL     3
#define COMPILER_CLANG     4

#ifdef _MSC_VER
# define COMPILER COMPILER_MICROSOFT
#elif defined( __INTEL_COMPILER )
# define COMPILER COMPILER_INTEL
#elif defined( __BORLANDC__ )
# define COMPILER COMPILER_BORLAND
#elif defined( __GNUC__ )
# define COMPILER COMPILER_GNU
#elif defined( __clang__ )
# define COMPILER COMPILER_CLANG
#else
# pragma error "FATAL ERROR: Unknown compiler."
#endif
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// 宏定义
#ifndef NAME_PATH
# define NAME_PATH 255
#endif

#ifndef NAME_MAX
# define NAME_MAX 255
#endif

#ifndef MAX_NAME
# define MAX_NAME 256
#endif

#ifndef MAX_IP
# define MAX_IP 50
#endif

#ifndef MAX_BUF
# define MAX_BUF 256
#endif

#ifndef MAX_PATH
# define MAX_PATH 260
#endif

#define NAMESPACE_BEG(spaceName) namespace spaceName {
#define NAMESPACE_END }

#ifndef max
# define max(a, b) ((b) > (a) ? (b) : (a))
#endif
#ifndef min
# define min(a, b) ((b) < (a) ? (b) : (a))
#endif

// #define LIMIT_FREQUENCY

// 导入导出定义
#if PLATFORM == PLATFORM_WIN32
# ifdef _LIB
#  define CLASS_EXPORT
#  define CLASS_IMPORT
#  define API_EXPORT
#  define API_IMPORT
#  define VAR_EXPORT
#  define VAR_IMPORT
# else
#  define CLASS_EXPORT      __declspec(dllexport)
#  define CLASS_IMPORT      __declspec(dllimport)
#  define API_EXPORT        __declspec(dllexport)
#  define API_IMPORT        __declspec(dllimport)
#  define VAR_EXPORT        __declspec(dllexport)
#  define VAR_IMPORT        __declspec(dllimport)
# endif
#else
# define CLASS_EXPORT
# define CLASS_IMPORT
# define API_EXPORT
# define API_IMPORT
# define VAR_EXPORT
# define VAR_IMPORT
#endif

#ifdef _CORE_DLL_EXPORT
# define CORE_CLASS     CLASS_EXPORT
# define CORE_API       API_EXPORT
# define CORE_VAR       VAR_EXPORT
#else
# define CORE_CLASS     CLASS_IMPORT
# define CORE_API       API_IMPORT
# define CORE_VAR       VAR_IMPORT
#endif
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// 类型定义
NAMESPACE_BEG(core)

typedef char                    mchar;
typedef wchar_t                 wchar;
typedef std::string             mstring;
typedef std::wstring            wstring;

#ifdef _UNICODE
typedef wchar                   tchar;
typedef wstring                 tstring;
#else
typedef mchar                   tchar;
typedef mstring                 tstring;
#endif

typedef unsigned char           uchar;
typedef unsigned short          ushort;
typedef unsigned int            uint;
typedef unsigned long           ulong;

#if COMPILER != COMPILER_GNU
typedef signed __int64          int64;
typedef signed __int32          int32;
typedef signed __int16          int16;
typedef signed __int8           int8;
typedef unsigned __int64        uint64;
typedef unsigned __int32        uint32;
typedef unsigned __int16        uint16;
typedef unsigned __int8         uint8;
typedef INT_PTR                 intptr;
typedef UINT_PTR                uintptr;
#else // #if COMPILER != COMPILER_GNU
typedef int64_t                 int64;
typedef int32_t                 int32;
typedef int16_t                 int16;
typedef int8_t                  int8;
typedef uint64_t                uint64;
typedef uint32_t                uint32;
typedef uint16_t                uint16;
typedef uint8_t                 uint8;
typedef uint16_t                WORD;
typedef uint32_t                DWORD;

#ifdef _LP64
typedef int64                   intptr;
typedef uint64                  uintptr;
#else // #ifdef LP64
typedef int32                   intptr;
typedef uint32                  uintptr;
#endif // #ifdef LP64

#endif // #if COMPILER != COMPILER_GNU

NAMESPACE_END // namespace core

#if _MSC_VER >= 1500
#define UnorderedMap            std::tr1::unordered_map
#else
#define UnorderedMap            std::map
#endif

#if PLATFORM == PLATFORM_WIN32
#define IFNAMSIZ                16
typedef UINT_PTR                SOCKET;
#ifndef socklen_t
typedef int                     socklen_t;
#endif
typedef unsigned short          u_int16_t;
typedef unsigned long           u_int32_t;

#ifndef IFF_UP
enum
{
    IFF_UP                  = 0x1,
    IFF_BROADCAST           = 0x2,
    IFF_DEBUG               = 0x4,
    IFF_LOOPBACK            = 0x8,
    IFF_POINTOPOINT         = 0x10,
    IFF_NOTRAILERS          = 0x20,
    IFF_RUNNING             = 0x40,
    IFF_NOARP               = 0x80,
    IFF_PROMISC             = 0x100,
    IFF_MULTICAST           = 0x1000
};
#endif // #ifndef IFF_UP
#else // #if PLATFORM == PLATFORM_WIN32
typedef int                 SOCKET;
#endif // #if PLATFORM == PLATFORM_WIN32

// 输出格式宏定义
#if COMPILER != COMPILER_GNU
# define PRI64          "lld"
# define PRIu64         "llu"
# define PRIx64         "llx"
# define PRIX64         "llX"
# define PRTime         PRI64
# define PRIzu          "lu"
# define PRIzd          "ld"
#else // #if COMPILER != COMPILER_GNU
# ifdef _LP64
#  ifndef PRI64
#   define PRI64        "ld"
#  endif
#  ifndef PRIu64
#   define PRIu64       "lu"
#  endif
#  ifndef PRIx64
#   define PRIx64       "lx"
#  endif
#  ifndef PRIX64
#   define PRIX64       "lX"
#  endif
#  ifndef PRTime
#   define PRTime       PRI64
#  endif
# else // #ifdef _LP64
#  ifndef PRI64
#   define PRI64        "lld"
#  endif
#  ifndef PRIu64
#   define PRIu64       "llu"
#  endif
#  ifndef PRIx64
#   define PRIx64       "llx"
#  endif
#  ifndef PRIX64
#   define PRIX64       "llX"
#  endif
#  ifndef PRTime
#   define PRTime       "ld"
#  endif
# endif // #ifdef _LP64
# ifndef PRIzd
#  define PRIzd         "zd"
# endif
# ifndef PRIzu
#  define PRIzu         "zu"
# endif
#endif // #if COMPILER != COMPILER_GNU
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// 常用字符串函数定义
#if defined (unix)
# define __isnan isnan
# define __isinf isinf
# define __snprintf snprintf
# define __vsnprintf vsnprintf
# define __vsnwprintf vsnwprintf
# define __snwprintf swprintf
# define __stricmp strcasecmp
# define __strnicmp strncasecmp
# define __fileno fileno
# ifndef __va_copy
#  define __va_copy va_copy
# endif

# define __strtoq strtoq
# define __strtouq strtouq
# define __strtoll strtoll
# define __strtoull strtoull
# define __atoll atoll
#else // #if defined (unix)
# define __isnan _isnan
# define __isinf(x) (!_finite(x) && !_isnan(x))
# define __snprintf _snprintf
# define __vsnprintf _vsnprintf
# define __vsnwprintf _vsnwprintf
# define __snwprintf _snwprintf
# define __stricmp _stricmp
# define __strnicmp _strnicmp
# define __fileno _fileno
# define __va_copy(dst, src) dst = src

# define __strtoq _strtoi64
# define __strtouq _strtoui64
# define __strtoll _strtoi64
# define __strtoull _strtoui64
# define __atoll _atoi64
#endif // #if defined (unix)
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// 线程定义
#if PLATFORM == PLATFORM_WIN32
# define THREAD_ID                          HANDLE
# define THREAD_SINGNAL                     HANDLE
# define THREAD_SINGNAL_INIT(x)             x = CreateEvent(NULL, TRUE, FALSE, NULL)
# define THREAD_SINGNAL_DELETE(x)           CloseHandle(x)
# define THREAD_SINGNAL_SET(x)              SetEvent(x)
# define THREAD_MUTEX                       CRITICAL_SECTION
# define THREAD_MUTEX_INIT(x)               InitializeCriticalSection(&x)
# define THREAD_MUTEX_DELETE(x)             DeleteCriticalSection(&x)
# define THREAD_MUTEX_LOCK(x)               EnterCriticalSection(&x)
# define THREAD_MUTEX_UNLOCK(x)             LeaveCriticalSection(&x)    
#else
# define THREAD_ID                          pthread_t
# define THREAD_SINGNAL                     pthread_cond_t
# define THREAD_SINGNAL_INIT(x)             pthread_cond_init(&x, NULL)
# define THREAD_SINGNAL_DELETE(x)           pthread_cond_destroy(&x)
# define THREAD_SINGNAL_SET(x)              pthread_cond_signal(&x);
# define THREAD_MUTEX                       pthread_mutex_t
# define THREAD_MUTEX_INIT(x)               pthread_mutex_init (&x, NULL)
# define THREAD_MUTEX_DELETE(x)             pthread_mutex_destroy(&x)
# define THREAD_MUTEX_LOCK(x)               pthread_mutex_lock(&x)
# define THREAD_MUTEX_UNLOCK(x)             pthread_mutex_unlock(&x)        
#endif
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// memory management
#ifdef _USE_KMEM
# include "kmem/imembase.h"
# define malloc ikmem_malloc
# define realloc ikmem_realloc
# define free ikmem_free

# if PLATFORM == PLATFORM_WIN32

inline void* __cdecl operator new(size_t size) { return malloc(size); }
inline void* __cdecl operator new[](size_t size) { return malloc(size); }
 
inline void __cdecl operator delete(void* ptr) { free(ptr); }
inline void __cdecl operator delete[](void* ptr) { free(ptr); }

# else // #if PLATFORM == PLATFORM_WIN32

inline void* operator new(size_t size) throw(std::bad_alloc) { return malloc(size); }
inline void* operator new[](size_t size) throw(std::bad_alloc) { return malloc(size); }

inline void operator delete(void* ptr) throw() { free(ptr); }
inline void operator delete[](void* ptr) throw() { free(ptr); }

# endif // #if PLATFORM == PLATFORM_WIN32
#endif // #ifdef _USE_KMEM
//--------------------------------------------------------------------------

#endif // __CORE_BASE_H__
