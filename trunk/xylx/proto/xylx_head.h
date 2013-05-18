#ifndef XYLX_HEAD_H
#define XYLX_HEAD_H

#include "ef_btype.h"

struct Header{
   int32 magic;
   int32 len;
   int32 cmd;
   int64 session;
   int32 seq;    
}

#endif
