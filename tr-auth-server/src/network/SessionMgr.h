/*
 *  SessionMgr.h
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 02.06.11.
 *  Copyright 2011 Overheat. All rights reserved.
 *
 */

#ifndef _TR_SESSIONMGR_H_
#define _TR_SESSIONMGR_H_

#include "types.h"
#include <ctime>
#include <cstdlib>
#include <sstream>
#include <string>
#include "DBMgr.h"

namespace tr
{
	namespace net
	{
		class CSessionMgr
		{
		private:
			static CSessionMgr* _instance;
			CSessionMgr()
			{
				session_timeout = 50;
				server_timeout = 120;
			}
			uint64_t generate_unique_key();
		public:
			uint32_t session_timeout;
			uint32_t server_timeout;
			static CSessionMgr& instance();
			uint64_t generate_session( const char* acc, int64_t UID );
			void remove_expired_sessions();
			void remove_expired_servers();
			
			void wipe_servers();
			void wipe_sessions();
		};
	}
}
#endif