/*
 *  SMSG_GSHandoff.h
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 02.06.11.
 *  Copyright 2011 Overheat. All rights reserved.
 *
 */

#ifndef _TR_SMSG_AUTHGSHANDOFF_H_
#define _TR_SMSG_AUTHGSHANDOFF_H_

#include "serverpacket.h"

class SMSG_AuthGSHandoff : public CServerPacket
{
private:
	uint64_t sessionID;
	uint8_t serverID;
public:
	SMSG_AuthGSHandoff(uint64_t sessionID, uint8_t serverID)
	: CServerPacket( AuthCodes::AuthGSHandoff )
	{
		this->sessionID = sessionID;
		this->serverID = serverID;
	}
	
	void write_impl( CPacketBuffer& buf )
	{
		buf.put( get_opcode() );
		buf.putUInt( (sessionID & 0xFFFFFFFFFF) );
		buf.putUInt( (sessionID >> 32) );
		buf.put( serverID );
	}
};

#endif