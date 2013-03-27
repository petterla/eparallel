#include "zvm_op_test.h"
#include "zvm_op_code.h"
#include "zvm_op_proc.h"
#include "zvm_function.h"
#include "zvm_class.h"
#include <iostream>
#include <time.h>

namespace zvm{

	int zvm_op_test(){

		stack	s;
		s.push_frame();
		
		zvm_push_pop_test(s);
		zvm_int_op_test(s);
		zvm_function_test(s);
		zvm_class_test(s);

		return	0;

	}

	int zvm_push_pop_test(stack& s){

		return	0;
	}

	int	zvm_load_op_test(stack&  s){

		
		return	0;

	}

	//op num push from right
	int	zvm_int_op_test(stack&	s){

		s.eax() = 1;
		s.ebx() = 2;

		operation	oa;
		oa.m_op_code = IADD;

		assert(SUCCESS == op_process(&oa,&s));
		assert(3 == s.eax());
		assert(0 == s.get_op_stack().size());

		s.eax() = 2;
		s.ebx() = 1;

		operation	os;
		os.m_op_code = ISUB;

		assert(SUCCESS == op_process(&os,&s));
		assert(1 == s.eax());
		assert(0 == s.get_op_stack().size());

		s.eax() = 2;
		s.ebx() = 3;

		operation	om;
		om.m_op_code = IMUT;
		assert(SUCCESS == op_process(&om,&s));
		assert(6 == s.eax());
		assert(0 == s.get_op_stack().size());

		s.eax() = 7;
		s.ebx() = 2;

		operation	od;
		od.m_op_code = IDIV;
		assert(SUCCESS == op_process(&od,&s));
		assert(7 / 2 == s.eax());
		assert(0 == s.get_op_stack().size());

		s.eax() = 7;
		s.ebx() = 4;

		operation	or;
		or.m_op_code = IREM;
		assert(SUCCESS == op_process(&or,&s));
		assert(7 % 4 == s.eax());
		assert(0 == s.get_op_stack().size());

		return	0;

	}

	static int zvm_function_test0(stack& s){

		function f("test_function0");

		operation op0 = {PUSH_ICONST};
		op0.m_op.m_push_iconst.m_i = 1;
		operation op1 = {PUSH_ICONST};
		op1.m_op.m_push_iconst.m_i = 2;
		operation op2 = {ILOADA};
		op2.m_op.m_iloada.m_idx = 0;
		operation op3 = {ILOADB};
		op3.m_op.m_iloadb.m_idx = 1;
		operation op4 = {IADD};

		f.add_op(&op0);
		f.add_op(&op1);
		f.add_op(&op2);
		f.add_op(&op3);
		f.add_op(&op4);

		s.current_frame().set_function((s64)&f);

		assert(SUCCESS == f.process(&s));
		assert(3 == s.eax());
		assert(0 == s.current_frame().size());
		assert(0 == s.get_op_stack().size());

		operation op5 = {ISTOREA};
		op5.m_op.m_istorea.m_idx = 0;
		operation op6 = {PUSH_NULL};

		f.add_op(&op5);
		f.add_op(&op6);
		assert(SUCCESS == f.process(&s));
		assert(0 == s.current_frame().size());
		assert(0 == s.get_op_stack().size());


		operation op7 = {JUMP};
		f.add_op(&op7);
		assert(SUCCESS != f.process(&s));
		assert(0 == s.current_frame().size());
		assert(0 == s.get_op_stack().size());

		return	0;
	}

	static s64 test_f(s64 c){
		s64 sum = 0;

		for(s64 i = 0; i < c; ++i){
			sum += i;
		}
	
		return	sum;
	}


	static int zvm_function_test1(stack& s){

		function f("test_function0");
		s.clear();

		/**************************
		int sum = 0;
		int i = 0;
		for(; i < 10; ++i){
			sum += i;
		}
		return sum;
		**************************/

		s64 i = 0;

		std::cout << "input loop count:" <<std::endl;
		std::cin >> i;

		//sum = 0;
		operation op0 = {PUSH_ICONST};
		op0.m_op.m_push_iconst.m_i = 0;
		//i = 0;
		operation op1 = {PUSH_ICONST};
		op1.m_op.m_push_iconst.m_i = 0;
		//sum += i;
		operation op2 = {ILOADA};
		op2.m_op.m_iloada.m_idx = 0;
		operation op3 = {ILOADB};
		op3.m_op.m_iloadb.m_idx = 1;
		operation op4 = {IADD};
		operation op5 = {ISTOREA};
		op5.m_op.m_istorea.m_idx = 0;
		//++i;
		operation op6 = {ILOADA};
		op6.m_op.m_iloada.m_idx = 1;
		operation op7 = {IINC};
		operation op8 = {ISTOREA};
		op8.m_op.m_istorea.m_idx = 1;

		operation op9 = {ICONSTB};
		op9.m_op.m_iconstb.m_i = i;
		operation op10 = {ILOADA};
		op10.m_op.m_iloada.m_idx = 1;
		operation op11 = {JSM};
		op11.m_op.m_jsm.m_pc = 2;
		//return sum
		operation op12 = {ILOADA};
		op12.m_op.m_iloada.m_idx = 0;
		operation op13 = {IRET};

		f.add_op(&op0);
		f.add_op(&op1);
		f.add_op(&op2);
		f.add_op(&op3);
		f.add_op(&op4);
		f.add_op(&op5);
		f.add_op(&op6);
		f.add_op(&op7);
		f.add_op(&op8);
		f.add_op(&op9);
		f.add_op(&op10);
		f.add_op(&op11);
		f.add_op(&op12);
		f.add_op(&op13);
		s.current_frame().set_function((s64)&f);

		time_t t = time(NULL);

		s64 k = test_f(i * 60);
		time_t n = time(NULL);

		int ret = f.process(&s);
		s64 j = s.eax();
		assert(0 == s.current_frame().size());

		time_t n1 = time(NULL);

		std::cout << "loop " << i << " :test_function0() spend:" 
			<< n1 - n << " s,f() spend:" << n - t 
			<< " s, j:"<< j << " ,k:" << k << std::endl;

		return	0;
	}

	int zvm_function_test(stack& s){

		zvm_function_test0(s);

		zvm_function_test1(s);

		return	0;
	}

	int zvm_class_test(stack& s){
		classdef c("test_class");

		c.add_member(LOCAL_TYPE_INT);
		c.add_member(LOCAL_TYPE_FLOAT);

		obj* o = c.create_ent_obj(&s);

		assert(o->get_member_count(&s) == 2);

		s.deallocate(o);

		return	0;

	}

}