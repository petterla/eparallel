#ifndef	BE_TEST_H
#define BE_TEST_H

int		test_init		(int	threadnum);
int		alloc_test		();
int		zalloc_test		();
int		swpalloc_test	();
int		test_stop		();
int		malloc_test		();
int		hashmap_test	();
int		hashlist_test	();
int		hashvector_test	();
int		vector_test		();
int		list_test		();
int		lflist_test		();
int		autoptr_test	();
int		lockfree_test	(int	threadnum);
int		cpoy_test		();

#endif/*BE_TEST_H*/