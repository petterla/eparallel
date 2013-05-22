#ifndef	EF_WORK_THREAD_H
#define	EF_WORK_THREAD_H

#include "ef_common.h"
#include "mysql_adapter.h"


namespace	group{

using namespace ef;
	class	group_db;

	class	work_thread{
	public:
		work_thread(group_db *db,
			const char* host = "", const char* user = "", 
			const char* passwd = "" , int32 port = 0, 
			const char* database = "");

		
		~work_thread();

		int32	init();
		int32	clear();
		int32	run();
		ma::adapter& mysql_adapter(){
			return m_mysql;
		}		
	private:

		static const char *tag; 
		group_db	*m_db;
		const char	*m_host;
		const char	*m_user;
		const char	*m_passwd;
		int32		m_port;
		const char	*m_dbtabase;
		ma::adapter     m_mysql;

	};

};

#endif/*EF_WORK_THREAD_H*/
