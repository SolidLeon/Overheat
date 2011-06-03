/*
 *  server.h
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 29.05.11.
 *  Copyright 2011 HTL. All rights reserved.
 *
 */
#ifndef _SERVER_H_
#define _SERVER_H_

#include <vector>

#include "serversocket.h" 
#include "log.h"
#include "selector.h"
#include "PacketBuffer.h"

#include "connection.h"
#include "trconnection.h"

namespace tr 
{
	namespace net 
	{
		//Class CServer
		//Due to simplifing communication, the server provides an send method, to send 
		//to one specific Connection, or to all connected clients.
		//-SolidLeon
		class CServer
		{
		private:
			static tr::util::CLog log;
			static const int MAX_CLIENTS = 1000;
			CServerSocket server;
			CSelector selector;
			//CSocket clients[MAX_CLIENTS];// <- These must be replaces by the appropitae connection implementation, by the factory, or with templates
			std::vector<CConnection*> clients;
			//main loop
			void main_loop();
			
			//handler
			void on_accept( CSocket& );
			void on_read( CSocket&, packet::CPacketBuffer& );

		public:
			CServer( int );
			void start();
			
			void send( CConnection&, CPacketBuffer& );
			void send_broadcast( CPacketBuffer& );
		}; //server

	} //net
} //tr

#endif