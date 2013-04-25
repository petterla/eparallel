/*
* Copyright (c) 2010
* efgod@126.com.
*
* Permission to use, copy, modify, distribute and sell this software
* and its documentation for any purpose is hereby granted without fee,
* provided that the above copyright notice appear in all copies and
* that both that copyright notice and this permission notice appear
* in supporting documentation.  efgod@126.com. makes no
* representations about the suitability of this software for any
* purpose.  It is provided "as is" without express or implied warranty.
*/


#ifndef	BE_LIST_H
#define BE_LIST_H

#include <cassert>
#include "be_utility.h"
#include "be_alloc.h"

namespace	be{


	template<class	T,	class ALLOC	=	alloc> 
	class list
	{
	public:
		template<class VT>
		struct	node{
			typedef	VT	value_type;
			value_type		val;
			node*			next;
			node*			prev;
			node(value_type	t	=	value_type(),	node*	nxt	=	0,	node*	prv	=	0):
			val(t),	next(nxt),	prev(prv){}
		};
		template<class NODE>
		class	list_iterator{
		public:
			typedef	bidirectional_iterator_tag		iterator_category;
			typedef ptrdiff_t						difference_type;
			typedef size_t							size_type;
			typedef	typename	NODE::value_type	value_type;
			typedef	value_type&						reference;
			typedef	value_type*						pointer;

			list_iterator(NODE*	nod	=	0):m_cur(nod){
			}
			reference	operator*()
			{
				return	m_cur->val;
			}
			pointer		operator->()
			{
				return	&(m_cur->val);
			}
			list_iterator&	operator++()
			{
				m_cur	=	m_cur->next;
				return	*this;
			}
			list_iterator	operator++(int)
			{
				list_iterator	itor	=	*this;
				++*this;
				return	itor;
			}
			list_iterator&	operator--()
			{
				m_cur	=	m_cur->prev;
				return	*this;
			}
			list_iterator	operator--(int)
			{
				list_iterator	itor	=	*this;
				--*this;
				return	itor;
			}

			bool	operator	==	(const	list_iterator&	other)
			{
				return	m_cur	==	other.m_cur;
			}
			bool	operator	!=	(const	list_iterator&	other)
			{
				return	m_cur	!=	other.m_cur;
			}
		private:
			friend	class	list;
			NODE*	m_cur;
		};

		typedef	node<T>							node_type;
		typedef	node<const	T>					const_node_type;
		typedef	list_iterator<node_type>		iterator;
		typedef	list_iterator<const_node_type>	const_iterator;
		typedef	T								value_type;
		typedef	T&								reference;
		typedef	T*								pointer;
		typedef	const	T&						const_reference;
		typedef	const	T*						const_pointer;
		typedef	size_t							size_type;
		typedef	allocator<node_type,ALLOC>		node_allocator;
		typedef	mem_cache<node_type,ALLOC>		node_cache;

		list():m_head(node_allocator::allocate()),m_size(0),m_cache(sizeof(node_type))
		{
			m_head->next	=	m_head;
			m_head->prev	=	m_head;
		}

		~list()
		{
			clear();
			node_allocator::deallocate(m_head);
		}

		list(const	list&	other){
			iterator	itor	=	other.begin();
			for(itor;	itor	!=	other.end();	++itor){
				push_back(*itor);
			}
		}

		list&	operator	=	(const	list&	other){
			list	tmp(other);
			swap(tmp);
			return	*this;
		}

		size_type	size()const
		{
			return	m_size;
		}
		bool		empty()const
		{
			return	!m_size;
		}
		reference	front()
		{
			return	m_head->next->val;
		}
		const_reference	front()	const
		{
			return	m_head->next->val;
		}
		reference	back()
		{
			return	m_head->prev->val;
		}
		const_reference	back()	const
		{
			return	m_head->prev->val;
		}

		iterator	begin()
		{
			return	iterator(m_head->next);
		}

		const_iterator	begin()const
		{
			return	const_iterator((const_node_type*)m_head->next);
		}

		iterator	end()
		{
			return	iterator(m_head);
		}

		const_iterator	end()const
		{
			return	const_iterator((const_node_type*)m_head);
		}

		iterator insert(iterator pos, const T& x)
		{
			node_type*	nod	=	pos.m_cur;
			node_type*	tmp	=	0;
			tmp				=	m_cache.allocate(x);
			if(!tmp){
				tmp	=	node_allocator::allocate(x);
			}
			tmp->next		=	nod;
			tmp->prev		=	nod->prev;
			nod->prev->next	=	tmp;
			nod->prev		=	tmp;
			++m_size;
			return	iterator(tmp);
		}


		template<class ForwardItor>
		void	insert(iterator pos,	ForwardItor	first,	ForwardItor	last)
		{
			for(ForwardItor	itor	=	first;	itor	!=	last;	++itor){
				insert(itor,	*itor);
			}
		}

		iterator erase(iterator position) {
			node_type* next_node = (node_type*) (position.m_cur->next);
			node_type* prev_node = (node_type*) (position.m_cur->prev);
			prev_node->next = next_node;
			next_node->prev = prev_node;
			if(m_cache.size()	<	m_size){
				m_cache.deallcate(position.m_cur);
			}else{
				node_allocator::deallocate(position.m_cur);
			}
			--m_size;
			return iterator(next_node);
		}

		iterator	erase(iterator first, 	iterator last)
		{
			iterator	ret	=	first;
			while (first != last)
				ret	=	erase(first++);
			return ret;
		}

		void		push_back(const T&	t)
		{
			insert(end(),	t);
		}

		void		push_front(const T&	t)
		{
			insert(begin(),	t);
		}

		void pop_front() { 
			if(m_size)
			{
				erase(begin()); 
			}
		}
		void pop_back() { 
			if(m_size)
			{
				iterator tmp = end();
				erase(--tmp);
			}
		}

		void		clear()
		{
			node_type*	nod	=	m_head->next;
			for(;	nod	!=	m_head;	){
				node_type*	next	=	nod->next;
				node_allocator::deallocate(nod);
				nod	=	next;
				--m_size;
			}
			assert(m_size	==	0);
		}

		void		swap(list&	other)
		{
			node_type*	nod	=	m_head;
			size_type	sz	=	m_size;
			m_head		=	other.m_head;
			m_size		=	other.m_size;
			other.m_head=	nod;
			other.m_size=	sz;
		}
	protected:
	private:

		node_type*	m_head;
		size_type	m_size;
		node_cache	m_cache;
	};

};

#endif/*BE_LIST_H*/

