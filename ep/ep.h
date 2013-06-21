#ifndef EP_H
#define EP_H

#include "ep_define.h"
#include "ep_type.h"
#include "ep_job.h"
#include <map>
#include "be_thread.h"

namespace	ep{
	
	class job;

	class thread_pool;

	class parallel{
	public:

		parallel(u32 max_child_task_num 
			= DEFAULT_MAX_CHILD_TASK_NUM);

		~parallel();

		s32     init();

		s32     destroy();		

		job*    create_job(u32 priority);

		s32     run_job(u64 j);

		//when job run,could cancel
		s32     cancel_job(u64 j);

		s32     destroy_job(u64 j);

	private:

		u32 m_max_child_task_num;

		thread_pool* m_thread_pool;

		typedef std::map<u64, job*> job_map;

		job_map m_jobs;

		be::MUTEX m_cs;

		static u64 s_j_id;

	};

};

#endif/*EP_H*/
