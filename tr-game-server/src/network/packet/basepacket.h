/*
 *  basepacket.h
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 30.05.11.
 *  Copyright 2011 Overheat. All rights reserved.
 *
 */

#ifndef _BASEPACKET_H_
#define _BASEPACKET_H_

namespace tr
{
	namespace net
	{
		namespace packet
		{
			class CBasePacket
			{
			public:
				enum PacketType
				{
					CLIENT_PACKET,
					SERVER_PACKET
				};
			private:
				int opcode;
				int packet_size;
				PacketType packet_type;
			protected:
				void set_packet_size( int packet_size )
				{
					this->packet_size = packet_size;
				}
				void set_opcode( int opcode )
				{
					this->opcode = opcode;
				}
			public:
				CBasePacket( int opcode, PacketType packet_type )
				: packet_type( packet_type )
				{
					this->opcode = opcode;
					packet_size = 0;
				}
				
				int get_opcode( ) const
				{
					return this->opcode;
				}
				
				int get_packet_size() const
				{
					return this->packet_size;
				}
				
				PacketType get_packet_type() const
				{
					return packet_type;
				}
				
			};//CBasePacket
		}//packet
	}//net
}//tr
#endif _BASEPACKET_H_