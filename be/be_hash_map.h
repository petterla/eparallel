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
#ifndef BE_HASH_MAP_H
#define BE_HASH_MAP_H


#include <cassert>
#include "be_utility.h"
#include "be_alloc.h"

namespace be{

	enum { hash_num_primes = 28 };

	static const size_t hash_prime_list[hash_num_primes] =
	{
		53ul,         97ul,         193ul,       389ul,       769ul,
		1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
		49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
		1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
		50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul, 
		1610612741ul, 3221225473ul, 4294967291ul
	};

	inline size_t hash_next_prime(size_t n)
	{
		const size_t* first = hash_prime_list;
		const size_t* last = hash_prime_list + (int)hash_num_primes;
		const size_t* pos = first;
		for(; pos < last && *pos <= n; ++pos);
		return pos == last ? *(last - 1) : *pos;
	}

	template<class T>
	struct hashnode{
		typedef T  value_type;
		typedef typename value_type::second_type data_type;
		typedef size_t size_type;
		value_type  m_val;
		size_type  hash_val;
		hashnode*  next;
		hashnode(const value_type& t = T()):m_val(t),hash_val(0),next(0){}
	};

	template<class KEY, class VALUE, class HASHFUN = hash<KEY>,
	class EUQAL = euqal<KEY>, class ALLOC=alloc >
	class hash_map
	{
	public:
		typedef KEY       key_type;
		typedef VALUE      data_type;
		typedef size_t      size_type;
		typedef HASHFUN      hasher;
		typedef EUQAL      key_equal;
		typedef std::pair<KEY, VALUE>  value_type;

		typedef hashnode<value_type>  node;

		friend struct _Hashtable_iterator;
		friend struct _Hashtable_const_iterator;

		struct _Hashtable_iterator {  
			typedef forward_iterator_tag    iterator_category;
			typedef typename node::data_type   value_type;
			typedef ptrdiff_t       difference_type;
			typedef typename node::size_type   size_type;
			typedef typename node::data_type&  reference;
			typedef typename node::data_type*  pointer;



			_Hashtable_iterator(node* n, hash_map* tab) 
				: m_cur(n), m_ht(tab) {}
			_Hashtable_iterator(): m_cur(0), m_ht(0) {}
			reference operator*() const { return m_cur->m_val.second; }
			pointer operator->() const { return &(operator*()); }
			_Hashtable_iterator& operator++()
			{
				const node* old = m_cur;
				m_cur = m_cur->next;
				if (!m_cur) {
					size_type bucket = m_ht->m_hash(old->m_val.first);
					while (!m_cur && ++bucket < m_ht->m_msxsize)
						m_cur = m_ht->m_buck[bucket];
				}
				return *this;
			}
			_Hashtable_iterator operator++(int)
			{
				iterator itor = *this;
				++(*this);
				return  itor;
			}
			bool operator==(const _Hashtable_iterator& it) const
			{ return m_cur == it.m_cur; }
			bool operator!=(const _Hashtable_iterator& it) const
			{ return m_cur != it.m_cur; }
			_Hashtable_iterator& operator=(const _Hashtable_iterator& it)
			{
				m_cur = it.m_cur;
				return *this;
			}
			friend class hash_map;
			private:
			node*    m_cur;
			hash_map* const m_ht;
		};

		struct _Hashtable_const_iterator {  
			typedef forward_iterator_tag   iterator_category;
			typedef const typename node::data_type  value_type;
			typedef ptrdiff_t    difference_type;
			typedef typename node::size_type  size_type;
			typedef const typename node::data_type& reference;
			typedef const typename node::data_type* pointer;



			_Hashtable_const_iterator(node* n, const hash_map* tab) 
				: m_cur(n), m_ht(tab) {}
			_Hashtable_const_iterator(): m_cur(0), m_ht(0) {}
			reference operator*() const { return m_cur->m_val.second; }
			pointer operator->() const { return &(operator*()); }
			_Hashtable_const_iterator& operator++()
			{
				const node* old = m_cur;
				m_cur = m_cur->next;
				if (!m_cur) {
					size_type bucket = m_ht->m_hash(old->m_val.first);
					while (!m_cur && ++bucket < m_ht->m_msxsize)
						m_cur = m_ht->m_buck[bucket];
				}
				return *this;
			}
			_Hashtable_const_iterator operator++(int)
			{
				iterator itor = *this;
				++(*this);
				return  itor;
			}
			bool operator==(const _Hashtable_const_iterator& it) const
			{ return m_cur == it.m_cur; }
			bool operator!=(const _Hashtable_const_iterator& it) const
			{ return m_cur != it.m_cur; }
			_Hashtable_const_iterator& operator=(const _Hashtable_const_iterator& it)
			{
				m_cur = it.m_cur;
				return *this;
			}
			friend class hash_map;
			private:
			node*    m_cur;
			const hash_map* m_ht;
		};


		typedef _Hashtable_iterator   iterator;
		typedef _Hashtable_const_iterator const_iterator;
		typedef allocator<node, ALLOC>  node_allocator; 
		typedef mem_cache<node, ALLOC>  node_cache;

