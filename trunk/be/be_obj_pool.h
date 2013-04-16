#ifndef	BE_OBJ_POOL
#define BE_OBJ_POOL

#include "be_alloc.h"
#include "be_list.h"

namespace	be{
	template<class T,	class ALLOC	=	alloc,	class OBJALLOC	=	allocator<T,	alloc>	>
	class	obj_pool{
	public:
		~obj_pool(){
			objlist::iterator	itor	=	m_objs.begin();
			objlist::iterator	itend	=	m_objs.end();
			for(itor;	itor	!=	itend;	++itor)
				OBJALLOC::deallocate(*itor);
		}
		T*		allocate(){
			if(m_objs.size())
				return	m_objs.front();
			return	0;
		}
		void	deallocate(T*	pt)
		{
			m_objs.push_front(pt);
		}
	private:
		typedef	list<T*,	ALLOC>	objlist;
		objlist	m_objs;
	};
};

#endif/*BE_OBJ_POOL*/