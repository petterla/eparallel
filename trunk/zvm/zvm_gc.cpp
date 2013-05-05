#include "zvm_gc.h"
#include "zvm_stack.h"
#include "zvm_lock.h"

namespace zvm{
	stack	g_gc_stack;

	struct entnode{
		entnode* m_prev;
		entnode* m_next;
		gc_type* m_e;

		entnode():
			m_prev(NULL),
			m_next(NULL),
			m_e(NULL){

		}

	};
	
	class	entlist{
	public:
		entlist(stack& s)
			:m_size(0),m_stack(s){
			m_head.m_next = &m_head;
			m_head.m_prev = &m_head;
			m_cur = &m_head;
		}
		s32 add(entnode* n){
			ZVM_DEBUG_PRINT("entlist::add:%p\n", n->m_e);
			m_lock.lock(0);
			n->m_prev = m_head.m_prev;
			m_head.m_prev->m_next = n;
			n->m_next = &m_head;
			m_head.m_prev = n;
			++m_size;
			m_lock.unlock(0);
			assert(m_size > 0);

			return	SUCCESS;
		}
		s32 del(entnode* n){
			ZVM_DEBUG_PRINT("entlist::del:%p\n", n->m_e);
			m_lock.lock(0);
			if(n == m_cur){
				m_cur = n->m_next;
			}
			n->m_prev->m_next = n->m_next;
			n->m_next->m_prev = n->m_prev;
			--m_size;
			m_lock.unlock(0);
			n->m_prev = NULL;
			n->m_next = NULL;
			m_stack.free_mem(n, sizeof(entnode));
			assert(m_size >= 0);

			return	SUCCESS;
		}
		entnode* get(){
			if(!m_size){
				return	NULL;
			}
			entnode* e = NULL;
			m_lock.lock(0);
			e = m_cur;
			if(e == &m_head)
				e = e->m_next;
			m_cur = e->m_next;
			m_lock.unlock(0);
			return	e;
		}
		//do not lock,because if type_gc,it can only 
		//destroy at the gc thread
		//so n in (1) is always safe!
		s32 check_one_node(){
			s32 num = 0;
	
			entnode* n = get();//(1)
			if(n){
				ZVM_DEBUG_PRINT("entlist::check_one_node:%p\n", n->m_e);
				num += n->m_e->try_collect(&g_gc_stack);
			}

			return	num;
		}
		s32 size(){
			return	m_size;
		}
	private:
		spin_lock m_lock;
		entnode  m_head;
		entnode* m_cur;
		s32 m_size;
		stack& m_stack;
	};


	bool gc_type::find_loop(stack* s, entry* e, bool first){
		bool ret = false;
		//loop path
		if(m_status == STATUS_SIGN){
			if((entry*)this == e){
				ret = true;
				goto exit;
			}
			ret = false;
			goto exit;
		}
		m_status = STATUS_SIGN;
		begin(s);
		for(obj* r = get_refer_obj(s);r;r = get_refer_obj(s)){
			//notice, all reference obj should check the loop
			bool t = r->find_loop(s, e, false);
			ret = ret || t;
		}
		m_status = STATUS_NULL;
exit:
		if(ret)
			m_flag = FLAG_LOOP;
		if(!first && m_flag)
			++m_loop_cnt;
		ZVM_DEBUG_PRINT("find_loop:%p ,loop_cnt:%d ,ref_cnt:%d "
			",flag:%d, first:%d, ret:%d\n",
			this, m_loop_cnt, ref_count(), m_flag, first, ret);
		return	ret;
	}
 
	bool gc_type::check_live(stack* s){
		bool ret = false;
		if(m_flag != FLAG_LOOP){
			ZVM_DEBUG_PRINT("check_live:%p ,loop_cnt:%d ,ref_cnt:%d "
				",flag:%d, m_flag != FLAG_LOOP, not live\n",
				this, m_loop_cnt, ref_count(), m_flag);
			return	false;
		}
		if(m_loop_cnt < ref_count()){
			ZVM_DEBUG_PRINT("check_live:%p ,loop_cnt:%d ,ref_cnt:%d "
				",flag:%d, m_loop_cnt < ref_count(), live\n",
				this, m_loop_cnt, ref_count(), m_flag);
			return true;
		}
		if(m_status == STATUS_CHECK){
			ZVM_DEBUG_PRINT("check_live:%p ,loop_cnt:%d ,ref_cnt:%d "
				",flag:%d, m_status == STATUS_CHECK, not live\n",
				this, m_loop_cnt, ref_count(), m_flag);
			return false;
		}
		m_status = STATUS_CHECK;

		begin(s);
		for(obj* r = get_refer_obj(s);r;r = get_refer_obj(s)){
			ret = r->check_live(s);
			if(ret){
				break;
			}
		}
		m_status = STATUS_NULL;
		return	ret;
	}

	bool gc_type::reset(stack* s, bool force){
		bool ret = true;
		if(force)
			m_status = STATUS_NULL;
		if(m_status == STATUS_RESET)
			return	true;
		m_status = STATUS_RESET;
		m_flag = FLAG_NULL;
		m_loop_cnt = 0;
		begin(s);
		for(obj* r = get_refer_obj(s);r;r = get_refer_obj(s)){
			r->reset(s);
		}
		m_status = STATUS_NULL;
		return	ret;
	}

	static entlist g_e_list(g_gc_stack);

	s32 gc::add(gc_type* e){
		entnode* n = (entnode*)g_gc_stack.alloc_mem(sizeof(entnode));
		if(n){
			new (n) entnode();
			n->m_e = e;
			e->set_ext(n);
			g_e_list.add(n);
			return	SUCCESS;
		}
		return	OUT_OF_MEM;
	}
	s32 gc::del(gc_type* e){
		entnode* n = (entnode*)e->get_ext();
		if(n){
			g_e_list.del(n);
		}
		return	SUCCESS;
	}
	s32 gc::try_collect(s32 cnt){
		s32 total = cnt;
		s32 num = 0;
		if(cnt < 0){
			total = g_e_list.size();
		}
		for(int i = 0; i < total; ++i){
			num += g_e_list.check_one_node();
		}	
		return	num;
	}
}
