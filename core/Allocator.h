#ifndef __CORE_ALLOCATOR_H__
#define __CORE_ALLOCATOR_H__

#include <assert.h>
#include <memory.h>

NAMESPACE_BEG(core)
class BufferAllocatorDummy
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
class BufferAllocatorHeap
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
class BufferAllocatorStack
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
class BufferAllocatorStackOrHeap : public BufferAllocatorStack<_init_size>,
								   public BufferAllocatorHeap<_init_size>
{
	typedef BufferAllocatorHeap<_init_size>	  heap;
	typedef BufferAllocatorStack<_init_size>  stack;
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
class BufferAllocatorEx : public Allocator
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
NAMESPACE_END // namespace core

#endif // __CORE_ALLOCATOR_H__