		hash_map(size_type sz = 0):m_hash(hasher()),m_equal(key_equal())
					   ,m_cache(sizeof(node)),m_buck(0),m_size(0),m_msxsize(sz)
		{
			buck newbuck = (buck)ALLOC::allocate(sizeof(node*) * sz);
			for(size_type l = 0; l < sz; ++l)
				newbuck[l] = 0;
		}

		hash_map(const hash_map& other):m_hash(other.m_hash),m_equal(other.m_equal),
			m_buck(0),m_size(0),m_msxsize(0)
		{
			size_type sz = hash_next_prime(other.m_size + 1);
			resize(sz);
			const_iterator itor = (other).begin();
			for(itor; itor != other.end(); ++itor){
				insert(itor.m_cur->m_val);
			}
		}

		~hash_map(){
			size_type bksz = m_msxsize;
			for(size_type i = 0; i < bksz; ++i){
				remove_buck(i);
			}
			ALLOC::deallocate(m_buck, sizeof(node*) * m_msxsize);
		}

		hash_map& operator = (const hash_map& other){
			hash_map tmp(other);
			swap(tmp);
			return *this;
		}

		hasher  hash_funct() const 
		{
			return m_hash; 
		}
		key_equal key_eq() const
		{
			return m_equal; 
		}
		size_type size() const
		{
			return m_size;
		}
		size_type max_size() const
		{
			return m_msxsize; 
		}
		bool empty() const
		{
			return !m_size; 
		}
		void swap(hash_map& other)
		{ 
			size_type sz = m_size;
			m_size   = other.m_size;
			other.m_size = sz;
			sz    = m_msxsize;
			m_msxsize  = other.m_msxsize;
			other.m_msxsize = sz;
			buck  tbk = m_buck;
			m_buck   = other.m_buck;
			other.m_buck = tbk;
		}

		bool operator == (const hash_map& other)
		{
			if (m_size != other.m_size)
				return false;
			for (size_t n = 0; n < m_msxsize; ++n) {
				node* cur1 = m_buck[n];
				node* cur2 = other.m_buck[n];
				for ( ; cur1 && cur2 && cur1->m_val == cur2->m_val;
						cur1 = cur1->next, cur2 = cur2->next)
				{}
				if (cur1 || cur2)
					return false;
			}
			return true;
		}

		iterator begin() {
			for(size_type i = 0; i < m_size; ++i)
			{
				node* cur = m_buck[i];
				if(cur) return iterator(cur, this);
			}
			return end();
		}

		iterator end() {
			return iterator(0, this);
		}
		const_iterator begin() const {
			for(size_type i = 0; i < m_size; ++i)
			{
				node* cur = m_buck[i];
				if(cur) return const_iterator(cur, this);
			}
			return end();
		}
		const_iterator end() const {
			return const_iterator(0, this); 
		}

		std::pair<iterator,bool> insert(const value_type& obj)
		{
			if(m_size + 1 > m_msxsize){
				size_type sz = hash_next_prime(m_size + 1);
				resize(sz);
			}
			size_type hs = m_hash(obj.first);
			size_type pos = hs % m_msxsize;

			for(node* tmp = m_buck[pos]; tmp; tmp = tmp->next)
			{
				if(m_equal(obj.first, tmp->m_val.first)) 
					return std::pair<iterator, bool>(iterator(tmp,this), false);
			}
			node* nod = m_cache.allocate(obj);
			if(!nod){
				nod  = node_allocator::allocate(obj);
			}
			nod->next = m_buck[pos];
			m_buck[pos] = nod;
			nod->hash_val= hs;
			++m_size;
			return std::pair<iterator, bool>(iterator(nod,this), true);
		}

		size_type erase(const key_type& ky)
		{
			size_type cnt = 0;
			size_type hs = m_hash(ky);
			size_type pos = hs % m_msxsize;
			node*  prev= 0; 
			for(node* tmp = m_buck[pos]; tmp;)
			{
				node* next= tmp->next;
				if(m_equal(ky, tmp->m_val.first))
				{
					if(prev) prev->next = tmp->next;
					else  m_buck[pos] = tmp->next;
					if(m_cache.size() >= m_msxsize - m_size)
					{
						node_allocator::deallocate(tmp);
					}else{
						m_cache.deallcate(tmp);
					}
					++cnt;
					--m_size;
				}else{
					prev = tmp;
				}
				tmp = next;
			}
			return cnt;
		}

		void erase(iterator itor)
		{
			node* p = itor.m_cur;
			if (p) {
				const size_type n = m_hash(p->m_val.first);
				node* cur = m_buck[n];

				if (cur == p) {
					m_buck[n] = cur->next;
					if(m_cache.size() >= m_msxsize - m_size)
					{
						node_allocator::deallocate(cur);
					}else{
						m_cache.deallcate(cur);
					}
					--m_size;
				}
				else {
					node* next = cur->next;
					while (next) {
						if (next == p) {
							cur->next = next->next;
							if(m_cache.size() >= m_msxsize - m_size)
							{
								node_allocator::deallocate(next);
							}else{
								m_cache.deallcate(next);
							}
							--m_size;
							break;
						}
						else {
							cur = next;
							next = cur->next;
						}
					}
				}
			}
		}

