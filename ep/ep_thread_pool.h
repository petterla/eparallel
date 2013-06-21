#ifndef	EP_THREAD_POOL_H
#define EP_THREAD_POOL_H

#include "ep_define.h"
#include "ep_type.h"
#include "be_thread.h"
#include <list>

namespace ep{

#define MUTI_THREADS_SAFE

#define MUTI_THREADS_UNSAFE

	class task;

	class task_queue{
	public:

		typedef	std::list<task*> tlist;
		typedef tlist::iterator titeraotr;

		enum{
			MAX_PRIORITY_NUM = 32,
		};

		task_queue();

		~task_queue();

		//priority must < 32,lower is more early
		//could not add when stop
		s32 add_task(task* op, u32 priority);

		s32 del_task(task* op, u32 priority);

		task* get_task();

		task* try_get_task();

		u32 size(){
			return	m_size;
		}

		u32 total_task_count(){
			return	m_total_task_cnt;
		}

		s32 stop();

	private:

		s32 clear();

		s32 clear_one_priority(u32 priority);

		task* get_task_by_priority(u32 priority);

		tlist m_oplists[MAX_PRIORITY_NUM];
		be::MUTEX m_cs;
		be::SEMAPHORE m_semp;

		u32 m_size;
		//total count from first task
		u32 m_total_task_cnt;
		bool m_stop;

	};

	class task{
	public:

		task(){

		}

		virtual ~task(){

		}

		virtual s32 recycle(){
			delete this;
			return	0;
		}

		virtual s32 run_process(){
			process();
			recycle();
			return	0;
		}

		virtual	s32	process(){
			return	0;
		}

	};

	class permanent_thread;

	class temporary_thread;

	class master_thread;

	class thread_pool{
	public:

		enum{
			DEFAULT_MAX_THREAD_NUM = 128,
			DEFAULT_MIN_THREAD_NUM = 32,
			DEFAULT_TASK_QUE_MAX_SIZE = 4096, 
			DEFAULT_SCHEDULE_SPAN = 50,//ms
		};
		
		//maxthread must >= minthread
		thread_pool( u32 minthread = DEFAULT_MIN_THREAD_NUM,
				u32 maxthread = DEFAULT_MAX_THREAD_NUM,
				u32 schedulespan = DEFAULT_SCHEDULE_SPAN,
				u32 quemaxsz = DEFAULT_SCHEDULE_SPAN,
				bool busycontrol = false);

		~thread_pool();

		MUTI_THREADS_UNSAFE s32 init();

		MUTI_THREADS_UNSAFE s32 free();

		//t != NULL, priority < 32,lower is process early
		//when task run,it must be recycle it self
		MUTI_THREADS_SAFE s32 add_task(task *t, u32 priority);

		//del the task from the task queue
		MUTI_THREADS_SAFE s32 del_task(task *t, u32 priority);

		MUTI_THREADS_SAFE u32 get_task_count();

		MUTI_THREADS_UNSAFE s32 run();

		MUTI_THREADS_UNSAFE s32 stop();

	private:

		s32 check_need_more_thread();

		//return the thread num real start
		s32 start_temp_thread(u32 cnt);

		friend class master_thread;

		bool m_run;

		u32 m_threads_cnt;
		u32 m_tmp_threads_cnt;
		u32 m_tmp_thread_idx;

		//if the task_q size is increase
		//we need start more temp thread;
		u32 m_task_q_past_sz;

		bool m_too_busy;

		u32 m_schedule_span;
		bool m_busy_control;

		//only m_busy_control == true and
		//task queue size > m_task_que_max_size
		//could not add task
		u32 m_task_que_max_size;

		master_thread* m_master_thread;
		permanent_thread* m_threads;
		temporary_thread* m_tmp_threads;

		task_queue* m_task_q;

	};

};


#endif/*EP_THREAD_POOL_H*/
