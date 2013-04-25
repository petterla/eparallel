#ifndef ZVM_POOL_H
#define ZVM_POOL_H

#include "zvm_type.h"
#include "zvm_lock.h"

namespace zvm{

	template<class T>
	class pool{
	public:
		pool():m_head(NULL){

		}

		T* allocate(){

			if(!m_head){
				return	(T*)new	node();
			}

			node* t = m_head;
			m_head = t->m_next;

			return	&t->m_t;
		}

		void deallocate(T* t){

			node* n = (node*)t;
			n->m_next = m_head;
			m_head = n;

		}

	private:
		struct node{
			T m_t;
			node* m_next;

			node():m_next(NULL){

			}
		};

		node* m_head;
	};

	template<class T>
	class lock_pool{
	public:
		lock_pool():m_head(NULL){

		}

		T* allocate(){

			if(!m_head){
				return	(T*)new	node();
			}

			m_lock.lock(1);
			node* t = m_head;
			m_head = t->m_next;
			m_lock.unlock(1);

			return	&t->m_t;
		}

		void deallocate(T* t){

			node* n = (node*)t;

			m_lock.lock(1);
			n->m_next = m_head;
			m_head = n;
			m_lock.unlock(1);

		}

	private:
		struct node{
			T m_t;
			node* m_next;

			node():m_next(NULL){

			}
		};

		node* m_head;
		simple_lock m_lock;
	};

}

#endif/*ZVM_POOL_H*/

