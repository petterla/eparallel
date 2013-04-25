#include "zvm_excp_handler.h"
#include "zvm_stack.h"

namespace zvm{

	bool excp_handler::handle(stack* s){

		obj* e = (obj*)s->get_exception();
		if((m_excp_type == "" 
			|| m_excp_type == e->type(s))
			&& s->pc() >= m_start_pc
			&& s->pc() < m_end_pc){
				if(!m_throw_again){
					e->set_entry(s, NULL);
					s->deallocate(e);
				}
				s->pc() = m_ent_addr;
				return	true;
		}

		return	false;

	}

}

