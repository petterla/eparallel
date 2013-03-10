#ifndef ZVM_CLASS_H
#define ZVM_CLASS_H

#include "zvm_obj.h"
#include <string>
#include <vector>

namespace zvm{
	class function;
	class stack;

	class classdef:public entry{
	public:
		~classdef();
		function* get_member_function(u32 idx);
		obj* get_static_field(u32 idx);
		std::string& name();
		entry* create_ent(stack* s);
	private:
		std::string m_name;
		std::vector<s32> m_members_define;
		std::vector<function*> m_member_functions;
		std::vector<local_t> m_static_fields;
	};

	class user_type:public entry{
	public:
		user_type();
		~user_type();
		s32 init(classdef* c);
		virtual s32 recycle(stack* s);
	private:
		std::vector<local_t> m_member;
		classdef* m_class;
	};

}


#endif