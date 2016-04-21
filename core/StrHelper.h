#ifndef __CORE_STRHELPER_H__
#define __CORE_STRHELPER_H__

#include "CoreBase.h"

NAMESPACE_BEG(core)

std::string& ltrim(std::string &s);
std::string& rtrim(std::string &s);
std::string  trim(std::string s);

int replace(std::string& str, const std::string& pattern, const std::string& newpat);
int replace(std::wstring& str, const std::wstring& pattern, const std::wstring& newpat);

char* strToUpper(char* s);
char* strToLower(char* s);
std::string stringToLower(const std::string& str);
std::string stringToUpper(const std::string& str);

int coreStrICmp(const char *src, const char *dest);

bool strHasSuffix(const char *str, const char *suffix);
bool strIHasSuffix(const char *str, const char *suffix);

int bytes2string(unsigned char *pSrc, int nSrcLen, unsigned char *pDst, int nDstMaxLen);
int string2bytes(unsigned char *szSrc, unsigned char *pDst, int nDstMaxLen);

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

std::vector< std::string >
splits(const std::string& s, const std::string& delim, const bool keep_empty = true);

NAMESPACE_END // namespace core

#endif // __CORE_STRHELPER_H__
