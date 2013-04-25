#include "be_test.h"
#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#endif
#include <stdio.h>
#include "be_atomic.h"
#include "be_alloc.h"
#include "be_hash_map.h"
#include <ext/hash_map>
#include <vector>
#include "be_hash_list.h"
#include "be_list.h"
#include "be_vector.h"
#include "be_hash_vector.h"
#include "be_thread.h"

using	namespace	be;
be::THREADHANDLE g_threads[256]	=	{0};
bool		g_run		=	false;
int		g_thnum		=	0;
volatile	long	g_alloc		=	0;
//lock_free_list<int>	g_lflist;
struct p_node 
{
	p_node*	next;
};

#ifndef _WIN32
typedef	be::s64 __int64;
typedef be::s64 LARGE_INTEGER;
typedef be::u64 ULONG;
#endif

ULONG	run_process	(void*)
{
	__int64	i	=	0;

	//LONGLONG		QPart1,QPart2;
	//double			dfMinus, dfFreq, dfTim; 
	long	sz	=	0;
#ifdef	_WIN32
	LARGE_INTEGER	fre;
	LARGE_INTEGER	cnt, prevcnt;
	QueryPerformanceFrequency(&fre);
	QueryPerformanceCounter(&prevcnt);
#else
	struct timeval t,t1;
	gettimeofday(&t, NULL);
#endif
	p_node*	pv	=	0,*pv1	=	0,*pvt	=	0;
	while(g_run)
	{
		const	long	sz	=	8;
		pv	=	(p_node*)alloc::allocate(sz);

		if(pv){
			if(!pvt)	pvt	=	pv;	
			pv->next	=	pv1;
			pv1			=	pv;
			atomic_increment32(&g_alloc);
			++i;
		}
		else	continue;
		if(i%	0xFFFFF	==	0)
		{
#ifdef	_WIN32
			QueryPerformanceCounter(&cnt);
			printf("\n{ALLOC %d, spend:%lf seconds}\n",0xFFFFF, (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#else
			gettimeofday(&t1, NULL);
			printf("\n{ALLOC %d, spend:%lf seconds}\n",0xFFFFF, (double)(t1.tv_sec * 1000000 + t1.tv_usec - (t.tv_sec * 1000000 + t.tv_usec))	/ 100000);
#endif
			alloc::m_pool[0].print();
#ifdef	_WIN32
			QueryPerformanceCounter(&prevcnt);
#else
			gettimeofday(&t, NULL);
#endif
			break;
		}

	}
	p_node*	pv2	=	0;
	while(pv1)
	{
		if(!pv1->next	&&	pv1	!=	pvt)
		{
//			alloc::deallocate(pv1,	8);
			break;
		}
		++sz;

		pv2	=	pv1;
		pv1	=	pv1->next;
		pv2->next	=	0;
		alloc::deallocate(pv2,	8);
	}
	printf("real sz:%ld\n", sz);
	return	0;
}

int		test_init	(int	threadnum)
{
	g_thnum		=	threadnum;
	return	0;
}

int		alloc_test	()
{
	g_run	=	true;
	for(int	i	=	0;	i	<	g_thnum;	++i)
	{
		be_thread_create(&g_threads[i],0, (PTHREAD_FUNC)run_process, 0);
	}
	for(int	i	=	0;	i	<	g_thnum;	++i)
	{
		be_thread_join(&g_threads[i]);
	}
	return	0;
}

static	zmem_pool	g_zmem_pool(8);

ULONG	zallocrun_process	(void*)
{
	__int64	i	=	0;
	long	sz	=	0;

#ifdef	_WIN32
	LARGE_INTEGER	fre;
	LARGE_INTEGER	cnt, prevcnt;
	QueryPerformanceFrequency(&fre);
	QueryPerformanceCounter(&prevcnt);
#else
	struct timeval t,t1;
	gettimeofday(&t, NULL);
#endif
	p_node*	pv	=	0,*pv1	=	0,*pvt	=	0;
	while(g_run)
	{
		//const	long	sz	=	8;
		pv	=	(p_node*)g_zmem_pool.allocate();

		if(pv){
			if(!pvt)	pvt	=	pv;	
			pv->next	=	pv1;
			pv1			=	pv;
			atomic_increment32(&g_alloc);
			++i;
		}
		else	continue;
		//	alloc::deallocate(pv,sz);
		//	InterlockedDecrement(&g_alloc);
		if(i%	0xFFFFF	==	0)
		{
#ifdef	_WIN32
			QueryPerformanceCounter(&cnt);
			printf("\n{ZALLOC %d, spend:%lf seconds}\n",0xFFFFF, (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#else
			gettimeofday(&t1, NULL);
			printf("\n{ZALLOC %d, spend:%lf seconds}\n",0xFFFFF, (double)(t1.tv_sec * 1000000 + t1.tv_usec - (t.tv_sec * 1000000 + t.tv_usec))	/ 100000);
#endif
			g_zmem_pool.print();
#ifdef	_WIN32
			QueryPerformanceCounter(&prevcnt);
#else
			gettimeofday(&t, NULL);
#endif
			break;
		}

	}
	p_node*	pv2	=	0;
	while(pv1)
	{
		if(!pv1->next	&&	pv1	!=	pvt)
		{
	//		g_zmem_pool.deallocate(pv1);
			break;
		}
		++sz;

		pv2	=	pv1;
		pv1	=	pv1->next;
		pv2->next	=	0;
		g_zmem_pool.deallocate(pv2);
	}
	printf("real sz:%ld\n", sz);
	return	0;
}

int		zalloc_test()
{
	g_run	=	true;
	for(int	i	=	0;	i	<	g_thnum;	++i)
	{
		//g_threads[i]		=	CreateThread(NULL,0, (ULONG	(__stdcall*)(void*))zallocrun_process, 0, 0, NULL);
		be_thread_create(&g_threads[i],0, (PTHREAD_FUNC)zallocrun_process, 0);
	}
	for(int	i	=	0;	i	<	g_thnum;	++i)
	{
		//WaitForSingleObject(g_threads[i],	INFINITE);
		be_thread_join(&g_threads[i]);
	}
	return	0;
}

static	swplfmem_pool	g_swpmem_pool(8);

ULONG	swpallocrun_process	(void*)
{
	__int64	i	=	0;


	long	sz	=	0;

#ifdef	_WIN32
	LARGE_INTEGER	fre;
	LARGE_INTEGER	cnt, prevcnt;
	QueryPerformanceFrequency(&fre);
	QueryPerformanceCounter(&prevcnt);
#else
	struct timeval t,t1;
	gettimeofday(&t, NULL);
#endif
	p_node*	pv	=	0,*pv1	=	0,*pvt	=	0;
	while(g_run)
	{
		//const	long	sz	=	8;
		pv	=	(p_node*)g_swpmem_pool.allocate();

		if(pv){
			if(!pvt)	pvt	=	pv;	
			pv->next	=	pv1;
			pv1			=	pv;
			atomic_increment32(&g_alloc);
			++i;
		}
		else	continue;
		//	alloc::deallocate(pv,sz);
		//	InterlockedDecrement(&g_alloc);
		if(i%	0xFFFFF	==	0)
		{
			//QueryPerformanceCounter(&cnt);
			//printf("\n{SWPALLOC %d, spend:%lf seconds}\n",0xFFFFF, (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#ifdef	_WIN32
			QueryPerformanceCounter(&cnt);
			printf("\n{SWPALLOC %d, spend:%lf seconds}\n",0xFFFFF, (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#else
			gettimeofday(&t1, NULL);
			printf("\n{SWPALLOC %d, spend:%lf seconds}\n",0xFFFFF, (double)(t1.tv_sec * 1000000 + t1.tv_usec - (t.tv_sec * 1000000 + t.tv_usec))	/ 100000);
#endif
			g_swpmem_pool.print();
#ifdef	_WIN32
			QueryPerformanceCounter(&prevcnt);
#else
			gettimeofday(&t, NULL);
#endif
			break;
		}

	}
	p_node*	pv2	=	0;
	while(pv1)
	{
		if(!pv1->next	&&	pv1	!=	pvt)
		{
	//		g_zmem_pool.deallocate(pv1);
			break;
		}
		++sz;

		pv2	=	pv1;
		pv1	=	pv1->next;
		pv2->next	=	0;
		g_swpmem_pool.deallocate(pv2);
	}
	printf("real sz:%ld\n", sz);
	return	0;
}

int		swpalloc_test()
{
	g_run	=	true;
	for(int	i	=	0;	i	<	g_thnum;	++i)
	{
		//g_threads[i]		=
		//	CreateThread(NULL,0, (ULONG	(__stdcall*)(void*))swpallocrun_process, 0, 0, NULL);
		be_thread_create(&g_threads[i],0, (PTHREAD_FUNC)swpallocrun_process, 0);
	}
	for(int	i	=	0;	i	<	g_thnum;	++i)
	{
		//WaitForSingleObject(g_threads[i],	INFINITE);
		be_thread_join(&g_threads[i]);
	}
	return	0;
}

int		test_stop()
{
	g_run	=	false;
	be_sleep(1000);
	return	0;
}

int		malloc_process	()
{
	__int64	i	=	0;

#ifdef	_WIN32
	LARGE_INTEGER	fre;
	LARGE_INTEGER	cnt, prevcnt;
	QueryPerformanceFrequency(&fre);
	QueryPerformanceCounter(&prevcnt);
#else
	struct timeval t,t1;
	gettimeofday(&t, NULL);
#endif
	p_node*	pv	=	0/*,*pv1	=	0,*pvt	=	0*/;
	while(g_run)
	{
		pv	=	(p_node*)new	char[10];
		if(pv){
			atomic_increment32(&g_alloc);
			++i;
		}

		else	continue;
		delete	[]	pv;
		atomic_decrement32(&g_alloc);
		if(i%	0xFFFFF	==	0)
		{
			//QueryPerformanceCounter(&cnt);
			//printf("\n{MALLOC %d, spend:%lf seconds}\n",0xFFFFF, (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#ifdef	_WIN32
			QueryPerformanceCounter(&cnt);
			printf("\n{MALLOC %d, spend:%lf seconds}\n",0xFFFFF, (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#else
			gettimeofday(&t1, NULL);
			printf("\n{MALLOC %d, spend:%lf seconds}\n",0xFFFFF, (double)(t1.tv_sec * 1000000 + t1.tv_usec - (t.tv_sec * 1000000 + t.tv_usec))	/ 100000);
#endif

#ifdef	_WIN32
			QueryPerformanceCounter(&prevcnt);
#else
			gettimeofday(&t, NULL);
#endif
			break;
		}
		++i;
	}
	return	0;
}

int		malloc_test	()
{
	g_run	=	true;
	for(int	i	=	0;	i	<	g_thnum;	++i)
		//g_threads[i]		=	CreateThread(NULL,0, (ULONG	(__stdcall*)(void*))malloc_process, 0, 0, NULL);
		be_thread_create(&g_threads[i],0, (PTHREAD_FUNC)malloc_process, 0);
	for(int	i	=	0;	i	<	g_thnum;	++i)
	{
		//WaitForSingleObject(g_threads[i],	INFINITE);
		be_thread_join(&g_threads[i]);
	}
	return	0;
}


int		hashmap_test()
{
	//__int64	i	=	0;

#ifdef	_WIN32
	LARGE_INTEGER	fre;
	LARGE_INTEGER	cnt, prevcnt;
	QueryPerformanceFrequency(&fre);
	QueryPerformanceCounter(&prevcnt);
#else
	struct timeval t,t1;
	gettimeofday(&t, NULL);
#endif
	be::hash_map<int,	int>	hmap;
	for(int	i	=	0;	i	<	0xFFFFF;	++i)
	{
		//for(int	j	=	0;	j	<	0xFF;	++j)
		{
			hmap[i]	=	i;

			//if(*(hmap.find(i))	!=	i)	printf("i:%d,j:%d,hmap,Find ERROR!",i,i);
		}
	}
	assert(hmap.find(0xFFFFFFF)	==	hmap.end());
	//QueryPerformanceCounter(&cnt);
	//printf("\n{BE	HASHMAP operator[]	insert %d	time, spend:%lf seconds}\n",0xFFFFF, (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#ifdef	_WIN32
	QueryPerformanceCounter(&cnt);
	printf("\n{BE HASHMAP operator[] insert %d time, spend:%lf seconds}\n",0xFFFFF, (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#else
	gettimeofday(&t1, NULL);
	printf("\n{BE HASHMAP operator[] insert %d time, spend:%lf seconds}\n",0xFFFFF, (double)(t1.tv_sec * 1000000 + t1.tv_usec - (t.tv_sec * 1000000 + t.tv_usec))	/ 100000);
#endif

#ifdef	_WIN32
	QueryPerformanceCounter(&prevcnt);
#else
	gettimeofday(&t, NULL);
#endif
	for(int	i	=	0;	i	<	0xFFFFF;	++i)
	{
		{
			if(hmap[i]	!=	i)	printf("i:%d,j:%d,hmap[i]:%d,ERROR!",i,i,hmap[i]);
		}
	}
	//QueryPerformanceCounter(&cnt);
	//printf("\n{BE	HASHMAP operator[] find	%d	time, spend:%lf seconds}\n",0xFFFFF, (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#ifdef	_WIN32
	QueryPerformanceCounter(&cnt);
	printf("\n{BE HASHMAP operator[] find %d time, spend:%lf seconds}\n",0xFFFFF, (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#else
	gettimeofday(&t1, NULL);
	printf("\n{BE HASHMAP operator[] find %d time, spend:%lf seconds}\n",0xFFFFF, (double)(t1.tv_sec * 1000000 + t1.tv_usec - (t.tv_sec * 1000000 + t.tv_usec))	/ 100000);
#endif

#ifdef	_WIN32
	QueryPerformanceCounter(&prevcnt);
#else
	gettimeofday(&t, NULL);
#endif
	for(int	i	=	0;	i	<	0xFFFFF;	++i)
	{
		//for(int	j	=	0;	j	<	0xFF;	++j)
		{
			size_t	cnt	=	hmap.erase(i);
			if(cnt	!=	1)	printf("i:%d,j:%d, erase ERROR!",i,i);
		}
	}
	//QueryPerformanceCounter(&cnt);
	//printf("\n{BE	HASHMAP erase %d	time, spend:%lf seconds}\n",0xFFFFF, (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#ifdef	_WIN32
	QueryPerformanceCounter(&cnt);
	printf("\n{BE HASHMAP erase %d time, spend:%lf seconds}\n",0xFFFFF, (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#else
	gettimeofday(&t1, NULL);
	printf("\n{BE HASHMAP erase %d timee, spend:%lf seconds}\n",0xFFFFF, (double)(t1.tv_sec * 1000000 + t1.tv_usec - (t.tv_sec * 1000000 + t.tv_usec))	/ 100000);
#endif

#ifdef	_WIN32
	QueryPerformanceCounter(&prevcnt);
#else
	gettimeofday(&t, NULL);
#endif
	for(int	i	=	0;	i	<	0xFFFFF;	++i)
	{
		{
			hmap[i]	=	i;
		}
	}
	//QueryPerformanceCounter(&cnt);
	//printf("\n{BE	HASHMAP operator[]	insert %d	time, spend:%lf seconds}\n",0xFFFFF, (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#ifdef	_WIN32
	QueryPerformanceCounter(&cnt);
	printf("\n{BE HASHMAP operator[] insert %d time, spend:%lf seconds}\n",0xFFFFF, (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#else
	gettimeofday(&t1, NULL);
	printf("\n{BE HASHMAP operator[] insert %d time, spend:%lf seconds}\n",0xFFFFF, (double)(t1.tv_sec * 1000000 + t1.tv_usec - (t.tv_sec * 1000000 + t.tv_usec))	/ 100000);
#endif

#ifdef	_WIN32
	QueryPerformanceCounter(&prevcnt);
#else
	gettimeofday(&t, NULL);
#endif
	for(int	i	=	0;	i	<	0xFFFFF;	++i)
	{
		{
			if(hmap[i]	!=	i)	printf("i:%d,j:%d,hmap[i]:%d,ERROR!",i,i,hmap[i]);
		}
	}
	//QueryPerformanceCounter(&cnt);
	//printf("\n{BE	HASHMAP operator[] find	%d	time, spend:%lf seconds}\n",0xFFFFF, (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#ifdef	_WIN32
	QueryPerformanceCounter(&cnt);
	printf("\n{BE HASHMAP operator[] find %d time, spend:%lf seconds}\n",0xFFFFF, (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#else
	gettimeofday(&t1, NULL);
	printf("\n{BE HASHMAP operator[] find %d time, spend:%lf seconds}\n",0xFFFFF, (double)(t1.tv_sec * 1000000 + t1.tv_usec - (t.tv_sec * 1000000 + t.tv_usec))	/ 100000);
#endif
	return	0;
}


int		hashlist_test	()
{
	//__int64	i	=	0;

#ifdef	_WIN32
	LARGE_INTEGER	fre;
	LARGE_INTEGER	cnt, prevcnt;
	QueryPerformanceFrequency(&fre);
	QueryPerformanceCounter(&prevcnt);
#else
	struct timeval t,t1;
	gettimeofday(&t, NULL);
#endif
	be::hash_list<int>	hashlist;
	for(int	i	=	0;	i	<	0xFFFF;	++i)
	{
		hashlist.push_back(i);
	}
	for(int	i	=	0;	i	<	0xFFFF;	++i)
	{
		if(hashlist.front()	!=	i)
		{
			printf("i:%d,hashlist push ERROR!",i);
		}
		hashlist.pop_front();
	}
	for(int	i	=	0;	i	<	0xFFFF;	++i)
	{
		hashlist.push_front(i);
	}
	for(int	i	=	0;	i	<	0xFFFF;	++i)
	{
		if(hashlist.back()	!=	i)
		{
			printf("i:%d,hashlist push ERROR!",i);
		}
		hashlist.pop_back();
	}
	//QueryPerformanceCounter(&cnt);
	//printf("\n{BE	hash_list operator[] %d	time, spend:%lf seconds}\n",0xFFFFFF, (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#ifdef	_WIN32
	QueryPerformanceCounter(&cnt);
	printf("\n{BE hash_list operator[] %d time, spend:%lf seconds}\n",0xFFFFF, (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#else
	gettimeofday(&t1, NULL);
	printf("\n{BE hash_list operator[] %d time, spend:%lf seconds}\n",0xFFFFF, (double)(t1.tv_sec * 1000000 + t1.tv_usec - (t.tv_sec * 1000000 + t.tv_usec))	/ 100000);
#endif
	return	0;
}

int		hashvector_test	()
{
	//__int64	i	=	0;

#ifdef	_WIN32
	LARGE_INTEGER	fre;
	LARGE_INTEGER	cnt, prevcnt;
	QueryPerformanceFrequency(&fre);
	QueryPerformanceCounter(&prevcnt);
#else
	struct timeval t,t1;
	gettimeofday(&t, NULL);
#endif
	be::hash_vector<int>	hashlist;
	for(int	i	=	0;	i	<	0xFFFF;	++i)
	{
		hashlist.push_back(i);
	}

	for(int	i	=	0xFFFF	-	1	;	i	>=	0;	--i)
	{
		if(hashlist.back()	!=	i)
		{
			printf("i:%d,hashlist push ERROR!",i);
		}
		hashlist.pop_back();
	}
	//QueryPerformanceCounter(&cnt);
	//printf("\n{BE	hash_list operator[] %d	time, spend:%lf seconds}\n",0xFFFFFF, (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#ifdef	_WIN32
	QueryPerformanceCounter(&cnt);
	printf("\n{BE hash_vector operator[] %d time, spend:%lf seconds}\n",0xFFFFF, (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#else
	gettimeofday(&t1, NULL);
	printf("\n{BE hash_vector operator[] %d time, spend:%lf seconds}\n",0xFFFFF, (double)(t1.tv_sec * 1000000 + t1.tv_usec - (t.tv_sec * 1000000 + t.tv_usec))	/ 100000);
#endif
	return	0;
}

int	list_test()
{
	//__int64	i	=	0;

#ifdef	_WIN32
	LARGE_INTEGER	fre;
	LARGE_INTEGER	cnt, prevcnt;
	QueryPerformanceFrequency(&fre);
	QueryPerformanceCounter(&prevcnt);
#else
	struct timeval t, t1;
	gettimeofday(&t, NULL);
#endif
	be::list<int>	list;
	for(int	i	=	0;	i	<	0xFFFF;	++i)
	{
		list.push_back(i);
	}	

	for(int	i	=	0;	i	<	0xFFFF;	++i)
	{
		if(i	!=	list.front())printf("Pop front Error!\n");
		list.pop_front();
	}	
	list.pop_front();
	for(int	i	=	0;	i	<	0xFFFF;	++i)
	{
		list.push_front(i);
	}	


	for(int	i	=	0;	i	<	0xFFFF;	++i)
	{
		if(i	!=	list.back())
		{
			printf("Pop back Error!\n");
		}
		list.pop_back();
	}
	list.pop_back();

	for(int	i	=	0;	i	<	0xFFFF;	++i)
	{
		list.push_back(i);
	}	
	be::list<int>::iterator	itor	=	list.begin();
	for(int	i	=	0;	itor	!=	list.end();	++itor,	++i)
	{
		if(i	!=	*itor){
			printf("iterator ++ Error!\n");
		}
	}

	//QueryPerformanceCounter(&cnt);
	//printf("\n{BE	list operator[] %d	time, spend:%lf seconds}\n",0xFFFFFF, (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#ifdef	_WIN32
	QueryPerformanceCounter(&cnt);
	printf("\n{BE list operator[] %d time, spend:%lf seconds}\n",0xFFFFF, (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#else
	gettimeofday(&t1, NULL);
	printf("\n{BE list operator[] %d time, spend:%lf seconds}\n",0xFFFFF, (double)(t1.tv_sec * 1000000 + t1.tv_usec - (t.tv_sec * 1000000 + t.tv_usec))	/ 100000);
#endif
	return	0;
}


int	vector_test()
{
	//__int64	i	=	0;

	int				ia[100];
	for(int	i	=	0;	i	<	100;	++i)
	{
		ia[i]	=	i;
	}

	be::vector<int>	vector;


	vector.insert(vector.begin(),	ia,	ia	+	100);
	for(int	i	=	0;	i	<	100;	++i)
	{
		if(vector[i]	!=	i){
			printf("insert(T,	ITOR,	ITOR)	test	fail!\n");
		};
	}
	for(int	i	=	0;	i	<	100;	++i)
	{
		if(i	!=	vector.front())
		{
			printf("insert(T,	ITOR,	ITOR)!\n");
		}
		vector.pop_front();
	}	

#ifdef	_WIN32
	LARGE_INTEGER	fre;
	LARGE_INTEGER	cnt, prevcnt;
	QueryPerformanceFrequency(&fre);
	QueryPerformanceCounter(&prevcnt);
#else
	struct timeval t, t1;
	gettimeofday(&t, NULL);
#endif
	for(int	i	=	0;	i	<	0xFFFF;	++i)
	{
		vector.push_back(i);
	}	
	vector.insert(vector.begin()	+	1000,	ia,	ia	+	100);
	for(int	i	=	0;	i	<	100;	++i)
	{
		if(vector[1000+i]	!=	i){
			printf("insert(T,	ITOR,	ITOR)	test	fail!\n");
		};
	}

	//QueryPerformanceCounter(&cnt);
	//printf("\n{BE	vector	push_back	finish!spend:%lf seconds}\n", (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#ifdef	_WIN32
	QueryPerformanceCounter(&cnt);
	printf("\n{BE	vector	push_back	finish!spend:%lf seconds}\n", (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#else
	gettimeofday(&t1, NULL);
	printf("\n{BE	vector	push_back	finish!spend:%lf seconds}\n", (double)(t1.tv_sec * 1000000 + t1.tv_usec - (t.tv_sec * 1000000 + t.tv_usec))	/ 100000);
#endif

#ifdef	_WIN32
	QueryPerformanceCounter(&prevcnt);
#else
	gettimeofday(&t, NULL);
#endif

	for(int	i	=	0;	i	<	1000;	++i)
	{
		if(i	!=	vector.front())
		{
			printf("Pop front Error!\n");
		}
		vector.pop_front();
	}	

	for(int	i	=	0;	i	<	100;	++i)
	{
		if(i	!=	vector.front())
		{
			printf("Pop front Error!\n");
		}
		vector.pop_front();
	}

	for(int	i	=	1000;	i	<	0xFFFF;	++i)
	{
		if(i	!=	vector.front())
		{
			printf("Pop front Error!\n");
		}
		vector.pop_front();
	}

	//QueryPerformanceCounter(&cnt);
	//printf("\n{BE	vector	pop_front	finish!spend:%lf seconds}\n", (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#ifdef	_WIN32
	QueryPerformanceCounter(&cnt);
	printf("\n{BE vector pop_front finish!spend:%lf seconds}\n", (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#else
	gettimeofday(&t1, NULL);
	printf("\n{BE vector pop_front finish!spend:%lf seconds}\n", (double)(t1.tv_sec * 1000000 + t1.tv_usec - (t.tv_sec * 1000000 + t.tv_usec))	/ 100000);
#endif

#ifdef	_WIN32
	QueryPerformanceCounter(&prevcnt);
#else
	gettimeofday(&t, NULL);
#endif
	//prevcnt	=	cnt;
	vector.pop_front();
	for(int	i	=	0;	i	<	0xFFFF;	++i)
	{
		vector.push_front(i);
	}	

	//QueryPerformanceCounter(&cnt);
	//printf("\n{BE	vector push_front %d	time, spend:%lf seconds}\n",0xFFFF, (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#ifdef	_WIN32
	QueryPerformanceCounter(&cnt);
	printf("\n{BE vector push_front %d time!spend:%lf seconds}\n", 0xFFFF, (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#else
	gettimeofday(&t1, NULL);
	printf("\n{BE vector push_front %d time!spend:%lf seconds}\n", 0xFFFF, (double)(t1.tv_sec * 1000000 + t1.tv_usec - (t.tv_sec * 1000000 + t.tv_usec))	/ 100000);
#endif

#ifdef	_WIN32
	QueryPerformanceCounter(&prevcnt);
#else
	gettimeofday(&t, NULL);
#endif
	//prevcnt	=	cnt;
	for(int	i	=	0;	i	<	0xFFFF;	++i)
	{
		if(i	!=	vector.back())
		{
			printf("Pop back Error!\n");
		}
		vector.pop_back();
	}
	vector.pop_back();

	//QueryPerformanceCounter(&cnt);
	//printf("\n{BE	vector pop_back %d	time, spend:%lf seconds}\n",0xFFFF, (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#ifdef	_WIN32
	QueryPerformanceCounter(&cnt);
	printf("\n{BE vector pop_back %d time!spend:%lf seconds}\n", 0xFFFF, (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#else
	gettimeofday(&t1, NULL);
	printf("\n{BE vector pop_back %d time!spend:%lf seconds}\n", 0xFFFF, (double)(t1.tv_sec * 1000000 + t1.tv_usec - (t.tv_sec * 1000000 + t.tv_usec))	/ 100000);
#endif

#ifdef	_WIN32
	QueryPerformanceCounter(&prevcnt);
#else
	gettimeofday(&t, NULL);
#endif
	//prevcnt	=	cnt;
	for(int	i	=	0;	i	<	0xFFFF;	++i)
	{
		vector.push_back(i);
	}	
	be::vector<int>::iterator	itor	=	vector.begin();
	for(int	i	=	0;	itor	!=	vector.end();	++itor,	++i)
	{
		if(i	!=	*itor){
			printf("iterator ++ Error!\n");
		}
	}

	//QueryPerformanceCounter(&cnt);
	//printf("\n{BE	vector operator[] %d	time, spend:%lf seconds}\n",0xFFFF, (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);

#ifdef	_WIN32
	QueryPerformanceCounter(&cnt);
	printf("\n{BE vector operator[] %d time!spend:%lf seconds}\n", 0xFFFF, (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#else
	gettimeofday(&t1, NULL);
	printf("\n{BE vector operator[] %d time!spend:%lf seconds}\n", 0xFFFF, (double)(t1.tv_sec * 1000000 + t1.tv_usec - (t.tv_sec * 1000000 + t.tv_usec))	/ 100000);
#endif


	std::vector<int>	vc_vector;
	//QueryPerformanceCounter(&prevcnt);
#ifdef	_WIN32
	QueryPerformanceCounter(&prevcnt);
#else
	gettimeofday(&t, NULL);
#endif

	for(int	i	=	0;	i	<	0xFFFF;	++i)
	{
		vc_vector.push_back(i);
	}	
	//QueryPerformanceCounter(&cnt);
	//printf("vc_vector	push_back	finish!spend:%lf seconds}\n", (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#ifdef	_WIN32
	QueryPerformanceCounter(&cnt);
	printf("\n{vc_vector operator[] finish!spend:%lf seconds}\n", (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#else
	gettimeofday(&t1, NULL);
	printf("\n{vc_vector operator[] finish!spend:%lf seconds}\n", (double)(t1.tv_sec * 1000000 + t1.tv_usec - (t.tv_sec * 1000000 + t.tv_usec))	/ 100000);
#endif

#ifdef	_WIN32
	QueryPerformanceCounter(&prevcnt);
#else
	gettimeofday(&t, NULL);
#endif
	for(int	i	=	0;	i	<	0xFFFF;	++i)
	{
		if(i	!=	vc_vector.front())
		{
			printf("Pop front Error!\n");
		}
		vc_vector.erase(vc_vector.begin());
	}	

	//QueryPerformanceCounter(&cnt);
	//printf("vc_vector	pop_front	finish!spend:%lf seconds}\n", (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#ifdef	_WIN32
	QueryPerformanceCounter(&cnt);
	printf("\n{vc_vector pop_front finish!spend:%lf seconds}\n", (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#else
	gettimeofday(&t1, NULL);
	printf("\n{vc_vector pop_front finish!spend:%lf seconds}\n", (double)(t1.tv_sec * 1000000 + t1.tv_usec - (t.tv_sec * 1000000 + t.tv_usec))	/ 100000);
#endif
	return	0;
}

int		autoptr_test()
{
	typedef	allocator<int,	alloc>	int_alloc;
	typedef	array_auto_ptr<int,	int_alloc>	autoptr;

	autoptr	ptr;

	int*	tmp	=	int_alloc::allocate_n(314);
	for(int	i	=	0;	i	<	314;	++i)
	{
		tmp[i]	=	i;
	}
	ptr.reset(tmp,	314);

	return	0;
}

int		lflist_test		()
{
	//__int64	i	=	0;
	//LARGE_INTEGER	fre;
	//LARGE_INTEGER	cnt, prevcnt;
	//QueryPerformanceFrequency(&fre);
	//QueryPerformanceCounter(&prevcnt);
	//QueryPerformanceCounter(&cnt);
	return	0;
}

int		lfl_process	()
{
	//__int64	i	=	0;
	//LARGE_INTEGER	fre;
	//LARGE_INTEGER	cnt, prevcnt;
	//QueryPerformanceFrequency(&fre);
	//QueryPerformanceCounter(&prevcnt);
	//while(g_run	&&	i	<	0xFFFF	+1)
	//{
	//	g_lflist.push_front(i);
	//	++i;
	//}
	return	0;
}

int		lockfree_test	(int thds)
{
	/*g_run	=	true;
	g_thnum	=	thds;
	for(int	i	=	0;	i	<	g_thnum;	++i)
	g_threads[i]		=	CreateThread(NULL,0, (ULONG	(__stdcall*)(void*))lfl_process, 0, 0, NULL);
	for(int	i	=	0;	i	<	g_thnum;	++i)
	{
	WaitForSingleObject(g_threads[i],	INFINITE);
	}
	if(g_lflist.size()	!=	(0Xffff+1)	*	g_thnum)
	{
	printf("lock_free_list test fail!\n");
	}*/
	return	0;
}

int		cpoy_test		()
{
	//__int64	i	=	0;

	int	ia[1024];
	int	ib[1024];

	for(int	k	=	0;	k	<	1024;	++k)
	{
		ib[k]	=	k;
	}
#ifdef	_WIN32
	LARGE_INTEGER	fre;
	LARGE_INTEGER	cnt, prevcnt;
	QueryPerformanceFrequency(&fre);
	QueryPerformanceCounter(&prevcnt);
#else
	struct timeval t, t1;
	gettimeofday(&t, NULL);
#endif

	for(int	i	=	0;	i	<	0xFFFFF;	++i)
	{
		for(int	j	=	0;	j	<	1024;	++j)
		{
			ia[j]	=	j;
		}
	}
	//QueryPerformanceCounter(&cnt);
	//printf("\n{for loop	 %d	time, spend:%lf seconds}\n",0xFFFFF, (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#ifdef	_WIN32
	QueryPerformanceCounter(&cnt);
	printf("\n{for loop	 %d	time,spend:%lf seconds}\n", 0xFFFFF, (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#else
	gettimeofday(&t1, NULL);
	printf("\n{for loop	 %d	time,spend:%lf seconds}\n", 0xFFFFF, (double)(t1.tv_sec * 1000000 + t1.tv_usec - (t.tv_sec * 1000000 + t.tv_usec))	/ 100000);
#endif

#ifdef	_WIN32
	QueryPerformanceCounter(&prevcnt);
#else
	gettimeofday(&t, NULL);
#endif

	for(int	i	=	0;	i	<	0xFFFFF;	++i)
	{
		memmove(ia,	ib,	sizeof(ib));
	}
	//QueryPerformanceCounter(&cnt);
	//printf("\n{memmove loop	 %d	time, spend:%lf seconds}\n",0xFFFFF, (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#ifdef	_WIN32
	QueryPerformanceCounter(&cnt);
	printf("\n{memmove loop	%d	time,spend:%lf seconds}\n", 0xFFFFF, (double)(cnt.QuadPart	-	prevcnt.QuadPart)	/	fre.QuadPart);
#else
	gettimeofday(&t1, NULL);
	printf("\n{memmove loop %d	time,spend:%lf seconds}\n", 0xFFFFF, (double)(t1.tv_sec * 1000000 + t1.tv_usec - (t.tv_sec * 1000000 + t.tv_usec))	/ 100000);
#endif
	return	0;
}
