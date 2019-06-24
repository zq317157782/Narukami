/*
MIT License

Copyright (c) 2019 ZhuQian

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once
#include "narukami.h"
#include "platform.h"
#include "sse.h"

NARUKAMI_BEGIN

#ifndef NARUKAMI_L1_CACHE_LINE
	#define NARUKAMI_L1_CACHE_LINE 64 //default
#endif



template <int LINE_SIZE>
inline void *alloc_aligned(size_t size){
#ifdef NARUKAMI_IS_OSX
	void* ptr;
	if(posix_memalign(&ptr,LINE_SIZE,size)!=0){
		ptr=nullptr;
	}
	return ptr;
#elif defined(NARUKAMI_IS_WIN) && defined(_MSC_VER)
	return _aligned_malloc(size, LINE_SIZE);
#else
	void* ptr;
	if(posix_memalign(&ptr,LINE_SIZE,size)!=0){
		ptr=nullptr;
	}
	return ptr;
#endif
}

//L1 aligned
inline void *alloc_aligned(size_t size){
	return alloc_aligned<NARUKAMI_L1_CACHE_LINE>(size);
}

template<typename T>
inline T *alloc_aligned(size_t size){
	return alloc_aligned(size * sizeof(T));
}

inline void free_aligned(void * ptr){
	if(!ptr){
		return;
	}
#ifdef NARUKAMI_IS_OSX
	free(ptr);
#elif defined(NARUKAMI_IS_WIN) && defined(_MSC_VER)
	_aligned_free(ptr);
#else
	free(ptr);
#endif
}


inline void * memcpy(void* const dst,const void* const src,size_t size){
	return std::memcpy(dst,src,size);
}


#define STACK_ALLOC(T,count) (T*)alloca(count*sizeof(T))

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template <class T>
struct SSEAllocator {
  using value_type = T;
  SSEAllocator() noexcept{};
  template <class U> SSEAllocator (const SSEAllocator<U>&) noexcept{};
  T* allocate (std::size_t n){return reinterpret_cast<T*>(alloc_aligned<SSE_LINE_SIZE>(n*sizeof(T)));}
  void deallocate (T* p, std::size_t n){free_aligned(p);}
};

// template <class T, class U>
// constexpr bool operator== (const SSEAllocator<T>&, const SSEAllocator<U>&) noexcept{return true;}

// template <class T, class U>
// constexpr bool operator!= (const SSEAllocator<T>&, const SSEAllocator<U>&) noexcept{return false;}

//memory arena from pbrt

NARUKAMI_END

void* operator new(size_t sz);
void operator delete(void* ptr);