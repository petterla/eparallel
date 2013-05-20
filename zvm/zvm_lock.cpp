#include "zvm_lock.h"
#include "be_atomic.h"

namespace zvm{

	s32	simple_lock::lock(s32	par){

		if(m_owner	==	par){
			++m_cnt;
			return	0;
		}

		while (be::atomic_compare_exchange32(
			(volatile s32*)&m_owner, par, 0) != 0);
		assert(m_cnt == 0);
		++m_cnt;

		return	0;
	}

	s32	simple_lock::unlock(s32	par){

		if(m_owner	!=	par){
			return	-1;
		}

		--m_cnt;
		if(!m_cnt){
			be::atomic_exchange32((volatile	s32*)&m_owner, 0);
		}

		return	0;

	}

	s32	simple_lock::try_lock(s32 par){

		if(m_owner	==	par){

			++m_cnt;
			return	0;

		}

		if(be::atomic_compare_exchange32(
			(volatile s32*)&m_owner, par, 0) == 0){
			assert(m_cnt == 0);
			++m_cnt;
			return	0;		

		}

		return	-1;

	}

	s32 simple_lock::try_unlock(s32 par){

		if(m_owner	!=	par){
			return	-1;
		}

		--m_cnt;
		if(!m_cnt){
			be::atomic_exchange32((volatile	s32*)&m_owner,	0);
		}

		return	0;

	}

	s32	spin_lock::lock(s32 par){

		while (be::atomic_compare_exchange32(
			(volatile s32*)&m_flag, 1, 0) != 0);

		return	0;

	}

	s32	spin_lock::unlock(s32 par){

		if(be::atomic_compare_exchange32(
			(volatile s32*)&m_flag, 0, 1) == 1){
			return	0;
		}

		return	-1;

	}

	s32	spin_lock::try_lock(s32 par){

		if(be::atomic_compare_exchange32(
			(volatile s32*)&m_flag, 1, 0) == 0){
			return	0;		
		}

		return	-1;

	}

	s32 spin_lock::try_unlock(s32 par){

		if(be::atomic_compare_exchange32(
			(volatile s32*)&m_flag, 0, 1) == 1){
			return	0;
		}

		return	-1;

	}

}

