#include "ef_net_thread.h"
#include "ef_connection.h"
#include "ef_operator.h"
#include "base/ef_utility.h"
#include "base/ef_log.h"
#include <sys/epoll.h>
#include <errno.h>
#include <string.h>
#include <cassert>


namespace	ef{

const	char*	net_thread::tag="net_thread";

	net_thread::net_thread(int32 max_fds)
		:m_max_fds(max_fds),
		m_status(STATUS_INIT),
		m_epl(INVALID_SOCKET),
		m_ctlfd(INVALID_SOCKET),
		m_ctlfd1(INVALID_SOCKET),
		m_cur_id(1)
	{
		mutex_init(&m_opcs);
		m_epl = epoll_create(m_max_fds);	
	}

	net_thread::~net_thread(){
		stop();
		while(m_status != STATUS_CLOSED){
			sleep(1);
		}
		del_all_connections();
		sock_close(m_ctlfd);
		sock_close(m_ctlfd1);
		mutex_destroy(&m_opcs);
	}


	int32	net_thread::get_id(){
		
		int32 id = atomic_increment32((volatile int32*)&m_cur_id);
		id = atomic_compare_exchange32((volatile int32*)&m_cur_id, 0, 1);
		return	id;
	}

	int32	net_thread::start_ctl(){
		int32	ret = 0;
		int32	port = 50000;
		while(m_ctlfd == INVALID_SOCKET && port < 60000){
			m_ctlfd = udp_server_create(NULL, port);
			m_ctlport = port;
			++port;
		}

		if(m_ctlfd == INVALID_SOCKET){
			write_log(tag, EF_LOG_LEVEL_ERROR, "create ctl sock fail!");
			return	-1;
		}

		m_ctlfd1 = socket(AF_INET, SOCK_DGRAM, 0);
		if(m_ctlfd1 == INVALID_SOCKET){
			sock_close(m_ctlfd);
			m_ctlfd = INVALID_SOCKET;
		}

		set_socket_nonblocking(m_ctlfd);
		struct epoll_event	ev;
		ev.events = EPOLLIN;
		ev.data.fd = m_ctlfd;
		ret = epoll_ctl(m_epl, EPOLL_CTL_ADD, m_ctlfd, &ev);

		if(ret < 0){
			sock_close(m_ctlfd);
			m_ctlfd = INVALID_SOCKET;	
			sock_close(m_ctlfd1);
			m_ctlfd1 = INVALID_SOCKET;	
		}
		return	ret;
	}

	int32	net_thread::add_connection(connection *con){
		assert(con);
		struct in_addr	addr;
		int32	port;
		con->get_addr(addr,port);
		mutex_take(&m_opcs);	
		if(m_con_map.find(con->get_id()) == m_con_map.end()){
			m_con_map.insert(con_map::value_type(con->get_id(), con));
			write_log(tag,EF_LOG_LEVEL_NOTIFY,"con:%p, id:%u, fd:%d, %s:%d add sucess!",
				con, con->get_id(), con->get_fd(), inet_ntoa(addr),port);			
		}else{
			write_log(tag,EF_LOG_LEVEL_ERROR,"con:%p, id:%u, fd:%d, %s:%d has be added!",
				con, con->get_id(), con->get_fd(), inet_ntoa(addr),port);
		}
		mutex_give(&m_opcs);
		return	0;
	}

	int32	net_thread::del_connection(connection *con){
		assert(con);	
		struct in_addr	addr;
		int32	port;	
		con->get_addr(addr,port);
		mutex_take(&m_opcs);	
		con_map::iterator itor = m_con_map.find(con->get_id());
		if(itor != m_con_map.end()){
			m_con_map.erase(itor);
			write_log(tag,EF_LOG_LEVEL_NOTIFY,"con:%p, id:%u, fd:%d, %s:%d del success!",
				con, con->get_id(), con->get_fd(), inet_ntoa(addr),port);			
		}else{
			write_log(tag,EF_LOG_LEVEL_ERROR,"con:%p, id:%u, fd:%d, %s:%d del not find!",
				con, con->get_id(), con->get_fd(), inet_ntoa(addr),port);	
		}
		mutex_give(&m_opcs);
		return	0;
	}

	connection*	net_thread::get_connection(int32 id){
		con_map::iterator itor = m_con_map.find(id);
		if(itor != m_con_map.end()){
			return	(*itor).second;
		}
		return	NULL;
	}

