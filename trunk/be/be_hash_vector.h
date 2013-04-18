#ifndef	BE_HASH_VECTOR_H
#define BE_HASH_VECTOR_H

#include "be_utility.h"
#include "be_vector.h"

namespace	be{
	template<class KEY,	class HASHFUN	=	hash<KEY>, class EUQAL	=	euqal<KEY>, class ALLOC=alloc >
	class	hash_vector{
	public:
		typedef	std::pair<KEY,	size_t>		node;	
		typedef	vector<node,	ALLOC>		vector_t;

		class hliterator
		{
		public:
			typedef random_access_iterator_tag			iterator_category;
			typedef KEY									value_type;
			typedef ptrdiff_t							difference_type;
			typedef value_type*							pointer;
			typedef value_type&							reference;
			typedef	typename	vector_t::iterator		vecitor;
			hliterator(vecitor	itor	=	vecitor()):m_itor(itor){}

			hliterator&	operator++(){
				++m_itor;
				return	*this;
			}
			hliterator	operator++(int){
				hliterator	other	=	*this;
				++*this;
				return	other;
			}

			reference	operator[]	(difference_type offset)const
			{
				return (*(*this + offset));
			}

			hliterator&	operator	+=	(difference_type	sz)
			{
				m_itor	+=	sz;
				return	*this;
			}

			hliterator	operator	+	(difference_type	sz)const
			{
				hliterator	itor	=	*this;
				itor	+=	sz;
				return	itor;
			}

			hliterator&	operator--(){
				--m_itor;
				return	*this;
			}
			hliterator	operator--(int){
				hliterator	other	=	*this;
				++*this;
				return	other;
			}
			hliterator&	operator	-=	(difference_type	sz)
			{
				m_itor	-=	sz;
				return	*this;
			}

			hliterator	operator	-	(difference_type	sz)const
			{
				hliterator	itor	=	*this;
				itor	-=	sz;
				return	itor;
			}

			difference_type	operator	-	(const	hliterator&	other)const
			{
				return	m_itor	-	other.m_itor;
			}


			reference	operator*()
			{
				return	(*m_itor).first;
			}

			pointer	operator->()
			{
				return	&(*this);
			}

			bool	operator	==	(const	hliterator&	other)const
			{
				return	m_itor	==	other.m_itor;
			}

			bool	operator	!=	(const	hliterator&	other)const
			{
				return	m_itor	!=	other.m_itor;
			}
			bool	operator	>	(const	hliterator&	other)const
			{
				return	m_itor	>	other.m_itor;
			}
			bool	operator	<	(const	hliterator&	other)const
			{
				return	m_itor	<	other.m_itor;
			}

			bool	operator	>=	(const	hliterator&	other)const
			{
				return	m_itor	>=	other.m_itor;
			}
			bool	operator	<=	(const	hliterator&	other)const
			{
				return	m_itor	<=	other.m_itor;
			}

		protected:
		private:
			vecitor	m_itor;
		};

		class const_hliterator
		{
		public:
			typedef random_access_iterator_tag				iterator_category;
			typedef const	KEY								value_type;
			typedef ptrdiff_t								difference_type;
			typedef value_type*								pointer;
			typedef value_type&								reference;
			typedef	typename	vector_t::const_iterator	vecitor;
			const_hliterator(vecitor	itor	=	vecitor()):m_itor(itor){}

			const_hliterator&	operator++(){
				++m_itor;
				return	*this;
			}
			const_hliterator	operator++(int){
				hliterator	other	=	*this;
				++*this;
				return	other;
			}

			reference	operator[]	(difference_type offset)const
			{
				return (*(*this + offset));
			}

			const_hliterator&	operator	+=	(difference_type	sz)
			{
				m_itor	+=	sz;
				return	*this;
			}

			const_hliterator	operator	+	(difference_type	sz)const
			{
				const_hliterator	itor	=	*this;
				itor	+=	sz;
				return	itor;
			}

