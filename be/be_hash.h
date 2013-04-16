#ifndef BE_HASH_H
#define BE_HASH_H


namespace	be{

inline	size_t	hash(long l){
	return	(size_t)l;
}

inline	size_t	hash(unsigned long l){
	return	(size_t)l;
}

inline	size_t	hash(double l){
	return	(size_t)l;
}

inline	size_t	hash(__int64 l){
	return	(size_t)l;
}

inline	size_t	hash(unsigned __int64 l){
	return	(size_t)l;
}

inline	size_t	hash(const char* str){
	size_t	rst	=	0;
	for(int i = 0; str[i]; ++i)
		rst	=	5*rst + str[i];
	return	rst;
}

template<class T>
struct hashfun{
	size_t	operator()(const T& t){
		return	hash(t);
	}
};

template<class T>
struct hashfun<T*>{
	size_t	operator()(const T* t){
		return	hash((__int64)t);
	}
};

};
#endif