#ifndef ZVM_FRAME_H
#define ZVM_FRAME_H

#include <stdlib.h>
#include <string.h>
#include "zvm_type.h"
#include "zvm_err_no.h"

namespace zvm{

	class frame{
	public:
		frame()
			:m_size(0),
			m_max_size(0),
			m_locals(0),
			m_ebp(0),
			m_prev_frame(0)
		{
		}

		~frame(){

		}

		s32 set_locals(void* p, u32 max_sz){
			m_locals = (local_t *)p;
			m_max_size = max_sz;
			return	SUCCESS;
		}

		frame* prev_frame(){
			return	m_prev_frame;
		}

		s32 set_prev_frame(frame* p){
			m_prev_frame = p;
			return	SUCCESS;
		}

		s64	get_local(u32 idx){

			assert(idx < m_size);
			return	m_locals[idx].m_local;

		}

		s32	set_local(u32 idx, s64 p){

			assert(idx < m_size);
			m_locals[idx].m_local = p;

			return	SUCCESS;

		}

		s32 set_ebp(u32 ebp){
			m_ebp = ebp;

			return	SUCCESS;
		}

		u32 get_ebp(){
			return	m_ebp;
		}

		s32 push(s64 p, s32 t){
			if(m_size >= m_max_size){
				return	OVER_FLOW;
			}

			m_locals[m_size].m_local = p;
			m_locals[m_size++].m_type = t;

			return	SUCCESS;
		}

		s64	pop(s32& t){
			if(m_size){
				t = m_locals[--m_size].m_type;
				return	m_locals[m_size].m_local;
			}
			return	0;
		}

		s64	pop(){
			if(m_size){
				return	m_locals[--m_size].m_local;
			}
			return	0;
		}

		u32 size(){
			return	m_size;
		}

		s32 set_function(s64 f)
		{
			m_function = f;
			return	SUCCESS;
		}

		s64 get_function(){
			return	m_function;
		}

	private:

		u32 m_size;
		u32 m_max_size;
		local_t *m_locals;

		s64 m_function;
		//ebp register
		u32 m_ebp;
		frame* m_prev_frame;

	};

}

#endif