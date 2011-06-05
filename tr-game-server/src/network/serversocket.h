/*
 *  serversocket.h
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 29.05.11.
 *  Copyright 2011 Overheat. All rights reserved.
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
			
			
		public:
			CServerSocket( int );
			
			CSocket* accept( void );
			void init();
			
			void _register( CSelector& );
            
            uint32_t get_ip()
            {
                return servaddr.sin_addr.s_addr;
            }
            uint32_t get_port()
            {
                return servaddr.sin_port;
            }
		}; //CServerSocket
	}// net
}//tr

#endif //_SERVERSOCKET_H_