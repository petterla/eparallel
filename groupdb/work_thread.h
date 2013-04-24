#ifndef	EF_WORK_THREAD_H
#define	EF_WORK_THREAD_H

#include "ef_common.h"

using	namespace	ef;

namespace	group{

	class	group_db;

	class	work_thread{
	public:
		work_thread(group_db *db,
			const char* addr = "", const char* user = "", const char* passwd = "" );

		
		~work_thread();

		int32	init();

		int32	clear();

		int32	run();
		
	private:

		static const char *tag; 

		group_db	*m_db;

	};

};

#endif/*EF_WORK_THREAD_H*/
