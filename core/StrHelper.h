#ifndef __CORE_STRHELPER_H__
#define __CORE_STRHELPER_H__

#include "CoreBase.h"

NAMESPACE_BEG(core)

CORE_API std::string& ltrim(std::string &s);

CORE_API std::string& rtrim(std::string &s);
CORE_API std::string  trim(std::string s);

CORE_API int replace(std::string& str, const std::string& pattern, const std::string& newpat);
CORE_API int replace(std::wstring& str, const std::wstring& pattern, const std::wstring& newpat);

CORE_API char* strToUpper(char* s);
CORE_API char* strToLower(char* s);
CORE_API std::string stringToLower(const std::string& str);
CORE_API std::string stringToUpper(const std::string& str);

CORE_API int coreStrICmp(const char *src, const char *dest);

CORE_API bool strHasSuffix(const char *str, const char *suffix);
CORE_API bool strIHasSuffix(const char *str, const char *suffix);

CORE_API int bytes2string(unsigned char *pSrc, int nSrcLen, unsigned char *pDst, int nDstMaxLen);
CORE_API int string2bytes(unsigned char *szSrc, unsigned char *pDst, int nDstMaxLen);

template<typename T>
inline void split(const std::basic_string< T >& s, T c, std::vector< std::basic_string< T > > &v)
{
    if(s.size() == 0)
        return;

    typename std::basic_string< T >::size_type i = 0;
    typename std::basic_string< T >::size_type j = s.find(c);

    while(j != std::basic_string<T>::npos)
    {
        std::basic_string<T> buf = s.substr(i, j - i);

        if(buf.size() > 0)
            v.push_back(buf);

        i = ++j; j = s.find(c, j);
    }

    if(j == std::basic_string<T>::npos)
    {
        std::basic_string<T> buf = s.substr(i, s.length() - i);
        if(buf.size() > 0)
            v.push_back(buf);
    }
}

CORE_API std::vector< std::string >
splits(const std::string& s, const std::string& delim, const bool keep_empty = true);

NAMESPACE_END // namespace core

#endif // __CORE_STRHELPER_H__
