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


#ifndef	BE_ALLOC_H
#define BE_ALLOC_H

#ifdef _WIN32
#include <xmemory>
#endif

#include "be_atomic.h"
#include "be_type_traits.h"
#include <iostream>
#include <string.h>
#include <cassert>

#ifdef	_DEBUG
#define ALLOC_DEBUG
#endif/*DEBUG*/

#ifdef _WIN32
#pragma warning(push) 

#pragma warning(disable:4311)
#pragma warning(disable:4312)
#endif

namespace	be{



	class lfmem_pool
	{
	public:
#define		VOLATILE_	
		enum{
			EXPANSION_SIZE	=	256,
		};

		lfmem_pool(long	elesz	=	8,
			long	cap	=	EXPANSION_SIZE,	long	escal	=	EXPANSION_SIZE)
			:freelist(0),elesize(elesz),escalate(escal),memblocks(0)
#ifdef ALLOC_DEBUG			
			,capacity(0),used(0),left(0),loop(0)
#endif/*ALLOC_DEBUG*/
		{
			if(elesize	<	(long)sizeof(node*))	elesize	=	(long)sizeof(node*);
#ifdef	ALLOC_DEBUG
			elesize	+=	sizeof(unsigned	int)	*	2;
#endif
		}
		~lfmem_pool()
		{
			for(;	memblocks;){
				block*	next	=	(block*)memblocks->next;
				delete	(char*)memblocks;
				memblocks		=	next;
			}
		}
#ifdef ALLOC_DEBUG

		inline	void	fill_flag	(void*	pv)
		{
			char*	pt	=	(char*)pv;
			*(unsigned int*)pt	=	(unsigned	int)this;
			*(unsigned int*)(pt	+	elesize	-	sizeof(unsigned	int))
				=	(unsigned	int)this	+	0XFF;
		}

		inline	bool	check_flag	(void*	pv)
		{
			bool	ret	=	0;
			char*	pt	=	(char*)pv;	
			ret	=	*(unsigned int*)(pt	-	sizeof(unsigned	int))
				==	(unsigned	int)this
				&&	*(unsigned int*)(pt	+	elesize	-	sizeof(unsigned	int)	*	2)	
				==	(unsigned	int)this	+	0XFF;	
			*(long*)(pt	+	elesize	-	sizeof(unsigned	int)	*	2)	
				=	(unsigned	int)this;
			return	ret;
		}

#endif/*ALLOC_DEBUG*/

		inline	void	add_mem		(void*	pv)
		{
			VOLATILE_	block*	pblock		=	(VOLATILE_	block*)pv;
			VOLATILE_	block*	blocks		=	(VOLATILE_	block*)memblocks;
			do{
				blocks			=	(VOLATILE_	block*)memblocks;
				pblock->next	=	blocks;
			}while(atomic_compare_exchange32((volatile	long*)&memblocks,	
				(long)pblock,	(long)blocks)	!=	(long)blocks);
		}

