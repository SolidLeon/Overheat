/*
 *  connection.cpp
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 30.05.11.
 *  Copyright 2011 Overheat. All rights reserved.
 *
 */

#include "connection.h"

using namespace tr::net;

CConnection::CConnection( CSocket& socket, CSelector& selector )
: socket( socket ), selector( selector ), m_out( 8192 ), m_in( 8192 )
{
	m_out.byte_order(CPacketBuffer::BO_LITTLE_ENDIAN);
	m_in.byte_order(CPacketBuffer::BO_LITTLE_ENDIAN);
	
	unsigned int IP = 0;
	sockaddr_in peername;
	socklen_t pLen = sizeof( peername );
	getsockname( socket.get_socket(), (sockaddr*)&peername, &pLen);
	IP = peername.sin_addr.s_addr;
	sprintf(ip, "%d.%d.%d.%d",(IP&0xFF), ((IP>>8)&0xFF), ((IP>>16)&0xFF), ((IP>>24)&0xFF));
	printf(">> New Connection from: %s\n", ip);
}

void CConnection::on_read()
{
	//printf("CConnection: on_read\n");
	try 
	{
		socket.read( m_in );
	} 
	catch(CConnectionClosedEx ex)
	{
        throw;
	}
}



CSocket& CConnection::get_socket()
{
	return socket;
}

std::vector<CPacketBuffer*>& CConnection::get_msg_queue()
{
	return msg_queue;
}

bool CConnection::is_close_requested() const
{
	return !closed && close_requested;
}

bool CConnection::is_closed() const
{
	return socket.get_socket() < 0 || closed;
}

void CConnection::close()
{
	close_requested = true;
}

void CConnection::on_close()
{
	socket.close();
	closed = true;
}

void CConnection::send( CPacketBuffer& buf )
{
	socket.send( buf );
}

void CConnection::send( )
{
	socket.send( m_out );
}

void CConnection::send( char* buf, int len )
{
	socket.send( buf, len );
}

CPacketBuffer& CConnection::get_write_buffer()
{
	return this->m_out;
}