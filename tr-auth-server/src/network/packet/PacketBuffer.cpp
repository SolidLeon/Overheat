/*
 *  PacketBuffer.cpp
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 29.05.11.
 *  Copyright 2011 HTL. All rights reserved.
 *
 */

#include "PacketBuffer.h"

using tr::net::packet::CPacketBuffer;

CPacketBuffer::CPacketBuffer( int capacity ) : m_bo( BO_BIG_ENDIAN )
{
	m_capacity = capacity;
	m_limit = m_capacity;
	
	m_buf = new uint8_t[m_capacity];
	reset();
}

CPacketBuffer::~CPacketBuffer( )
{
	delete[] m_buf;
}

void CPacketBuffer::reset()
{
	m_position = 0;
}

void CPacketBuffer::rewind()
{
	m_position = 0;
	m_mark = 0;
}

void CPacketBuffer::flip()
{
	m_limit = m_position;
	m_position = 0;
	m_mark = 0;
}

void CPacketBuffer::clear()
{
	m_position = 0;
	m_mark = 0;
	m_limit = m_capacity;
}

uint32_t CPacketBuffer::remaining()
{
	return m_limit - m_position;
}
CPacketBuffer& CPacketBuffer::slice()
{
	CPacketBuffer* pb = new CPacketBuffer( remaining() );
	while( remaining() )
	{
		pb->put( get() );
	}
	pb->rewind();
	return *pb;
}

void CPacketBuffer::byte_order( ByteOrder bo )
{
	this->m_bo = bo;
}

uint8_t CPacketBuffer::get()
{
	if( m_position > m_limit )
	{
		throw "BufferUnderflowException";
	}
	uint8_t v = m_buf[m_position];
	m_position += 1;
	return v;
}

uint16_t CPacketBuffer::getUShort()
{
	uint16_t v = 0;
	if( m_bo == BO_BIG_ENDIAN )
	{
		v = m_buf[m_position]<<8;
		v = m_buf[m_position+1];
	}
	else 
	{
		v = m_buf[m_position];
		v = m_buf[m_position+1]<<8;
	}
	m_position += 2;
	return v;
}

uint32_t CPacketBuffer::getUInt()
{
	uint16_t v = 0;
	if( m_bo == BO_BIG_ENDIAN )
	{
		v = m_buf[m_position]<<24;
		v = m_buf[m_position+1]<<16;
		v = m_buf[m_position+2]<<8;
		v = m_buf[m_position+3];
	}
	else 
	{
		v = m_buf[m_position];
		v = m_buf[m_position+1]<<8;
		v = m_buf[m_position+2]<<16;
		v = m_buf[m_position+3]<<24;
	}
	m_position += 4;
	return v;
}

void CPacketBuffer::put( uint8_t b )
{
	if( m_position > m_limit )
	{
		throw "BufferOverflowException";
	}
	m_buf[m_position] = b;
	m_position += 1;
}

void CPacketBuffer::wrap( uint8_t* src, int len )
{
	if( len > m_capacity )
		len = m_capacity;
	memcpy( m_buf, src, len );
	m_position = len;
}

void CPacketBuffer::putUShort( uint16_t h )
{
	if( m_bo == BO_BIG_ENDIAN )
	{
		m_buf[m_position]	= (uint8_t)(h>>8);
		m_buf[m_position+1] = (uint8_t)h;
	}
	else
	{
		m_buf[m_position]	= (uint8_t)h;
		m_buf[m_position+1]	= (uint8_t)(h>>8);
	}
	m_position += 2;
}

void CPacketBuffer::putUInt( uint32_t d ) 
{
	if( m_bo == BO_BIG_ENDIAN )
	{
		m_buf[m_position]	= (uint8_t)(d>>24);
		m_buf[m_position+1]	= (uint8_t)(d>>16);
		m_buf[m_position+2]	= (uint8_t)(d>>8);
		m_buf[m_position+3] = (uint8_t)d;
	}
	else
	{
		m_buf[m_position]	= (uint8_t)(d);
		m_buf[m_position+1]	= (uint8_t)(d>>8);
		m_buf[m_position+2]	= (uint8_t)(d>>16);
		m_buf[m_position+3]	= (uint8_t)(d>>24);
	}
	m_position += 4;
}

void CPacketBuffer::putChar( char c )
{
	m_buf[m_position] = c;
	m_position += 1;
}

CPacketBuffer& CPacketBuffer::operator<<( uint8_t b )
{
	put(b);
	return *this;
}

CPacketBuffer& CPacketBuffer::operator<<( char c )
{
	putChar(c);
	return *this;
}

CPacketBuffer& CPacketBuffer::operator<<( const char* str )
{
	for( int i = 0; i < strlen(str); i++)
		putChar(str[i]);
	return *this;
}

void CPacketBuffer::debug_out( )
{
	//printf("\nPacket DEBUG\n");
	//printf("Position: %d\n", position());
	//printf("Limit: %d\n", limit());
	///printf("Capacity: %d\n", capacity());
	//printf("Mark: %d\n", m_mark);
	//printf("Remaining: %d\n", remaining());
	//printf("-- DATA --\n");
	printf("--------------------------------------------------------------------------------\nPacket: %d\n", limit());
	for(int i = 0; i < remaining(); i++)
	{
		if(i%0x10 == 0) printf("\n%04X: ", i);
		printf("%02X  ", m_buf[i]);
	}
	printf("\n\n");
}