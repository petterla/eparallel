#include "zvm_class.h"
#include "zvm_function.h"
#include "zvm_stack.h"
#include "be_atomic.h"

namespace zvm{
#ifdef ZVM_ENTRY_DEBUG
	int entry::s_ent_cnt = 0;
#endif
	classdef::~classdef(){
		free();
	}

	s32 classdef::recycle(stack* s){
		be::atomic_decreament32((volatile be::s32*)&m_ref_cnt);
		if(m_ref_cnt < 0){
			delete	this;
		}
		return	SUCCESS;
	}

	s32	classdef::init(u32 membcnt, 
		u32 membfuncnt, 
		u32 staticfiedcnt){
			m_members_define.reserve(membcnt);
			m_member_functions.reserve(membfuncnt);
			m_static_fields.reserve(staticfiedcnt);

			return	devide_from(m_parent);
	}

	s32 classdef::import_class(classdef* c){
		m_classes.push_back(c);
		return	NULL;
	}
	classdef* classdef::get_class(u32 idx){
		if(idx < m_classes.size()){
			return	m_classes[idx];
		}
		return	NULL;
	}

	s32	classdef::free(){
		for(size_t i = 0; i < m_static_fields.size(); ++i){
			if(m_static_fields[i].m_type == LOCAL_TYPE_OBJ){
				obj* o = (obj*)m_static_fields[i].m_local;
				delete o;
			}
		}

		for(size_t j = 0; j < m_member_functions.size(); ++j){
			if(m_member_functions[j].m_devide){
				delete	m_member_functions[j].m_fun;
			}
		}

		for(size_t k = 0; k < m_classes.size(); ++k){
			m_classes[k]->recycle(NULL);
		}

		return	SUCCESS;
	}

	s32	classdef::devide_from(classdef* p){
		m_parent = p;
		for(size_t i = 0; i < p->m_member_functions.size();
			++i){
			member_function m;
			m.m_fun = p->m_member_functions[i].m_fun;
			m.m_devide = true;
			m_member_functions.push_back(m);
		}

		return	SUCCESS;
	}

	s32 classdef::add_member(const member& m){
		m_members_define.push_back(m);
		return	SUCCESS;
	}

	function* classdef::get_member_function(u32 idx){
		if (idx < m_member_functions.size()){
			return	m_member_functions[idx].m_fun;
		}
		return	NULL;
	}

	s32 classdef::add_member_function(function* f){

		member_function s;
		s.m_devide = true;
		s.m_fun = f;		
		m_member_functions.push_back(s);

		return	SUCCESS;
	}

	s32 classdef::override_member_function
		(u32 idx, function* f){
		if (idx < m_member_functions.size()){
			if(m_member_functions[idx].m_devide){
				delete	m_member_functions[idx].m_fun;
			}
			m_member_functions[idx].m_fun = f;
			return	SUCCESS;
		}
		return	INVALID_INDEX;
	}

	class auto_recycle{
	public:
		auto_recycle(user_type* t, stack* s)
			:m_t(t),m_s(s){

		}
		void set_entry(user_type* t){
			m_t = t;
		}
		~auto_recycle(){
			if(m_t){
				m_t->recycle(m_s);
			}
		}
	private:
		user_type* m_t;
		stack* m_s;
	};

	obj* classdef::create_ent_obj(stack* s){
		entry* t = create_ent(s);
		if(!t)
			return	NULL;
		obj* o = s->allocate(t);
		if(!o){
			t->recycle(s);
			s->set_exception(OUT_OF_MEM);
		}
		return	o;	
	}

	//only allocate mem,allcate parent,
	//create null member obj
	entry* classdef::create_ent(stack* s){
		user_type* t = (user_type*)
			s->alloc_mem(sizeof(user_type) 
			+ m_members_define.size() * sizeof(s64));
		if(!t){
			s->set_exception(OUT_OF_MEM);
			return	NULL;
		}
		t->user_type::user_type(this, 
			m_members_define.size(), 
			(s64*)((s8*)t+sizeof(user_type)));

		auto_recycle ac(t, s);

		if(m_parent){
			t->m_parent = m_parent->create_ent_obj(s);
			if(!t->m_parent){
				return	NULL;
			}
		}
		for(size_t i = 0; i < m_members_define.size(); ++i){
			if(m_members_define[i].m_type != LOCAL_TYPE_OBJ){
				t->m_members[i] = m_members_define[i].m_val.m_default;
				continue;
			}
			t->m_members[i] = (s64)(s->allocate(NULL));
			if(!t->m_members[i])
				return	NULL;
		}
		ac.set_entry(NULL);

		return	t;
	}

	user_type::user_type(classdef* c,
		u32 member_cnt, 
		s64* members)
		:m_class(c),
		m_parent(NULL),
		m_member_count(member_cnt),
		m_members(members),
		m_flag(FLAG_NULL)
	{
		memset(members, 0, member_cnt * sizeof(s64));
	}


	s32 user_type::init(stack* s){
		s32 ret = SUCCESS;
		function* f = m_class->get_init_function();
		//in f,it will call parent.init
		if(f){
			return	f->process(s);
		}

		return	SUCCESS;
	}

	s32 user_type::init_super(stack* s){
		s32 ret = SUCCESS;
		function* f = m_class->get_parent()
			->get_init_function();
		//in f,it will call parent.init
		if(f){
			return	f->process(s);
		}

		return	SUCCESS;
	}

	user_type::~user_type(){

	}