		void*	allocate	()
		{

			VOLATILE_	node*		newbuf_head =	NULL; //note1
			VOLATILE_	node*		newbuf_tail	=	NULL;
			VOLATILE_	node*		target		=	NULL;
			VOLATILE_	head_node	newhead;	
			VOLATILE_	head_node	oldhead;
			do{
#ifdef ALLOC_DEBUG
				atomic_increment32(&loop);
#endif/*ALLOC_DEBUG*/

				oldhead =	*(VOLATILE_	head_node*)&freelist; //note2
				if (!oldhead.head)
				{
					if (!newbuf_head)
					{
							size_t	size	=	elesize	*	escalate	+	sizeof(block);
							char	*buf	=	::new char[size];
							memset(buf,	0,	size);
							newbuf_head		=	(VOLATILE_	node*)(buf	+	sizeof(block));
							char	*start	=	(char*)newbuf_head;
							newbuf_tail		=	newbuf_head;
							for (int i = 1 ; i < escalate; i++)
							{
								newbuf_tail->next	=	(VOLATILE_	node*)(start	+	elesize	*	i);
								newbuf_tail			=	(VOLATILE_	node*)newbuf_tail->next;

							}
							newbuf_tail->next	=	0;
							add_mem(buf);
#ifdef ALLOC_DEBUG
							atomic_add32(&capacity,	escalate);
							atomic_add32(&left,	escalate);

#endif/*ALLOC_DEBUG*/
					}
				}

				if (newbuf_head) //note3
				{
					newbuf_tail->next = (VOLATILE_	node*)oldhead.head;//note3
					//以上行不能写成如下形式
					//newbuf_tail->next = (VOLATILE_	node*)freelist;
					//因为如果freelist此时不等于oldheader,但是在执行到note5时可能又变成与oldheader相等的值，即发生了ABA现象
					//则执行会出错
					target			=	newbuf_head;
					newhead.head	=	(long)newbuf_head->next;

				}
				else if (oldhead.head)
				{
					target			=	(VOLATILE_	node*)oldhead.head;//note4
					newhead.head	=	(long)target->next;
				}
				newhead.magic	=	oldhead.magic	+	1;

			}
			while(atomic_compare_exchange64((long	long	volatile*)&freelist, *(long	long*)&newhead,
				*(long	long*)&oldhead)	!=	*(long	long*)&oldhead);//note5

#ifdef	ALLOC_DEBUG
			atomic_increment32(&used);
			atomic_decrement32(&left);
			fill_flag(target);
			return	(char*)target	+	sizeof(unsigned	int);
#else
			return	(void*)target;
#endif/*ALLOC_DEBUG*/
		}

		void	deallocate	(void*	pv)
		{
			if(!pv)	return;
#ifdef	ALLOC_DEBUG
			assert(check_flag(pv));
			pv	=	(char*)pv	-	sizeof(unsigned	int);
#endif/*ALLOC_DEBUG*/
			VOLATILE_	head_node	newhead;
			VOLATILE_	head_node	oldhead	=	*(VOLATILE_	head_node*)&freelist;
			newhead.head	=	(long)pv;
			do{
				oldhead				=	*(VOLATILE_	head_node*)&freelist;
				((node*)pv)->next	=	(node*)oldhead.head;
				newhead.magic		=	oldhead.magic	+	1;	
			}
			while (atomic_compare_exchange64((long	long	volatile*)&freelist,
				*(long	long*)&newhead,	*(long	long*)&oldhead) 
				!=	*(long	long*)&oldhead);

#ifdef ALLOC_DEBUG
			atomic_increment32(&left);
#endif/*ALLOC_DEBUG*/

		}   

		void	print	()
		{
#ifdef ALLOC_DEBUG
			std::cout<<"\n[mem_pool] address:"<<this
					<< ",elesize:"<<elesize<<"capacity:"
					<< capacity<< ",left:"<< left << "used:"
					<<used<<",loop:"<<loop<<"\n"; 
#else
			std::cout<<"\n[mem_pool] address:"<<this<<"\n";
#endif/*ALLOC_DEBUG*/
		}

	protected:
	private:
		struct	node 
		{
			volatile	node*	next;

		};

		struct	block{
			volatile	block*	next;
		};
		struct	head_node{
			long	head;
			long	magic;
			head_node(long	hd	=	0,	long	nxt	=	0):head(hd),	magic(nxt)
			{

			}
		};
		volatile	head_node	freelist;
		volatile	long		elesize;
		volatile	long		escalate;
		volatile	block*		memblocks;
#ifdef ALLOC_DEBUG

		volatile	long		capacity;
		volatile	long		used;
		volatile	long		left;
		volatile	long		loop;
#endif/*ALLOC_DEBUG*/

	};


	class swplfmem_pool
	{
	public:
#define		VOLATILE_	
		enum{
			EXPANSION_SIZE	=	256,
		};

