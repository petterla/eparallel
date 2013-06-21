#include "ep_thread_pool.h"
#include "be_sleep.h"


namespace ep{

	task_queue::task_queue()
		:m_size(0),
		m_total_task_cnt(0),
		m_stop(false)
	{

		be::be_mutex_init(&m_cs);
		//in windows the maxcnt must be set!
		be::be_sem_init(&m_semp, 0, (u32)0x7ffffff);
	}

	task_queue::~task_queue(){
		clear();
		be::be_mutex_destroy(&m_cs);
		be::be_sem_destroy(&m_semp);
	}

	s32 task_queue::add_task(task* op, u32 priority){
		if(priority >= MAX_PRIORITY_NUM){
			return EP_INVALID_TASK_PRIORITY;
		}

		if(m_stop){//(1)
			return	-1;
		}


		be::auto_lock l(&m_cs);//(2)

		//may be stop between (1) and (2),so we 
		//need check it again
		if(m_stop){
			return	-1;
		}

		//size_t s = m_oplists[priority].size();

		m_oplists[priority].push_back(op);
		++m_size;
		++m_total_task_cnt;

		//size_t s1 = m_oplists[priority].size();

		be::be_sem_give(&m_semp);
		EP_DEBUG_PRINT("task_queue:%p total count:%u, current size:%u \n", 
			this, this->total_task_count(), this->size());

		return	0;

	}

	task* task_queue::get_task(){
		s32 ret = 0;
		task* op = NULL;

		ret = be::be_sem_take(&m_semp);
		if(ret < 0){
			EP_DEBUG_PRINT("task_queue:%p take_sem_fail \n", 
				this, this->total_task_count(), this->size());
			return	NULL;
		}
		be::auto_lock l(&m_cs);
		for(u32 p = 0; p < MAX_PRIORITY_NUM; ++p){
			op = get_task_by_priority(p);
			if(op){
				break;
			}
		}	

		EP_DEBUG_PRINT("task_queue:%p total count:%u, current size:%u \n", 
			this, this->total_task_count(), this->size());

		return	op;
	}

	s32 task_queue::del_task(task* op, u32 priority){

		s32 ret = 0;

		//if op is in the queue,
		//be_sem_try_take must return sucess!
		ret = be::be_sem_try_take(&m_semp);
		if(ret < 0){
			EP_DEBUG_PRINT("be_sem_try_take fail\n");
			return	EP_TASK_NOT_IN_QUEUE;
		}
		
		be::auto_lock l(&m_cs);

		//check the task is still in the list
		titeraotr it = m_oplists[priority].begin();
		for(; it != m_oplists[priority].end(); ++it){
			if(*it == op)
				break;
		}

		//this task is still in the list
		if(it != m_oplists[priority].end()){
			m_oplists[priority].erase(it);
			--m_size;
		}else{
			be::be_sem_give(&m_semp);
			return	EP_TASK_NOT_IN_QUEUE;
		}

		EP_DEBUG_PRINT("task_queue:%p total count:%u, current size:%u \n", 
			this, this->total_task_count(), this->size());

		return	0;


	}

	s32 task_queue::clear(){

		be::be_mutex_take(&m_cs);
		for(u32 p = 0; p < MAX_PRIORITY_NUM; ++p){
			clear_one_priority(p);
		}

		return	0;

	}

	task* task_queue::get_task_by_priority(u32 priority){

		if(m_oplists[priority].size()){
			task* op = m_oplists[priority].front();
			m_oplists[priority].pop_front();
			--m_size;
			return	op;

		}

		return	NULL;

	}

	s32 task_queue::clear_one_priority(u32 priority){

		while(m_oplists[priority].size()){
			task* op = m_oplists[priority].front();
			if(op){
				op->recycle();
			}
			m_oplists[priority].pop_front();
			--m_size;
		}

		return	0;

	}

	task* task_queue::try_get_task(){

		task* op = NULL;
		s32 ret = 0;

		ret = be::be_sem_try_take(&m_semp);

		if(ret < 0){
			EP_DEBUG_PRINT("be_sem_try_take fail\n");
			return	NULL;
		}

		be::auto_lock l(&m_cs);

		for(u32 p = 0; p < MAX_PRIORITY_NUM; ++p){
			op = get_task_by_priority(p);
			if(op){
				break;
			}
		}	

		EP_DEBUG_PRINT("task_queue:%p total count:%u, current size:%u \n", 
			this, this->total_task_count(), this->size());

		return	op;

	}

	s32	task_queue::stop(){

		//if we do not lock it ,it may be m_stop = false
		//when add_task,but we set m_stop = true immediately
		//so after stop(),
		be::auto_lock l(&m_cs);

		m_stop = true;

		return	0;

	}
	
	class exit_task:public task{
	};	

	class thread{
	public:

		enum{
			STATUS_INIT = 0,
			STATUS_RUNNING = 1,
			STATUS_PAUSE = 2,
			STATUS_STOP = 3,
		};

		thread()

