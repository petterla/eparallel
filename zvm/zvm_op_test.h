#ifndef ZVM_OP_TEST
#define ZVM_OP_TEST

namespace zvm{

	class stack;

	int zvm_op_test();

	int	zvm_load_op_test(stack& s);

	int	zvm_int_op_test(stack& s);

	int zvm_push_pop_test(stack& s);

	int zvm_function_test(stack& s);

}

#endif