#ifndef	ZVM_GC_H
#define ZVM_GC_H

#include "zvm_obj.h"

namespace zvm{

	class type_gc;

	class gc{
	public:
		static s32 add(type_gc* e);
		static s32 del(type_gc* e);
		static s32 try_collect(s32 cnt);
	};


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
			gc::add(this);
		}

		~type_gc(){
			gc::del(this);
		}

		virtual s32 recycle(stack* s){
			dec_ref();
			return	SUCCESS;
		}

		bool try_collect(stack* s){
			if(ref_count() <= 0){
				clear(s);
				return	true;
			}
			if(sign(s, true) == false 
				&& check_live(s) == false){
				clear(s);
				return	true;
			}else{
				reset(s);
			}
			return	false;
		}

		s32 set_ext(void* p){
			m_ext_data = p;
			return	SUCCESS;
		}

		void* get_ext(){
			return	m_ext_data;
		}

	protected:
		s32 m_status;
		s32	m_flag;
		s32 m_loop_cnt;
		void* m_ext_data;
	};
}

#endif/*ZVM_GC_H*/