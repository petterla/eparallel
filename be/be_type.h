#ifndef BE_TYPE_H
#define BE_TYPE_H

namespace	be{

template<int ints, int longs>
struct type_help;

template<>
struct type_help<4, 8>{
typedef int s32;
typedef unsigned int u32;
typedef long s64;
typedef unsigned long u64;
};

template<>
struct type_help<4, 4>{
typedef int s32;
typedef unsigned int u32;
typedef long long s64;
typedef unsigned long long u64;
};

typedef char			s8;
typedef unsigned char		u8;
typedef short			s16;
typedef unsigned short		u16;
typedef type_help<sizeof(int), sizeof(long)>::s32 s32;
typedef type_help<sizeof(int), sizeof(long)>::u32 u32;
typedef	type_help<sizeof(int), sizeof(long)>::s64 s64;
typedef	type_help<sizeof(int), sizeof(long)>::u64 u64;
typedef float			f32;
typedef	double			d64;

};

#endif/*BE_TYPE_H*/

