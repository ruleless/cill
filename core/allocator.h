#ifndef __CORE_ALLOCATOR_H__
#define __CORE_ALLOCATOR_H__

#include <assert.h>
#include <memory.h>

#include "corebase.h"

NAMESPACE_BEG(core)

class AllocatorDummy
{
  public:
    size_t init_size() const
    {
        return 0;
    }

    void* _alloc(size_t bytes, size_t& capacity)
    {
        return NULL;
    }

    void* _realloc(void* ptr, size_t old_size, size_t new_size, size_t& capacity)
    {
        return NULL;
    }

    void _free(void* ptr)
    {
    }
};

template<size_t _init_size = 256>
class AllocatorHeap
{
  public:
    size_t init_size() const
    {
        return _init_size;
    }

    void* _alloc(size_t bytes, size_t& capacity)
    {
        if (bytes == 0)
            bytes = _init_size;
        char* buf = new char[bytes];
        if (!buf)
            bytes = 0;
        capacity = bytes;
        return buf;
    }

    void* _realloc(void* ptr, size_t old_size, size_t new_size, size_t& capacity)
    {
        assert((ptr && old_size) || (!ptr && old_size == 0) && new_size > 0);
        if (!ptr || new_size > capacity)
        {
            void* buf = this->_alloc(new_size, capacity);
            if (buf && ptr && old_size > 0)
                memcpy(buf, ptr, old_size);
            if (ptr)
                this->_free(ptr);
            return buf;
        }
        return ptr;
    }

    void _free(void* ptr)
    {
        char *buf = (char *)ptr;
        delete[] buf;
    }
};

template <size_t _init_size = 256>
class AllocatorStack
{
  protected:
    enum
    {
        _buf_size = _init_size ? ((_init_size + 7) & ~7) : 8
    };
    char _buf[_buf_size];
  public:
    size_t init_size() const
    {
        return _init_size;
    }

    void* _alloc(size_t bytes, size_t& capacity)
    {
        if (bytes > _buf_size)
            bytes = 0;
        capacity = bytes;
        return bytes ? _buf : 0;
    }

    void* _realloc(void* ptr, size_t old_size, size_t new_size, size_t& capacity)
    {
        return this->_alloc(new_size, capacity);
    }

    void _free(void* ptr)
    {
    }
};

template <size_t _init_size = 256>
class AllocatorStackOrHeap : public AllocatorStack<_init_size>,
                             public AllocatorHeap<_init_size>
{
    typedef AllocatorHeap<_init_size>     heap;
    typedef AllocatorStack<_init_size>  stack;
  public:
    size_t init_size() const
    {
        return _init_size;
    }

    void* _alloc(size_t bytes, size_t& capacity)
    {
        if (bytes <= stack::_buf_size)
            return stack::_alloc(bytes, capacity);
        return heap::_alloc(bytes, capacity);

    }

    void* _realloc(void* ptr, size_t old_size, size_t new_size, size_t& capacity)
    {
        if (new_size <= stack::_buf_size)
        {
            if (old_size <= stack::_buf_size)
                return stack::_realloc(ptr, old_size, new_size, capacity);
            else
            {
                void* buf = stack::_alloc(new_size, capacity);
                if (buf && ptr && old_size > 0)
                    memcpy(buf, ptr, new_size);
                this->_free(ptr);
                return buf;
            }
        }
        else if (ptr == stack::_buf)
        {
            void* buf = heap::_alloc(new_size, capacity);
            if (buf && old_size > 0)
                memcpy(buf, stack::_buf, old_size);
            return buf;
        }
        else
            return heap::_realloc(ptr, old_size, new_size, capacity);
    }

    void _free(void* ptr)
    {
        if (ptr != stack::_buf)
            heap::_free(ptr);
    }
};

template<class Allocator>
class AllocatorEx : public Allocator
{
    typedef Allocator base;
  public:
    void* _alloc(size_t bytes, size_t& capacity)
    {
        size_t align_size = bytes ? ((bytes + 7) & ~7) : 8;
        return base::_alloc(align_size, capacity);
    }

    void* _realloc(void* ptr, size_t old_size, size_t new_size, size_t& capacity)
    {
        if (!ptr || new_size > capacity)
        {
            size_t good_size = capacity ? capacity : new_size;
            while (good_size < new_size)
                good_size += (good_size >> 1);

            return base::_realloc(ptr, old_size, good_size, capacity);
        }
        return ptr;
    }
};

typedef AllocatorStackOrHeap<32>          AlctStackOrHeap32;
typedef AllocatorEx<AlctStackOrHeap32>    AlctEx32;

typedef AllocatorStackOrHeap<64>          AlctStackOrHeap64;
typedef AllocatorEx<AlctStackOrHeap64>    AlctEx64;

typedef AllocatorStackOrHeap<128>         AlctStackOrHeap128;
typedef AllocatorEx<AlctStackOrHeap128>   AlctEx128;

typedef AllocatorStackOrHeap<256>         AlctStackOrHeap256;
typedef AllocatorEx<AlctStackOrHeap256>   AlctEx256;

typedef AllocatorStackOrHeap<512>         AlctStackOrHeap512;
typedef AllocatorEx<AlctStackOrHeap512>   AlctEx512;

typedef AllocatorStackOrHeap<1024>        AlctStackOrHeap1024;
typedef AllocatorEx<AlctStackOrHeap1024>  AlctEx1024;

typedef AllocatorStackOrHeap<2048>        AlctStackOrHeap2048;
typedef AllocatorEx<AlctStackOrHeap2048>  AlctEx2048;

typedef AllocatorStackOrHeap<4096>        AlctStackOrHeap4096;
typedef AllocatorEx<AlctStackOrHeap4096>  AlctEx4096;

typedef AllocatorStackOrHeap<8192>        AlctStackOrHeap8192;
typedef AllocatorEx<AlctStackOrHeap8192>  AlctEx8192;

NAMESPACE_END // namespace core

#endif // __CORE_ALLOCATOR_H__
