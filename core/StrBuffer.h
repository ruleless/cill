#ifndef __CORE_STRBUFFER_H__
#define __CORE_STRBUFFER_H__

#include "CoreBase.h"
#include "Buffer.h"

NAMESPACE_BEG(core)

template <class Allocator>
class StrBuffer : public BasicBuffer, public Allocator
{
	typedef BasicBuffer            base;
	typedef Allocator              alct;
	typedef StrBuffer<Allocator>   ostrbuffer;
  public:
	StrBuffer()
	{
		base::_data = (char *)alct::_alloc(alct::init_size(), base::_cap);
	}

	StrBuffer(char* buf, size_t maxsize) : base(buf, maxsize)
	{
	}

	StrBuffer(const ostrbuffer& buf)
	{
		assign(buf);
	}

	~StrBuffer()
	{
		clear();
	}

	void clear()
	{
		if (base::data())
			alct::_free(base::data());
		base::clear_ex();
	}

	const char* c_str() const
	{
		return base::_data;
	}

	ostrbuffer& operator=(const ostrbuffer& buf)
	{
		return (assign(buf));
	}

	ostrbuffer& assign(const ostrbuffer& buf)
	{
		if (!base::data())
			base::_data = (char *)alct::_alloc(alct::init_size(), base::_cap);

		base::reset();
		if (buf.size() > 0)
		{
			push_back(buf.data(), buf.size());
		}
		return (*this);
	}

	ostrbuffer& push_back(const void* buf, size_t bytes)
	{
		if (buf == 0 || bytes == 0)
		{
			base::state(base::fail);
			return (*this);
		}

		if (bytes > base::right())
		{
			char* ret = (char*)alct::_realloc(base::_data, base::_cur, base::_cur + bytes, base::_cap);
			if (!ret)
			{
				base::state(base::fail);
				return (*this);
			}
			base::set_data(ret);
		}

		base::copy(buf, bytes);

		return (*this);
	}

	ostrbuffer& operator<< (int64 n)
	{
		char num[32] = {0};
		__snprintf(num, sizeof(num), "%"PRI64, n);
		return this->operator<<((const char *)num);
	}
	
	ostrbuffer& operator<< (uint64 n)
	{
		char num[32] = {0};
		__snprintf(num, sizeof(num), "%"PRIu64, n);
		return this->operator<<((const char *)num);
	}
	
	ostrbuffer& operator<< (int n)
	{
		char num[32] = {0};
		__snprintf(num, sizeof(num), "%d", n);
		return this->operator<<((const char *)num);
	}
	
	ostrbuffer& operator<< (uint n)
	{
		char num[32] = {0};
		__snprintf(num, sizeof(num), "%u", n);
		return this->operator<<((const char *)num);
	}
	
	ostrbuffer& operator<< (short n)
	{
		char num[32] = {0};
		__snprintf(num, sizeof(num), "%d", n);
		return this->operator<<((const char *)num);
	}
	
	ostrbuffer& operator<< (ushort n)
	{
		char num[32] = {0};
		__snprintf(num, sizeof(num), "%u", n);
		return this->operator<<((const char *)num);
	}

	ostrbuffer& operator<< (void *p)
	{
		char num[32] = {0};
		__snprintf(num, sizeof(num), "%p", p);
		return this->operator<<((const char *)num);
	}

	ostrbuffer& operator<< (const char* value)
	{
		this->trim_eof();

		if (value)
			return this->push_back((const void*)value, strlen(value) + sizeof(char));;
		base::state(base::fail);
		return *this;
	}

	ostrbuffer& operator<< (char* value)
	{
		return this->operator<<((const char*)value);
	}

	ostrbuffer& operator<< (const wchar_t* value)
	{
		trim_eof();

		if (value)
			return this->push_back((const void*)value, (wcslen(value) + 1) * sizeof(wchar_t));
		base::state(base::fail);
		return *this;
	}

	ostrbuffer& operator<< (wchar_t* value)
	{
		return this->operator<<((const wchar_t*)value);
	}

	ostrbuffer& operator<< (const std::string& value)
	{
		return this->operator<<(value.c_str());
	}

	ostrbuffer& operator<< (std::string& value)
	{
		return this->operator<<(value.c_str());
	}

	ostrbuffer& operator<< (const std::wstring& value)
	{
		return this->operator<<(value.c_str());
	}

	ostrbuffer& operator<< (std::wstring& value)
	{
		return this->operator<<(value.c_str());
	}

	void trim_eof()
	{
		while (base::_data && base::_cur > 0 && *(base::_data+base::_cur-1) == '\0')
		{
			base::dec(1);
		}
	}
};

typedef StrBuffer<AlctEx32>        ostrbuf32;
typedef StrBuffer<AlctEx64>        ostrbuf64;
typedef StrBuffer<AlctEx128>       ostrbuf128;
typedef StrBuffer<AlctEx256>       ostrbuf256, ostrbuf;
typedef StrBuffer<AlctEx512>       ostrbuf512;
typedef StrBuffer<AlctEx1024>      ostrbuf1024;
typedef StrBuffer<AlctEx2048>      ostrbuf2048;
typedef StrBuffer<AlctEx4096>      ostrbuf4096;
typedef StrBuffer<AlctEx8192>      ostrbuf8192;

NAMESPACE_END // namespace core

#endif // __CORE_STRBUFFER_H__
