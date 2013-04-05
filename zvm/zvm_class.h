#ifndef ZVM_CLASS_H
#define ZVM_CLASS_H

#include "zvm_obj.h"
#include "zvm_gc.h"
#include <string>
#include <vector>

namespace zvm{
	class function;
	class stack;

	struct member_function{
		function* m_fun;
		bool m_devide;
	};

	struct member{
		s32	m_type;
		union{
			u32 m_clss_idx;
			s64 m_default;
		} m_val;
	};

	/*****************************
	[0 - n]parent member function
	[n+1 - last]member function
	*****************************/
	class classdef:public entry{
	public:
		classdef(const std::string& name, 
			classdef* p = NULL)
			:m_name(name),
			m_parent(p),
			m_ref_cnt(1)
		{
		}
		~classdef();
		virtual s32 recycle(stack* s);
		s32 init(u32 membcnt, u32 membfuncnt, 
			u32 staticfiedcnt);
		s32 free();
		s32 set_init_function(function* f){
			m_init_function = f;
			return	SUCCESS;
		}
		s32 import_class(classdef* c);
		classdef* get_class(u32 idx);
		function* get_init_function(){
			return	m_init_function;
		}
		u32 member_function_count(){
			return	(u32)m_member_functions.size();
		}
		function* get_member_function(u32 idx);
		s32 add_member_function(function* f);
		s32 override_member_function(u32 idx, function* f);
		u32 static_field_count(){
			return	(u32)m_static_fields.size();
		}
		obj* get_static_field(u32 idx);
		s32 add_static_field(u32 type);
		u32 member_count(){
			return	(u32)m_members_define.size();
		}
		s32 add_member(const member& m);
		std::string& name();
		entry* create_ent(stack* s);
		obj* create_ent_obj(stack* s);
		classdef* get_parent(){
			return	m_parent;
		}
		member member_define(u32 idx)const{
			assert(idx < m_members_define.size());
			return	m_members_define[idx];
		}
		u32 member_type(u32 idx)const{
			assert(idx < m_members_define.size());
			return	m_members_define[idx].m_type;
		}
	private:
		s32 devide_from(classdef* p);


		friend class user_type;
		std::string m_name;
		classdef* m_parent;
		function* m_init_function;
		std::vector<member> m_members_define;
		std::vector<classdef*> m_classes;
		std::vector<member_function> m_member_functions;
		std::vector<local_t> m_static_fields;
		volatile s32 m_ref_cnt;
	};

	/*****************************

	*****************************/
	class user_type:public type_gc{
	public:

		user_type(classdef* c,
			u32 member_cnt, 
			s64* members);
		~user_type();
		virtual s32 init(stack* s);
		virtual entry* clone(stack* s);
		virtual bool sign(stack* s, bool first);
		virtual bool check_live(stack* s);
		virtual bool reset(stack* s);
		virtual s32 init_super(stack* s);
		virtual s32 clear(stack* s);
		virtual u32 get_member_count();
		virtual s64 get_member_i(stack* s, u32 idx);
		virtual obj* get_member_o(stack* s, u32 idx);
		virtual s32 set_member_i(stack* s, u32 idx, s64 i);
		virtual s32 set_member_o(stack* s, u32 idx, obj* o);
	private:
		friend class classdef;
		classdef* m_class;
		obj* m_parent;
		u32 m_member_count;
		s64* m_members;
		s32  m_flag;
	};

}


#endif