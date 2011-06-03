/*
 *  SMSG_AuthErrorAccountBlocked.h
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 31.05.11.
 *  Copyright 2011 Overheat. All rights reserved.
 *
 */



#ifndef _TR_SMSG_AUTHERRORACCOUNTBLOCKED_H_
#define _TR_SMSG_AUTHERRORACCOUNTBLOCKED_H_

#include "serverpacket.h"
#include "authcodes.h"
using namespace tr::net;
using namespace tr::net::packet;

class SMSG_AuthErrorAccountBlocked : public CServerPacket
{
private:
	int err_code;
public:
	SMSG_AuthErrorAccountBlocked( int err_code )
	: CServerPacket( AuthCodes::AuthErrorAccountBlocked )
	{
		this->err_code = err_code;
	}
	
	void write_impl( CPacketBuffer& buf )
	{
		buf.put( get_opcode() );
		buf.putUInt(err_code);
	}
};

#endif