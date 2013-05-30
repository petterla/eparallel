/*
 * Copyright (c) 2004-2005 Sergey Lyubka <valenok@gmail.com>
 * All rights reserved
 *
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Sergey Lyubka wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 */
 /****************************************************************
 *
 * Thanks for Sergey Lynbka, I hope I could meet U and buy U a beer 
 *
 * ***************************************************************/

#ifndef BE_MD5_H 
#define	BE_MD5_H

#include "be_type.h"

namespace be{

typedef struct MD5Context {
	u32	buf[4];
	u32	bits[2];
	u8	in[64];
} MD5_CTX;

void MD5Init(MD5_CTX *ctx);
void MD5Update(MD5_CTX *ctx, u8 const *buf, u32 len);
void MD5Final(u8 digest[16], MD5_CTX *ctx);

s32 file_md5 (s8 const * fname, u8 * md5val);

}

#endif /*MD5_HEADER_INCLUDED */
