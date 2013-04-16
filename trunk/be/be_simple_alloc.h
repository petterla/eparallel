#ifndef BE_SIMPLE_ALLOC_H
#define BE_SIMPLE_ALLOC_H

#include <xmemory>
#include <cassert>

namespace	be{
#pragma warning(push) 

#pragma warning(disable:4311)
#pragma warning(disable:4312)

	class simple_mem_pool
	{
	public:

		simple_mem_pool(long	elesz):elesize(elesz),freenum(0),usenum(0),freelist(0)
		{
			if(elesize	<	sizeof(node*))	elesize	=	sizeof(node*);
			elesize	+=	sizeof(unsigned	int)	*	2;
		}
		~simple_mem_pool()
		{
			for(;	freelist;){
				node*	next	=	(node*)freelist->next;
				alloc::deallocate(freelist,elesize);
				freelist		=	next;
			}
		}

		inline	void	fill_flag	(void*	pv)
		{
			char*	pt	=	(char*)pv;
			*(unsigned int*)pt	=	(unsigned	int)this;
			*(unsigned int*)(pt	+	elesize	-	sizeof(unsigned	int))
				=	(unsigned	int)this;
		}

		inline	bool	check_flag	(void*	pv)
		{
			char*	pt	=	(char*)pv;	
			bool	ret	=	*(unsigned int*)(pt	-	sizeof(unsigned	int))
				==	(unsigned	int)this
				&&	*(long*)(pt	+	elesize	-	sizeof(unsigned	int)	*	2)
				==	(unsigned	int)this;	
			if(!ret){
				exit(1);
			}
			return	ret;
		}

		void*	allocate	()
		{
			if(!freelist){
				freelist	=	(node*)alloc::allocate(elesize);
				freelist->next	=	0;
			}	
			void*	target	=	freelist;
			freelist		=	freelist->next;
			++usenum;
			fill_flag(target);
			return	(char*)target	+	sizeof(unsigned	int);
		}

		void	deallocate	(void*	pv)
		{
			if(!pv)	return;
			assert(check_flag(pv));
			pv	=	(char*)pv	-	sizeof(unsigned	int);
			--usenum;
			node*	newhead	=	(node*)pv;
			if(usenum	<	freenum){
				newhead->next	=	freelist;
				freelist		=	newhead;
				++freenum;
			}else{
				alloc::deallocate(newhead,	elesize);
			}
		}   

		void	print	()
		{
			printf("\n[mem_pool] address:0x%xlu, elesize:%d\n", 
				(unsigned	long)(this), elesize);

		}

	protected:
	private:
		struct	node 
		{
			node*	next;

		};

		node*		freelist;
		long		elesize;
		long		freenum;
		long		usenum;
	};

	class	simple_alloc{
	public:

	};
#pragma warning(pop)

};

#endif/*BE_SIMPLE_ALLOC_H*/