		swplfmem_pool(long	elesz,	long	cap	=	EXPANSION_SIZE,
			long	escal	=	EXPANSION_SIZE)
			:index(1),bucknumbits(0),elesize(elesz),escalate(escal),memblocks(0)
#ifdef ALLOC_DEBUG			
			,capacity(0),used(0),left(0),loop(0)
#endif/*ALLOC_DEBUG*/
		{
			if(elesize	<	(long)sizeof(node*))	elesize	=	(long)sizeof(node*);
#ifdef	ALLOC_DEBUG
			elesize	+=	sizeof(unsigned	int)	*	2;
#endif
			bucketnum	=	128;
			buckets		=	new	bucket[bucketnum];
			memset(buckets,	0,	sizeof(buckets)*bucketnum);
			s32	i	=	0;
			while(i	<	32){
				if(bucketnum	&	(1	<<	i)){
					bucknumbits	=	i;
				}
				++i;
			}
			buckets[0].used	=	1;
			buckets[0].head	=	NULL;
		}
		~swplfmem_pool()
		{
			for(;	memblocks;){
				block*	next	=	(block*)memblocks->next;
				delete	(char*)memblocks;
				memblocks		=	next;
			}
		}
#ifdef ALLOC_DEBUG

		inline	void	fill_flag	(void*	pv)
		{
			char*	pt	=	(char*)pv;
			*(unsigned int*)pt	=	(unsigned	int)this;
			*(unsigned int*)(pt	+	elesize	-	sizeof(unsigned	int))
				=	(unsigned	int)this	+	0XFF;
		}

		inline	bool	check_flag	(void*	pv)
		{
			bool	ret	=	0;
			char*	pt	=	(char*)pv;	
			ret	=	*(unsigned int*)(pt	-	sizeof(unsigned	int))
				==	(unsigned	int)this
				&&	*(unsigned int*)(pt + elesize - sizeof(unsigned int) * 2)	
				==	(unsigned	int)this	+	0XFF;	
			*(long*)(pt	+	elesize	-	sizeof(unsigned	int)	*	2)	
				=	(unsigned	int)this;
			return	ret;
		}

#endif/*ALLOC_DEBUG*/

		inline	void*	get_bucket(u32&	idx){
			u32	i	=	0;
			//u32	i	=	1;
			s32	j	=	0;
			bucket*	buck	=	NULL;

			while(1){
				i	=	(u32)atomic_increment32(&index);
				++j;
#ifdef ALLOC_DEBUG
				//atomic_increment32(&loop);
#endif/*ALLOC_DEBUG*/	
				i	%=	bucketnum;
				assert(i<bucketnum	&&	i	>=	0);
				buck	=	buckets	+	i;
				if(!buck->used && atomic_compare_exchange32(
					&buck->used,	1,	0)	==	0){
						idx	=	i;
						break;
				}
				++i;

			}
			//atomic_increment32(&index);
			//atomic_add32(&index, j);
			return	buck;
		}

		inline	void*	get_old_bucket(u32&	idx){
			idx	=	(magic	&
				(~
				((~0)<<bucknumbits)
				));
			assert(idx<bucketnum	&&	idx	>=	0);
			return	buckets	+		idx;
		}

		inline	void	add_mem		(void*	pv)
		{
			VOLATILE_	block*	pblock		=	(VOLATILE_	block*)pv;
			VOLATILE_	block*	blocks		=	(VOLATILE_	block*)memblocks;
			do{
				blocks			=	(VOLATILE_	block*)memblocks;
				pblock->next	=	blocks;
			}while(atomic_compare_exchange32((volatile	long*)&memblocks,	
				(long)pblock,	(long)blocks)	!=	(long)blocks);
		}

