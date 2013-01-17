#include "ep_job.h"
#include "ep_thread_pool.h"
#include <cassert>

namespace ep{

	class ctfcb:public task{

	public:

		ctfcb(childtask *ct, job* j)

			:m_child_task(ct), 
			
			m_job(j){

				if(m_job){
					m_job = m_job->clone();

				}

		}

		virtual ~ctfcb(){
		
			if(m_job)

				m_job->recycle();
			
		}

		virtual s32 recycle(){

			delete this;

			return	0;

		}

		//after process,do not recycle
		virtual s32 run_process(){

			process();

			return	0;

		}

		s32 set_status(u32 status){

			m_child_task->set_status(status);

			return	0;

		}

		virtual	s32	process(){

 			//if job is not running,do nothing
 			if(m_job->status() != STATUS_RUN){
 
 				EP_DEBUG_PRINT("m_job:%p is not run!\n", m_job);

				m_child_task->set_status(STATUS_CANCEL);

				m_job->handle_child_task_finish(m_child_task);
 
 				return	0;
 
 			}

			m_child_task->set_status(STATUS_RUN);
			
			//do not recycle
			s32 ret = m_child_task->process();

			//fail
			if(ret < 0){
				
				if(m_child_task->retry_times() 
				
				< m_child_task->max_retry_times()){

					//retry
					if(m_job->retry_child_task(this) >= 0){

						m_child_task->set_retry_times(
						
							m_child_task->retry_times() + 1);

					}

				}else{

					//return fail
					m_child_task->set_status(STATUS_FAIL);

					m_job->handle_child_task_finish(m_child_task);

				}

			}else{

				m_child_task->set_status(STATUS_FINISH);

				m_job->handle_child_task_finish(m_child_task);

			}

			return	0;

		}

		childtask* child_task(){

			return m_child_task;

		}


	private:

		childtask* m_child_task;

		job* m_job;

	};

	s32 childtask::set_job(job* j){

		m_job = j->clone();

		return	0;

	}

	job* childtask::get_job(){

		return m_job;

	}

	job::job(thread_pool* tp, u32 priority, bool autodestory)

		:m_id(0),

		m_tp(tp),

		m_finish_num(0),

		m_combiner(NULL),

		m_status(STATUS_INIT),

		m_priority(priority),

		m_refs(1),

		m_autodestory(autodestory),

		m_obj(NULL),

		m_obj_clear(NULL)

	{
		
		be::be_mutex_init(&m_cs);

		be::be_sem_init(&m_semp, 0, 0x7FFFFFFF);

	}

	job::~job(){

		clear();

		be::be_sem_destroy(&m_semp);

		be::be_mutex_destroy(&m_cs);

	}

	s32 job::recycle(){

		{

			be::auto_lock l(&m_cs);

			--m_refs;

		}

		if(m_refs <= 0){

			delete	this;

		}

		return 0;

	}

	//inc the ref
	job* job::clone(){

		be::auto_lock l(&m_cs);

		++m_refs;

		return	this;

	}
	
	s32 job::clear(){

		wait_for_all_child_task();

		be::auto_lock l(&m_cs);

		if(m_combiner){

			m_combiner->recycle();

			m_combiner = NULL;

		}
		
		ctfcb_vec::iterator i = m_chlid_task_callbasks.begin();

		for(; i != m_chlid_task_callbasks.end(); ++i){

			(*i)->recycle();

		}

		m_chlid_task_callbasks.clear();

		childtask_vec::iterator itor = m_child_tasks.begin();

		for(; itor != m_child_tasks.end(); ++itor){

			(*itor)->recycle();

		}

		m_child_tasks.clear();

		if(m_obj_clear){

			m_obj_clear(this, m_obj);

		}

		m_status = STATUS_INIT;

		return	0;
	}

	s32	job::add_child_task(childtask* t){

		assert(t);

		t->set_job(this);

		be::auto_lock l(&m_cs);

		m_child_tasks.push_back(t);

		return 0;

	}



	s32	job::cancel(){

		stop(STATUS_CANCEL);

		return 0;

	}

	s32	job::retry_child_task(ctfcb* c){

		assert(c);

		be::auto_lock l(&m_cs);

		if(m_status != STATUS_RUN){

			return JOB_INVALID_STATUS;

		}

		m_tp->add_task(c, m_priority);

		return 0;

	}

	s32 job::dispatch_child_task(){

		be::auto_lock l(&m_cs);

		if(m_status != STATUS_INIT){

			return	JOB_INVALID_STATUS;

		}

		m_status = STATUS_RUN;

		childtask_vec::iterator itor = m_child_tasks.begin();

		for(; itor != m_child_tasks.end(); ++itor){

			ctfcb* ctf = new ctfcb(*itor, this);

			m_tp->add_task(ctf, m_priority);

			m_chlid_task_callbasks.push_back(ctf);

		}

		return	0;

	}

	s32 job::handle_child_task_finish(childtask* c){

		assert(c);

		EP_DEBUG_PRINT("job:%p handle child_task:%p status:%u finish\n",
			
			this, c, c->status());

		if(m_combiner){

			m_combiner->child_task_finish(c);

		}

		bool all_finish = false;

		{
			be::auto_lock l(&m_cs);

			++m_finish_num;

			if(m_finish_num == m_child_tasks.size()){

				all_finish = true;

			}
		}

		if(all_finish && m_combiner){

			m_combiner->all_child_task_finish();

		}

		be::be_sem_give(&m_semp);

		if(all_finish && m_autodestory){

			recycle();

		}
		
		return	0;

	}


	//仅仅是将子任务从队列中删除
	//如果删除成功，则将子任务状态设置为cancel
	//如果删除失败，要么还未加入队列，
	//要么正在执行，这时候都不会改变其状态

	s32	job::stop(u32 status){

		int ret = 0;

		childtask_vec ctl;

		if(m_status == STATUS_RUN){

			be::auto_lock l(&m_cs);

			if(m_status != STATUS_RUN){

				return JOB_INVALID_STATUS;

			}

			m_status = status;

			ctfcb_vec::iterator itor1 = m_chlid_task_callbasks.begin();

			for(; itor1 != m_chlid_task_callbasks.end(); ++itor1){

				ret = m_tp->del_task(*itor1, m_priority);

				if(ret >= 0){

					(*itor1)->set_status(status);

					ctl.push_back((*itor1)->child_task());

				} 

			}

		}

		childtask_vec::iterator itor = ctl.begin();

		for(;itor != ctl.end(); ++itor){

			handle_child_task_finish((*itor));

		}

		return 0;

	}

	s32 job::run(){

		return dispatch_child_task();

	}

	s32 job::wait_for_all_child_task(){

		size_t s = m_chlid_task_callbasks.size();

		for(size_t i = 0; i < s; ++i){
			
			be::be_sem_take(&m_semp);
		
		}

		return 0;

	}

};