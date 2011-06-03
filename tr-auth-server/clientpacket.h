/*
 *  clientpacket.h
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 01.06.11.
 *  Copyright 2011 InfiniteRasa. All rights reserved.
 *
 */

#ifndef _TR_CLIENTPACKET_H_
#define _TR_CLIENTPACKET_H_

#include "basepacket.h"
#include "PacketBuffer.h"
#include "connection.h"

using tr::crypto::CCryptMgr;

namespace tr
{
	namespace net
	{
		namespace packet
		{
			class CClientPacket : public CBasePacket
			{
			protected:
				virtual void read_impl( CPacketBuffer& buf ) = 0;
			public:
				CClientPacket( )
				: CBasePacket(0, CBasePacket::CLIENT_PACKET )
				{
					
				}
				void read( CPacketBuffer& buf )
				{
					//read packet size
					set_packet_size( buf.getUShort() );
					//read opcode
					set_opcode( buf.get() );
					read_impl( buf );
				}
			};
		}
	}
}

#endif