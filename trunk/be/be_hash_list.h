#ifndef BE_HASH_LIST_H
#define BE_HASH_LIST_H

#include "be_hash_map.h"
#include "be_list.h"


namespace	be{
	template<class KEY,	class HASHFUN	=	hash<KEY>, class EUQAL	=	euqal<KEY>, class ALLOC=alloc >
	class	hash_list{
	public:

		typedef	list<KEY,	ALLOC>				list_t;
		typedef	typename	list_t::iterator	   	hval;
		typedef	hash_map<KEY, hval, HASHFUN, EUQAL, ALLOC>	map;

		template<class VT>
		class hliterator
		{
		public:
			typedef bidirectional_iterator_tag		iterator_category;
			typedef VT					value_type;
			typedef typename	hval::difference_type	difference_type;
			typedef value_type*				pointer;
			typedef value_type&				reference;
			hliterator(const hval& hv):m_itor(hv){}

			hliterator&	operator++(){
				++m_itor;
				return	*this;
			}
			hliterator	operator++(int){
				hliterator	other	=	*this;
				++*this;
				return	other;
			}
			reference	operator*()
			{
				return	(*m_itor);
			}

			pointer	operator->()
			{
				return	&(*this);
			}

			bool	operator	==	(const	hliterator&	other)
			{
				return	m_itor	==	other.m_itor;
			}

			bool	operator	!=	(const	hliterator&	other)
			{
				return	m_itor	!=	other.m_itor;
			}

		protected:
		private:
			hval	m_itor;
		};

		typedef	hliterator<KEY>			iterator;
		typedef	hliterator<const KEY>	const_iterator;
		hash_list(){}
		~hash_list(){}
		void	push_back(const KEY& ky)
		{
			if(m_map.find(ky) == m_map.end())
			{
				hval	itor	=	m_list.insert(m_list.end(), ky);
				m_map[ky]	=	itor;
			}
		}
		void	push_front(const KEY& ky)
		{
			if(m_map.find(ky) == m_map.end())
			{
				hval	itor	=	m_list.insert(m_list.begin(), ky);
				m_map[ky]	=	itor;
			}
		}

		void	pop_back()
		{
			if(m_list.size()){
				m_map.erase(m_list.back());
				m_list.pop_back();
			}
		}

		void	pop_front()
		{
			if(m_list.size()){
				m_map.erase(m_list.front());
				m_list.pop_front();
			}
		}

		iterator	find(const KEY& ky)
		{
			if(m_map.find(ky) == m_map.end())
			{
				return	iterator(m_list.end());
			}
			return	iterator(m_map[ky]);
		}

		const_iterator	find(const KEY& ky)const 
		{
			if(m_map.find(ky) == m_map.end())
			{
				return	const_iterator(m_list.end());
			}
			return	const_iterator(m_map[ky]);
		}

		iterator	begin()
		{
			return	iterator(m_list.begin());
		}

		const_iterator	begin()const
		{
			return	const_iterator(m_list.begin());
		}

		iterator	end()
		{
			return	iterator(m_list.end());
		}

		const_iterator	end()const
		{
			return	const_iterator(m_list.end());
		}

		KEY&	front()
		{
			return	m_list.front();
		}

		KEY&	back()
		{
			return	m_list.back();
		}

		bool	empty()const
		{
			return	!m_list.size();
		}

		size_t	size()const
		{
			return	m_list.size();
		}

		void	erase(iterator	pos)
		{
			m_map.erase((*(hval)pos));
			m_list.erase((hval)pos);
		}

		size_t	erase(const KEY& ky)
		{
			if(find(ky) != end())
			{
				hval	vl	=	m_map[ky];
				m_map.erase(ky);
				m_list.erase(vl);
				return	1;
			}
			return	0;
		}
	private:

		list_t	m_list;
		map	m_map;
	};
};


#endif/*BE_HASH_LIST_H*/

