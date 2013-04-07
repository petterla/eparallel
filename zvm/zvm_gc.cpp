#include "zvm_gc.h"
#include "zvm_stack.h"
#include "zvm_lock.h"

namespace zvm{
	stack	g_gc_stack;

	struct entnode{
		entnode* m_prev;
		entnode* m_next;
		type_gc* m_e;

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
		//destory at the gc thread
		//so n in (1) is always safe!
		s32 check_one_node(){
			s32 num = 0;
	
			entnode* n = get();//(1)
			if(n){
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

	static entlist g_e_list(g_gc_stack);

	s32 gc::add(type_gc* e){
		entnode* n = (entnode*)g_gc_stack.alloc_mem(sizeof(entnode));
		if(n){
			n->entnode::entnode();
			n->m_e = e;
			e->set_ext(n);
			g_e_list.add(n);
			return	SUCCESS;
		}
		return	OUT_OF_MEM;
	}
	s32 gc::del(type_gc* e){
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