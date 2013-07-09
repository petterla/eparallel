#ifndef	EF_CONNECTION_H
#define	EF_CONNECTION_H

#include "ef_common.h"
#include "ef_sock.h"
#include "ef_timer.h"
#include "ef_loop_buf.h"
#include <string>
#include <list>

namespace	ef{

class	net_thread;
class	message;

class	connection{
	public:
		enum{
			FLAG_NULL = 0,
			FLAG_SAFE_CLOSE = 1,
		};
		connection(SOCKET fd = INVALID_SOCKET, uint32 id = 0);
		virtual	~connection();

		int32	set_notify(int32 noti);
		int32	get_notify(){
			return	m_noti;
		}

		int32	clear();
		int32	start_timer(int32 id, int32 timeout);
		int32	stop_time(int32 id);
		//close after all send buf has send
		int32	safe_close();

		virtual	int32	handle_read();
		virtual	int32	handle_write();
		virtual	int32	handle_timer(int32 id);
		virtual	int32	handle_pack();
		virtual	int32	recycle();
		virtual	int32	on_create();

		//when recv buf is more than len,call handle_pack
		int32	set_notify_pack(int32 len){
			m_noti_len = len;
			return	0;
		}

		//peek from recv buf
		int32	peek_buf(char* out, int32 outlen){
			return	m_buf.peek((uint8*)out, outlen);
		}
		//read from recv buf
		int32	read_buf(char* out, int32 outlen){
			return	m_buf.read((uint8*)out, outlen);
		}
		//recv buf len
		int32	buf_len(){
			return	m_buf.size();
		}

		void	set_fd(SOCKET fd){
			m_fd = fd;
		}

		uint32	send_queue_size(){
			return	m_send_queue_size;
		}

		SOCKET	get_fd() const{
			return	m_fd;
		}

		void	set_id(uint32 id){
			m_id = id;
		}

		uint32	get_id() const{
			return	m_id;
		}
		
		int32	set_thread(net_thread *thr);

		net_thread*	get_thread(){
			return	m_thread;
		}

		int32	set_addr(struct in_addr addr, int32 port){
			m_addr = addr;
			m_port = port;
			return	0;
		}

		int32	get_addr(struct in_addr &addr, int32 &port) const{
			addr = m_addr;
			port = m_port;
			return	0;
		}

		int32	send_message(const std::string &msg);
		
	private:
		int32	find_del_timer(int32 id, timer &tm);
		int32	do_send();
		int32	send_(const char* buf, uint32 len);

		const static char *tag;
		
		SOCKET		m_fd;
		uint32		m_id;
		net_thread	*m_thread;
		int32		m_noti;
		struct in_addr	m_addr;
		int32	m_port;
		
		int32		m_noti_len;
		loop_buf	m_buf;
		std::list<timer>	m_timers;
		std::list<std::string>	m_msgs;
		uint32	m_send_queue_size;
		uint32	m_cur_msg_start;
		int32	m_flag;
};


};

#endif