			:m_status(STATUS_INIT),
			m_run(false),
			m_create_thread(false)
		{

		}

		virtual ~thread(){
			stop();
		}

		s32 run();

		s32 stop();

		virtual s32 run_process(){
			return	0;
		}

		u32 status(){
			return	m_status;
		}

		s32 set_status(u32 st){
			m_status = st;
			return	0;
		}

		s32 join(){
			if(m_create_thread){
				be::be_thread_join(&m_thread);
				m_create_thread = false;
			}
			return	0;
		}

	private:

		static s32 run_thread(thread* t);

		volatile u32 m_status;
		volatile bool m_run;
		volatile bool m_create_thread;

		be::THREADHANDLE m_thread;
	};

	class permanent_thread:public thread{
	public:

		permanent_thread(task_queue *q = NULL)
			:m_opq(q){
		}

		s32 set_task_queue(task_queue *q){
			m_opq = q;
			return	0;
		}

		virtual s32 run_process();

	private:	
		task_queue*	m_opq;

	};

	class temporary_thread:public thread{
	public:

		temporary_thread(task_queue *q = NULL)
			:m_opq(q){

		}

		s32 set_task_queue(task_queue *q){
			m_opq = q;
			return	0;

		}

		//if can not get task, temporary_thread exit;
		virtual s32 run_process();

	private:

		task_queue*	m_opq;

	};

	class master_thread:public thread{
	public:
		master_thread(thread_pool* p)
			:m_pool(p){

		}

		//evry 50 ms do a check
		virtual s32 run_process(){
			m_pool->check_need_more_thread();
			be::sleep_ms(m_pool->m_schedule_span);
			return	0;
		}

	private:
		thread_pool* m_pool;
	};


	s32 permanent_thread::run_process(){
		s32 ret = 0;
		task* t = m_opq->get_task();

		if(!t){
			return	ret;
		}

		if(!dynamic_cast<exit_task*>(t)){
			EP_DEBUG_PRINT("permanent_thread:%p task_queue:%p size:%d\n",
				this, m_opq, m_opq->size());
			t->run_process();

		}else{
			ret = EP_EXIT_FOR_NULL_TASK;
		}

		return	ret;

	}

	s32 temporary_thread::run_process(){

		s32 ret = 0;
		task* t = m_opq->try_get_task();

		if(t){
			EP_DEBUG_PRINT("temporary_thread:%p task_queue:%p size:%d\n",
				this, m_opq, m_opq->size());
			t->run_process();
		}else{
			ret = -1;
			EP_DEBUG_PRINT("temporary_thread:%p exit,task_que size:%u\n", 
				this, m_opq->size());

		}

		return	ret;

	}

	s32 thread::run_thread(thread* t){

		s32 ret = 0;
		t->set_status(STATUS_RUNNING);

		while(t->m_run){
			ret = t->run_process();
			if(ret < 0){
				break;
			}
		}

		t->m_run = false;
		t->set_status(STATUS_STOP);

		return	ret;

	}


	s32 thread::run(){

		s32 ret = 0;
		//has being running
		if(m_status == STATUS_RUNNING){
			return	EP_THREAD_HAS_START;
		}

		m_run = true;
		EP_DEBUG_PRINT("thread:%p run\n", this);

		ret = be::be_thread_create(&m_thread, NULL, 
			(be::PTHREAD_FUNC)thread::run_thread, (thread*)this);

		if(ret < 0){
			m_run = false;
			ret = EP_THREAD_CRETE_FAIL;
		}else{
			m_create_thread = true;
		}

		return	ret;

	}


	s32	thread::stop(){

		EP_DEBUG_PRINT("thread:%p stop\n", this);
		m_run = false;
		join();
		return	0;

	}

	thread_pool::thread_pool( u32 minthread,
		u32 maxthread,
		u32 schedulespan,
		u32 tskqmaxsz,
		bool busycontrol)
		:m_run(false),
		m_threads_cnt(minthread),
		m_tmp_threads_cnt(maxthread - minthread),
		m_tmp_thread_idx(0),
		m_task_q_past_sz(0),
		m_too_busy(false),
		m_schedule_span(schedulespan),
		m_busy_control(busycontrol),
		m_task_que_max_size(tskqmaxsz),
		m_master_thread(NULL),
		m_threads(NULL),
		m_tmp_threads(NULL),
		m_task_q(NULL)
	{

	}

	thread_pool::~thread_pool(){
		free();
	}

