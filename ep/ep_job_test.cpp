#include "ep_job_test.h"
#include "ep_thread_pool.h"
#include "ep_job.h"
#include <stdio.h>


namespace ep{
	
	thread_pool	g_test_tp(2, 4);

	class test_cjob:public childtask{

	public:

		test_cjob(){

			m_i = s_i;

			++s_i;

		}

		s32	process(){

			printf("test_cjob:%d\n", m_i);
			
			be::be_sleep(1000);

			return 0;

		}

	private:

		s32 m_i;

		static s32 s_i;

	};

	s32 test_cjob::s_i = 0;

	class test_combainer:public combiner{
	public:

		s32 child_task_finish(childtask* c){

			printf("test_combainer::process child_task:%p finish,status:%d\n",
				
				c, c->status());

			return 0;

		}

		s32 all_child_task_finish(){

			printf("test_combainer::all_child_task_finish\n");

			return 0;

		}


	};

	class test_combainer_cancel:public combiner{
	public:

		s32 child_task_finish(childtask* c){

			printf("test_combainer_cancel::process child_task:%p finish,status:%d\n",

				c, c->status());

			c->get_job()->cancel();

			return 0;

		}

		s32 all_child_task_finish(){

			printf("test_combainer_cancel::all_child_task_finish\n");

			return 0;

		}
	
	private:


	};

	s32 job_test_init(){

		g_test_tp.init();

		g_test_tp.run();

		return 0;

	}

	s32 job_test_free(){

		g_test_tp.stop();

		g_test_tp.free();

		return 0;

	}


	s32 job_test_1(){

		printf("\n****************job_test_1**************\n");

		job* j = new job(&g_test_tp);

		for(int i = 0; i < 10; ++i){

			j->add_child_task(new test_cjob);

		}

		j->run();

		delete j;

		return 0;
	}

	s32 job_test_2(){

		printf("\n****************job_test_2**************\n");

		job* j = new job(&g_test_tp);

		for(int i = 0; i < 10; ++i){

			j->add_child_task(new test_cjob);

		}

		j->run();

		j->cancel();

		delete j;

		return 0;

	}

	s32 job_test_3(){

		printf("\n****************job_test_3**************\n");

		job* j = new job(&g_test_tp);

		for(int i = 0; i < 10; ++i){

			j->add_child_task(new test_cjob);

		}

		j->set_combiner(new test_combainer);

		j->run();

		delete j;

		return 0;
	}

	s32 job_test_4(){

		printf("\n****************job_test_4**************\n");

		job* j = new job(&g_test_tp);

		for(int i = 0; i < 10; ++i){

			j->add_child_task(new test_cjob);

		}

		j->set_combiner(new test_combainer);

		j->run();

		j->cancel();

		delete j;

		return 0;
	}

	s32 job_test_5(){

		printf("\n****************job_test_5**************\n");

		job* j = new job(&g_test_tp);

		for(int i = 0; i < 10; ++i){

			j->add_child_task(new test_cjob);

		}

		j->set_combiner(new test_combainer_cancel);

		j->run();

		j->cancel();

		delete j;

		return 0;
	}

	s32 job_test_6(){

		printf("\n****************job_test_6**************\n");

		job* j = new job(&g_test_tp);

		j->set_auto_destroy(true);

		for(int i = 0; i < 10; ++i){

			j->add_child_task(new test_cjob);

		}

		j->set_combiner(new test_combainer_cancel);

		j->run();

		j->cancel();

		return 0;
	}

};
