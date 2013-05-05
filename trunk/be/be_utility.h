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
#ifndef BE_UTILITY_H
#define BE_UTILITY_H

#ifdef	_WIN32
#include <stddef.h>
#include <xstring>
#endif/*_WIN32*/
#include "be_type_traits.h"


namespace	be{

	struct input_iterator_tag
	{	// identifying tag for input iterators
	};

	struct output_iterator_tag
	{	// identifying tag for output iterators
	};

	struct forward_iterator_tag
		: public input_iterator_tag
	{	// identifying tag for forward iterators
	};

	struct bidirectional_iterator_tag
		: public forward_iterator_tag
	{	// identifying tag for bidirectional iterators
	};

	struct random_access_iterator_tag
		: public bidirectional_iterator_tag
	{	// identifying tag for random-access iterators
	};

	template <class Key> struct hash { };

	inline size_t hash_string(const char* s)
	{
		unsigned long h = 0; 
		for ( ; *s; ++s)
			h = 5*h + *s;

		return size_t(h);
	}

	template<>	struct hash<char*>
	{
		size_t operator()(const char* s) const { return hash_string(s); }
	};

	template<>	struct hash<const char*>
	{
		size_t operator()(const char* s) const { return hash_string(s); }
	};


	template<>	struct hash<	std::string	>
	{
		size_t operator()(const std::string& s) const { return hash_string(s.c_str()); }
	};

	template<>	struct hash<char> {
		size_t operator()(char x) const { return x; }
	};
	template<>	struct hash<unsigned char> {
		size_t operator()(unsigned char x) const { return x; }
	};
	template<>	struct hash<signed char> {
		size_t operator()(unsigned char x) const { return x; }
	};
	template<>	struct hash<short> {
		size_t operator()(short x) const { return x; }
	};
	template<>	struct hash<unsigned short> {
		size_t operator()(unsigned short x) const { return x; }
	};
	template<>	struct hash<int> {
		size_t operator()(int x) const { return x; }
	};
	template<>	struct hash<unsigned int> {
		size_t operator()(unsigned int x) const { return x; }
	};
	template<>	struct hash<long> {
		size_t operator()(long x) const { return x; }
	};
	template<>	struct hash<unsigned long> {
		size_t operator()(unsigned long x) const { return x; }
	};
	template<class	T>struct hash<T*> {
		size_t operator()(T*	x) const { return (size_t)x; }
	};

	template<class T>
	struct	euqal{
		bool	operator()(const	T&	t1,	const	T&	t2){
			return	t1	==	t2;
		}
	};


	template<class ITOR>
	struct	itor_catg
	{
		typedef	typename	ITOR::iterator_category	iterator_category;
	};

	template<class T>
	struct	itor_catg<T*>
	{
		typedef	random_access_iterator_tag	iterator_category;
	};


	template<class ITOR>
	inline	typename	ITOR::difference_type	distance__(ITOR	l,	ITOR	r,	input_iterator_tag)
	{
		typename ITOR::difference_type	ret	=	0;
		for (; l != r; ++l)
			++ret;
		return	ret;
	};

	template<class ITOR>
	inline	typename	ITOR::difference_type	distance__(ITOR	l,	ITOR	r,	forward_iterator_tag)
	{
		typename ITOR::difference_type	ret	=	0;
		for (; l != r; ++l)
			++ret;
		return	ret;
	};

	template<class ITOR>
	inline	typename	ITOR::difference_type	distance__(ITOR	l,	ITOR	r,	bidirectional_iterator_tag)
	{
		typename ITOR::difference_type	ret	=	0;
		for (; l != r; ++l)
			++ret;
		return	ret;
	};

	template<class ITOR>
	inline	typename	ITOR::difference_type	distance__(ITOR	l,	ITOR	r,	random_access_iterator_tag)
	{
		return	r	-	l;
	};

	template<class ITOR>
	inline	typename	ITOR::difference_type	be_distance(ITOR	l,	ITOR	r)
	{

		return	distance__(l,	r,	typename itor_catg<ITOR>::iterator_category());
	};

	template<class ITOR>
	inline	ptrdiff_t	be_distance(ITOR*	l,	ITOR*	r)
	{

		return	r	-	l;
	};

	template<class T>
	inline	void	be_copy(T*	dst,	size_t	sz,	const	T*	src)
	{
		copy_pod(dst,	sz,	src,	typename is_pod<T>::value_type());
	}
	template<class T>
	inline	void	copy_pod(T*	dst,	size_t	sz,	const	T*	src,	true_type)
	{
		memmove(dst,	src,	sz	*	sizeof(T));
	}
	template<class T>
	inline	void	copy_pod(T*	dst,	size_t	sz,	const	T*	src,	false_type)
	{
		for(size_t	i	=	0;	i	<	sz;	++i)
			dst[i]	=	src[i];
	}

	template<class T,	class ITOR>
	inline	void	be_copy(T*	dst,	ITOR	first,	ITOR	last)
	{
		for(;first	!=	last;	++first,	++dst)
			*dst	=	*first;
	}

	template<class T>
		inline	void	be_copy(T*	dst,	T*	first,	T*	last)
	{
		be_copy(dst,	(size_t)(last	-	first),	first);
	}

	template<class T>
		inline	void	be_copy(T*	dst,	const	T*	first,	const	T*	last)
	{
		be_copy(dst,	(size_t)(last	-	first),	first);
	}



	template<class T>
		inline	void	be_copy_backward(T*	dst,	size_t	sz,	const	T*	src)
	{
		copy_pod_backward(dst,	sz,	src, typename is_pod<T>::value_type());
	}
	template<class T>
		inline	void	copy_pod_backward(T*	dst,	size_t	sz,	const	T*	src,	true_type)
	{
		memmove(dst,	src,	sz	*	sizeof(T));
	}
	template<class T>
		inline	void	copy_pod_backward(T*	dst,	size_t	sz,	const	T*	src,	false_type)
	{
		if(!sz)		return;
		for(size_t	i	=	sz;	i	>	0;	--i)
			dst[i-1]	=	src[i-1];
	}

	template<class T>
		inline	void	be_copy_backward(T*	dst,	T*	first,	T*	last)
	{
		be_copy_backward(dst,	(size_t)(last	-	first),	first);
	}

	template<class T>
		inline	void	be_copy_backward(T*	dst,	const	T*	first,	const	T*	last)
	{
		be_copy_backward(dst,	(size_t)(last	-	first),	first);
	}

	template<class T>
	inline	void	destroy(T*	dst,	size_t	sz)
	{
		destroy_pod(dst,	sz,	typename is_pod<T>::value_type());
	}

	template<class	T>
		inline	void	destroy_pod(T*	dst,	size_t	sz,	true_type)
	{
		return;
	}

	template<class	T>
		inline	void	destroy_pod(T*	dst,	size_t	sz,	false_type)
	{
		for(size_t	i	=	0;	i	<	sz;	++i)
			(dst	+	i)->T::~T();
	}

	template<class T>
		inline	void	destroy(T*	fst,	T*	lst)
	{
		destroy_pod(fst,	lst,	typename is_pod<T>::value_type());
	}

	template<class	T>
		inline	void	destroy_pod(T*	fst,	T*	lst,	true_type)
	{
		return;
	}

	template<class	T>
		inline	void	destroy_pod(T*	fst,	T*	lst,	false_type)
	{
		for(T*	i	=	fst;	i	<	lst;	++i)
			i->T::~T();
	}

	
}


#endif/*BE_UTILITY_H*/