	s32	thread_pool::init(){

		if(m_task_q || m_master_thread 
			|| m_threads || m_tmp_threads){
			return	EP_HAS_BEEN_INITED;
		}

		s32 ret = 0;

		bool isfail = false;

		do{
			m_task_q = new task_queue();
			if(!m_task_q){
				isfail = true;
				ret = EP_CREATE_TASK_QUEUE_FAIL;
				break;
			}

			m_master_thread = new master_thread(this);
			if(!m_master_thread){
				isfail = true;
				ret = EP_MALLOC_THREAD_FAIL;
				break;
			}

			m_threads = new permanent_thread[m_threads_cnt];

			if(!m_threads){
				isfail = true;
				ret	= EP_MALLOC_THREAD_FAIL;
				break;
			}

			u32 i = 0;

			for(; i < m_threads_cnt; ++i){
				m_threads[i].set_task_queue(m_task_q);
			}

			if(!m_tmp_threads_cnt){
				break;
			}

			m_tmp_threads = new temporary_thread[m_tmp_threads_cnt];

			if(!m_tmp_threads){
				isfail = true;
				ret	= EP_MALLOC_THREAD_FAIL;
				break;
			}

			i = 0;

			for(; i < m_tmp_threads_cnt; ++i){
				m_tmp_threads[i].set_task_queue(m_task_q);
			}

		}while(0);

		if(isfail){

			if(m_task_q){
				delete m_task_q;
			}
			
			if(m_master_thread){
				delete m_master_thread;
			}

			if(m_threads){
				delete [] m_threads;
			}

			if(m_tmp_threads){
				delete [] m_tmp_threads;
			}

		}
		
		return	0;

	}

	s32	thread_pool::free(){

		if(!m_threads || !m_tmp_threads){
			return	EP_HAS_NOT_BEEN_INITED;
		}

		if(m_run){
			return	EP_CAN_NOT_FREE_WHEN_RUNNING;
		}

		if(m_master_thread){
			delete [] m_master_thread;
			m_master_thread = NULL;
		}
		
		if(m_threads){
			delete [] m_threads;
			m_threads = NULL;
		}

		if(m_tmp_threads){
			delete [] m_tmp_threads;
			m_tmp_threads = NULL;
		}

		if(m_task_q){
			delete m_task_q;
			m_task_q = NULL;
		}

		return	0;

	}

	s32 thread_pool::add_task(task *t, u32 p){

		s32 ret = 0;

		if(!t){
			return EP_INVALID_ARGU;
		}

		if(!m_run){
			return	EP_DO_NOT_START;
		}
		
		ret = m_task_q->add_task(t, p);

		if(ret < 0){
			if(m_too_busy)
				return	EP_TOO_BUSY;
		}
		
		return	ret;

	}

	s32 thread_pool::check_need_more_thread(){

		s32 ret = 0;
		u32 task_q_sz = m_task_q->size();

		m_too_busy = false;

		//the task_que size increase,so we need 
		//more temp thread
		if(task_q_sz > m_task_q_past_sz 
			&& m_task_q->total_task_count() > m_threads_cnt){
			ret = start_temp_thread(task_q_sz - m_task_q_past_sz);
			if(ret <= 0 && m_busy_control 
				&& task_q_sz >= m_task_que_max_size ){
				m_too_busy = true;
			}
		}

		m_task_q_past_sz = task_q_sz;

		return	0;

	}

	s32	thread_pool::start_temp_thread(u32 cnt){

		//we can start most m_tmp_threads_cnt temp thread;
		if(cnt > m_tmp_threads_cnt){
			cnt = m_tmp_threads_cnt;
		}

		u32 i = 0;

		for(u32 c = 0; i < cnt 
			&& c < m_tmp_threads_cnt; ++c){
			if(m_tmp_threads[m_tmp_thread_idx].run() >= 0){
				++i;
			}

			++m_tmp_thread_idx;
			//in the loop ,m_tmp_threads_cnt must > 0
			m_tmp_thread_idx %= m_tmp_threads_cnt;			
		}

		EP_DEBUG_PRINT("start_temp_thread count:%d\n", i);
		
		return i;
	}

	s32 thread_pool::run(){

		s32 ret = 0;

		m_run = true;
		ret = m_master_thread->run();
		EP_DEBUG_PRINT("m_master_thread:%p m_threads[0]:%p\n", 
			m_master_thread, &m_threads[0]);

		if(ret < 0){
			m_run = false;
			return	EP_MASTER_START_FAIL;
		}

		for(u32 i = 0; i < m_threads_cnt; ++i){
			m_threads[i].run();
		}

		return	ret;

	}

	s32 thread_pool::stop(){

		s32 ret = 0;

		m_run = false;//(1)
		m_task_q->stop();//(2)
		//after (2), no new task can add
		m_master_thread->stop();
		//wait tmp thread stop first
		for(u32 i = 0; i < m_tmp_threads_cnt; ++i){
			m_tmp_threads[i].join();
		}


		for(u32 i = 0; i < m_threads_cnt; ++i){		
			task* t = new exit_task;
			m_task_q->add_task(t, task_queue::MAX_PRIORITY_NUM - 1);
		}

		while(m_task_q->size()){
			be::sleep_ms(20);
		}

		for(u32 i = 0; i < m_threads_cnt; ++i){
			m_threads[i].stop();
		}

		return	ret;

	}

	u32 thread_pool::get_task_count(){
		return	m_task_q->size();
	}

	//del the task from the task queue
	s32 thread_pool::del_task(task *t, u32 priority){
		return	m_task_q->del_task(t, priority);
	}

};
