#ifndef ZVM_TYPE_H
#define ZVM_TYPE_H

#include <cassert>
#include <stddef.h>

namespace zvm{

	typedef unsigned long long  u64;

	typedef long long           s64;

	typedef unsigned int        u32;

	typedef int                 s32;

	typedef unsigned short int  u16;

	typedef short int           s16;

	typedef unsigned char       u8;

	typedef char                s8;

	typedef	float				f32;

	typedef double				f64;

	enum{
		LOCAL_TYPE_INT = 0,
		LOCAL_TYPE_FLOAT,
		LOCAL_TYPE_OBJ,
	};

	struct local_t{
		s64 m_local;
		s32 m_type;
	};

};


#endif/**/