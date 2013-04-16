#ifndef BE_FIFO_H
#define BE_FIFO_H

#include "be_atomic.h"
#include "be_alloc.h"

#pragma warning(push) 

#pragma warning(disable:4311)
#pragma warning(disable:4312)

namespace	be{

template<class T,	class ALLOC	=	alloc>
class	fifo{
private:
	struct	node{
		T		val;
		node*	next;
		node(const	T&	t):val(t),	next(NULL)
		{

		}
		node(const node& t):val(t.val),next(t.next)
		{

		}
	};
	typedef	allocator<node,	ALLOC>	node_allocator;
public:

	fifo()
	{

	}
	~fifo()
	{
		while(m_info.head)	pop();
	}

	bool	push(const	T&	t)
	{
		bool	ret		=	true;
		node*	nod		=	node_allocator::allocate(t);
		info	oldinfo	=	*(info*)&m_info;
		info	newinfo;

		nod->next		=	NULL;
		newinfo.tail	=	(long)nod;
		do{
			oldinfo	=	*(info*)&m_info;

			if(!oldinfo.head){
				newinfo.head	=	(long)nod;
			}else{
				newinfo.head	=	oldinfo.head;
			}

			assert(newinfo.head);
		}while(atomic_compare_exchange64((volatile	long	long*)&m_info,
			*(long	long*)&newinfo,	*(long	long*)&oldinfo)	
			!=	*(long	long*)&oldinfo);
		if(oldinfo.tail){
			((node*)oldinfo.tail)->next	=	nod;
		}
		atomic_increament32(&m_sz);
		return	ret;
	}

	bool	pop(T&	t)
	{
		bool	ret		=	false;
		node*	nod		=	NULL;
		info	oldinfo	=	*(info*)&m_info;
		info	newinfo;
		do{
			oldinfo	=	*(info*)&m_info;
			nod		=	(node*)oldinfo.head;
			if(!nod)	break;
			if(oldinfo.head	!=	oldinfo.tail)
			{
				newinfo.head	=	(long)nod->next;
				newinfo.tail	=	oldinfo.tail;
				if(!newinfo.head)	return	false;//中间有间断
			}else{
				newinfo.head	=	NULL;
				newinfo.tail	=	NULL;
			}
		}while(atomic_compare_exchange64((volatile	long	long*)&m_info,
			*(long	long*)&newinfo,	*(long	long*)&oldinfo)
				!=	*(long	long*)&oldinfo);
		if(nod){
			t	=	nod->val;
			node_allocator::deallocate(nod);
			ret	=	true;
			atomic_decreament32(&m_sz);
			
		}
		return	ret;
	}

	bool	pop()
	{
		bool	ret		=	false;
		node*	nod		=	NULL;
		info	oldinfo	=	*(info*)&m_info;
		info	newinfo;
		do{
			oldinfo	=	*(info*)&m_info;
			nod		=	(node*)oldinfo.head;
			if(!nod)	break;
			if(oldinfo.head	!=	oldinfo.tail)
			{
				newinfo.head	=	(long)nod->next;
				newinfo.tail	=	oldinfo.tail;
				if(!newinfo.head)	return	false;//中间有间断
			}else{
				newinfo.head	=	NULL;
				newinfo.tail	=	NULL;
			}
		}while(atomic_compare_exchange64((volatile	long	long*)&m_info,
			*(long	long*)&newinfo,	*(long	long*)&oldinfo)
			!=	*(long	long*)&oldinfo);
		if(nod){
			node_allocator::deallocate(nod);
			ret	=	true;
			atomic_decreament32(&m_sz);

		}
		return	ret;
	}

private:
	struct	info{
		long	head;
		long	tail;
		info(long	hd	=	0,	long	tal	=	0):head(hd),tail(tal)
		{

		}
	};
	volatile	info	m_info;
	volatile	long	m_sz;
};


};

#pragma warning(pop) 

#endif/*BE_FIFO_H*/