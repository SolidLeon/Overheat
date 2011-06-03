/*
 *  serverpacket.h
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 30.05.11.
 *  Copyright 2011 HTL. All rights reserved.
 *
 */


#ifndef _SERVERPACKET_H_
#define _SERVERPACKET_H_

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
			class CServerPacket : public CBasePacket
			{
			public:
				CServerPacket( int opcode )
				: CBasePacket( opcode, CBasePacket::SERVER_PACKET )
				{
				}
				
				void write( CConnection& conn )
				{
					CPacketBuffer& pb = conn.get_write_buffer();
					pb.clear();
					pb.putUShort(0);
					
					write_impl( pb );
					
					pb.flip();
					pb.putUShort( pb.remaining() );
					pb.rewind();
					
					//just to be correct, update packet_size
					set_packet_size(pb.remaining());
				}
				
				virtual void write_impl( CPacketBuffer& buf ) = 0;
				
			};//CBasePacket
		}//packet
	}//net
}//tr

#endif//_SERVERPACKET_H_
