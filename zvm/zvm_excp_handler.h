#ifndef ZVM_EXCP_HANDLER_H
#define ZVM_EXCP_HANDLER_H

#include "zvm_type.h"
#include <string>
#include <vector>

namespace zvm{

	class stack;

	struct excp_handler{
	public:
		excp_handler(const std::string excp_type,
					u32 start_pc,
					u32 end_pc,
					u32 ent_addr,
					u32 throw_again = false)
					:m_excp_type(excp_type),
					m_start_pc(start_pc),
					m_end_pc(end_pc),
					m_ent_addr(ent_addr),
					m_throw_again(throw_again)
		{

		}

		bool handle(stack* s);

	public:
		std::string	m_excp_type;
		u32 m_start_pc;
		u32 m_end_pc;
		u32 m_ent_addr;
		u32 m_throw_again;
	};

	typedef std::vector<excp_handler> ehandlers;

}

#endif/*ZVM_EXCP_HANDLER_H*/