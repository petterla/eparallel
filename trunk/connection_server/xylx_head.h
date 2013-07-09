#ifndef XYLX_HEAD_H
#define XYLX_HEAD_H

#include "ef_btype.h"

using namespace ef;

#pragma pack(push)
#pragma pack(1)
struct Header{
   int32 magic;
   int32 len;
   int32 serv;
};
#pragma pack(pop)
#endif
