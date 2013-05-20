#ifndef ZVM_TYPE_H
#define ZVM_TYPE_H

#include <cassert>
#include <stddef.h>
#include <stdio.h>
#include "be_type.h"

namespace zvm{

	typedef be::u64  u64;

	typedef be::s64  s64;

	typedef be::u32  u32;

	typedef be::s32  s32;

	typedef be::u16  u16;

	typedef be::s16  s16;

	typedef be::u8   u8;

	typedef be::s8   s8;

	typedef be::f32  f32;

	typedef be::d64  f64;

	enum{
		LOCAL_TYPE_INT = 0,
		LOCAL_TYPE_FLOAT,
		LOCAL_TYPE_OBJ,
	};

	struct local_t{
		s64 m_local;
		s32 m_type;
	};
#define ZVM_DEBUG
#ifdef ZVM_DEBUG
	#define ZVM_DEBUG_PRINT(...)	printf(__VA_ARGS__)
#else
	#define ZVM_DEBUG_PRINT(...)     ((void)0)
#endif
};


#endif/**/