	int32	net_thread::close_all_connections(){
		std::list<int32> ids;	
		mutex_take(&m_opcs);
		for(con_map::iterator it = m_con_map.begin(); it != m_con_map.end(); ++it){
			ids.push_back(it->first);
		}
		mutex_give(&m_opcs);
		for(std::list<int32>::iterator i = ids.begin(); i != ids.end(); ++i){
			close_connection(*i);
		}
		return	0;
	}


	int32	net_thread::stop(){
		write_log(tag,EF_LOG_LEVEL_ERROR,"net_thread:%p stop!", this);
		int32 status = atomic_compare_exchange32(&m_status, STATUS_RUNNING, STATUS_CLOSING);
		if(status == STATUS_RUNNING){
			close_all_connections();
		}	
		return	0;
	}

	int32	net_thread::init(){
		int32 ret = 0; 

		ret = start_ctl();
		if(ret < 0){
			write_log(tag,EF_LOG_LEVEL_ERROR,"net_thread:%p init fail -4!", this);
			return	-4;
		}
		return	0;
	}

	size_t	net_thread::connections_count(){
		size_t i = 0;
		mutex_take(&m_opcs);
		i = m_con_map.size();
		mutex_give(&m_opcs);
		return	i;
	}

	int32	net_thread::run(){
		int32	ret = 0;
		int32	nfds = 0;
		m_status = STATUS_RUNNING;
		write_log(tag,EF_LOG_LEVEL_ERROR,"net_thread:%p run start!", this);
		if(m_epl < 0){
			write_log(tag,EF_LOG_LEVEL_ERROR,"net_thread:%p run stop -1!", this);
			return	-1;
		}


		struct epoll_event *events = new struct epoll_event[m_max_fds];

		if(!events){
			write_log(tag,EF_LOG_LEVEL_ERROR,"net_thread:%p run stop -2!", this);
			return	-2;
		}

		time_tv tv;
		while(1){
			if(m_status != STATUS_RUNNING&& !connections_count()){
				m_status = STATUS_CLOSED;
				break;
			}

			process_op();
			process_timer(tv);
			if(tv.m_sec > 0 || (tv.m_sec == 0 && tv.m_usec > 0 )){
				nfds = epoll_wait(m_epl,events, m_max_fds, 
						tv.m_sec * 1000 + tv.m_usec / 1000);
			}else{
				nfds = epoll_wait(m_epl,events, m_max_fds, 2000);
			}
			if(nfds < 0){
				write_log(tag,EF_LOG_LEVEL_ALL,"epoll error,errno:%d!", errno);				
				if(errno != SOCK_EINTR){	
					write_log(tag,EF_LOG_LEVEL_ERROR,"stop,errno:%d!", errno);				
					break;
					//error
				}
			}
			if(nfds == 0){
				write_log(tag,EF_LOG_LEVEL_ALL,"epoll timeout!");				
				//time out
				continue;
			}
			for(int i = 0; i < nfds; ++i){
				if(events[i].data.fd == m_ctlfd){
					process_op();
					continue;
				}
				struct in_addr	addr;
				int32	port;
				connection	*con = (connection *)events[i].data.ptr;
				con->get_addr(addr,port);	
				if(events[i].events & EPOLLIN){
					ret = con->handle_read();
				}else	if(events[i].events & EPOLLOUT){
					ret = con->handle_write();
				}

				if(ret < 0){
					write_log(tag,EF_LOG_LEVEL_NOTIFY,"con:%p, id:%u, fd:%d, %s:%d"
						" handle event fail, recycle!",
						con, con->get_id(), con->get_fd(), inet_ntoa(addr), port);	
					con->recycle();
				}else{
					write_log(tag,EF_LOG_LEVEL_NOTIFY,"con:%p, id:%u, fd:%d, %s:%d"
						" handle event!",
						con, con->get_id(), con->get_fd(), inet_ntoa(addr), port);	
				}
			}
		}

		write_log(tag,EF_LOG_LEVEL_ERROR,"net_thread:%p run stop!", this);

		return 0;
	}

