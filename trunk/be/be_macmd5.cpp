#include "be_md5.h"
#include "be_macmd5.h"


namespace be{

void Md5Val(u8 * input, u32 inputLen, u8 output[16])
{
	MD5_CTX ctx;

	MD5Init(&ctx);
	MD5Update(&ctx, (u8 *)input, inputLen);
	MD5Final((u8 *)output, &ctx);

	//memcpy(output, ctx.digest, 16);

	return;
}

}
