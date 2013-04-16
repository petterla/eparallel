#include "ep_thread_pool_test.h"
#include "ep_task.h"
#include "ep_thread_pool.h"
#include <iostream>
#include <vector>

namespace ep{

	class	ttask:public task{
	public:
		ttask(){
			
		}
		virtual	s32	process(){

			++m_cnt;

			std::cout << "ttask:" 
				
				<< m_cnt <<std::endl;

			be::be_sleep(100);

			return	0;

		}
	private:

		volatile static int	m_cnt;

	};

	volatile int ttask::m_cnt = 0;

	s32	thread_pool_test_1(){
		
		thread_pool	tp(1, 3);

		tp.init();

		tp.run();

		for(int i = 0; i < 100; ++i){

			tp.add_task(new ttask(), 0);

			be::be_sleep(20);

		}
		
		std::cerr << "stop" << std::endl;

		tp.stop();

		tp.free();

		return	0;

	}


	s32	thread_pool_test_2(){

		thread_pool	tp(1, 2);

		tp.init();

		tp.run();

		std::vector<ttask*> tlist;

		for(int i = 0; i < 2; ++i){

			ttask* t = new ttask();

			tlist.push_back(t);

		}

		for(int i = 0; i < 2; ++i){

			tp.add_task(tlist[i], 0);

		}

		for(int i = 0; i < 2; ++i){

			tp.del_task(tlist[i], 0);

		}

		std::cerr << "stop" << std::endl;

		tp.stop();

		tp.free();

		return	0;

	}

	s32 std_list_test(){

		typedef std::list<int> tlist;

		tlist l;

		tlist::iterator itor;

		tlist::iterator itor1;

		l.push_back(1);

		itor = --l.end();

		l.push_back(2);

		itor1 = --l.end();
		
		printf("itor:%d, itor1:%d\n", *itor, *itor1);

		return 0;

	}


	s32 std_list_test_1(){

		typedef std::list<int> tlist;

		tlist l;

		tlist::iterator itor;

		tlist::iterator itor1;

		l.push_back(1);

		itor = --l.end();

		l.erase(itor);

		l.push_back(2);

		itor1 = --l.end();

		printf("itor:%d, itor1:%d\n", *itor, *itor1);

		return 0;

	}

};