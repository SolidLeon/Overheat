/*
 *  DBMgr.h
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 01.06.11.
 *  Copyright 2011 InfiniteRasa. All rights reserved.
 *
 */

#ifndef _TR_DBMGR_H_
#define _TR_DBMGR_H_

#include <mysql.h>
#include <string>

namespace tr
{
	namespace util
	{
		class DBMgr
		{
		private:
			static DBMgr *instance;
			
			MYSQL *dbHandle;
			std::string dbHost;
			uint32_t dbPort;
			std::string dbUser;
			std::string dbName;
			std::string dbPass;
			std::string UsersTable;
			
			DBMgr(const char *dbHost, uint32_t dbPort, const char *dbUser, const char *dbPass, const char *dbName);
		public:
			~DBMgr();
			static DBMgr& get_instance();
			static DBMgr& create(const char *dbHost, uint32_t dbPort, const char *dbUser, const char *dbPass, const char *dbName);
			
			bool query(const char *sql);
			MYSQL_RES* store_result();
			unsigned long long num_rows(MYSQL_RES* result);
			MYSQL_ROW fetch_row(MYSQL_RES* result);
			void free_result(MYSQL_RES* result);
			
			int validate_player( const char *name, const char *pw, int64_t *UID );
		};
	}
}

#endif