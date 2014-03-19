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
#ifndef EF_ATOMIC_H
#define EF_ATOMIC_H


#include "ef_btype.h"

namespace ef{

	int64  atomic_compare_exchange64 (volatile int64* dst,
			int64 val, int64 cmp);
	bool atomic_compare_exchange64_1 (volatile int64* dst, 
			int64 val, int64 cmp);
	int64  atomic_exchange64   (volatile int64* dst, 
			int64 val);
	int64  atomic_add64    (volatile int64  * dst, 
			int64 val);
	//The function returns the resulting incremented value.
	int64  atomic_increment64   (volatile int64  * dst);
	//The function returns the resulting decremented value.
	int64  atomic_decrement64   (volatile int64  * dst);
	int32  atomic_compare_exchange32 (volatile int32* dst, int32 val, 
			int32 cmp);
	bool atomic_compare_exchange32_1 (volatile int32* dst, int32 val, 
			int32 cmp);
	int32  atomic_exchange32   (volatile int32* dst, int32 val);
	int32  atomic_add32    (volatile int32* dst, int32 val);
	//The function returns the resulting incremented value.
	int32  atomic_increment32   (volatile int32* dst);
	//The function returns the resulting decremented value.
	int32  atomic_decrement32   (volatile int32* dst);

	void* atomic_compare_exchange_ptr (volatile void** dst,
			void* val, void* cmp);
	void* atomic_exchange_ptr   (volatile void** dst, 
			void* val);

	int32  get_cpu_core();

	int32  atomic_test();

};



#endif/*BE_ATOMIC_H*/

