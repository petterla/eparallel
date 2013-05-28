#include "ef_connection.h"
#include "ef_net_thread.h"
#include "ef_log.h"
#include "ef_utility.h"
#include <cassert>
#include <errno.h>


namespace ef{

const char* connection::tag = "connection";

connection::connection(SOCKET fd, uint32 id)
	:m_fd(fd),m_id(id),m_thread(NULL),
	m_noti(0),m_noti_len(0),m_buf(),
	m_cur_msg_start(0),m_flag(FLAG_NULL)
{

}

connection::~connection(){
	clear();
}

int32	connection::recycle(){
	delete	this;
	return	0;
}

int32	connection::clear(){
	if(m_thread){
		if(m_noti & ADD_READ || m_noti & ADD_WRITE){
			m_thread->set_notify(this, 0);
		}

		std::list<timer>::iterator	itor = m_timers.begin();
		for(;itor != m_timers.end(); ++itor){
			m_thread->del_timer(*itor);
		}

		m_timers.clear();
		m_thread->del_connection(this);
		m_thread = NULL;
	}

	if(m_fd != INVALID_SOCKET){
		sock_close(m_fd);
		m_fd = INVALID_SOCKET;
	}
	m_noti = 0;
	m_id = 0;

	return	0;
}

int32	connection::safe_close(){
	//if has no to send
	if(m_msgs.begin() == m_msgs.end()){
		return	recycle();
	}
	if(m_thread){
		if(m_noti & ADD_READ){
			m_thread->set_notify(this, m_noti | DEL_READ);
		}

		std::list<timer>::iterator	itor = m_timers.begin();
		for(;itor != m_timers.end(); ++itor){
			m_thread->del_timer(*itor);
		}

		m_timers.clear();
		m_thread->del_connection(this);
		m_thread = NULL;
	}
	m_flag = FLAG_SAFE_CLOSE;
	return	do_send();
}

int32	connection::handle_read(){
	int32	ret = 0;
	char	tmpbuf[16 * 1024];
	int32	actrcv = 0;

	while(true){
		actrcv = 0;
		ret = tcp_nb_receive(m_fd, tmpbuf, sizeof(tmpbuf), &actrcv);
		if(ret <= 0){
			break;
		}
		printf("read len:%d, actrcv:%d\n", ret, actrcv);
		if(m_buf.size() + actrcv > m_buf.capacity()){
			m_buf.resize(m_buf.capacity() + actrcv);
		}
		int32 wlen =  m_buf.write((uint8*)tmpbuf, actrcv);
		printf("wlen:%d\n", wlen);	

		while(m_buf.size() && m_buf.size() >= m_noti_len){
			printf("buf_size:%d\n", m_buf.size());
			ret = handle_pack();
			if(ret < 0){
				return	ret;
			}
		}
	}
	
	return	ret;
}

int32	connection::handle_write(){
	int32	ret	= do_send();
	return	ret;
}

int32	connection::handle_pack(){
	return	0;
}

int32	connection::on_create(){
	set_notify(ADD_READ);
	return	0;

}
	
int32	connection::set_thread(net_thread *thr){
	m_thread = thr;
	return	0;
}

int32	connection::set_notify(int32 noti){
	int32	ret = 0;
	if(!m_thread){
		return	-2;
	}

	ret = m_thread->set_notify(this, noti);
	if(ret < 0){
		return	ret;
	}
	m_noti = noti;

	return ret;
}

int32	connection::find_del_timer(int32 id, timer &tm){
	std::list<timer>::iterator itor = m_timers.begin();
	for(; itor != m_timers.end(); ++itor){
		if(itor->get_id() == id){
			tm = *itor;
			m_timers.erase(itor);
			return	0;
		}
	}
	return	-1;
}

int32	connection::start_timer(int32 id, int32 timeout){
	int32	ret = 0;
	timer	tm;
	ret = find_del_timer(id, tm);
	if(ret == 0){
		m_thread->del_timer(tm);
	}

	timeval	tv;
	gettimeofday(&tv, NULL);
	tv.tv_sec += timeout / 1000;
	tv.tv_usec += timeout % 1000 * 1000;
	timer	tm1(this, id, tv);
	m_thread->add_timer(tm1);
	m_timers.push_back(tm1);
	return	0;
}

int32	connection::stop_time(int32 id){
	int32	ret = 0;
	timer	tm;
	ret = find_del_timer(id, tm);
	if(ret == 0){
		m_thread->del_timer(tm);
	}
	return	0;
}


int32	connection::handle_timer(int32 id){
	return	0;
}



int32	connection::send_message(const std::string &msg){
	int32	ret = 0;
	m_msgs.push_back(msg);
	ret = do_send();
	return	ret;
}



int32	connection::send_(const char* buf, uint32 len){
	int32	errcode = 0;
	int32	ret = 0;
do_:
	ret = send(m_fd, buf, (int32)len, 0);
	if(ret > 0){
		return	ret;
	}else	if(ret < 0){
		errcode = sock_errno;
		if (errcode ==  SOCK_EINTR) {
			//fprintf(stdout, "[TRACE] tcp_send : signal EINTR\n");
			goto	do_;
		}else if (errcode == SOCK_EAGAIN 
			|| errcode == SOCK_EWOULDBLOCK) {
			return	0;
		}else{
			return	-1;
		}
	}else{
		return	-1;
	}
	return	ret;
}

//send the msg list
int32	connection::do_send(){

	int32	noti = 0;
	while(m_msgs.begin() != m_msgs.end()){
		std::string	msg = m_msgs.front();
		uint32	len = msg.length() - m_cur_msg_start;
		int32	ret = send_(msg.c_str() + m_cur_msg_start, len);
		if(ret < 0){
			return	ret;
		}
		
		if((uint32)ret == len){
			m_cur_msg_start = 0;
			m_msgs.pop_front();
		}else{
			m_cur_msg_start += ret;
			if((m_noti & ADD_WRITE) == 0){
				//add write noti
				noti = (m_noti | ADD_WRITE);
				set_notify(noti);
			}
			return	0;
		}
	}
	if(m_flag == FLAG_SAFE_CLOSE){
		return	-2;
	}

	if(m_noti & ADD_WRITE){
		//clear write noti
		noti = (m_noti & DEL_WRITE);
		set_notify(noti);
	}
	return	0;
}

};


