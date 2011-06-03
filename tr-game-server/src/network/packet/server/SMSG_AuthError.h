/*
 *  SMSG_AuthError.h
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 31.05.11.
 *  Copyright 2011 InfiniteRasa. All rights reserved.
 *
 */

#ifndef _TR_SMSG_AUTHERROR_H_
#define _TR_SMSG_AUTHERROR_H_

#include "serverpacket.h"
#include "authcodes.h"
using namespace tr::net;
using namespace tr::net::packet;

class SMSG_AuthError : public CServerPacket
{
private:
	int err_code;
public:
	SMSG_AuthError( int err_code )
	: CServerPacket( AuthCodes::AuthError )
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