	int32	net_thread::process_op(){
		
		const int32	loop = 32;
		int32	i = 0;
		net_operator	*op = NULL;
		char	buf[128];
		struct	sockaddr	addr;
		socklen_t	frmlen = sizeof(addr);
		recvfrom(m_ctlfd, buf, sizeof(buf), 0, &addr, &frmlen);
		while(1){
			mutex_take(&m_opcs);
			if(m_ops.begin() != m_ops.end() && i < loop){
				op = m_ops.front();
				m_ops.pop_front();
				mutex_give(&m_opcs);
			}else{
				mutex_give(&m_opcs);
				break;
			}
			assert(op);
			op->process(this);
			delete	op;
		}

		return	0;
	}

	int32	net_thread::send_ctl(){

		int32	ret = 0;
		char	ctl[1];
		struct sockaddr_in	addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_ANY);	
		addr.sin_port = htons(m_ctlport);
		ret = sendto(m_ctlfd1, ctl, sizeof(ctl), 0, (sockaddr*)&addr, sizeof(addr));
		if(ret < 0){
			write_log(tag, EF_LOG_LEVEL_ERROR, "send ctl msg fail");
			return	-1;
		}else{
			write_log(tag, EF_LOG_LEVEL_NOTIFY, "send ctl msg");
		}
		return	ret;
	}

	int32	net_thread::process_timer(time_tv &diff){
		timeval t;
		int32	ret = 0;
		gettimeofday(&t, NULL);
		time_tv tv(t);
		const	int max_timer_one_loop = 256;
		struct	tpair{
			timer_key	key;
			timer		tm;
		};

		tpair	timeouts[max_timer_one_loop];
		timer_map::iterator itor = m_timer_map.begin();
		int	i = 0;
		diff.m_sec = 0;
		diff.m_usec = 0;
		mutex_take(&m_opcs);
		while(itor != m_timer_map.end() && i < max_timer_one_loop){
			time_tv tv1 = (*itor).first.tv;
			if(tv_cmp(tv, tv1) >= 0){
				timeouts[i].key = (*itor).first;
				timeouts[i].tm = (*itor).second;
				++itor;
				++i;
			}else{
				diff = tv_diff(tv1, tv);
				break;
			}
		}
		mutex_give(&m_opcs);

		for(int j = 0; j < i; ++j){
			timeouts[j].tm.timeout();
		}
	
		mutex_take(&m_opcs);	
		for(int k = 0; k < i; ++k){
			m_timer_map.erase(timeouts[k].key);
		}
		mutex_give(&m_opcs);

		return	ret;
	}


	int32	net_thread::set_notify(connection *con, int32 noti){
		assert(con);
		struct in_addr	addr;
		int32	port;
		con->get_addr(addr,port);
	
		int32	ret = 0;
		struct epoll_event	ev;
		int32	oldnoti = con->get_notify();
		ev.events = 0;
		if(noti & ADD_READ){
			ev.events |= EPOLLIN;
		}

		if(noti & ADD_WRITE){
			ev.events |= EPOLLOUT;
		}

		ev.data.ptr = con;
		if(noti & ADD_READ || noti & ADD_WRITE){
			if(oldnoti & EPOLLIN || oldnoti & EPOLLOUT){
				ret = epoll_ctl(m_epl, EPOLL_CTL_MOD, con->get_fd(), &ev);
				write_log(tag,EF_LOG_LEVEL_NOTIFY,"con:%p, id:%u, fd:%d,"
					" %s:%d ctl_mod setnoti:%x!",
					con, con->get_id(), con->get_fd(), 
					inet_ntoa(addr), port, noti);	
			}else{
				ret = epoll_ctl(m_epl, EPOLL_CTL_ADD, con->get_fd(), &ev);
				write_log(tag,EF_LOG_LEVEL_NOTIFY,"con:%p, id:%u, fd:%d,"
					" %s:%d ctl_add setnoti:%x!",
					con, con->get_id(), con->get_fd(), 
					inet_ntoa(addr), port, noti);	
			}
		}else{
			if(oldnoti & EPOLLIN || oldnoti & EPOLLOUT){
				ret = epoll_ctl(m_epl, EPOLL_CTL_DEL, con->get_fd(), &ev);
				write_log(tag,EF_LOG_LEVEL_NOTIFY,"con:%p, id:%u, fd:%d,"
					" %s:%d ctl_del setnoti:%x!",
					con, con->get_id(), con->get_fd(), 
					inet_ntoa(addr), port, noti);	
			}
		}

		if(ret >= 0){		
			write_log(tag,EF_LOG_LEVEL_NOTIFY,"con:%p, id:%u, fd:%d,"
				" %s:%d setnoti:%x sucess!",
				con, con->get_id(), con->get_fd(), 
				inet_ntoa(addr), port, noti);	
		}else{
			write_log(tag,EF_LOG_LEVEL_ERROR,"con:%p, id:%u, fd:%d,"
				" %s:%d add setnoti:%x fail!",
				con, con->get_id(), con->get_fd(), 
				inet_ntoa(addr), port, noti);	
		}
		return	0;		
	}

	int32	net_thread::add_timer(timer	tm){
		timer_key	key;
		connection	*con = tm.get_con();
		struct in_addr	addr;
		int32	port;
		
		key.tv = tm.get_time_out_time();
		if(con){
			con->get_addr(addr,port);	
			key.con_id = con->get_id();
		}else{
			key.con_id = 0;
		}
		key.id = tm.get_id();
		mutex_take(&m_opcs);		
		m_timer_map[key] = tm;
		mutex_give(&m_opcs);

		write_log(tag,EF_LOG_LEVEL_NOTIFY,"con:%p, id:%u, fd:%d,"
			" %s:%d add timer:%d!",
			con, con ? con->get_id() : 0, con ? con->get_fd() : -1, 
			con ? inet_ntoa(addr) : "0", con ? port : 0, tm.get_id());	

		return	0;
	}

	int32	net_thread::del_timer(timer	tm){
		timer_key	key;
		connection	*con = tm.get_con();
		struct in_addr	addr;
		int32	port;

		con->get_addr(addr,port);	
		key.con_id = con->get_id();
		key.id = tm.get_id();
		key.tv = tm.get_time_out_time();

		mutex_take(&m_opcs);		
		m_timer_map.erase(key);
		mutex_give(&m_opcs);

		write_log(tag,EF_LOG_LEVEL_NOTIFY,"con:%p, id:%u, fd:%d,"
			" %s:%d del timer:%d!",
			con, con->get_id(), con->get_fd(), 
			inet_ntoa(addr), port, tm.get_id());	
		return	0;
	}

	int32	net_thread::close_connection(int32 id){
		net_operator	*op = new	close_connection_op(id);

		mutex_take(&m_opcs);
		m_ops.push_back(op);
		mutex_give(&m_opcs);
		send_ctl();

		return	0;

	}

	int32	net_thread::do_close_connection(int32 id){
		int32	ret = 0;
		connection	*con = get_connection(id);
		if(con){
			ret = con->safe_close();
		}

		return	ret;
	}

	int32	net_thread::send_message(int32 id, const std::string &msg){
		net_operator	*op = new	send_message_op(id, msg);
		mutex_take(&m_opcs);
		m_ops.push_back(op);
		mutex_give(&m_opcs);
		send_ctl();
		return	0;
	}

	int32	net_thread::do_send_message(int32 id, const std::string &msg){
		int32	ret = 0;
		connection	*con = get_connection(id);
		if(con){
			ret = con->send_message(msg);
			if(ret < 0){
				con->recycle();
			}
		}
		return	0;
	}

	int32	net_thread::find_del_timer(int32 id, timer& tm){
		mutex_take(&m_opcs);
		thread_timer_map::iterator itor = m_timers.find(id);
		if(itor != m_timers.end()){
			tm = itor->second;
			m_timers.erase(itor);
			mutex_give(&m_opcs);
			return  0;
		}
		mutex_give(&m_opcs);
		return  -1;

	}

	int32	net_thread::start_timer(int32 id, 
			int32 timeout, 
			timer_handler* handler){
		int32   ret = 0; 
		timer   tm; 
		ret = find_del_timer(id, tm); 
		if(ret == 0){ 
			del_timer(tm); 
		} 
			
		timeval tv;
		gettimeofday(&tv, NULL);
		tv.tv_sec += timeout / 1000;
		tv.tv_usec += timeout % 1000 * 1000;
		timer   tm1(NULL, id, tv, handler);

		mutex_take(&m_opcs);
		m_timers[id] = tm1;
		mutex_give(&m_opcs);
		return	add_timer(tm1);
	}

	int32	net_thread::stop_timer(int32 id){
		int32   ret = 0;
		timer   tm;
		ret = find_del_timer(id, tm);
		if(ret == 0){
			del_timer(tm);
		}
		return  0;
	}

}

