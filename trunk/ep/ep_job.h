#ifndef EP_TASK_H
#define EP_TASK_H

#include "ep_define.h"
#include "ep_type.h"
#include "be_thread.h"
#include <vector>

namespace	ep{

	enum{

		STATUS_INIT = 0,
		STATUS_RUN = 1,
		STATUS_FINISH = 2,
		STATUS_FAIL = 3,
		STATUS_CANCEL = 4,

	};

	class	job;
	
	class	childtask{

	friend class job;

	public:

		childtask()

			:m_status(STATUS_INIT),
			
			m_max_retry_times(0),

			m_retry_times(0)
			
		{


		}


		virtual s32 recycle(){

			delete this;

			return 0;

		}

		virtual	s32	process(){

			return 0;

		}

		virtual	~childtask(){

		}

		u32 status(){

			return m_status;

		}

		s32 set_status(u32 s){

			m_status = s;

			return 0;

		}

		u32 max_retry_times(){

			return m_max_retry_times;

		}


		s32 set_max_retry_times(u32 rt){

			m_max_retry_times = rt;

			return 0;

		}

		u32 retry_times(){

			return	m_retry_times;

		}

		s32 set_retry_times(u32 rt){

			m_retry_times = rt;

			return 0;

		}

		job* get_job();

	private:

		s32 set_job(job* j);

		u32 m_status;

		u32 m_max_retry_times;

		u32 m_retry_times;

		job* m_job;

	};

	class	combiner{

	public:

		virtual s32 recycle(){

			delete this;

			return 0;

		}

		virtual	s32	child_task_finish(childtask* c){

			return 0;

		}

		virtual s32 all_child_task_finish(){

			return 0;

		}

		virtual	~combiner(){

		}

	private:

	};

	class	ctfcb;

	class	thread_pool;

	typedef s32 (*OBJCLEAR)(job*, void* obj);

	class	job{

	public:

		friend class parallel;

		friend class ctfcb;

		job(thread_pool* tp, 
			
			u32 priority = PORIORITY_NORMAL,
			
			bool autodestory = false);

		~job();	

		s32 set_job_id(u64 id){

			m_id = id;

			return 0;
		}

	public:

		enum{

			JOB_SUCESS = 0,

			JOB_INVALID_STATUS = -1,

		};

		s32 run();

		s32	cancel();

		s32	add_child_task(childtask* t);

		u32 child_task_num(){

			return	m_child_tasks.size();

		}

		s32	set_combiner(combiner* cb){

			m_combiner = cb;

			return 0;

		}

		s32 recycle();

		//inc the ref
		job* clone();

		s32 get_child_task_status(std::vector<u32>& status);

		u32 status(){

			return m_status;

		}

		u64 job_id(){

			return m_id;

		}

		s32 set_auto_destory(bool autodestory){

			m_autodestory = autodestory;

			return true;

		}

		bool auto_destroy(){

			return m_autodestory;

		}

		s32 set_obj(void* obj, OBJCLEAR cl){

			m_obj = obj;

			m_obj_clear = cl;

			return 0;

		}

	private:

		s32 clear();

		s32	stop(u32 status);

		s32 dispatch_child_task();

		s32 retry_child_task(ctfcb* c);

		s32 handle_child_task_finish(childtask* c);

		s32 wait_for_all_child_task();

		u64 m_id;

		thread_pool* m_tp;

		typedef std::vector<childtask*> childtask_vec;

		childtask_vec m_child_tasks;

		typedef std::vector<ctfcb*> ctfcb_vec;

		//when child task finish,call bask
		ctfcb_vec m_chlid_task_callbasks;

		u32 m_finish_num;

		combiner* m_combiner;

		u32 m_status;

		u32 m_priority;

		u32 m_refs;

		//auto destory when finish
		bool m_autodestory;

		void* m_obj;

		OBJCLEAR m_obj_clear;

		be::MUTEX m_cs;

		be::SEMAPHORE m_semp;

	};

};

#endif/*EP_TASK_H*/