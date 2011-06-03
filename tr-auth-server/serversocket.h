/*
 *  serversocket.h
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 29.05.11.
 *  Copyright 2011 HTL. All rights reserved.
 *
 */

#ifndef _SERVERSOCKET_H_
#define _SERVERSOCKET_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>

#include "log.h"
#include "socket.h"
#include "selector.h"

#include <string>

namespace tr 
{
	namespace net
	{
		//Simple Server Socket implemenatation
		//Takes care of creating an listening socket
		//TODO:
		// merge with CSocket, by inheritance
		//-SolidLeon
		class CServerSocket
		{
		private:
			static tr::util::CLog log;
			/* listening socket */
			int socket;
			/* port to listen on */
			int port;
			/* socker address structure */
			struct sockaddr_in servaddr;
			
			void init();
			
		public:
			CServerSocket( int );
			
			CSocket* accept( void );
			
			void _register( CSelector& );
		}; //CServerSocket
	}// net
}//tr

#endif //_SERVERSOCKET_H_