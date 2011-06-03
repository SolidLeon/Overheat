/*
 *  serversocket.cpp
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 29.05.11.
 *  Copyright 2011 Overheat. All rights reserved.
 *
 */

#include "serversocket.h"

using tr::net::CServerSocket;
using tr::net::CSocket;
using tr::util::CLog;

static CLog log = CLog::get_logger("ServerSocket");

//Create a new socket connected to the given port
//-SolidLeon
CServerSocket::CServerSocket( int port )
{
	this->port = port;
	init();
}

//Initializes this server socket
//-SolidLeon
void CServerSocket::init()
{
	::log.info( "Initialize ServerSocket..." );
	
	int rc;
	//create socket
	socket = ::socket(AF_INET, SOCK_STREAM, 0 );
	if( socket < 0 )
	{
		::log.error( "Can not open socket" );
	}
	
	//deactivate address already in use
	int optval = 1;
	::setsockopt( socket, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval, sizeof(int) );
	
	/* create struct sockaddr_in */
	memset( (void*) &servaddr, 0, sizeof(servaddr) );
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);
	
	/* bind */
	rc = ::bind( socket, (struct sockaddr *) &servaddr, sizeof(servaddr) );
	if( rc < 0 )
	{
		::log.error( "Can not bind socket" );
	}
	
	rc = ::listen( socket, 5) ;
	if( rc < 0 )
	{
		::log.error( "Can not set socket to listen mode" );
	}
	
	::log.info( "ServerSocket initialization completed" );
	
}

//Accepts a new connection from this socket
//This is a blocking call at accept()
//Returns the new connected socket as CSocket object
//-SolidLeon
CSocket* CServerSocket::accept( void )
{
	int connected_socket = 0;
	struct sockaddr_in cliaddr;
	socklen_t sz = 0;
	connected_socket = ::accept( socket, (struct sockaddr *)&cliaddr, &sz);
	
	CSocket *cli = new CSocket(connected_socket, cliaddr);
	return cli;
}

//Registrates this server socket to the given selector, with an
//accept interest. (Registrates at the readfds, but gets stored special inside the selector)
//-SolidLeon
void CServerSocket::_register( CSelector& selector )
{
	selector.register_accept( socket );
}