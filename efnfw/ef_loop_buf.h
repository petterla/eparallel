#ifndef	EF_LOOP_BUF_H
#define	EF_LOOP_BUF_H

#include"ef_common.h"

namespace	ef{

class	loop_buf{
	public:

		loop_buf(uint32	cap = 8192);

		~loop_buf();

		uint32	capacity();
		uint32	resize(uint32 cap);
		uint32	read(uint8	*buf, uint32 len);
		uint32	write(const uint8	*buf, uint32 len);
		uint32	peek(uint8	*buf, uint32 len);
		uint32	clear();

	private:
		uint8	*m_buf;
		uint32	m_cap;
		uint32	m_size;
		uint32	m_start;
		
};

};


#endif/**/
