#ifdef _WIN32
#include <Windows.h>
#endif/*_WIN32*/

#include "ef_atomic.h"
#include <cassert>

namespace ef{

	int64 atomic_compare_exchange64(volatile int64* dst,
			int64  val, int64 cmp)
	{
		int64  old;
#if   (defined(_MSC_VER)   &&   _MSC_VER   >   1000) 
		int32* poldl = (int32*)&old;
		int32* poldh = (int32*)poldl + 1;
		int32* pvall = (int32*)&val;
		int32* pvalh = (int32*)pvall + 1;
		int32* pcmpl = (int32*)&cmp;
		int32* pcmph = (int32*)pcmpl + 1;
		__asm{
			mov  eax,     pcmpl;//将
			mov  eax,     [eax];//将cmp低位存储到eax中
			mov  edx,     pcmph;
			mov  edx,     [edx];//将cmp高位存储到edx中
			mov  ebx,     pvall;
			mov  ebx,     [ebx];
			mov  ecx,     pvalh;
			mov  ecx,     [ecx];
			mov  edi,  dst;
			lock cmpxchg8b qword  ptr    [edi]; 
			mov  edi,  poldl;
			mov  [edi],  eax;
			mov  edi,  poldh;
			mov  [edi],  edx;
		}
#endif /*(defined(_MSC_VER)   &&   _MSC_VER   >   1000) */
#if defined(__GNUC__)
		old = __sync_val_compare_and_swap(dst, cmp, val);
#endif/*defined(__GNUC__)*/


		return old;
	}

	bool atomic_compare_exchange64_1 (volatile int64* dst,
			int64 val, int64 cmp)
	{
		return atomic_compare_exchange64(dst, val, cmp) == cmp;
	}

	int32  atomic_compare_exchange32 (volatile int32* dst,
			int32 val, int32 cmp)
	{
		int32 ret = val;
#ifdef _WIN32
		return InterlockedCompareExchange(dst, val, cmp);
#endif/*_WIN32*/
#if defined(__GNUC__)
		ret = __sync_val_compare_and_swap(dst, cmp, val);
#endif
		return ret;
	}

	bool atomic_compare_exchange32_1 (volatile int32* dst,
			int32 val, int32 cmp)
	{
		return atomic_compare_exchange32(dst, val, cmp) == cmp;
	}

	int32  atomic_exchange32 (volatile int32* dst, int32 val)
	{
#ifdef _WIN32
		return InterlockedExchange(dst, val);
#endif/*_WIN32*/
#if defined(__GNUC__)
		return __sync_lock_test_and_set(dst, val); 
#endif
	}

	int32  atomic_add32  (volatile int32* dst, int32 val)
	{
#ifdef _WIN32
		return InterlockedExchangeAdd(dst, val);
#endif/*_WIN32*/
#if defined(__GNUC__)
		return  __sync_add_and_fetch(dst, val);
#endif
	}

	int32  atomic_increment32 (volatile int32* dst)
	{
#ifdef _WIN32
		return InterlockedIncrement(dst);
#endif/*_WIN32*/
#if defined(__GNUC__)
		return __sync_add_and_fetch(dst, 1);
#endif
	}

	int32  atomic_decrement32 (volatile int32* dst)
	{
#ifdef _WIN32
		return InterlockedDecrement(dst);
#endif/*_WIN32*/
#if defined(__GNUC__)
		return  __sync_sub_and_fetch(dst, 1);
#endif
	}


	int64  atomic_increment64 (volatile int64* dst)
	{
#ifdef _WIN32
#endif/*_WIN32*/
#if defined(__GNUC__)
		return  __sync_add_and_fetch(dst, 1);
#endif/*defined(__GNUC__)*/
		return 0;
	}

	int64  atomic_decrement64 (volatile int64* dst)
	{
#ifdef _WIN32
		return 0;
#endif/*_WIN32*/
#if defined(__GNUC__)
		return __sync_sub_and_fetch(dst, 1); 
#endif/*defined(__GNUC__)*/
	}


	void* atomic_compare_exchange_ptr (volatile void** dst,
			void* val, void* cmp)
	{
#ifdef _WIN32
		if(sizeof(void*) == 8){
			return (void*)atomic_compare_exchange64((volatile int64*)(dst),
					(int64)val, (int64)cmp);
		}else{
			return (void*)atomic_compare_exchange32((volatile int32*)(dst),
					(int32)val, (int64)cmp);
		}

#endif
#ifdef __GNUC__

		return __sync_val_compare_and_swap((void**)dst, (void*)cmp, (void*)val);
#endif
	}

	int64  atomic_exchange64(volatile int64* dst, int64 val)
	{
		*dst = val;
		return val;
	}

	void* atomic_exchange_ptr  (volatile void** dst, 
			void* val)
	{
		*dst = val;
		return val;
	}

	int32  get_cpu_core()
	{
		return 16;
	}

	int32  atomic_test(){
		volatile int32 a = 0;

		assert(0 == atomic_compare_exchange32(&a, 1, 0));
		assert(1 == a);
		assert(2 == atomic_increment32(&a));
		assert(2 == a);
		assert(1 == atomic_decrement32(&a));
		assert(1 == a); 
		return 0;
	}

};
