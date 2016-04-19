#ifndef __CORE_CILLDEFINE_H__
#define __CORE_CILLDEFINE_H__

//--------------------------------------------------------------------------
// 宏定义
#ifndef NAME_PATH
#	define NAME_PATH 255
#endif

#ifndef NAME_MAX
#	define NAME_MAX 255
#endif

#ifndef MAX_NAME
#	define MAX_NAME 256
#endif

#ifndef MAX_IP
#	define MAX_IP 50
#endif

#ifndef MAX_BUF
#	define MAX_BUF 256
#endif

#ifndef MAX_PATH
#	define MAX_PATH 260
#endif

#define NAMESPACE_BEG(spaceName) namespace spaceName {
#define NAMESPACE_END }
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// 常用字符串函数定义
#if defined (unix)
#	define __isnan isnan
#	define __isinf isinf
#	define __snprintf snprintf
#	define __vsnprintf vsnprintf
#	define __vsnwprintf vsnwprintf
#	define __snwprintf swprintf
#	define __stricmp strcasecmp
#	define __strnicmp strncasecmp
#	define __fileno fileno
#	ifndef __va_copy
#		define __va_copy va_copy
#	endif

#	define __strtoq strtoq
#	define __strtouq strtouq
#	define __strtoll strtoll
#	define __strtoull strtoull
#	define __atoll atoll
#else // unix
#	define __isnan _isnan
#	define __isinf(x) (!_finite(x) && !_isnan(x))
#	define __snprintf _snprintf
#	define __vsnprintf _vsnprintf
#	define __vsnwprintf _vsnwprintf
#	define __snwprintf _snwprintf
#	define __stricmp _stricmp
#	define __strnicmp _strnicmp
#	define __fileno _fileno
#	define __va_copy(dst, src) dst = src

#	define __strtoq _strtoi64
#	define __strtouq _strtoui64
#	define __strtoll _strtoi64
#	define __strtoull _strtoui64
#	define __atoll _atoi64
#endif // unix
//--------------------------------------------------------------------------

#endif // __CORE_CILLDEFINE_H__
