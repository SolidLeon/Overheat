/*
 *  SMSG_AuthOk.h
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 01.06.11.
 *  Copyright 2011 Overheat. All rights reserved.
 *
 */
 
#ifndef _TR_SMSG_AUTHOK_H_
#define _TR_SMSG_AUTHOK_H_
 
#include "serverpacket.h"
#include "authcodes.h"

using namespace tr::net;
using tr::net::packet::CServerPacket;

class SMSG_AuthOk : public CServerPacket
{
private:
public:
	SMSG_AuthOk()
	: CServerPacket( AuthCodes::AuthOk )
	{
	}
	
	void write_impl( CPacketBuffer& buf )
	{		
		buf.put( get_opcode() );
		buf.putUInt(0x00);
		buf.putUInt(0x00);
		buf.putUInt(0x00);
		buf.putUInt(0x00);
		buf.putUInt(0x00);
		buf.putUInt(0x00);
		buf.putUInt(0x00);
		buf.putUInt(0x00);
		buf.putUInt(0x00);
		buf.put(0x00);
	}
	
};
 
#endif

