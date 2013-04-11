#ifndef ZVM_OBJ_H
#define ZVM_OBJ_H

#include "zvm_type.h"
#include "zvm_err_no.h"
#include "zvm_lock.h"
#include "be_atomic.h"
#include <string>

#define ZVM_ENTRY_DEBUG

namespace zvm{

	class stack;
	class obj;

	class entry{
	public:

		entry()
			:m_ref_cnt(1){
#ifdef ZVM_ENTRY_DEBUG
			++s_ent_cnt;
#endif
		}

		virtual	~entry(){
#ifdef ZVM_ENTRY_DEBUG
			--s_ent_cnt;
#endif
		}

		virtual	std::string type() const{
			return	"";
		}

		virtual s32 init(stack* s){
			return	SUCCESS;
		}

		virtual s32 init_super(stack* s){
			return	SUCCESS;
		}

		virtual	entry* reference(stack* s){
			be::atomic_increament32(&m_ref_cnt);
			return	this;
		}

		virtual entry* clone(stack* s){
			return	NULL;
		}
		virtual s64 get_member_i(stack* s, u32 idx){
			return	0;
		}

		virtual obj* get_member_o(stack* s, u32 idx){
			return	NULL;
		}

		virtual s32 set_member_i(stack* s, u32 idx, s64 i){
			return	SUCCESS;
		}

		virtual s32 set_member_o(stack* s, u32 idx, obj* o){
			return	SUCCESS;
		}

		virtual	s32 recycle(stack* s){
			be::s32 c = be::atomic_decreament32(&m_ref_cnt);
			if(c <= 0){
				return	clear(s);
			}
			return	SUCCESS;
		}

		virtual s32 clear(stack* s){
			delete this;
			return	SUCCESS;
		}

		virtual u32 get_member_count(){
			return	0;
		}

		virtual bool check_live(stack* s){
			return	true;
		}

		//check if loop reference
		//if not return true;else return false
		virtual bool find_loop(stack* s, entry* e, bool first){
			return	true;
		}

		virtual bool reset(stack* s){
			return	true;
		}

		virtual obj* create_entry_obj(stack* s){
			return	NULL;
		}

		s32 ref_count(){
			return	(s32)m_ref_cnt;
		}

		s32 inc_ref(){
			return	be::atomic_increament32(&m_ref_cnt);
		}

		s32 dec_ref(){
			return	be::atomic_decreament32(&m_ref_cnt);
		}

	private:
		volatile be::s32 m_ref_cnt;

#ifdef ZVM_ENTRY_DEBUG
	public:
		static int s_ent_cnt;
#endif
	};

	class obj{
	public:
		obj(entry* ent = NULL)
			:m_ent(ent){

		}

		~obj(){
			set_entry((stack*)1, NULL);
		}

		s32 set_entry(stack* s, entry* ent){

			entry* e = (entry*)m_ent;
			lock(s);
			m_ent = ent;
			unlock(s);
			if(e){
				return e->recycle(s);
			}

			return	SUCCESS;
		}

		s32 clear(stack* s){
			return	set_entry(s, NULL);
		}

		s32 force_clear(stack* s){
			entry* e = (entry*)m_ent;
			lock(s);
			m_ent = NULL;
			unlock(s);
			if(e){
				return e->entry::recycle(s);
			}

			return	SUCCESS;
		}

		s32	lock(stack* s){
			return	m_lock.lock((s32)(s64)s);
		}

		entry* entry_reference(stack* s){

			lock(s);
			auto_simple_unlock ul(m_lock, (s32)(s64)s);

			if(m_ent){
				return	m_ent->reference(s);
			}
			return	NULL;
		}

		entry* entry_clone(stack* s){
			lock(s);
			auto_simple_unlock ul(m_lock, (s32)(s64)s);

			if(m_ent){
				return	m_ent->clone(s);
			}
			return	NULL;
		}

		s32 assign(stack* s, obj* o){
			entry* e = o->entry_reference(s);
			if(!e){
				return	OUT_OF_MEM;
			}
			return	set_entry(s, e);
		}

		s32 unlock(stack* s){
			return	m_lock.unlock((s32)(s64)s);
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

		bool find_loop(stack* s, entry* e, bool fisrt){
			lock(s);
			auto_simple_unlock ul(m_lock, (s32)(s64)s);
			if(m_ent)
				return	m_ent->find_loop(s, e, fisrt);
			return	true;
		}

		bool check_live(stack* s){
			lock(s);
			auto_simple_unlock ul(m_lock, (s32)(s64)s);
			if(m_ent)
				return	m_ent->check_live(s);
			return	true;
		}

		bool reset(stack* s){
			lock(s);
			auto_simple_unlock ul(m_lock, (s32)(s64)s);
			if(m_ent)
				return	m_ent->reset(s);
			return	true;
		}

		s32 init(stack* s){
			if(m_ent){
				return	m_ent->init(s);
			}
			return	SUCCESS;
		}

		obj* create_entry_obj(stack* s){
			lock(s);
			auto_simple_unlock ul(m_lock, (s32)(s64)s);
			assert(m_ent);
			return	m_ent->create_entry_obj(s);
		}

		s64 get_member_i(stack* s, u32 idx){
			lock(s);
			auto_simple_unlock ul(m_lock, (s32)(s64)s);
			assert(m_ent);

			return	m_ent->get_member_i(s, idx);
		}

		obj* get_member_o(stack* s, u32 idx){
			lock(s);
			auto_simple_unlock ul(m_lock, (s32)(s64)s);
			assert(m_ent);
			return	m_ent->get_member_o(s, idx);
		}

		s32 set_member_i(stack* s, u32 idx, s64 i){
			lock(s);
			auto_simple_unlock ul(m_lock, (s32)(s64)s);
			assert(m_ent);
			return	m_ent->set_member_i(s, idx, i);
		}

		s32 set_member_o(stack* s, u32 idx, obj* o){
			lock(s);
			auto_simple_unlock ul(m_lock, (s32)(s64)s);
			assert(m_ent);
			return	m_ent->set_member_o(s, idx, o);
		}

		u32 get_member_count(stack* s){
			lock(s);
			auto_simple_unlock ul(m_lock, (s32)(s64)s);
			assert(m_ent);
			return	m_ent->get_member_count();
		}

		entry* get_entry(){
			return	m_ent;
		}

	private:
		simple_lock m_lock;
		entry* m_ent;
	};

}

#endif/*ZVM_OBJ_H*/