		void*	allocate	()
		{
			node*		oldhead		=	NULL;
			node*		newbuf_head =	NULL; //note1
			node*		newbuf_tail	=	NULL;
			node*		target		=	NULL;
			u32			idx			=	0;
			bucket*		buck		=	(bucket*)get_bucket(idx);
			s32			oldmagic	=	0;
			s32			newmagic	=	0;
			u32			oldidx		=	0;
			bucket*		oldbuck		=	NULL;
			atomic_increment32(&count);
			newmagic	=	(count	<<	bucknumbits)	+	idx;
			do{

				oldmagic	=	magic;

				oldbuck	=	(bucket*)get_old_bucket(oldidx);
#ifdef ALLOC_DEBUG
				atomic_increment32(&loop);
#endif/*ALLOC_DEBUG*/
				oldhead	=	oldbuck->head;
				if(!oldhead){
					if (!newbuf_head)
					{
						size_t	size	=	elesize	*	escalate	+	sizeof(block);
						char	*buf	=	::new char[size];
						memset(buf,	0,	size);
						newbuf_head		=	(node*)(buf	+	sizeof(block));
						char	*start	=	(char*)newbuf_head;
						newbuf_tail		=	newbuf_head;
						for (int i = 1 ; i < escalate; i++)
						{
							newbuf_tail->next	=	(node*)(start	+	elesize	*	i);
							newbuf_tail			=	(node*)newbuf_tail->next;

						}
						newbuf_tail->next	=	0;
						add_mem(buf);
#ifdef ALLOC_DEBUG
						atomic_add32(&capacity,	escalate);
						atomic_add32(&left,	escalate);

#endif/*ALLOC_DEBUG*/
					}			
				}
				if (newbuf_head) //note3
				{
					newbuf_tail->next = (node*)oldbuck->head;//note3

					target			=	newbuf_head;
					buck->head		=	newbuf_head->next;

				}
				else if (oldhead)
				{
					target		=	oldhead;//note4
					buck->head	=	target->next;
				}

			}while(atomic_compare_exchange32(&magic,	newmagic,oldmagic)
					!=	oldmagic);

			oldbuck->used	=	0;
#ifdef	ALLOC_DEBUG
			atomic_increment32(&used);
			atomic_decrement32(&left);
			fill_flag(target);
			assert((u32)target	!=	(u32)this);
			return	(char*)target	+	sizeof(unsigned	int);
#else
			return	(void*)target;
#endif/*ALLOC_DEBUG*/
		}

		void	deallocate	(void*	pv)
		{
			if(!pv)		return;
			atomic_increment32(&count);
#ifdef	ALLOC_DEBUG
			assert(check_flag(pv));
			pv	=	(char*)pv	-	sizeof(unsigned	int);
#endif/*ALLOC_DEBUG*/

			u32			idx			=	0;
			bucket*		buck		=	(bucket*)get_bucket(idx);
			s32			oldmagic	=	0;
			s32			newmagic	=	0;
			u32			oldidx		=	0;
			bucket*		oldbuck		=	NULL;

	
			buck->head	=	(node*)pv;
			do{
				oldmagic	=	magic;
				newmagic	=	(count	<<	bucknumbits)	+	idx;
				oldbuck		=	(bucket*)get_old_bucket(oldidx);
				((node*)pv)->next	=	oldbuck->head;

			}
			while (atomic_compare_exchange32(&magic,	newmagic,	oldmagic)
				!=	oldmagic);
			oldbuck->used	=	0;
#ifdef ALLOC_DEBUG
			atomic_increment32(&left);
#endif/*ALLOC_DEBUG*/
		}   

		void	print	()
		{
#ifdef ALLOC_DEBUG
			std::cout<< "\n[mem_pool] address:" << this
				<< ",elesize:"<< elesize<< ",capacity:"
				<< capacity <<",left:"<< left << ",used:"
				<< used << ",loop:" << loop << ",index:" 
				<< index << "\n";
#else
			std::cout<< "\n[mem_pool] address:" << this <<"\n";
#endif/*ALLOC_DEBUG*/
		}

	protected:
	private:
		volatile	s32	magic;
		volatile	s32	count;
		volatile	s32	index;
		struct	node 
		{
			node*	next;
		};

		struct	bucket{
			volatile	s32	used;
			node*			head;
		};

		struct	block{
			volatile	block*	next;
		};

		u32			bucketnum;
		bucket*		buckets;
		s32			bucknumbits;

		volatile	long		elesize;
		volatile	long		escalate;
		volatile	block*		memblocks;
#ifdef ALLOC_DEBUG

		volatile	long		capacity;
		volatile	long		used;
		volatile	long		left;
		volatile	long		loop;
#endif/*ALLOC_DEBUG*/

	};

#define RTRY_LOCK
	class zmem_pool
	{
	public:
		enum{
			GROUP_NUM		=	64,
			EXPANSION_SIZE	=	32,
		};

