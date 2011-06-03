/*
 *  GSMgr.h
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 02.06.11.
 *  Copyright 2011 Overheat. All rights reserved.
 *
 */
 
#ifndef _TR_GSMGR_H_
#define _TR_GSMGR_H_

#include <vector>
#include "types.h"

namespace tr
{
	namespace net 
	{
		class CGSMgr
		{
		private:
			struct GameServer_t
			{
				uint8_t server_id;
				uint32_t host;
				uint32_t port;
				uint8_t age_limit;
				uint8_t pk_flag;
				uint16_t current_users;
				uint16_t max_users;
				uint8_t status;
				uint64_t check_time;
				uint8_t _static;
			};
			std::vector<GameServer_t*> game_servers;
		public:
			void load_gs_table();
		};
	}
}

#endif

