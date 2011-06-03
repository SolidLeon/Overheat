/*
 *  SMSG_HELLO.h
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 30.05.11.
 *  Copyright 2011 Overheat. All rights reserved.
 *
 */


#ifndef _TR_SMSG_HELLO_
#define _TR_SMSG_HELLO_

#include "serverpacket.h"
#include "authcodes.h"

using namespace tr::net;
using tr::net::packet::CServerPacket;

class SMSG_HELLO : public CServerPacket
{
private:
public:
	SMSG_HELLO()
	: CServerPacket( AuthCodes::AuthHello )
	{
	}
	
	void write_impl( CPacketBuffer& buf )
	{		
		buf.put( get_opcode() );
		buf.putUInt(0xDEAD0E01);
		buf.putUInt(0x00);
	}
	
};

#endif