		zmem_pool(s32	elesz,s32	group=GROUP_NUM,s32	escal=EXPANSION_SIZE)
			:groupnum(group),escalate(escal),index(0),memblocks(0)

#ifdef ALLOC_DEBUG			
			,capacity(0),used(0),left(0),loop(0)
#endif/*ALLOC_DEBUG*/
		{
			heads=	new	head_node[groupnum];
				elesize	=	elesz	>	(s32)sizeof(node*)	?
							elesz	:	(s32)sizeof(node*);
#ifdef	ALLOC_DEBUG
				elesize	+=	sizeof(unsigned	int)	*	2;
#endif

		}

		~zmem_pool(){
			for(;	memblocks;){
				block*	next	=	(block*)memblocks->next;
				delete	(char*)memblocks;
				memblocks		=	next;
			}
			delete	[]	heads;
		}

#ifdef ALLOC_DEBUG

		inline	void	fill_flag	(void*	pv)
		{
			char*	pt	=	(char*)pv;
			*(unsigned int*)pt	=	(unsigned	int)this;
			*(unsigned int*)(pt	+	elesize	-	sizeof(unsigned	int))
				=	(unsigned	int)this	+	0XFF;
		}

		inline	bool	check_flag	(void*	pv)
		{
			bool	ret	=	0;
			char*	pt	=	(char*)pv;	
			ret	=	*(unsigned int*)(pt	-	sizeof(unsigned	int))
				==	(unsigned	int)this
				&&	*(unsigned	int*)(pt + elesize - sizeof(unsigned int) * 2)	
				==	(unsigned	int)this	+	0XFF;	
			*(unsigned	int*)(pt + elesize - sizeof(unsigned int) * 2)	
				=	(unsigned	int)this;
				return	ret;
		}

#endif/*ALLOC_DEBUG*/

		inline	void	add_mem		(void*	pv)
		{
			block*	pblock		=	(block*)pv;
			block*	blocks		=	(block*)memblocks;
			do{
				blocks			=	(block*)memblocks;
				pblock->next	=	blocks;
			}while(atomic_compare_exchange_ptr((volatile	void**)&memblocks,	
				(void*)pblock,	(void*)blocks)	!=	(void*)blocks);
		}

		inline	void*	try_lock(u32	idex){
			u32		i	=	idex;
			register	bool	ret	=	false;
#ifdef ALLOC_DEBUG
			atomic_increment32(&loop);
#endif/*ALLOC_DEBUG*/
			for(;i	<	groupnum	&&	
					!(ret=	heads[i].try_lock());	++i){
#ifdef ALLOC_DEBUG
				atomic_increment32(&loop);
#endif/*ALLOC_DEBUG*/
			}
			if(i	<	groupnum){
				return	&heads[i];
			}
#ifdef ALLOC_DEBUG
			atomic_increment32(&loop);
#endif/*ALLOC_DEBUG*/
			for(i	=	0;i	<	idex	&&	
				!(ret=	heads[i].try_lock());	++i){
#ifdef ALLOC_DEBUG
					atomic_increment32(&loop);
#endif/*ALLOC_DEBUG*/
			}
			if(i	<	idex){
				return	&heads[i];
			}
			return	NULL;
		}

		inline	void*	try_lock(	){
			u32		i	=	0;
			register	bool	ret	=	false;

#ifdef ALLOC_DEBUG
			atomic_increment32(&loop);
#endif/*ALLOC_DEBUG*/

			for(;i	<	groupnum	&&	
				!(ret=	heads[i].try_lock());	++i){
#ifdef ALLOC_DEBUG
					atomic_increment32(&loop);
#endif/*ALLOC_DEBUG*/
			}
			if(i	<	groupnum){
				return	&heads[i];
			}
			return	NULL;
		}


