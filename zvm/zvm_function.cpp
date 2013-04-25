#include "zvm_function.h"
#include "zvm_err_no.h"
#include "zvm_op_proc.h"

namespace zvm{

	function::function(const std::string& name)
		:m_name(name){
			//first op is catch op
			operation acatch_op;
			acatch_op.m_op_code = ACATCH;
			m_ops.push_back(acatch_op);

	}

	function::~function(){

	}

	s32 function::load_code(const std::string& code){
		return	SUCCESS;
	}

	s32 function::process(stack* s){
		assert(s);
		u32& pc = s->pc();
		pc = 1;
		s32 ret = SUCCESS;
		operation* op = (operation*)&m_ops[0];
		operation* ops = op;
		u32 cnt = (u32)m_ops.size();
		for(; pc < cnt;){
			op = ops + pc;
			ret = op_process(op, s, ret);
			//if ret < 0, pc = 0 else pc++
			pc = (0 - (ret == SUCCESS)) & (pc + 1);
			//if(ret == SUCCESS)
			//	pc++;
			//else{
			//	pc = 0;
			//}
		}

		//clear locals
		u32 sz = s->current_frame().size();
		for(; sz; --sz){
			s64 l;
			s32 t;
			l = s->current_frame().pop(t);
			if(t == LOCAL_TYPE_OBJ){
				obj* o = (obj*)l;
				if(o){
					s->deallocate(o);
				}
			}
		}

		if(s->get_exception()){
			ret = RET_ATHROW;
		}

		return	ret;
	}

	s32 function::add_op(operation* op){

		m_ops.push_back(*op);

		return	SUCCESS;
	}

	s32 function::add_excp_handler(excp_handler *h){
		m_excp_handlers.push_back(*h);
		return	SUCCESS;
	}

	bool function::handler_excp(stack* s){
		for(size_t i = 0; i < m_excp_handlers.size(); ++i){
			if(m_excp_handlers[i].handle(s)){
				return	true;
			}

		}
		return	false;
	}

}

