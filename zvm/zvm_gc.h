#ifndef	ZVM_GC_H
#define ZVM_GC_H

#include "zvm_obj.h"

namespace zvm{
	class type_gc:public entry{
	public:
		enum{
			STATUS_NULL = -1,
			STATUS_SIGN = 0,
			STATUS_CHECK = 1,
			STATUS_CLONE = 2,
			STATUS_CLEAR = 3,
			STATUS_RESET = 4,
		};

		enum{
			FLAG_NULL = 0,
			FLAG_LOOP = 1,
		};



		type_gc()
			:m_status(STATUS_NULL),
			m_flag(FLAG_NULL),
			m_loop_cnt(0)

		{

		}

		virtual s32 recycle(stack* s){
			dec_ref();
			return	SUCCESS;
		}

		s32	try_collect(stack* s){
			if(ref_count() <= 0){
				return	clear(s);
			}
			if(sign(s, true) == false 
				&& check_live(s) == false){
				return	clear(s);
			}else{
				reset(s);
			}
			return	SUCCESS;
		}

	protected:
		s32 m_status;
		s32	m_flag;
		s32 m_loop_cnt;
	};
}

#endif/*ZVM_GC_H*/