		void*	allocate(){
			node*		newbuf_head =	NULL; //note1
			node*		newbuf_tail	=	NULL;
			node*		target		=	NULL;
			head_node*	head		=	NULL;
			char*		buf			=	NULL;

#ifdef	RTRY_LOCK
			u32			i			=	(u32)index	%	groupnum;
			while(!(head	=	(head_node*)try_lock(i)));
#else
			while(!(head	=	(head_node*)try_lock()));
#endif


			if(head->head){
				target		=	head->head;
				head->head	=	target->next;

			}else{

				size_t	size	=	elesize	*	escalate	+	sizeof(block);
				buf				=	::new char[size];
				newbuf_head		=	(node*)(buf	+	sizeof(block));
				char	*start	=	(char*)newbuf_head;
				newbuf_tail		=	newbuf_head;
				for (int i = 1 ; i < escalate; i++)
				{
					start	+=	elesize;
					newbuf_tail->next	=	(node*)(start);
					newbuf_tail			=	(node*)newbuf_tail->next;

				}

				newbuf_tail->next	=	head->head;
				head->head			=	newbuf_head->next;

#ifdef ALLOC_DEBUG
				atomic_add32(&capacity,	escalate);
				atomic_add32(&left,	escalate);

#endif/*ALLOC_DEBUG*/

			}
			head->unlock();
			atomic_increment32(&index);
			if(buf){
				target	=	newbuf_head;
				add_mem(buf);
			}
#ifdef	ALLOC_DEBUG
			atomic_increment32(&used);
			atomic_decrement32(&left);
			fill_flag(target);
			return	(char*)target	+	sizeof(unsigned	int);

#else
			return	(void*)target;
#endif/*ALLOC_DEBUG*/
		}

		void	deallocate(void*	pv){

			head_node*	head	=	NULL;

#ifdef	ALLOC_DEBUG
			assert(check_flag(pv));
			pv	=	(char*)pv	-	sizeof(unsigned	int);
#endif/*ALLOC_DEBUG*/

#ifdef	RTRY_LOCK
			u32			i	=	(u32)index	%	groupnum;
			while(!(head	=	(head_node*)try_lock(i)));
			//++index;
#else
			while(!(head	=	(head_node*)try_lock()));
#endif
			((node*)pv)->next	=	head->head;
			head->head	=	(node*)pv;
			head->unlock();
#ifdef ALLOC_DEBUG
			atomic_increment32(&left);
#endif/*ALLOC_DEBUG*/
		}

	
		void	print	()
		{
#ifdef ALLOC_DEBUG
			std::cout << "\n[zmem_pool] address:" <<this 
				<< ",elesize:" << elesize << ",capacity:"
				<< ",left:" << left << ",used:" << used 
				<< "loop:" << loop << "\n";
#else
			std::cout << "\n[zmem_pool] address:" <<this << "\n";
#endif/*ALLOC_DEBUG*/
		}

	private:
		struct	node{
			node*	next;
		};

		struct	block{
			volatile	block*	next;
		};

		struct	head_node{
			node*				head;
			volatile	s32		lock;
			head_node():head(NULL),lock(0){

			}
			inline	bool	try_lock(){
				return	atomic_compare_exchange32(&lock,	1,	0)
					==	0;
			}
			inline	bool	unlock(){
				return	atomic_compare_exchange32(&lock,	0,	1)
					==	1;
			}
		};

		head_node*	heads;
		u32			groupnum;		
		s32			elesize;
		s32			escalate;
		volatile	s32		index;
		volatile	block*	memblocks;	

#ifdef ALLOC_DEBUG

		volatile	s32		capacity;
		volatile	s32		used;
		volatile	s32		left;
		volatile	s32		loop;
#endif/*ALLOC_DEBUG*/
	};

	template<size_t	ptsz>
	class	smem_pool
	{
	public:
		typedef	zmem_pool	pool_t;
	};

	template<>
	class	smem_pool<4>
	{
	public:
		typedef	lfmem_pool	pool_t;
	};

	typedef	smem_pool<sizeof(void*)>::pool_t	mem_pool;


