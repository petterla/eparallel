#include "zvm_op_test.h"
#include "be_atomic.h"

int main(){

	be::atomic_test();
	zvm::zvm_op_test();

	return	0;

}
