#ifndef	BE_STRING_H
#define BE_STRING_H

#include <string.h>


#ifdef _WIN32
	inline bool nocaseequal(char c1, char c2){
		if(c1 == c2){
			return true;
		}
		if(c1 >= 'A' && c1 <= 'Z' && c1 + ('a' - 'A') == c2){
			return true;
		}
		if(c1 >= 'a' && c1 <= 'z' && c1 - ('a' - 'A') == c2){
			return true;
		}
		return	false;
	}
	inline int strncasecmp(const char* s1, const char* s2, size_t l){
		size_t i = 0;
		for(; s1[i] && s2[i] && nocaseequal(s1[i], s2[i]) && i < l; ++i);
		if(i == l){
			return	0;
		}
		return	s1[i] - s2[i];
	}
#endif


#endif/*BE_STRING_H*/