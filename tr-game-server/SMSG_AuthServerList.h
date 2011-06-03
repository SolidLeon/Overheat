/*
 *  SMSG_AuthServerList.h
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 01.06.11.
 *  Copyright 2011 InfiniteRasa. All rights reserved.
 *
 */

#ifndef _TR_SMSG_AUTHSERVERLIST_H_
#define _TR_SMSG_AUTHSERVERLIST_H_

#include "serverpacket.h"
#include "authcodes.h"

using namespace tr::net;
using tr::net::packet::CServerPacket;

class CAuthServer
{
public:
		unsigned char server_id;
		unsigned int host;
		unsigned int port;
		unsigned char age_limit;
		unsigned char pk_flag;
		unsigned short current_user_count;
		unsigned short max_user_count;
		unsigned char server_status;
};

class SMSG_AuthServerList : public CServerPacket
{
private:
	unsigned char server_count;
	unsigned char last_server_id;
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
		
		CAuthServer s = {
			1, 16777343, 8001, 18, 1, 1, 100, 1
		};
		
		buf.put(server_count);
		buf.put(last_server_id);
		//TODO: Loop through server list
		buf.put(s.server_id);
		buf.putUInt(s.host);
		buf.putUInt(s.port);
		buf.put(s.age_limit);
		buf.put(s.pk_flag);
		buf.putUShort(s.current_user_count);
		buf.putUShort(s.max_user_count);
		buf.put(s.server_status);
		
		
	}
	
};


#endif