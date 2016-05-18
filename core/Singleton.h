#ifndef __CORE_SINGLETON_H__
#define __CORE_SINGLETON_H__

#include "CoreBase.h"

template <typename T>
class Singleton
{
  protected:
	static T* mSingleton;
  public:
	Singleton(void)
	{
		assert(!mSingleton);
#if defined(_MSC_VER) && _MSC_VER < 1200
		int offset = (int)(T*)1 - (int)(Singleton <T>*)(T*)1;
		mSingleton = (T*)((int)this + offset);
#else
		mSingleton = static_cast< T* >(this);
#endif
	}

	~Singleton(void) { assert(mSingleton);  mSingleton = 0; }

	static T& getSingleton(void)
	{
		assert(mSingleton);
		return (*mSingleton);
	}
	static T* getSingletonPtr(void)
	{
		return mSingleton;
	}
};

#define SINGLETON_INIT(T) template<> T* Singleton<T>::mSingleton = 0;

#endif // __CORE_SINGLETON_H__
