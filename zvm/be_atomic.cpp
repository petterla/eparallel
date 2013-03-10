#include "be_atomic.h"
#ifdef	_WIN32
#include <Windows.h>
#endif/*_WIN32*/
#ifdef	_LINUX_
#include <include/asm/atomic.h>
#endif/*_LINUX_*/

namespace	be{

s64	atomic_compare_exchange64(volatile	s64*	dst,
							s64		val,	s64	cmp)
{
	s64		old;
	s32*	poldl	=	(s32*)&old;
	s32*	poldh	=	(s32*)poldl	+	1;
	s32*	pvall	=	(s32*)&val;
	s32*	pvalh	=	(s32*)pvall	+	1;
	s32*	pcmpl	=	(s32*)&cmp;
	s32*	pcmph	=	(s32*)pcmpl	+	1;
#if   (defined(_MSC_VER)   &&   _MSC_VER   >   1000) 
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
	__asm__ __volatile__("movl %0, %%eax" : :"m"(pcmpl) : );
	__asm__ __volatile__("movl %0, %%edx" : :"m"(pcmph) : );
	__asm__ __volatile__("movl %0, %%ebx" : :"m"(pvall) : );
	__asm__ __volatile__("movl %0, %%ecx" : :"m"(pvalh) : );
	__asm__ __volatile__("lock; cmpxchg8b %0" : :"m"(dst) : );
	__asm__ __volatile__("movl %%ebx, %0" : :"m"(poldl) : );
	__asm__ __volatile__("movl %%ecx, %0" : :"m"(poldh) : );
#endif/*defined(__GNUC__)*/


	return	old;
}

bool	atomic_compare_exchange64_1	(volatile	s64*	dst,
									s64	val,	s64	cmp)
{
	return	atomic_compare_exchange64(dst,	val,	cmp)	==	cmp;
}

long	atomic_compare_exchange32	(volatile	s32*	dst,
											 s32	val,	s32	cmp)
{
#ifdef	_WIN32
	return	InterlockedCompareExchange(dst,	val,	cmp);
#endif/*_WIN32*/
#if	defined(__GNUC__)
	s32		old	=	cmp;
	__asm__ __volatile__("movl %0, %%eax"::"r"(cmp):);
	__asm__ __volatile__("lock cmpxchg %1,%0"::"m"(dst), "r"(val):);
	__asm__ __volatile__("movl %%eax, %0" :"=r"(old) : : );
	return	old;
#endif/*defined(__GNUC__)*/
}

bool	atomic_compare_exchange32_1	(volatile	s32*	dst,
											 s32	val,	s32	cmp)
{
	return	atomic_compare_exchange32(dst,	val,	cmp)	==	cmp;
}

s32			atomic_exchange32	(volatile	s32*	dst,	s32	val)
{
#ifdef	_WIN32
	return	InterlockedExchange(dst,	val);
#endif/*_WIN32*/
#ifdef _LINUX_
	atomic_t*	t	=	(atomic_t*)dst;	
	atomic_set(t,	val);
	return	val;
#endif/*_LINUX_*/
}

s32			atomic_add32		(volatile	s32*	dst,	s32	val)
{
#ifdef	_WIN32
	return	InterlockedExchangeAdd(dst,	val);
#endif/*_WIN32*/
#ifdef _LINUX_
	atomic_t*	t	=	(atomic_t*)dst;	
	atomic_add(val,	t);
	return	val;
#endif/*_LINUX_*/
}

s32			atomic_increament32	(volatile	s32*	dst)
{
#ifdef	_WIN32
	return	InterlockedIncrement(dst);
#endif/*_WIN32*/
#ifdef _LINUX_
	atomic_t*	t	=	(atomic_t*)dst;	
	atomic_inc(t);
	return	*dst;
#endif/*_LINUX_*/
}

s32			atomic_decreament32	(volatile	s32*	dst)
{
#ifdef	_WIN32
	return	InterlockedDecrement(dst);
#endif/*_WIN32*/
#ifdef _LINUX_
	atomic_t*	t	=	(atomic_t*)dst;	
	atomic_dec(t);
	return	*dst;
#endif/*_LINUX_*/
}


s64			atomic_increament64	(volatile	s64*	dst)
{
#ifdef	_WIN32
	return	0;
#endif/*_WIN32*/
#ifdef _LINUX_
	atomic_t*	t	=	(atomic_t*)dst;	
	atomic_inc(t);
	return	*dst;
#endif/*_LINUX_*/
}

s64			atomic_decreament64	(volatile	s64*	dst)
{
#ifdef	_WIN32
	return	0;
#endif/*_WIN32*/
#ifdef _LINUX_
	atomic_t*	t	=	(atomic_t*)dst;	
	atomic_dec(t);
	return	*dst;
#endif/*_LINUX_*/
}

void*		atomic_compare_exchange_ptr	(volatile	void**	dst,
											 void*	val,	void*	cmp)
{
	if(sizeof(void*)	==	8){
		return	(void*)atomic_compare_exchange64((volatile s64*)(dst),
						(s64)val,	(s64)cmp);
	}else{
		return	(void*)atomic_compare_exchange32((volatile s32*)(dst),
			(s32)val,	(s32)cmp);
	}
}

s64	atomic_exchange64(volatile s64* dst, s64 val)
{
	*dst	=	val;
	return	val;
}

void*		atomic_exchange_ptr			(volatile	void**	dst,	
											 void*	val)
{
	if(sizeof(void*)	==	8){
		return	(void*)atomic_exchange64((volatile s64*)(dst),
			(s64)val);
	}else{
		return	(void*)atomic_exchange32((volatile s32*)(dst),
			(s32)val);
	}
}

s32		get_cpu_core()
{
	return	16;
}

};
