#include "StrHelper.h"

NAMESPACE_BEG(core)

std::string& ltrim(std::string &s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

std::string &rtrim(std::string &s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

std::string trim(std::string s)
{
	return ltrim(rtrim(s));
}

int replace(std::string& str, const std::string& pattern, const std::string& newpat)
{ 
	int count = 0; 
	const size_t nsize = newpat.size(); 
	const size_t psize = pattern.size(); 

	for(size_t pos = str.find(pattern, 0);
		pos != std::string::npos; 
		pos = str.find(pattern, pos+nsize))
	{ 
		str.replace(pos, psize, newpat);
		count++; 
	} 

	return count;
}

int replace(std::wstring& str, const std::wstring& pattern, const std::wstring& newpat)
{ 
	int count = 0;
	const size_t nsize = newpat.size(); 
	const size_t psize = pattern.size(); 

	for(size_t pos = str.find(pattern, 0);  
		pos != std::wstring::npos; 
		pos = str.find(pattern, pos+nsize))
	{ 
		str.replace(pos, psize, newpat); 
		count++; 
	}

	return count; 
}

char* strToUpper(char* s)
{
	assert(s != NULL);

	while(*s)
	{
		*s = toupper((unsigned char)*s);
		s++;
	};

	return s; 
}

char* strToLower(char* s)
{
	assert(s != NULL);

	while(*s)
	{
		*s = tolower((unsigned char)*s);
		s++;
	};

	return s; 
}

std::string stringToLower(const std::string& str)
{
	std::string t = str;
	std::transform(t.begin(), t.end(), t.begin(), ::tolower);
	return t;
}

std::string stringToUpper(const std::string& str)
{
	std::string t = str;
	std::transform(t.begin(), t.end(), t.begin(), ::toupper);
	return t;
}

int coreStrICmp(const char *src, const char *dest)
{
	int srclen = strlen(src), destlen = strlen(dest);
	if (srclen != destlen)
		return srclen < destlen ? -1 : 1;

	int srcch, destch;
	for (int i = 0; i < srclen; ++i)
	{
		srcch = tolower(src[i]);
		destch = tolower(dest[i]);
		if (srcch == destch)
			continue;

		return srcch < destch ? -1 : 1;
	}
	return 0;
}

bool strHasSuffix(const char *str, const char *suffix)
{
	int n = strlen(str);
	int k = strlen(suffix);
	const char *destsuffix = n > k ? str + n-k : NULL;
	if (destsuffix != NULL && strcmp(destsuffix, suffix) == 0)
	{
		return true;
	}
	return false;
}

bool strIHasSuffix(const char *str, const char *suffix)
{
	int n = strlen(str);
	int k = strlen(suffix);
	const char *destsuffix = n > k ? str + n-k : NULL;
	if (destsuffix != NULL && coreStrICmp(destsuffix, suffix) == 0)
	{
		return true;
	}
	return false;
}

int bytes2string(unsigned char *src, int srcsize, unsigned char *dst, int dstsize)
{     
	if (dst != NULL)
	{  
		*dst = 0;  
	}  

	if (src == NULL || srcsize <= 0 || dst == NULL || dstsize <= srcsize * 2)
	{  
		return 0;  
	}  

	const char szTable[] = "0123456789ABCDEF";

	for(int i=0; i<srcsize; ++i)     
	{     
		*dst++ = szTable[src[i] >> 4];     
		*dst++ = szTable[src[i] & 0x0f];   
	}     

	*dst = 0;
	return srcsize * 2;
}

int string2bytes(unsigned char* src, unsigned char* dst, int dstsize)
{  
	if(src == NULL)
		return 0;  

	int iLen = strlen((char *)src);
	if (iLen <= 0 || iLen%2 != 0 || dst == NULL || dstsize < iLen/2) 
	{  
		return 0;  
	}  

	iLen /= 2;  
	strToUpper((char *)src); 
	for (int i=0; i<iLen; ++i)  
	{  
		int iVal = 0;  
		unsigned char *pSrcTemp = src + i*2;  
		sscanf((char *)pSrcTemp, "%02x", &iVal);  
		dst[i] = (unsigned char)iVal;  
	}  

	return iLen;  
}

std::vector< std::string >
splits(const std::string& s, const std::string& delim, const bool keep_empty)
{
	std::vector< std::string > result;

	if (delim.empty())
	{
		result.push_back(s);
		return result;
	}

	std::string::const_iterator substart = s.begin(), subend;

	while (true)
	{
		subend = std::search(substart, s.end(), delim.begin(), delim.end());
		std::string temp(substart, subend);
		if (keep_empty || !temp.empty())
		{
			result.push_back(temp);
		}
		if (subend == s.end())
		{
			break;
		}
		substart = subend + delim.size();
	}

	return result;
}

NAMESPACE_END // namespace core
