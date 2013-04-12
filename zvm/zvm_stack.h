#ifndef ZVM_STACK_H
#define ZVM_STACK_H


#include "zvm_type.h"
#include "zvm_frame.h"
#include "zvm_op_stack.h"
#include "zvm_obj.h"
#include "zvm_pool.h"
#include "zvm_exception.h"

namespace zvm{

	class frame;
	class obj;
	class entry;

	class stack{
	public:
		enum{
			STACK_DEFAULT_SIZE = 16 * 1024,
		};

		stack()
			:m_size(0),
			m_excp(0),
			m_cur_frame(NULL)
		{

		}

		~stack(){

		}

		s32 push_frame(){

			if(m_cur_frame && STACK_DEFAULT_SIZE 
				- (m_cur_frame->size() * sizeof(local_t))
				< (sizeof(frame) << 1)){
				return	OVER_FLOW;
			}
			frame* p = m_cur_frame;
			m_cur_frame = !p ? (frame*)m_stack :
				(frame*)(p + p->size() * sizeof(local_t));
			new (m_cur_frame) frame();
			local_t* l = (local_t*)(m_cur_frame + sizeof(frame));
			m_cur_frame->set_locals(l, 
				(u32)(m_stack + STACK_DEFAULT_SIZE - (u8*)l) / sizeof(local_t));
			m_cur_frame->set_prev_frame(p);
			m_size++;
			return	SUCCESS;

		}

		s32 pop_frame(){

			if(m_size){
				m_cur_frame = m_cur_frame->prev_frame();
				--m_size;
				return	SUCCESS;
			}
			return	OVER_FLOW;

		}

		frame& current_frame(){

			return	*m_cur_frame;

		}

		op_stack&	get_op_stack(){

			return	m_op_stack;

		}

		obj* allocate(entry* ent){
			obj* o = m_obj_pool.allocate();
			if(o){
				o->set_entry(this, ent);
			}
			//printf("allocate:%p\n", o);
			return	o;
		}

		s32 deallocate(obj* o){
			if(o){
				o->clear(this);
				m_obj_pool.deallocate(o);
			}
			//printf("deallocate:%p\n", o);
			return	SUCCESS;
		}

		void* alloc_mem(u32 sz){
			char* p = new char[sz];
			//printf("alloc_mem:%p\n", p);
			return	p;
		}

		void free_mem(void* p,u32 sz){
			//printf("free_mem:%p\n", p);
			delete [] (char*)(p);
		}

		s32 set_exception(s64 e){
			m_excp = e;
			return	SUCCESS;
		}

		s64 get_exception(){
			return	m_excp;
		}

		obj& get_ret_reg(){
			return	m_ret_obj;
		}

		s32 set_pc(u32 pc){
			m_pc = pc;
			return	SUCCESS;
		}

		u32	get_pc(){
			return	m_pc;
		}

		u32& pc(){
			return	m_pc;
		}

		s32 clear(){
			m_pc = 0;
			m_excp = 0;
			m_ret_obj.clear(this);
			return	SUCCESS;
		}

		s64& eax(){
			return	m_eax;
		}

		s64& ebx(){
			return	m_ebx;
		}

		s64& ecx(){
			return	m_ecx;
		}

		s64& edx(){
			return	m_edx;
		}

		//reg for this pointer
		s64& ths(){
			return m_ths;
		}

		//reg for class def
		s64& cls(){
			return	m_class;
		}
		
		excep_manager& get_excep_manager(){
			return	m_e_man;
		}

	private:
		s32 m_size;
		u8 m_stack[STACK_DEFAULT_SIZE];

		s64 m_eax;
		s64 m_ebx;
		s64 m_ecx;
		s64 m_edx;

		s64	m_excp;
		s64 m_ths;
		s64 m_class;

		obj m_ret_obj;

		u32 m_pc;

		frame* m_cur_frame;

		op_stack m_op_stack;

		pool<obj> m_obj_pool;

		excep_manager m_e_man;

	};

}


#endif/*ZVM_STACK_H*/