	entry* user_type::clone(stack* s){
		//avoid loop clone
		if(m_status != STATUS_NULL){
			m_status = STATUS_NULL;
			return	this;
		}

		m_status = STATUS_CLONE;
		//do clone,
		user_type* t = (user_type*)
			s->alloc_mem(sizeof(user_type) 
			+ m_member_count * sizeof(s64));
		if(!t){
			s->set_exception(OUT_OF_MEM);
			return	NULL;
		}
		t->user_type::user_type(m_class, 
			m_member_count, 
			(s64*)((s8*)t+sizeof(user_type)));

		auto_recycle ac(t, s);

		if(m_parent){
			entry* e = m_parent->entry_clone(s);
			if(!e){
				return	NULL;
			}
			t->m_parent = s->allocate(e);
			if(!t->m_parent){
				return	NULL;
			}
		}
		for(size_t i = 0; i < m_member_count; ++i){
			if(m_class->m_members_define[i].m_type 
				!= LOCAL_TYPE_OBJ){
				t->m_members[i] = m_members[i];
				continue;
			}
			entry* ent = ((obj*)m_members[i])->entry_clone(s);
			if(!ent){
				return	NULL;
			}
			t->m_members[i] = (s64)s->allocate(ent);
			if(!t->m_members[i])
				return	NULL;
		}
		ac.set_entry(NULL);
		m_status = STATUS_NULL;
		return	t;

	}

	bool user_type::sign(stack* s, bool first){
		bool ret = true;
		if(!first)
			++m_loop_cnt;
		if(m_status != STATUS_NULL){
			m_flag = FLAG_LOOP;
			return	false;
		}
		m_status = STATUS_SIGN;
		if(m_parent){
			m_parent->sign(s, false);
		}
		for(size_t i = 0; i < m_member_count; ++i){
			if(m_class->m_members_define[i].m_type
				== LOCAL_TYPE_OBJ){
					((obj*)m_members[i])->sign(s, false);
			}
		}
		if(m_flag == FLAG_LOOP)
			ret = false;
		m_status = STATUS_NULL;
		return	ret;
	}

	bool user_type::check_live(stack* s){
		bool ret = true;
		if(m_flag != FLAG_LOOP){
			return	true;
		}
		if(m_status != STATUS_NULL)
			return	true;
		m_status = STATUS_CHECK;
		if(m_loop_cnt >= ref_count()){
			ret = false;
			goto exit;
		}

		if(m_parent){
			ret = m_parent->check_live(s);
		}

		if(!ret){
			goto exit;
		}

		for(size_t i = 0; i < m_member_count; ++i){
			if(m_class->m_members_define[i].m_type
				== LOCAL_TYPE_OBJ){
				ret =((obj*)m_members[i])->check_live(s);
				if(!ret){
					goto exit;
				}
			}
		}
exit:
		m_status = STATUS_NULL;
		return	ret;
	}

	bool user_type::reset(stack* s){
		bool ret = true;
		if(m_status != STATUS_NULL)
			return	true;
		m_status = STATUS_RESET;
		m_flag = FLAG_NULL;
		m_loop_cnt = 0;
		if(m_parent){
			m_parent->reset(s);
		}
		for(size_t i = 0; i < m_member_count; ++i){
			if(m_class->m_members_define[i].m_type
				== LOCAL_TYPE_OBJ){
				((obj*)m_members[i])->reset(s);
			}
		}
		m_status = STATUS_NULL;
		return	ret;
	}

	//bool user_type::check(stack* s){
	//	bool ret = true;
	//	if(m_flag != FLAG_NULL){
	//		recycle(s);
	//		return	false;
	//	}
	//	m_flag = FLAG_CHECK;

	//	if(m_parent){
	//		m_parent->check(s);
	//	}
	//	for(size_t i = 0; i < m_member_count; ++i){
	//		if(m_class->m_members_define[i].m_type
	//			== LOCAL_TYPE_OBJ){
	//			((obj*)m_members[i])->check(s);
	//		}
	//	}

	//	m_flag = FLAG_NULL;
	//	return	ret;
	//}

	s32	user_type::clear(stack* s){
		//loop reference
		if(m_status != STATUS_NULL){
			m_status = STATUS_NULL;
			return	SUCCESS;
		}
		m_status = STATUS_CLEAR;
		obj* o = NULL;
		u32 member_cnt = m_member_count;
		for(u32 i = member_cnt; i > 0; --i){
			if(m_class->member_type(i - 1)
				== LOCAL_TYPE_OBJ){
				o = (obj*)m_members[i - 1];
				s->deallocate(o);
			}
		}

		if(m_parent){
			s->deallocate(m_parent);
		}
		//(1) must before (2),we can not use any member 
		//of a obj afer the memory is free
		m_status = STATUS_NULL;//(1)
		this->user_type::~user_type();
		s->free_mem(this, sizeof(user_type) 
			+ member_cnt * sizeof(s64));//(2)
		return	SUCCESS;
	}

	s64 user_type::get_member_i(stack* s, u32 idx){
		return	m_members[idx];
	}

	obj* user_type::get_member_o(stack* s, u32 idx){
		return	(obj*)m_members[idx];
	}

	s32 user_type::set_member_i(stack* s, u32 idx, s64 i){
		m_members[idx] = i;
		return	SUCCESS;
	}

	s32 user_type::set_member_o(stack* s, u32 idx, obj* o){
		obj* m = (obj*)m_members[idx];
		if(m){
			return	m->assign(s, o);
		}
		s->set_exception(ACCESS_NULL);
		return	ACCESS_NULL;
	}

	u32 user_type::get_member_count(){
		return	m_member_count;
	}

}