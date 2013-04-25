#ifndef ZVM_LOCK_H
#define ZVM_LOCK_H

#include "zvm_type.h"

namespace zvm{

	class	simple_lock{
	public:
		simple_lock():
			m_owner(0),
			m_cnt(0){

		}

		s32	lock(s32 par);

		s32 unlock(s32 par);

		s32	try_lock(s32 par);

		s32 try_unlock(s32 par);

	private:
		volatile	s32	m_owner;
		s32			m_cnt;
	};

	class	spin_lock{
	public:
		spin_lock():
		  m_flag(0){

		}

		s32 lock(s32 par);

		s32 unlock(s32 flag);

		s32 try_lock(s32 par);

		s32 try_unlock(s32 par);

	private:
		volatile	s32	m_flag;
	};

	class	rw_lock{

	};

	template<class T>
	class auto_lock{
	public:
		auto_lock(T& t, s32 p)
			:m_t(t),m_p(p){

			m_t.lock(p);

		}

		~auto_lock(){

			m_t.unlock(m_p);

		}

	private:
		T& m_t;
		s32 m_p;
	};

	typedef	auto_lock<simple_lock> auto_simple_lock;
	typedef	auto_lock<spin_lock> auto_spin_lock;

	template<class T>
	class auto_unlock{
	public:
		auto_unlock(T& t, s32 p)
			:m_t(t),m_p(p){

		}

		~auto_unlock(){

			m_t.unlock(m_p);

		}

	private:
		T& m_t;
		s32 m_p;
	};

	typedef	auto_unlock<simple_lock> auto_simple_unlock;
	typedef	auto_unlock<spin_lock> auto_spin_unlock;

}

#endif

