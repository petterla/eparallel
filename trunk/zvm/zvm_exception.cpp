#include "zvm_exception.h"
#include "zvm_stack.h"

namespace zvm{
		
	excep_manager::excep_manager(){
		m_e_map[OVER_FLOW] = new inner_exception("OVER_FLOW");
		m_e_map[OUT_OF_MEM] = new inner_exception("OUT_OF_MEM");
		m_e_map[DIV_ZERO] = new inner_exception("DIV_ZERO");
		m_e_map[INVALID_SYNC] = new inner_exception("INVALID_SYNC");
		m_e_map[INVALID_INDEX] = new inner_exception("INVALID_INDEX");
		m_e_map[UNKNOW_EXCP] = new inner_exception("UNKNOW_EXCP");
	}

	excep_manager::~excep_manager(){
		emap::iterator itor = m_e_map.begin();
		for(; itor != m_e_map.end(); ++itor){
			delete itor->second;
		}
	}

	inner_exception* excep_manager::get(s32 status){
		emap::iterator itor = m_e_map.find(status);
		if(itor != m_e_map.end()){
			return	itor->second;
		}
		return	m_e_map[UNKNOW_EXCP];
	}

	s32 set_exception(stack* s, s32 status){
		inner_exception* eent = s->get_excep_manager().get(status);
		obj* e = s->allocate(eent);
		s->set_exception((s64)e);
		return	SUCCESS;
	}

}