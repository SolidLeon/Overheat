/*
 *  socket.h
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 29.05.11.
 *  Copyright 2011 HTL. All rights reserved.
 *
 */

#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>

#include "log.h"
#include "packetbuffer.h"
#include "selector.h"

#include <cstdlib>
#include <sstream>
#include <string>
#include "closeconnectionex.h"

using tr::net::packet::CPacketBuffer;

namespace tr
{
	namespace net 
	{
		// Simple Socket Implementation
		//-SolidLeon
		class CSocket
		{
		private:
			static tr::util::CLog log;
			/* listening socket */
			int socket;
			/* port to listen on */
			int port;
			/* host address */
			char* host;
			/* socker address structure */
			struct sockaddr_in servaddr;
			/* client address structure */
			struct sockaddr_in cliaddr;		
			
			void init();
			
		public:
			CSocket( );
			CSocket( const char*, int );
			CSocket( int, struct sockaddr_in );
			~CSocket();
			
			void close( );
			void send( const char*, int );
			void send( packet::CPacketBuffer& );
			int read( char*, int );
			void read( packet::CPacketBuffer& );
			
			void _register( CSelector&, int );
			
			int get_socket() const { return socket; }
			
			bool is_ready() const;
			bool equals( const CSocket& other ) const;
			
		};//CSocket
		
	}//net
}//tr

#endif//_SOCKET_H_