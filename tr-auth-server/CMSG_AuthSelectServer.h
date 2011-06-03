/*
 *  CMSG_AuthSelectServer.h
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 01.06.11.
 *  Copyright 2011 InfiniteRasa. All rights reserved.
 *
 */

#ifndef _TR_CMSG_AUTHSELECTSERVER_H_
#define _TR_CMSG_AUTHSELECTSERVER_H_

#include "clientpacket.h"
//#include "authcodes.h"

using namespace tr::net;
using tr::net::packet::CClientPacket;

class CMSG_AuthSelectServer : public CClientPacket
{
private:
	unsigned int unk1;
	unsigned int unk2;
	unsigned char server_id;
public:
	void read_impl( CPacketBuffer& buf )
	{
		//size and opcode gets read by super classes
		unk1 = buf.getUInt();
		unk2 = buf.getUInt();
		server_id = buf.get();
	}
	
	unsigned int get_unk1() const { return unk1; }
	unsigned int get_unk2() const { return unk2; }
	unsigned char get_server_id() const { return server_id; }
};

#endif