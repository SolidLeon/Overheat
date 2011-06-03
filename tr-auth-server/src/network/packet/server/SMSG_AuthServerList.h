/*
 *  SMSG_AuthServerList.h
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 01.06.11.
 *  Copyright 2011 Overheat. All rights reserved.
 *
 */

#ifndef _TR_SMSG_AUTHSERVERLIST_H_
#define _TR_SMSG_AUTHSERVERLIST_H_

#include "serverpacket.h"
#include "authcodes.h"
#include "DBMgr.h"

using namespace tr::net;
using tr::net::packet::CServerPacket;
using tr::util::DBMgr;
class CAuthServer
{
public:
		uint8_t server_id;
		uint32_t host;
		uint32_t port;
		uint8_t age_limit;
		uint8_t pk_flag;
		uint16_t current_user_count;
		uint16_t max_user_count;
		uint8_t server_status;
};

class SMSG_AuthServerList : public CServerPacket
{
private:
	uint8_t server_count;
	uint8_t last_server_id;
public:
	SMSG_AuthServerList()
	: CServerPacket( AuthCodes::AuthServerList )
	{
		server_count = 1;
		last_server_id = 0x35;
	}
	
	void write_impl( CPacketBuffer& buf )
	{		
		buf.put( get_opcode() );
		
		MYSQL_RES* res;
		DBMgr::get_instance().query("SELECT server_id, host, port, age_limit, pk_flag, current_users, max_users, status FROM game_servers");
		res = DBMgr::get_instance().store_result();
		uint8_t server_count = DBMgr::get_instance().num_rows(res);
		
		buf.put(server_count);
		buf.put(last_server_id);
		MYSQL_ROW row;
		
		while( (row = DBMgr::get_instance().fetch_row(res)) )
		{
			CAuthServer s = {
				(uint8_t)atoi(row[0]), //server_id
				strtoul(row[1], NULL,10), //host 
				atoi(row[2]), //port
				(uint8_t)atoi(row[3]), //age_limit
				(uint8_t)atoi(row[4]), //pk_flag
				atoi(row[5]), //current_user_count
				atoi(row[6]), //max_user_count
				(uint8_t)atoi(row[7])  //server_status
			};
			buf.put(s.server_id);
			buf.putUInt(s.host);
			buf.putUInt(s.port);
			buf.put(s.age_limit);
			buf.put(s.pk_flag);
			buf.putUShort(s.current_user_count);
			buf.putUShort(s.max_user_count);
			buf.put(s.server_status);
		}
		
		
	}
	
};


#endif