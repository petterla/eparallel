#include "ef_id_pool.h"
#include "be_atomic.h"

namespace ef{
	uint32	get_inc_id(){
		static volatile be::s32 id = 0;
		be::atomic_increment32((volatile be::s32*)&id);
		be::atomic_compare_exchange32((volatile be::s32*)&id, 0, 1);
		return	id;	
	}
}
