/*
 *  PacketBuffer.h
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 29.05.11.
 *  Copyright 2011 HTL. All rights reserved.
 *
 */

#ifndef _PACKETBUFFER_H_
#define _PACKETBUFFER_H_

#include <iostream>
#include <string>
#include "types.h"

namespace tr
{
	namespace net
	{
		namespace packet
		{
			// PacketBuffer
			//Stores byte data
			//GET and PUT to modify the buffer
			//-SolidLeon
			class CPacketBuffer
			{
			public:
				enum ByteOrder { BO_LITTLE_ENDIAN, BO_BIG_ENDIAN };
			private:
				uint8_t *m_buf;
				int m_capacity; //always size of buf
				int m_position; //current pointer, buf[position]
				int m_limit; //current limit - limits the buffer to a "sub window"
				int m_mark;
				ByteOrder m_bo;
			public:
				CPacketBuffer( int );
				~CPacketBuffer( );
				
				void flip();
				void reset();
				void rewind();
				void clear();
				CPacketBuffer& slice();				
				uint32_t remaining();
				
				void byte_order( ByteOrder );
				
				uint8_t get( );
				uint16_t getUShort( );
				uint32_t getUInt( );
				
				CPacketBuffer& put( uint8_t );
				void wrap( uint8_t*, int len );
				void putArray( uint8_t*, int len );
				void putChar( char );
				void putUShort( uint16_t );
				void putUInt( uint32_t );
				
				CPacketBuffer& operator<<( uint8_t );
				CPacketBuffer& operator<<( char );
				CPacketBuffer& operator<<( const char* );
				
				int capacity() const { return m_capacity; }
				int limit() const { return m_limit; }
				uint8_t* array() const { return m_buf; }
				int position() const { return m_position; }
				void limit(int new_limit) { m_limit = new_limit; }
				void position(int new_position) 
				{ 
					m_position = new_position; 
					if( m_position > m_capacity )
					{
						m_position = m_capacity;
					}
				}
				
				void debug_out( );
				
			};//CPacketBuffer
		}//packet
	}//net
}//tr

#endif//_PACKETBUFFER_H_