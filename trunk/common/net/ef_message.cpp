#include "ef_message.h"
#include "ef_net_thread.h"

namespace ef{

	static char  hex_char(int32 i){

		if(i >= 0 && i <= 9){

			return '0' + i;

		}else if(i >= 10 && i <= 15){

			return 'A' + i - 10;

		}

		return 0;

	}

	static char* byte_to_hex(uint8 c, char *buf){

		buf[0] = hex_char(c / 16);
		buf[1] = hex_char(c % 16);

		return buf;
	}

	message::message()
	:m_thr(NULL), m_conid(0)
	{

	}

	message::message(net_thread *thr, int32 conid, const std::string& msg)
	:m_thr(thr), m_conid(conid), m_msg(msg)
	{

	}

	message::~message(){
	}

	int32 message::process(){
		return 0;
	}

	bool message::is_null() const{
		return m_msg.length() == 0;
	}

	size_t message::to_hex(char* buf, size_t buflen, size_t start) const{

		if(start >= m_msg.length()){
			return 0;
		}

		size_t need_sz = (m_msg.length() - start)* 2 + 1; 
		size_t real_sz = buflen >= need_sz ? need_sz / 2 : (buflen - 1) / 2;
		const uint8 *startbytes = (const uint8 *)m_msg.c_str() + start;

		for(size_t i = 0; i < real_sz; ++i){
			byte_to_hex(startbytes[i], &buf[i * 2]);
		} 

		return real_sz;
	}

	size_t message::to_hex(std::string &out, size_t start) const{

		if(start >= m_msg.length()){
			return 0;
		}

		const uint8 *startbytes = (const uint8 *)m_msg.c_str() + start;
		char tmp[2];
		size_t real_sz = m_msg.length() - start;

		for(size_t i = 0; i < real_sz; ++i){
			byte_to_hex(startbytes[i], tmp);
			out.append(tmp, sizeof(tmp));
		} 

		return out.length();

	}

	int32 message::respone(const std::string &str) const{

		return m_thr->send_message(m_conid, str);

	}

};

