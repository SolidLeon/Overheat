/*
 *  CMSG_AuthLogin.h
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 01.06.11.
 *  Copyright 2011 InfiniteRasa. All rights reserved.
 *
 */

#ifndef _TR_CMSG_AUTHLOGIN_H_
#define _TR_CMSG_AUTHLOGIN_H_



#include "serverpacket.h"
#include "authcodes.h"

using namespace tr::net;
using tr::net::packet::CServerPacket;

class CMSG_AuthLogin
{
private:
public:
	CMSG_AuthLogin()
	{
	}
	
	void read_impl( CPacketBuffer& buf )
	{
		packet_size = buf.getUShort();
		opcode = buf.get();
		
		
		unsigned char data[30];
		memcpy((void*)data, (void*)(m_in.array()+3), 30);
		//Special decryption just for user data
		CCryptMgr::instance().get_tr_crypt().TRDecrypt((unsigned char*) data, 30);
		
		char acc[14];
		char pw[16];
		memcpy((void*)acc, (void*)data, 14);
		memcpy((void*)pw, (void*)(data+14), 16);
		
		//TODO: Check user against DB
	}
	
};

#endif