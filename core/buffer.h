#ifndef __CORE_BUFFER_H__
#define __CORE_BUFFER_H__

#include <assert.h>
#include <memory.h>
#include <string>
#include "allocator.h"

NAMESPACE_BEG(core)

class BasicBuffer
{
  public:
    enum BufState
    {
        good = 0, eof  = 1, fail = 2,
    };
  protected:
    char*   _data;  // 存储区
    size_t  _cap;   // 最大容量
    size_t  _cur;   // 当前大小
    int     _state; // 当前状态
  public:
    BasicBuffer() : _data(0), _cap(0), _cur(0), _state(good)
    {
    }

    BasicBuffer(char* buf, size_t maxsize) : _data(buf), _cap(maxsize), _cur(0), _state(good)
    {
        assert(buf != 0 && maxsize > 0);
    }

    ~BasicBuffer() {}

    char* data() const
    {
        return _data;
    }

    void set_data(char* d)
    {
        _data = d;
    }

    size_t size() const
    {
        return _cur;
    }

    size_t cap() const
    {
        return _cap;
    }

    void cap(size_t cap)
    {
        _cap = cap;
    }

    int state() const
    {
        return _state;
    }

    char* begin() const
    {
        return _data;
    }

    char* current() const
    {
        return _data + _cur;
    }

    char* end() const
    {
        return _data + _cap;
    }

    char* pos(size_t n) const
    {
        if (n <= _cap)
            return _data + n;
        return 0;
    }

    size_t space() const
    {
        return right();
    }

    bool operator!() const
    {
        return !is_good();
    }

    operator bool() const
    {
        return is_good();
    }

    bool is_good() const
    {
        return _state <= eof;
    }

    bool is_eof() const
    {
        return _state == eof;
    }

    bool is_fail() const
    {
        return _state == fail;
    }

    void reset()
    {
        _cur   = 0;
        _state = good;
    }

    void offset(int n)
    {
        n >= 0 ? inc((size_t)n) : dec((size_t)-n);
    }
  protected:
    void clear_ex()
    {
        _data  = 0;
        _cur   = 0;
        _cap   = 0;
        _state = good;
    }

    void state(int state)
    {
        _state = state;
    }

    void check_eof()
    {
        if (_cur == 0 || _cur == _cap)
            state(eof);
    }

    size_t left() const
    {
        return _cur;
    }

    size_t right() const
    {
        return _cap - _cur;
    }

    void inc(size_t n)
    {
        if (n <= right())
            _cur += n;
        else
            state(fail);
        check_eof();
    }

    void dec(size_t n)
    {
        if (n <= left())
            _cur -= n;
        else
            state(fail);
        check_eof();
    }

    void copy(const void* buf, size_t bytes)
    {
        if (_data && buf && bytes <= right())
        {
            if (bytes > 0)
                memcpy(current(), buf, bytes);
            inc(bytes);
            check_eof();
        }
        else
        {
            state(fail);
        }
    }
};

template <class Allocator>
class OutBuffer : public BasicBuffer, public Allocator
{
    typedef BasicBuffer            base;
    typedef Allocator              alct;
    typedef OutBuffer<Allocator>   obuffer;
  public:
    OutBuffer()
    {
        base::_data = (char*)alct::_alloc(alct::init_size(), base::_cap);
    }

    OutBuffer(char* buf, size_t maxsize) : base(buf, maxsize)
    {
    }

    OutBuffer(const obuffer& buf)
    {
        assign(buf);
    }

    ~OutBuffer()
    {
        clear();
    }

    void clear()
    {
        if (base::data())
            alct::_free(base::data());
        base::clear_ex();
    }

    obuffer& operator=(const obuffer& buf)
    {
        return (assign(buf));
    }

    obuffer& assign(const obuffer& buf)
    {
        if (!base::data())
            base::_data = (char*)alct::_alloc(alct::init_size(), base::_cap);

        base::reset();
        if (buf.size() > 0)
        {
            push_back(buf.data(), buf.size());
        }
        return (*this);
    }

    obuffer& push_back(const void* buf, size_t bytes)
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

    template <class T>
    obuffer& operator<< (T value)
    {
        if (sizeof(T) > base::right())
        {
            char* ret = (char*)alct::_realloc(base::_data, base::_cur, base::_cur + sizeof(T), base::_cap);
            if (!ret)
            {
                base::state(base::fail);
                return (*this);
            }
            base::set_data(ret);
        }

        if (base::current() && sizeof(T) <= right())
        {
            *(T*)base::current() = value;
            base::inc(sizeof(T));
            base::check_eof();
        }
        else
        {
            base::state(base::fail);
        }
        return (*this);
    }

    obuffer& operator<< (const char* value)
    {
        if (value)
            return this->push_back((const void*)value, strlen(value) + sizeof(char));
        base::state(base::fail);
        return *this;
    }

