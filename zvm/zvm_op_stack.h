#ifndef ZVM_OP_STACK_H
#define ZVM_OP_STACK_H

#include "zvm_type.h"

namespace zvm{
	class op_stack{
	public:

		enum{
			OP_STACK_DEFAULT_SIZE = 32,
		};

		op_stack()
			:m_size(0){

		}

		~op_stack(){

		}

		//push argu from right to left
		s32 push(s64 p){
			if(m_size < OP_STACK_DEFAULT_SIZE){
				m_stack[m_size++] = p;
				return	SUCCESS;
			}
			return	OVER_FLOW;
			//return	(0 - (m_size == OP_STACK_DEFAULT_SIZE)) & OVER_FLOW;

		}

		u32 size(){
			return	m_size;
		}

		s64 pop(){

			if(m_size)
				return	m_stack[--m_size];
			return	0;

		}

		s64& top(){

			assert(m_size);
			return	m_stack[m_size - 1];

		}

		s32 set_top(s64 i){
			top() = i;
			return	SUCCESS;
		}

		s32 clear(){
			m_size = 0;
			return	SUCCESS;
		}

	private:

		u32 m_size;
		s64	m_stack[OP_STACK_DEFAULT_SIZE];

	};
}


#endif/*ZVM_OP_STACK_H*/