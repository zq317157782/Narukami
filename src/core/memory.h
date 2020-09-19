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
#include "math.h"
#include <list>
#include <vector>
#include "stat.h"

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
	return reinterpret_cast<T*>(alloc_aligned(size * sizeof(T)));
}

template<typename T,int LINE_SIZE>
inline T *alloc_aligned(size_t size){
	return reinterpret_cast<T*>(alloc_aligned<LINE_SIZE>(size * sizeof(T)));
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

#if defined(NARUKAMI_IS_WIN)
inline void * memcpy(void* const dst,const void* const src,size_t size)
{
	return std::memcpy(dst,src,size);
}
#endif

template<typename T>
inline T* memcpy_t(T* const dst,const T* const src,size_t size)
{
	return static_cast<T*>(memcpy(dst,src,size * sizeof(T)));
}

#define STACK_ALLOC(T,count) (T*)alloca(count*sizeof(T))

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

//memory arena from pbrt
#define ARENA_ALLOC(arena, Type) new ((arena).alloc(sizeof(Type))) Type
class MemoryArena{
	private:
		const size_t _block_size;
		uint8_t* _current_block;
		size_t _current_block_pos;
		size_t _current_alloc_size;
		std::list<std::pair<size_t,uint8_t*>> _used,_available;
	public:
		//default 256kb
		MemoryArena(const size_t block_size=262144):_block_size(block_size),_current_block(nullptr),_current_block_pos(0),_current_alloc_size(0){}
		
		void* alloc(size_t sz){
			//16 byte align
			sz=(sz+15)&(~15);

			if(_current_block_pos+sz > _current_alloc_size){
				if(_current_block){
					_used.push_back({_current_alloc_size,_current_block});
					_current_block = nullptr;
				}

				for (auto i = _available.begin(); i!= _available.end(); i++)
				{
					if(i->first>=sz){
						_current_alloc_size = i->first;
						_current_block = i->second;
						_available.erase(i);
						break;
					}
				}
				
				if(!_current_block){
					_current_alloc_size=max(_block_size,sz);
					_current_block = alloc_aligned<uint8_t>(_current_alloc_size);
				}

				_current_block_pos=0;
			}

			auto ret=_current_block+_current_block_pos;
			_current_block_pos+=sz;
			return ret;
		}

		template<typename T>
		T* alloc(size_t sz=1,bool run_ctor=true){
			auto ret=reinterpret_cast<T*>(alloc(sizeof(T)*sz));
			if(run_ctor){
				for (size_t i = 0; i < sz; ++i)
				{
					new (&ret[i]) T();
				}
			}
			return ret;
		}

		void reset(){
			_available.splice(_available.begin(),_used);
			_current_block_pos=0;
		}

		~MemoryArena(){
			for (auto &i : _used )
			{
				free_aligned(i.second);
			}
			for (auto &i : _available )
			{
				free_aligned(i.second);
			}
			
		}

};

STAT_COUNTER("memory/total memory pool alloc count",memory_pool_alloc_count)
STAT_MEMORY_COUNTER("memory/total memory pool alloc",memory_pool_alloc)
STAT_MEMORY_COUNTER("memory/total memory pool dealloc",memory_pool_dealloc)
template<typename T,int LINE_SIZE = NARUKAMI_L1_CACHE_LINE>
class MemoryPool
{
	private:
		struct Node
		{
			Node * next;
		};
		Node* _head = nullptr;
		std::vector<T*> _chuncks;
		const int32_t _chunck_element_num;
	public:
	
	MemoryPool(int32_t chunck_element_num = 20):_chunck_element_num(chunck_element_num){assert(sizeof(T)>=sizeof(Node));}
	~MemoryPool()
	{
		empty();
	}
	T* alloc()
	{
		if(_head == nullptr)
		{
			//no free node
			STAT_INCREASE_MEMORY_COUNTER(memory_pool_alloc, _chunck_element_num)
			T* chunck = alloc_aligned<T,LINE_SIZE>(_chunck_element_num);
			_chuncks.push_back(chunck);

			//insert from tail to head
			for(int32_t i= (_chunck_element_num - 1);i >= 0; --i)
			{
				Node* next = _head;
				_head = reinterpret_cast<Node*>(&chunck[i]);
				_head->next = next;
			}
		}

		T* ret = reinterpret_cast<T*>(_head);
		_head = _head->next;
		STAT_INCREASE_COUNTER(memory_pool_alloc_count, 1)
		return ret;
	}

	void dealloc(T* ptr)
	{
		if(ptr == nullptr)
		{
			return;
		}
		Node* next = _head;
		_head = reinterpret_cast<Node*>(ptr);
		_head->next = next;
	}

	void empty()
	{
		for(uint32_t i = 0;i<_chuncks.size();++i)
		{
			STAT_INCREASE_MEMORY_COUNTER(memory_pool_dealloc, _chunck_element_num)
			free_aligned(_chuncks[i]);
		}
		_chuncks.empty();
	}
};

NARUKAMI_END

// void* operator new(size_t sz);
// void operator delete(void* ptr);