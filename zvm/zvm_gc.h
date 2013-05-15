#ifndef	ZVM_GC_H
#define ZVM_GC_H

#include "zvm_obj.h"

namespace zvm{


	class gc_type;

	class gc{
	public:
		static s32 add(gc_type* e);
		static s32 del(gc_type* e);
		static s32 try_collect(s32 cnt);
	};


	class gc_type:public entry{
	public:
		enum{
			STATUS_NULL = 0,
			STATUS_SIGN = 1,
			STATUS_CHECK = 2,
			STATUS_CLONE = 3,
			STATUS_CLEAR = 4,
			STATUS_RESET = 5,
		};

		enum{
			FLAG_NULL = 0,
			FLAG_LOOP = 1,
		};



		gc_type()
			:m_loop_cnt(0),
			m_ext_data(NULL)
		{
			*(s32*)m_flgs = 0;
			gc::add(this);
		}

		~gc_type(){
			gc::del(this);
		}


		s32 status(){
			return	m_flgs[0];
		}

		s32 set_status(s32 sts){
			m_flgs[0] = (s8)sts;
			return	sts;
		}

		s32 flag(){
			return	m_flgs[1];
		}

		s32	set_flag(s32 flg){
			m_flgs[1] = (s8)flg;
			return	flg;
		}

		virtual	entry* reference(stack* s){
			inc_ref();
			ZVM_DEBUG_PRINT("node:%p reference,ref_cnt:%d!\n", 
				this, ref_count());
			set_need_check(false);
			return	this;
		}

		virtual s32 recycle(stack* s){
			dec_ref();
			ZVM_DEBUG_PRINT("node:%p recycle,ref_cnt:%d!\n", 
				this, ref_count());
			set_need_check(true);
			return	SUCCESS;
		}

		bool try_collect(stack* s){

			if(!need_check()){
				ZVM_DEBUG_PRINT("node:%p not need check!\n", this);
				return	false;
			}

			reset(s, true);
			if(ref_count() <= 0){
				clear(s);
				return	true;
			}
			if(find_loop(s, this, true) == true 
				&& check_live(s) == false){
				clear(s);
				return	true;
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

		virtual bool find_loop(stack* s, entry* e,bool first);

		virtual bool check_live(stack* s);

		virtual bool reset(stack* s, bool force);


		//get first refer obj
		virtual void begin(stack* s){
	
		}

		//get nest refer obj
		virtual obj* get_refer_obj(stack* s){
			return	NULL;
		}

	private:
		s32	need_check(){
			return	m_flgs[2];
		}

		s32	set_need_check(s32 n){
			m_flgs[2] = n;
			return	n;
		}

		s8 m_flgs[4];
		s32 m_loop_cnt;
		void* m_ext_data;
	};
}

#endif/*ZVM_GC_H*/