	class alloc
	{
	public:
		static	void*	allocate(size_t	sz)
		{
			if(!sz)	return	0;
			if(sz	>	MAX_SIZE){
				return	::new	char[sz];
			}
			size_t	idx	=	(sz	-	1)	/	MIN_SIZE;
			return	m_pool[idx].allocate();

		}
		static	void	deallocate(void*	vp,	size_t	sz)
		{
			if(!vp)	return;
			if(sz	>	MAX_SIZE){
				delete	[]	(char*)vp;
				return;
			}
			size_t	idx	=	(sz	-	1)	/	MIN_SIZE;
			return	m_pool[idx].deallocate(vp);
		}
		enum{
			MIN_SIZE	=	8,
			MAX_SIZE	=	128,
		};

		static	void	print()
		{
			for(size_t	i	=	0;	i	<	BUCK_NUM;	++i)
			{
				std::cout << "m_pool["<< i <<"]:\n";
				m_pool[i].print();
			}
		}

		static	const		size_t	BUCK_NUM=(MAX_SIZE	/	MIN_SIZE);

		static	mem_pool	m_pool[BUCK_NUM];
	};

	class malloc_alloc{
	public:
		static	void*	allocate(size_t	sz)
		{
			return	::new char[sz];
		}

		static	void	deallocate(void*	vp,	size_t	sz)
		{
			::delete [] (char*)vp;
		}
	};

	template<class T,	class ALLOC	=	alloc>
	class	allocator{
	public:

		static	T*	allocate__(false_type)
		{
			T*	pt	=	(T*)ALLOC::allocate(sizeof(T));
			::new	(pt)T();
			return	pt;
		}

		static	T*	allocate__(true_type)
		{
			T*	pt	=	(T*)ALLOC::allocate(sizeof(T));
			return	pt;
		}

		static	T*	allocate(){
			return	allocate__( typename is_pod<T>::value_type() );
		}

		static	T*	allocate__1(const T& t, false_type)
		{
			T*	pt	=	(T*)ALLOC::allocate(sizeof(T));
			::new	(pt)T(t);
			return	pt;
		}

		static	T*	allocate__1(const T& t, true_type)
		{
			T*	pt	=	ALLOC::allocate(sizeof(T));
			*pt	=	t;
			return	pt;
		}

		static	T*	allocate(const T&	t){
			return	allocate__1(t, typename is_pod<T>::value_type() );
		}

		static	T*	allocate__n__(size_t sz, false_type)
		{
			T*	pt	=	(T*)ALLOC::allocate(sizeof(T)	*	sz);
			for(size_t	i	=	0;	i	<	sz;	++i)
			{
				::new	(pt	+	i)T();
			}
			return	pt;
		}

		static	T*	allocate__n__(size_t sz, true_type)
		{
			T*	pt	=	(T*)ALLOC::allocate(sizeof(T)	*	sz);
			return	pt;
		}

		static	T*	allocate_n(size_t	sz)
		{
			return	allocate__n__(sz, typename is_pod<T>::value_type() );
		}

		static	void	deallocate__(T*	pt, false_type)
		{
			pt->T::~T();
			ALLOC::deallocate(pt,	sizeof(T));
		}

		static	void	deallocate__(T*	pt,true_type)
		{
			ALLOC::deallocate(pt,	sizeof(T));
		}

		static	void	deallocate(T*	pt)
		{
			deallocate__(pt, typename is_pod<T>::value_type() );
		}

		static	void	deallocate__n__(T* pt, size_t sz, false_type)
		{
			for(size_t	i	=	0;	i	<	sz;	++i)
			{
				(pt	+	i)->T::~T();
			}
			ALLOC::deallocate(pt,	sizeof(T)	*	sz);
		}

		static	void	deallocate__n__(T* pt, size_t sz, true_type)
		{
			ALLOC::deallocate(pt,	sizeof(T)	*	sz);
		}

		static	void	deallocate_n(T*	pt,	size_t sz)
		{
			deallocate__n__(pt, sz, typename is_pod<T>::value_type() );
		}

		static	void	deallocate(T*	pt,	size_t	sz)
		{
			deallocate__n__(pt,	sz, typename is_pod<T>::value_type() );
		}

	};