		void erase(iterator first, iterator last)
		{
			size_type f_bucket = first.m_cur ? 
				m_hash(first.m_cur->m_val.first) : m_msxsize;
			size_type l_bucket = last.m_cur ? 
				m_hash(last.m_cur->m_val.first) : m_msxsize;

			if (first.m_cur == last.m_cur)
				return;
			else if (f_bucket == l_bucket)
				erase_bucket(f_bucket, first.m_cur, last.m_cur);
			else {
				erase_bucket(f_bucket, first.m_cur, 0);
				for (size_type n = f_bucket + 1; n < l_bucket; ++n)
					erase_bucket(n, 0);
				if (l_bucket != m_msxsize)
					erase_bucket(l_bucket, last.m_cur);
			}
		}

		iterator find(const key_type& key) {
			if(!m_size)  return iterator(0,this);
			size_type hs = m_hash(key);
			size_type pos = hs % m_msxsize;

			for(node* tmp = m_buck[pos]; tmp;)
			{
				node* next= tmp->next;
				if(m_equal(key, tmp->m_val.first))
				{
					return iterator(tmp, this);
				}
				tmp = next;
			}
			return iterator(0,this);
		}
		const_iterator find(const key_type& key) const 
		{
			size_type hs = m_hash(key);
			size_type pos = hs % m_msxsize;
			node*  prev= 0; 
			for(node* tmp = m_buck[pos]; tmp;)
			{
				node* next= tmp->next;
				if(m_equal(key, tmp->m_val.first))
				{
					return const_iterator(tmp, this);
				}
				tmp = next;
			}
			return const_iterator(0,this);
		}

		data_type& operator[](const key_type& key) {
			if(m_size + 1 > m_msxsize){
				size_type sz = hash_next_prime(m_size + 1);
				resize(sz);
			}

			size_type hs= m_hash(key);
			size_type n = hs % m_msxsize;
			node* first = m_buck[n];

			for (node* cur = first; cur; cur = cur->next)
				if (m_equal(cur->m_val.first, key))
					return cur->m_val.second;

			node* tmp = m_cache.allocate(value_type(key,data_type())); 
			if(!tmp){
				tmp = node_allocator::allocate(value_type(key,data_type()));
			}
			tmp->next = first;
			tmp->hash_val = hs;
			m_buck[n] = tmp;
			++m_size;
			return tmp->m_val.second;
		}



	private:

		void remove_buck (size_type i)
		{
			node* nod = m_buck[i];
			for(; nod; ){
				node* next = nod->next;
				if(m_cache.size() >= m_msxsize - m_size)
				{
					node_allocator::deallocate(nod, 1);
				}else{
					m_cache.deallcate(nod);
				}
				nod = next;
			}
		}

		void erase_bucket(const size_type n, node* first, node* last)
		{
			node* cur = m_buck[n];
			if (cur == first)
				erase_bucket(n, last);
			else {
				node* next;
				for (next = cur->next; 
						next != first; 
						cur = next, next = cur->next)
					;
				while (next != last) {
					cur->next = next->next;
					//  _M_delete_node(next);
					if(m_cache.size() >= m_msxsize - m_size)
					{
						node_allocator::deallocate(next, 1);
					}else{
						m_cache.deallcate(next);
					}
					next = cur->next;
					--m_size;
				}
			}
		}

		void erase_bucket(const size_type n, node* last)
		{
			node* cur = m_buck[n];
			while (cur != last) {
				node* next = cur->next;
				if(m_cache.size() >= m_msxsize - m_size)
				{
					node_allocator::deallocate(cur, 1);
				}else{
					m_cache.deallcate(cur);
				}
				cur = next;
				m_buck[n] = cur;
				--m_size;
			}
		}

		void resize (size_type sz)
		{
			assert(m_size <= sz);
			buck newbuck = (buck)ALLOC::allocate(sizeof(node*) * sz);
			for(size_type l = 0; l < sz; ++l)
				newbuck[l] = 0;
			for(size_type i = 0; i < m_size; ++i)
			{
				node* cur = m_buck[i];
				for ( ; cur ;)
				{
					node* next = cur->next;
					cur->next = newbuck[cur->hash_val % sz];
					newbuck[cur->hash_val] = cur;
					cur = next;
				}
			}
			ALLOC::deallocate(m_buck, sizeof(node*) * m_msxsize);
			m_buck  = newbuck;
			m_msxsize = sz;

		}

	private:
		typedef  node** buck;

		mutable hasher  m_hash;
		mutable key_equal m_equal;
		node_cache   m_cache;
		buck    m_buck;
		size_type   m_size;
		size_type   m_msxsize;

	};


};

#endif/*BE_HASH_MAP_H*/
