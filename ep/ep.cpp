#include "ep.h"
#include "ep_task.h"
#include "ep_thread_pool.h"

namespace ep{

	u64	parallel::s_j_id = 0;

	job*	parallel::create_job(u32 priority){
		
		if(!m_thread_pool 
			
			|| m_thread_pool->get_task_count() >= m_max_child_task_num)

		{

			return NULL;

		}

		job* t = new job(m_thread_pool, priority);

		if(!t){

			return NULL;

		}

		be::auto_lock l(&m_cs);

		t->set_job_id(s_j_id);

		m_jobs[s_j_id] = t;

		++s_j_id;
		
		return	t;

	}

	s32		parallel::run_job(u64 jobid){

		job* t = 0;

		{

			be::auto_lock l(&m_cs);

			t = m_jobs[jobid];

		}

		if(t){

			t->run();

		}else{

			return EP_INVALID_ARGU;

		}

		return	0;

	}

	s32		parallel::cancel_job(u64 jobid){

		job* t = 0;

		{

			be::auto_lock l(&m_cs);

			t = m_jobs[jobid];

		}

		if(t){

			t->cancel();

		}else{

			return EP_INVALID_ARGU;

		}

		return 0;

	}

	s32		parallel::destroy_job(u64 jobid){

		job* t = 0;

		{

			be::auto_lock l(&m_cs);

			t = m_jobs[jobid];

			m_jobs.erase(jobid);

		}

		if(t){

			t->recycle();

		}else{

			return EP_INVALID_ARGU;

		}

		return 0;

	}

	parallel::parallel(u32 max_child_task_num)

		:m_max_child_task_num(max_child_task_num){



	}

	parallel::~parallel(){

		free();

	}

	s32		parallel::init(){

		be::be_mutex_init(&m_cs);

		m_thread_pool = new thread_pool();

		return 0;

	}

	s32		parallel::free(){

		be::be_mutex_take(&m_cs);

		//clear all job
		
		while(m_jobs.size()){

			job_map::iterator itor = m_jobs.begin();

			(itor->second)->recycle();

			m_jobs.erase(m_jobs.begin());

		}

		delete m_thread_pool;

		be::be_mutex_destroy(&m_cs);

		return 0;

	}	


};
