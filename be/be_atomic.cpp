#ifdef	_WIN32
#include <Windows.h>
#endif/*_WIN32*/

#include "be_atomic.h"
#include <cassert>

namespace	be{

    s64	atomic_compare_exchange64(volatile	s64*	dst,
        s64		val,	s64	cmp)
    {
        s64		old;
#if   (defined(_MSC_VER)   &&   _MSC_VER   >   1000) 
        s32*	poldl	=	(s32*)&old;
        s32*	poldh	=	(s32*)poldl	+	1;
        s32*	pvall	=	(s32*)&val;
        s32*	pvalh	=	(s32*)pvall	+	1;
        s32*	pcmpl	=	(s32*)&cmp;
        s32*	pcmph	=	(s32*)pcmpl	+	1;
        __asm{
            mov		eax,	   	pcmpl;//将
            mov		eax,	   	[eax];//将cmp低位存储到eax中
            mov		edx,	   	pcmph;
            mov		edx,	   	[edx];//将cmp高位存储到edx中
            mov		ebx,	   	pvall;
            mov		ebx,	   	[ebx];
            mov		ecx,	   	pvalh;
            mov		ecx,	   	[ecx];
            mov		edi,		dst;
            lock	cmpxchg8b	qword  ptr    [edi]; 
            mov		edi,		poldl;
            mov		[edi],		eax;
            mov		edi,		poldh;
            mov		[edi],		edx;
        }
#endif	/*(defined(_MSC_VER)   &&   _MSC_VER   >   1000) */
#if	defined(__GNUC__)
	old = __sync_val_compare_and_swap(dst, cmp, val);
#endif/*defined(__GNUC__)*/


        return	old;
    }

    bool	atomic_compare_exchange64_1	(volatile	s64*	dst,
        s64	val,	s64	cmp)
    {
        return	atomic_compare_exchange64(dst,	val,	cmp)	==	cmp;
    }

    s32		atomic_compare_exchange32	(volatile	s32*	dst,
        s32	val,	s32	cmp)
    {
	s32 ret = val;
#ifdef	_WIN32
        return	InterlockedCompareExchange(dst,	val,	cmp);
#endif/*_WIN32*/
#if	defined(__GNUC__)
	ret = __sync_val_compare_and_swap(dst, cmp, val);
#endif
	return	ret;
    }

    bool	atomic_compare_exchange32_1	(volatile	s32*	dst,
        s32	val,	s32	cmp)
    {
        return	atomic_compare_exchange32(dst,	val,	cmp)	==	cmp;
    }

    s32		atomic_exchange32	(volatile	s32*	dst,	s32	val)
    {
#ifdef	_WIN32
        return	InterlockedExchange(dst,	val);
#endif/*_WIN32*/
#if	defined(__GNUC__)
	return	__sync_lock_test_and_set(dst, val); 
#endif
    }

    s32		atomic_add32		(volatile	s32*	dst,	s32	val)
    {
#ifdef	_WIN32
        return	InterlockedExchangeAdd(dst,	val);
#endif/*_WIN32*/
#if	defined(__GNUC__)
	return  __sync_add_and_fetch(dst, val);
#endif
    }

    s32		atomic_increment32	(volatile	s32*	dst)
    {
#ifdef	_WIN32
        return	InterlockedIncrement(dst);
#endif/*_WIN32*/
#if	defined(__GNUC__)
	return	__sync_add_and_fetch(dst, 1);
#endif
    }

    s32		atomic_decrement32	(volatile	s32*	dst)
    {
#ifdef	_WIN32
        return	InterlockedDecrement(dst);
#endif/*_WIN32*/
#if	defined(__GNUC__)
	return  __sync_sub_and_fetch(dst, 1);
#endif
    }


    s64		atomic_increment64	(volatile	s64*	dst)
    {
#ifdef	_WIN32
#endif/*_WIN32*/
#if	defined(__GNUC__)
	return  __sync_add_and_fetch(dst, 1);
#endif/*defined(__GNUC__)*/
		return	0;
    }

    s64		atomic_decrement64	(volatile	s64*	dst)
    {
#ifdef	_WIN32
        return	0;
#endif/*_WIN32*/
#if	defined(__GNUC__)
		return	__sync_sub_and_fetch(dst, 1);	
#endif/*defined(__GNUC__)*/
    }


    void*	atomic_compare_exchange_ptr	(volatile	void**	dst,
        void*	val,	void*	cmp)
    {
#ifdef _WIN32
        if(sizeof(void*)	==	8){
            return	(void*)atomic_compare_exchange64((volatile s64*)(dst),
                (s64)val, (s64)cmp);
        }else{
            return	(void*)atomic_compare_exchange32((volatile s32*)(dst),
                (s32)val, (s64)cmp);
        }

#endif
#ifdef __GNUC__

	return __sync_val_compare_and_swap((void**)dst, (void*)cmp, (void*)val);
#endif
    }

    s64		atomic_exchange64(volatile s64* dst, s64 val)
    {
        *dst	=	val;
        return	val;
    }

    void*	atomic_exchange_ptr		(volatile	void**	dst,	
        void*	val)
    {
        *dst	=	val;
        return	val;
    }

    s32		get_cpu_core()
    {
        return	16;
    }

    s32		atomic_test(){
	volatile s32 a = 0;

	assert(0 == atomic_compare_exchange32(&a, 1, 0));
	assert(1 == a);
	assert(2 == atomic_increment32(&a));
	assert(2 == a);
	assert(1 == atomic_decrement32(&a));
	assert(1 == a);	
	return	0;
    }

};
