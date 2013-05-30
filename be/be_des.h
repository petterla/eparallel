
#ifndef BE_DES_H
#define BE_DES_H

#include "be_type.h"


namespace be{

#define DESE_SELFTEST_FAILED  -100
#define DESE_WRONG_KEYLEN     -101
#define DESE_WEAK_KEY         -102


/* Encryption/Decryption context of DES */

typedef struct _des_ctx {

    u32  encrypt_subkeys[32];
    u32  decrypt_subkeys[32];

} DesCtx;


/* Encryption/Decryption context of Triple-DES */

typedef struct _tripledes_ctx {

    u32  encrypt_subkeys[96];
    u32  decrypt_subkeys[96];

} TripleDesCtx;



void des_key_schedule    (u8        *, u32 *);
s32  des_setkey          (DesCtx       *, u8 *);
s32  des_ecb_crypt       (DesCtx       *, u8 *, u8 *, s32);
s32  tripledes_set2keys  (TripleDesCtx *, u8 *, u8 *);
s32  tripledes_set3keys  (TripleDesCtx *, u8 *, u8 *, u8 *);
s32  tripledes_ecb_crypt (TripleDesCtx *, u8 *, u8 *, s32);
s32  is_weak_key         (u8        *);


s32 DesSetKey  (void * ctx, u8 * key);
s32 DesEncrypt (void * ctx, u8 * inbuf, s32 len, u8 * outbuf, s32 * poutlen);
s32 DesDecrypt (void * ctx, u8 * inbuf, s32 len, u8 * outbuf, s32 * poutlen);

s32 Des3Set2Key (TripleDesCtx * ctx, u8 * key, u32 keylen );
s32 Des3Set3Key (TripleDesCtx * ctx, u8 * key, u32 keylen );
s32 Des3Encrypt (TripleDesCtx * ctx, u8 * inbuf, s32 len, u8 * outbuf, s32 * poutlen);
s32 Des3Decrypt (TripleDesCtx * ctx, u8 * inbuf, s32 len, u8 * outbuf, s32 * poutlen);


const s8 * selftest            (void);
s32          do_tripledes_setkey (struct _tripledes_ctx *, u8 *, u32 );
void         do_tripledes_encrypt(struct _tripledes_ctx *, u8 *, u8 *);
void         do_tripledes_decrypt(struct _tripledes_ctx *, u8 *, u8 *);

const s8 * des_get_info( 
				size_t * keylen,
				size_t * blocksize, 
				size_t * contextsize,
				s32	(**setkeyf)  ( void *c, u8 *key, u32 keylen ),
				void (**encryptf)( void *c, u8 *outbuf, u8 *inbuf ),
				void (**decryptf)( void *c, u8 *outbuf, u8 *inbuf )
		     );

/*
 * Handy macros for encryption and decryption of data
 */
#define des_ecb_encrypt(ctx, from, to)		des_ecb_crypt(ctx, from, to, 0)
#define des_ecb_decrypt(ctx, from, to)		des_ecb_crypt(ctx, from, to, 1)
#define tripledes_ecb_encrypt(ctx, from, to)	tripledes_ecb_crypt(ctx, from, to, 0)
#define tripledes_ecb_decrypt(ctx, from, to)	tripledes_ecb_crypt(ctx, from, to, 1)

}

#endif /*_DES_H_*/

