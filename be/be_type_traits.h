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
#ifndef BE_TYPE_TRAITS_H
#define BE_TYPE_TRAITS_H

#ifdef _WIN32
typedef	__int64						s64;
typedef	unsigned	__int64			u64;	
#else
typedef	long	long				s64;
typedef	unsigned	long	long	u64;	
#endif

namespace	be{
	struct	true_type{
		enum{
			value	=	true,
		};
		char	fill[1];
	};
	struct	false_type{
		enum{
			value	=	false,
		};
		char	fill[2];
	};

#if 0
	template<class U,	U	u>
	struct	test;

	template<class T>
	struct	has_operator_new{
		template<class U>
		static true_type check_sig(
			U*, 
			test<
			void *(*)(),
			&U::operator	new
			>* = NULL
			);
		template<class U>
		static false_type check_sig(...);

		enum{
			value	=	(check_sig<T>(0)())::value,
		};

	};
#endif

	template<class T>
	struct	is_pod{
		typedef	false_type	value_type;
		enum{
			value	=	value_type::value,
		};
	};

	template<>
	struct	is_pod<char>{
		typedef	true_type	value_type;
		enum{
			value	=	value_type::value,
		};
	};
	template<>
	struct	is_pod<unsigned	char>{
		typedef	true_type	value_type;
		enum{
			value	=	value_type::value,
		};
	};
	template<>
	struct	is_pod<short>{
		typedef	true_type	value_type;
		enum{
			value	=	value_type::value,
		};
	};
	template<>
	struct	is_pod<unsigned	short>{
		typedef	true_type	value_type;
		enum{
			value	=	value_type::value,
		};
	};
	template<>
	struct	is_pod<int>{
		typedef	true_type	value_type;
		enum{
			value	=	value_type::value,
		};
	};
	template<>
	struct	is_pod<unsigned	int>{
		typedef	true_type	value_type;
		enum{
			value	=	value_type::value,
		};
	};
	template<>
	struct	is_pod<long>{
		typedef	true_type	value_type;
		enum{
			value	=	value_type::value,
		};
	};
	template<>
	struct	is_pod<unsigned	long>{
		typedef	true_type	value_type;
		enum{
			value	=	value_type::value,
		};
	};

	template<>
	struct	is_pod<s64>{
		typedef	true_type	value_type;
		enum{
			value	=	value_type::value,
		};
	};
	template<>
	struct	is_pod<u64>{
		typedef	true_type	value_type;
		enum{
			value	=	value_type::value,
		};
	};

	template<>
	struct	is_pod<float>{
		typedef	true_type	value_type;
		enum{
			value	=	value_type::value,
		};
	};
	template<>
	struct	is_pod<double>{
		typedef	true_type	value_type;
		enum{
			value	=	value_type::value,
		};
	};

	template<>
	struct	is_pod<bool>{
		typedef	true_type	value_type;
		enum{
			value	=	value_type::value,
		};
	};

};


#endif/*BE_TYPE_TRAITS_H*/