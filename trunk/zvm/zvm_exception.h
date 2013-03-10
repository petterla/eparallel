#ifndef ZVM_EXCEPTION_H
#define	ZVM_EXCEPTION_H

#include "zvm_obj.h"
#include <map>

namespace zvm{

	class inner_exception:public entry{
	public:
		inner_exception(const std::string& type)
			:m_type(type),
			m_throw_pc(0){

		}

		virtual s32 recycle(stack* s){
			m_throw_pc = 0;
			return	SUCCESS;
		}

		virtual std::string type(){
			return	m_type;
		}

		s32 set_throw_pc(u32 pc){
			m_throw_pc = pc;
			return	SUCCESS;
		}

	private:
		std::string m_type;
		u32 m_throw_pc;
	};

	class excep_manager{
	public:
		excep_manager();
		~excep_manager();
		inner_exception* get(s32 status);
	private:
		typedef std::map<s32, inner_exception*> emap;
		emap m_e_map;
	};

	s32 set_exception(stack* s, s32 status);

}

#endif/*ZVM_EXCEPTION_H*/