			const_hliterator&	operator--(){
				--m_itor;
				return	*this;
			}
			const_hliterator	operator--(int){
				const_hliterator	other	=	*this;
				++*this;
				return	other;
			}
			const_hliterator&	operator	-=	(difference_type	sz)
			{
				m_itor	-=	sz;
				return	*this;
			}

			const_hliterator	operator	-	(difference_type	sz)const
			{
				const_hliterator	itor	=	*this;
				itor	-=	sz;
				return	itor;
			}

			difference_type	operator	-	(const	const_hliterator&	other)const
			{
				return	m_itor	-	other.m_itor;
			}


			reference	operator*()
			{
				return	(*m_itor).first;
			}

			pointer	operator->()
			{
				return	&(*this);
			}

			bool	operator	==	(const	const_hliterator&	other)	const
			{
				return	m_itor	==	other.m_itor;
			}

			bool	operator	!=	(const	const_hliterator&	other)	const
			{
				return	m_itor	!=	other.m_itor;
			}
			bool	operator	>	(const	const_hliterator&	other)	const
			{
				return	m_itor	>	other.m_itor;
			}
			bool	operator	<	(const	const_hliterator&	other)	const
			{
				return	m_itor	<	other.m_itor;
			}

			bool	operator	>=	(const	const_hliterator&	other)	const
			{
				return	m_itor	>=	other.m_itor;
			}
			bool	operator	<=	(const	const_hliterator&	other)	const
			{
				return	m_itor	<=	other.m_itor;
			}

		protected:
		private:
			vecitor	m_itor;
		};

		typedef	hliterator			iterator;
		typedef	const_hliterator	const_iterator;

		hash_vector(){}
		~hash_vector(){}

		void	push_front	(const	KEY&	ky)
		{
			m_vector.push_front(node(ky,	m_hasher(ky)));
		}
		void	push_back	(const	KEY&	ky)
		{
			m_vector.push_back(node(ky,	m_hasher(ky)));
		}

		void	pop_front	(	)
		{
			m_vector.pop_front(	);
		}

		void	pop_back	(	)
		{
			m_vector.pop_back(	);
		}

		iterator	find(const KEY& ky)
		{
			typename vector_t::iterator	itor	=	m_vector.begin();
			typename vector_t::iterator	itend	=	m_vector.end();
			size_t	hashval	=	m_hasher(ky);
			for(;	itor	!=	itend;	++itor){
				if(hashval	==	(*itor).second	&&	m_euqal(ky,	(*itor).first)){
					return	itor;
				}
			}
			return	itend;
		}

		const_iterator	find(const KEY& ky)const 
		{
			typename vector_t::const_iterator	itor	=	m_vector.begin();
			typename vector_t::const_iterator	itend	=	m_vector.end();
			size_t	hashval	=	m_hasher(ky);
			for(;	itor	!=	itend;	++itor){
				if(hashval	==	(*itor).second	&&	m_euqal(ky,	(*itor).first)){
					return	itor;
				}
			}
			return	itend;
		}



		iterator	begin()
		{
			return	m_vector.begin();
		}

		const_iterator	begin()const
		{
			return	m_vector.begin();	
		}

		iterator	end()
		{
			return	m_vector.end();
		}

		const_iterator	end()const
		{
			return	m_vector.end();
		}

		KEY&	front()
		{
			return	*begin();
		}

		KEY&	back()
		{
			return	*end();
		}

		KEY&	operator[](ptrdiff_t	off)
		{
			return	m_vector[off].first;
		}

		const	KEY&	operator[](ptrdiff_t	off)const
		{
			return	m_vector[off].first;
		}

		bool	empty()const
		{
			return	!m_vector.size();
		}

		size_t	size()const
		{
			return	m_vector.size();
		}


	private:
		vector_t			m_vector;
		mutable	HASHFUN		m_hasher;
		mutable	EUQAL		m_euqal;
	};
};

#endif/*BE_HASH_VECTOR_H*/