    obuffer& operator<< (char* value)
    {
        return this->operator<<(((const char*)value));
    }

    obuffer& operator<< (const wchar_t* value)
    {
        if (value)
            return this->push_back((const void*)value, (wcslen(value) + 1) * sizeof(wchar_t));
        base::state(base::fail);
        return *this;
    }

    obuffer& operator<< (wchar_t* value)
    {
        return this->operator<<((const wchar_t*)value);
    }

    obuffer& operator<< (const std::string& value)
    {
        return this->operator<<(value.c_str());
    }

    obuffer& operator<< (std::string& value)
    {
        return this->operator<<(value.c_str());
    }

    obuffer& operator<< (const std::wstring& value)
    {
        return this->operator<<(value.c_str());
    }

    obuffer& operator<< (std::wstring& value)
    {
        return this->operator<<(value.c_str());
    }

    template <class T> obuffer& skip()
    {
        base::inc(sizeof(T));
        return (*this);
    }
};

class InBuffer : public BasicBuffer
{
    typedef BasicBuffer  base;
  protected:
    InBuffer() : BasicBuffer()
    {
    }
  public:
    InBuffer(const void* buffer, size_t size) : BasicBuffer((char*)buffer, size)
    {
    }

    ~InBuffer()
    {
    }

    size_t size() const
    {
        return cap();
    }

    template <class T>
    InBuffer& operator>> (T& value)
    {
        if (sizeof(T) <= base::right())
        {
            value = *(T*)base::current();
            base::inc(sizeof(T));
        }
        else
        {
            base::state(base::fail);
        }
        return (*this);
    }

    InBuffer& operator>> (const char*& value)
    {
        char* str = base::current();
        while (str < base::end() && *str++);
        size_t bytes = (size_t)(str - base::current());
        if (bytes > 0 && bytes <= base::right())
        {
            if (*(base::current() + bytes - sizeof(char)) != 0)
            {
                base::state(base::eof | base::fail);
                return (*this);
            }
            value = base::current();
            base::inc(bytes);
        }
        else
        {
            value = 0;
            base::state(base::eof | base::fail);
        }

        return (*this);
    }

    InBuffer& operator>> (char*& value)
    {
        return operator>>((const char*&)value);
    }

    InBuffer& operator>> (const wchar_t*& value)
    {
        wchar_t* str = (wchar_t*)base::current();
        wchar_t* end = (wchar_t*)base::end();
        while (str < end && *str++);

        size_t bytes = (size_t)((char*)str - base::current());
        if (bytes > 0 && bytes <= base::right())
        {
            if (*(const wchar_t*)(base::current() + bytes - sizeof(wchar_t)) != 0)
            {
                base::state(base::eof|base::fail);
                return (*this);
            }
            value = (wchar_t*)base::current();
            base::inc(bytes);
        }
        else
        {
            value = 0;
            base::state(base::eof|base::fail);
        }

        return (*this);
    }

    InBuffer& operator>> (wchar_t*& value)
    {
        return operator>>((const wchar_t*&)value);
    }

    InBuffer& operator>> (std::string& value)
    {
        value.clear();
        const char* str = 0;
        operator>>((const char*&)str);
        if (str)
            value = str;

        return (*this);
    }

    InBuffer& operator>> (std::wstring& value)
    {
        value.clear();
        const wchar_t* str = 0;
        operator>>((const wchar_t*&)str);
        if (str)
            value = str;

        return (*this);
    }

    template <class T>
    InBuffer& skip()
    {
        base::inc(sizeof(T));
        return (*this);
    }

    InBuffer& pop(void* buffer, size_t bytes)
    {
        if (buffer == 0 || bytes == 0)
        {
            base::state(base::fail);
            return (*this);
        }

        if (bytes <= base::right())
        {
            memcpy(buffer, base::current(), bytes);
            base::inc((bytes));
        }
        else
        {
            base::state(base::eof | base::fail);
        }

        return (*this);
    }
};

typedef OutBuffer<AlctEx32>               obuf32;
typedef OutBuffer<AlctEx64>               obuf64;
typedef OutBuffer<AlctEx128>              obuf128;
typedef OutBuffer<AlctEx256>              obuf256, obuf;
typedef OutBuffer<AlctEx512>              obuf512;
typedef OutBuffer<AlctEx1024>             obuf1024;
typedef OutBuffer<AlctEx2048>             obuf2048;
typedef OutBuffer<AlctEx4096>             obuf4096;
typedef OutBuffer<AlctEx8192>             obuf8192;
typedef OutBuffer<AllocatorDummy>         ofixbuf;

NAMESPACE_END // namespace core

#endif // __CORE_BUFFER_H__
