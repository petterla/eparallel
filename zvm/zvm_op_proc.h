#ifndef ZVM_OP_PROC_H
#define ZVM_OP_PROC_H

#include "zvm_type.h"
#include "zvm_err_no.h"
#include "zvm_op_code.h"
#include "zvm_op.h"
#include "zvm_stack.h"
#include "zvm_obj.h"
#include "zvm_function.h"
#include "zvm_class.h"

namespace zvm{

	inline obj* dup(stack* s){

		assert(s);
		obj* o = (obj*)s->eax();
		assert(o);
		obj* no = s->allocate(o->clone_entry(s));
		return	no;

	}

	inline obj* dupfo(stack* s){

		assert(s);
		obj* o = (obj*)s->get_op_stack().pop();
		assert(o);
		obj* no = s->allocate(o->clone_entry(s));
		return	no;

	}

	inline s32 op_process(const operation* op, stack* s, s32 status = SUCCESS){
		obj* ol = NULL;
		function* f = NULL;
		classdef* c = NULL;
		bool catched = false;
		assert(s);

		switch(op->m_op_code){
			case AB:
				s->ebx() = s->eax();
				return	SUCCESS;

			case AC:
				s->ecx() = s->eax();
				return	SUCCESS;

			case AD:
				s->edx() = s->eax();
				return	SUCCESS;

			case BA:
				s->eax() = s->ebx();
				return	SUCCESS;

			case BC:
				s->ecx() = s->ebx();
				return	SUCCESS;

			case BD:
				s->edx() = s->ebx();
				return	SUCCESS;

			case CA:
				s->eax() = s->ecx();
				return	SUCCESS;

			case CB:
				s->ebx() = s->ecx();
				return	SUCCESS;

			case CD:
				s->edx() = s->ecx();
				return	SUCCESS;

			case DA:
				s->eax() = s->edx();
				return	SUCCESS;

			case DB:
				s->ebx() = s->edx();
				return	SUCCESS;

			case DC:
				s->ecx() = s->edx();
				return	SUCCESS;

			case AOPSTACK:
				return	s->get_op_stack().push(s->eax());

			case BOPSTACK:
				return	s->get_op_stack().push(s->ebx());

			case COPSTACK:
				return	s->get_op_stack().push(s->ecx());

			case DOPSTACK:
				return	s->get_op_stack().push(s->edx());

			case OPSTACKA:
				s->eax() = s->get_op_stack().pop();
				return	SUCCESS;

			case OPSTACKB:
				s->ebx() = s->get_op_stack().pop();
				return	SUCCESS;

			case OPSTACKC:
				s->ecx() = s->get_op_stack().pop();
				return	SUCCESS;

			case OPSTACKD:
				s->edx() = s->get_op_stack().pop();
				return	SUCCESS;

			case ICONST:
				return	s->get_op_stack().
					push(op->m_op.m_iconst.m_i);

			case ICONSTA:
				s->eax() = op->m_op.m_iconsta.m_i;
				return	SUCCESS;

			case ICONSTB:
				s->ebx() = op->m_op.m_iconstb.m_i;
				return	SUCCESS;

			case ICONSTC:
				s->ecx() = op->m_op.m_iconstc.m_i;
				return	SUCCESS;

			case ICONSTD:
				s->edx() = op->m_op.m_iconstd.m_i;
				return	SUCCESS;

			case ILOAD:
				return	s->get_op_stack().push(
					s->current_frame().
					get_local(op->m_op.m_iload.m_idx));
			case ILOADA:
				s->eax() = s->current_frame().
					get_local(op->m_op.m_iloada.m_idx);
				return	SUCCESS;

			case ILOADB:
				s->ebx() = s->current_frame().
					get_local(op->m_op.m_iloadb.m_idx);
				return	SUCCESS;

			case ILOADC:
				s->ecx() = s->current_frame().
					get_local(op->m_op.m_iloadc.m_idx);
				return	SUCCESS;

			case ILOADD:
				s->edx() = s->current_frame().
					get_local(op->m_op.m_iloadd.m_idx);
				return	SUCCESS;

			case ISTORE:
				assert(s->get_op_stack().size() == 1);
				return	s->current_frame().set_local
					(op->m_op.m_istore.m_idx,
					s->get_op_stack().pop());

			case ISTOREA:
				return	s->current_frame().set_local
					(op->m_op.m_istorea.m_idx,
					s->eax());

			case ISTOREB:
				return	s->current_frame().set_local
					(op->m_op.m_istoreb.m_idx,
					s->ebx());

			case ISTOREC:
				return	s->current_frame().set_local
					(op->m_op.m_istorec.m_idx,
					s->ecx());

			case ISTORED:
				return	s->current_frame().set_local
					(op->m_op.m_istored.m_idx,
					s->edx());

			case IADD:
				s->eax() += s->ebx();
				return	SUCCESS;

			case ISUB:
				s->eax() -= s->ebx();
				return	SUCCESS;

			case IMUT:
				s->eax() *= s->ebx();
				return	SUCCESS;

			case IDIV:
				if(s->ebx()){
					s->eax() /= s->ebx();
					return	SUCCESS;
				}
				return	DIV_ZERO;

			case IREM:
				if(s->ebx()){
					s->eax() %= s->ebx();
					return	SUCCESS;
				}
				return	DIV_ZERO;

			case ISHL:
				s->eax() <<= s->ebx();
				return	SUCCESS;		

			case ISHR:
				s->eax() >>= s->ebx();
				return	SUCCESS;	

			case INEG:
				s->eax() = -(s->eax());
				return	SUCCESS;

			case IXNOT:
				s->eax() = ~(s->eax());
				return	SUCCESS;

			case IINC:
				++s->eax();
				return	SUCCESS;

			case NOT:
				s->eax() = !(s->eax());
				return	SUCCESS;

			case AND:
				s->eax() = (s->eax() && s->ebx());
				return	SUCCESS;

			case OR:
				s->eax() = (s->eax() || s->ebx());
				return	SUCCESS;

			case MONENTER:
				ol = (obj*)s->eax();
				if(ol){
					return	ol->lock(s);
				}
				return	LOCK_NULL_OBJ;

			case MONEXIT:
				ol = (obj*)s->eax();
				if(ol){
					return	ol->lock(s);
				}
				return	LOCK_NULL_OBJ;

			case ALOAD:
				return	s->get_op_stack().push(
					s->current_frame().
					get_local(op->m_op.m_aload.m_idx));

			case ALOADA:
				s->eax() = s->current_frame().
					get_local(op->m_op.m_aloada.m_idx);
				return	SUCCESS;

			case ALOADB:
				s->ebx() = s->current_frame().
					get_local(op->m_op.m_aloadb.m_idx);
				return	SUCCESS;

			case ALOADC:
				s->ecx() = s->current_frame().
					get_local(op->m_op.m_aloadc.m_idx);
				return	SUCCESS;

			case ALOADD:
				s->edx() = s->current_frame().
					get_local(op->m_op.m_aloadd.m_idx);
				return	SUCCESS;

			case PUSH_I:
				return	s->current_frame().push(
					s->eax(), 
					LOCAL_TYPE_INT);

			case PUSH_O:
			case PUSH_A:
				ol = dup(s);
				if(!ol){
					return	OUT_OF_MEM;
				}
				return	s->current_frame().push(
					(s64)ol, LOCAL_TYPE_OBJ);

			case PUSH_ICONST:
				return	s->current_frame().push(
					op->m_op.m_push_iconst.m_i,
					LOCAL_TYPE_INT);

			case PUSH_NULL:
				ol = s->allocate(NULL);
				if(!ol){
					return	OUT_OF_MEM;
				}
				return	s->current_frame().push(
					(s64)ol, LOCAL_TYPE_OBJ);

			case ATHROW:
				assert(s && s->get_op_stack().size() == 1);
				ol = dup(s);
				s->set_exception((s64)ol);
				return	RET_ATHROW;

			case ACATCH:
				if(!s->get_exception())
					set_exception(s, status);
				f = (function*)s->current_frame().get_function();
				if(f){
					catched = f->handler_excp(s);
				}

				if(!catched){
					s->set_pc((u32)(-1) - 1);
					return	SUCCESS;
				}
				//has catch
				return	SUCCESS;

			case JUMP:
				return	s->set_pc(op->m_op.m_jump.m_pc);

			case JE:
				if(s->eax() == s->ebx()){
					s->set_pc(op->m_op.m_je.m_pc);
				}
				return	SUCCESS;

			case JNE:
				if(s->eax() != s->ebx()){
					s->set_pc(op->m_op.m_jne.m_pc);
				}
				return	SUCCESS;

			case JLG:
				if(s->eax() > s->ebx()){
					s->set_pc(op->m_op.m_jlg.m_pc);
				}
				return	SUCCESS;

			case JSM:
				if(s->eax() < s->ebx()){
					s->set_pc(op->m_op.m_jsm.m_pc);
				}
				return	SUCCESS;

			case JLGE:
				if(s->eax() >= s->ebx()){
					s->set_pc(op->m_op.m_jlge.m_pc);
				}
				return	SUCCESS;

			case JSME:
				if(s->eax() <= s->ebx()){
					s->set_pc(op->m_op.m_jsme.m_pc);
				}
				return	SUCCESS;

			case JNULL:
				ol = (obj*)(s->eax());
				assert(ol);
				if(ol->is_null()){
					s->set_pc(op->m_op.m_jnull.m_pc);
				}
				return	SUCCESS;

			case JNNULL:
				ol = (obj*)(s->eax());
				assert(ol);
				if(!ol->is_null()){
					s->set_pc(op->m_op.m_jnnull.m_pc);
				}
				return	SUCCESS;

			case ARET:
				s->set_pc((u32)(-1) - 1);
				ol = (obj*)(s->eax());
				s->eax() = (s64)&(s->get_ret_reg());
				return	s->get_ret_reg().assign(s, ol);

			case IRET:
				s->set_pc((u32)(-1) - 1);
				return	SUCCESS;

			case RET:
				s->set_pc((u32)(-1) - 1);
				return	SUCCESS;

			case ANEW:
				c = (classdef*)(s->cls());
				assert(c);
				c = c->get_class(op->m_op.m_anew.m_idx);
				if(!c){
					s->eax() = (s64)c->create_entry_obj(s);
					return	SUCCESS;
				}else{
					s->set_exception(ACCESS_NULL);
					return	ACCESS_NULL;
				}

			case ANOTHOR:
				c = (classdef*)(s->cls());
				assert(c);
				s->eax() = (s64)c->create_entry_obj(s);
				return	SUCCESS;

			case AINIT:
				ol = (obj*)(s->ths());
				if(!ol->is_null()){
					s->eax() = (s64)ol->init(s);
					return	SUCCESS;
				}else{
					s->set_exception(ACCESS_NULL);
					return	ACCESS_NULL;
				}

			default:
				break;

		}

		return	SUCCESS;

	}
}

#endif