/*
* Copyright (c) 2010
* efgod@126.com.
*
* Permission to use, copy, modify, distribute and sell this software
* and its documentation for any purpose is hereby granted without fee,
* provided that the above copyright notice appear in all copies and
* that both that copyright notice and this permission notice appear
* in supporting documentation.  efgod@126.com. makes no
* representations about the suitability of this software for any
* purpose.  It is provided "as is" without express or implied warranty.
*/
#ifndef BE_ATOMIC_H
#define BE_ATOMIC_H


#include "be_type.h"

namespace	be{

s64		atomic_compare_exchange64	(volatile	s64*	dst,
											 s64	val,	s64	cmp);
bool	atomic_compare_exchange64_1	(volatile	s64*	dst,	
											 s64	val,	s64	cmp);
s64		atomic_exchange64			(volatile	s64*	dst,	
											 s64	val);
s64		atomic_add64				(volatile	s64		*	dst,	
											 s64	val);
s64		atomic_increament64			(volatile	s64		*	dst);
s64		atomic_decreament64			(volatile	s64		*	dst);
s32		atomic_compare_exchange32	(volatile	s32*	dst,	s32	val,	
											 s32	cmp);
bool	atomic_compare_exchange32_1	(volatile	s32*	dst,	s32	val,	
											 s32	cmp);
s32		atomic_exchange32			(volatile	s32*	dst,	s32	val);
s32		atomic_add32				(volatile	s32*	dst,	s32	val);
s32		atomic_increament32			(volatile	s32*	dst);
s32		atomic_decreament32			(volatile	s32*	dst);

void*	atomic_compare_exchange_ptr	(volatile	void**	dst,
											void*	val,	void*	cmp);
void*	atomic_exchange_ptr			(volatile	void**	dst,	
											void*	val);

s32		get_cpu_core();
};

#endif/*BE_ATOMIC_H*/