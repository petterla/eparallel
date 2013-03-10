#ifndef ZVM_ERR_NO_H
#define ZVM_ERR_NO_H

namespace	zvm{

	enum{
		SUCCESS = 0,
		RET_ATHROW = -1,
		RET_UNKNOW_OP = -2,
		UNKNOW_EXCP = -10,
		OVER_FLOW = -10001,
		DIV_ZERO = -10002,
		INVALID_SYNC = -10003,
		LOCK_NULL_OBJ = -10004,
		OUT_OF_MEM = -10005,
	};

}

#endif/*ZVM_ERR_NO_H*/