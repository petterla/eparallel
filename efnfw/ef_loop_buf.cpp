#include "ef_loop_buf.h"
#include <cassert>
#include <string.h>

namespace	ef{

		loop_buf::loop_buf(uint32 cap):
			m_cap(cap),
			m_size(0),
			m_start(0)
		{
			assert(m_size);
			m_buf = (uint8*)new uint8[cap];
		}

		loop_buf::~loop_buf(){
			delete	[]	m_buf;
		}

		uint32	loop_buf::resize(uint32 cap){
			assert(cap);
			uint32	ret = 0;
			uint8	*buf = (uint8*)new uint8[cap];	
			ret = read(buf, cap);
			uint8	*oldbuf = m_buf;
			m_buf = buf;
			delete	[] oldbuf;			
			m_size = ret;
			m_start = 0;

			return	ret;
		}

		uint32	loop_buf::read(uint8 *buf, uint32 len){
			uint32	realread = len;
			if(m_size <= len)
				realread = m_size;
			uint32	firsthalf = m_cap - m_start;			
			if(firsthalf < realread){
				memmove(buf, m_buf + m_start, firsthalf);
				memmove(buf + firsthalf, m_buf, realread - firsthalf);
				m_start = realread - firsthalf;
			}else{
				memmove(buf, m_buf + m_start, realread);
				m_start += realread;
			}

			m_size -= realread;					
			if(m_size <= 0)
				m_start = 0;

			return	realread;
		}

		uint32	loop_buf::write(const uint8 *buf, uint32 len){
			uint32	realwrite = len;
			if(realwrite > m_cap - m_size)
				realwrite = m_cap - m_size;
			uint32	stop = (m_start + m_size) % m_cap;
			uint32	firsthalf = m_cap - stop;
			if(firsthalf < realwrite){
				memmove(m_buf + stop, buf, firsthalf);
				memmove(m_buf, buf + firsthalf, realwrite - firsthalf);
			}else{
				memmove(m_buf + stop, buf, realwrite);
			}	
			m_size += realwrite;
			return	realwrite;
		}

		uint32	loop_buf::peek(uint8	*buf, uint32 len){
			uint32	realread = len;
			if(m_size <= len)
				realread = m_size;
			uint32	firsthalf = m_cap - m_start;			
			if(firsthalf < realread){
				memmove(buf, m_buf + m_start, firsthalf);
				memmove(buf + firsthalf, m_buf, realread - firsthalf);	
			}else{
				memmove(buf, m_buf + m_start, realread);
			}
		
			return	realread;

		}

		uint32	loop_buf::clear(){
			uint32	ret = m_size;
			m_size = 0;
			m_start = 0;

			return	ret;
		}

		uint32	loop_buf::capacity(){
			return	m_cap;
		}

};

