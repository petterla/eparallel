#ifndef	EF_MESSAGE_H
#define	EF_MESSAGE_H

#include "ef_common.h"
#include <string>
#include <list>

namespace	ef{

class	net_thread;

class	message{
	public:
		message();
		message(net_thread *thr, uint32 conid, std::string msg);
		~message();

		int32	process();

		bool	is_null() const;

		uint64	con_id() const{
			return	m_conid;
		}

		const std::string& msg() const{
			return	m_msg;
		}

		const char* get_msg() const{
			return	m_msg.c_str();
		}

		size_t	get_msg_len() const{
			return	m_msg.length();
		}

		int32	respone(const std::string &str) const;

		size_t	to_hex(char* buf, size_t buflen, size_t start) const;

		size_t	to_hex(std::string	&out, size_t start) const;
		
		//default operator = must be enough!
		//message&	operator = (const message& other); 
	private:

		struct	block{
			char	*m_buf;
			uint32	m_start;
			uint32	m_stop;
			uint32	m_size;

			block*	get_next(){
				return	(block*)(m_buf + m_size);
			}
		};
		
		net_thread	*m_thr;
		uint32	m_conid;
		std::string	m_msg;

};

	
};

#endif