	template<class	T,	class ALLOC	=	alloc>
	class mem_cache
	{
	public:
		mem_cache(long	elsz	=	sizeof(T)):m_freelist(0),m_elseize(elsz),m_size(0)
		{
		}
		~mem_cache()
		{
			for(;m_freelist;){
				node*	t	=	m_freelist;
				m_freelist	=	m_freelist->next;
				ALLOC::deallocate(t,	m_elseize);
			}
		}

		inline	T*	allocate(){
			return	allocate_pod(typename is_pod<T>::value_type() );
		}

		inline	T*	allocate_pod(true_type){
			if(!m_freelist){
				return	0;
			}
			T*	ret		=	(T*)m_freelist;
			m_freelist	=	(node*)m_freelist->next;
			return		ret;
		}

		inline	T*	allocate_pod(false_type){
			if(!m_freelist){
				return	0;
			}

			T*	ret		=	(T*)m_freelist;
			m_freelist	=	(node*)m_freelist->next;
			::new	(ret)T();
			return		ret;
		}

		template<class T1>
		inline	T*	allocate(const	T1&	t){
			return	allocate_pod(t,	typename is_pod<T>::value_type() );
		}

		template<class T1>
		inline	T*	allocate_pod(const	T1&	t,true_type){
			if(!m_freelist){
				return	0;
			}
			T*	ret		=	(T*)m_freelist;
			m_freelist	=	(node*)m_freelist->next;
			*ret		=	t;
			--m_size;
			return		ret;
		}

		template<class T1>
		inline	T*	allocate_pod(const	T1&	t,false_type){
			if(!m_freelist){
				return	0;
			}

			T*	ret		=	(T*)m_freelist;
			m_freelist	=	(node*)m_freelist->next;
			::new	(ret)T(t);
			--m_size;
			return		ret;
		}



		inline	void	deallcate_pod(T*	pt,	true_type)
		{
			node*	nod	=	(node*)pt;
			nod->next	=	m_freelist;
			m_freelist	=	nod;
			++m_size;
		}

		inline	void	deallcate_pod(T*	pt,	false_type)
		{
			pt->T::~T();
			node*	nod	=	(node*)pt;
			nod->next	=	m_freelist;
			m_freelist	=	nod;
			++m_size;
		}

		inline	void	deallcate(T*	pt)
		{
			return	deallcate_pod(pt,typename is_pod<T>::value_type() );
		}

		inline	size_t	size()const
		{
			return	m_size;
		}

	protected:
	private:
		struct	node{
			node*	next;
		};
		node*	m_freelist;
		long	m_elseize;
		size_t	m_size;
	};

	template<class T>
	class	t_pool{
	public:

		static	T*	allocate__(false_type)
		{
			T*	pt	=	(T*)m_pool.allocate();
			::new	(pt)T();
			return	pt;
		}

		static	T*	allocate__(true_type)
		{
			T*	pt	=	(T*)m_pool.allocate();
			return	pt;
		}

		static	T*	allocate(){
			return	allocate__(typename is_pod<T>::value_type() );
		}

		template<class T1>
		static	T*	allocate__1(const T1&	t, false_type)
		{
			T*	pt	=	(T*)m_pool.allocate();
			::new	(pt)T(t);
			return	pt;
		}

		template<class T1>
		static	T*	allocate__1(const T1&	t, true_type)
		{
			T*	pt	=	(T*)m_pool.allocate();
			return	pt;
		}

		template<class T1>
		static	T*	allocate(const T1&	t){
			return	allocate__1(t, typename is_pod<T>::value() );
		}

		static	void	deallocate__(T*	pt, false_type)
		{
			pt->T::~T();
			m_pool.deallocate(pt);
		}

		static	void	deallocate__(T*	pt, true_type)
		{
			m_pool.deallocate(pt);
		}

		static	void	deallocate(T*	pt)
		{
			deallocate__(pt, typename is_pod<T>::value() );
		}
	public:
		static	mem_pool	m_pool;
	};

	template<class	T>
	mem_pool	t_pool<T>::m_pool	=	mem_pool(sizeof(T));	
};

#ifdef _WIN32
#pragma warning(pop)
#endif

#endif/*BE_ALLOC_H*/