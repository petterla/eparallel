#ifndef ZVM_FUNCTION_H
#define ZVM_FUNCTION_H

#include <string>
#include <vector>
#include "zvm_type.h"
#include "zvm_op.h"
#include "zvm_excp_handler.h"

namespace zvm{

	class stack;


	class function{
	public:
		function(const std::string& name);
		~function();
		s32 load_code(const std::string& code);
		s32 process(stack* s);
		s32 add_op(operation* op);
		//catch code is at m_ops[0]
		s32 add_excp_handler(excp_handler* h);
		bool handler_excp(stack* s);
	private:
		std::string m_name;

		typedef std::vector<operation> ops_t;
		ops_t m_ops;

		ehandlers m_excp_handlers;

	};

}

#endif/*ZVM_FUNCTION_H*/

