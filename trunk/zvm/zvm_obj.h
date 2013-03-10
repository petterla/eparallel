#ifndef ZVM_OBJ_H
#define ZVM_OBJ_H

#include "zvm_type.h"
#include "zvm_err_no.h"
#include "zvm_lock.h"
#include <string>

namespace zvm{

	class stack;

	class entry{
	public:
		virtual	~entry(){

		}

		virtual	std::string type() const{
			return	"";
		}

		virtual	entry* clone(stack* s){
			return	this;
		}

		virtual	s32 recycle(stack* s){
			delete this;
			return	SUCCESS;
		}
	};

	class obj{
	public:
		obj(entry* ent = NULL)
			:m_ent(ent){

		}

		~obj(){
			set_entry(NULL, (stack*)1);
		}

		s32 clear(stack* s){
			return	set_entry(NULL, s);
		}

		s32	lock(stack* s){
			return	m_lock.lock((s32)s);
		}

		s32 set_entry(entry* ent, stack* s){

			entry* e = (entry*)m_ent;
			lock(s);
			m_ent = ent;
			unlock(s);
			if(e){
				return e->recycle(s);
			}

			return	SUCCESS;
		}

		entry* clone_entry(stack* s){

			lock(s);
			auto_simple_unlock ul(m_lock, (s32)s);

			if(m_ent){
				return	m_ent->clone(s);
			}
			return	NULL;
		}

		s32 assign(obj* o, stack* s){
			entry* e = o->clone_entry(s);
			return	o->set_entry(e, s);
		}

		s32 unlock(stack* s){
			return	m_lock.unlock((s32)s);
		}

		s32 is_null(){
			return	m_ent == 0;
		}

		const std::string type(stack* s){
			std::string t = "NULL";
			lock(s);
			if(m_ent){
				t =	m_ent->type();
			}
			unlock(s);
			return	t;
		}

	private:
		simple_lock m_lock;
		entry* m_ent;
	};

}

#endif/*ZVM_OBJ_H*/