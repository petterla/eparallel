#ifndef	EF_LOOP_BUF_H
#define	EF_LOOP_BUF_H

#include"ef_common.h"

namespace	ef{

class	loop_buf{
	public:
		loop_buf(int32	cap = 8192);
		~loop_buf();

		int32	capacity();
		int32	resize(int32 cap);
		int32	read(uint8 *buf, int32 len);
		int32	write(const uint8 *buf, int32 len);
		int32	peek(uint8 *buf, int32 len);
		int32	clear();
		int32	size(){
			return	m_size;
		}

	private:
		uint8	*m_buf;
		int32	m_cap;
		int32	m_size;
		int32	m_start;
		
};

};


#endif/**/
