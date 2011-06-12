/*
 *  socket.cpp
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 29.05.11.
 *  Copyright 2011 Overheat. All rights reserved.
 *
 */

#include "socket.h"

using tr::net::CSocket;
using tr::util::CLog;

static CLog log = CLog::get_logger("Socket");

//Default Constructor
//-SolidLeon
CSocket::CSocket( )
{
	socket = 0;
	port = 0;
	host = 0;
	memset((void*)&servaddr, 0, sizeof(servaddr));
	memset((void*)&cliaddr, 0, sizeof(cliaddr));
}

//
// Connects this socket, connected to host at port
//-SolidLeon
CSocket::CSocket( const char* host, int port )
{
	this->host = new char[strlen(host)];
	strncpy(this->host, host, strlen(host));
	this->port = port;
	
	init();
}
CSocket::CSocket( int socket, struct sockaddr_in cliaddr )
{
	this->socket = socket;
	this->cliaddr = cliaddr;
}

CSocket::~CSocket()
{
	delete[] host;
}

//Creates the actual socket descriptor, initializes its address and connects it to the host
//-SolidLeon
void CSocket::init()
{
	int rc;
	rc = ::socket(AF_INET, SOCK_STREAM, 0);
	if( rc < 0 )
	{
		::log.error( "Can not create socket" );
	}
	
	memset((void*) &servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons( port );
	
	rc = ::connect( socket, (struct sockaddr *) &servaddr, sizeof(servaddr));
	if( rc < 0 )
	{
		::log.error( "Can not connect" );
	}
	
	::log.info( "Socket initialization completed" );
}

//Closes this socket
//-SolidLeon
void CSocket::close()
{
	::log.info("Socket closed");
	::close( socket );
}

//Sends the given buffer to the connected socket
//-SolidLeon
void CSocket::send( const char* buf, int size )
{
	if ( size == 0 )
		return;
	int rc = ::send( this->socket, buf, size, 0 );
	if( rc <= 0 )
	{
        throw new CConnectionClosedEx();
		::log.error( "Could not send data" );
	}
	//std::stringstream s;
	//s << "Send " << rc << " bytes";
	//std::string str;
	
	//::log.info(s.str().c_str());
}

//Sends the PacketBuffer
//-SolidLeon
void CSocket::send( packet::CPacketBuffer& pb )
{
	send( (char*)pb.array(), pb.limit() );
}

//Reads data into the given buffer
//-SolidLeon
int CSocket::read( char* buf, int size )
{
	int rc = ::recv( this->socket, buf, size, 0 );
	//std::stringstream s;
	//s << "Read Data " << rc;
	//::log.info(s.str().c_str());
	if( rc <= 0 )
	{
		//::log.error( "Could not read data" );
		throw new CConnectionClosedEx();
	}
	return rc;
}

//Reads data into the given PacketBuffer
//-SolidLeon
void CSocket::read( packet::CPacketBuffer& pb )
{
	pb.clear();
	pb.position( read( (char*) pb.array(), pb.capacity() ));
	pb.flip();
}

//Registrates given operations at the given selector
//-SolidLeon
void CSocket::_register( CSelector& selector, int ops )
{
	if( ops & CSelector::OP_READ )
		selector.register_read( socket );
		
	if( ops & CSelector::OP_WRITE )
		selector.register_write( socket );
}

//Checks if this socket is created and ready to work
// May need some improvements
//-SolidLeon
bool CSocket::is_ready() const
{
	//char b[50];
	//sprintf(b, "is_ready(): %d > 0: %d", socket, socket>0);
	//::log.info(b);
	return socket > 0;
}

//Checks if the two socket descriptors are the same
bool CSocket::equals( const CSocket& other ) const
{
	return other.get_socket() == socket;
}
