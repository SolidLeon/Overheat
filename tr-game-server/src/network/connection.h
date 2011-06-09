/*
 *  connection.h
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 30.05.11.
 *  Copyright 2011 Overheat. All rights reserved.
 *
 */


#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include <vector>
#include <string>
#include <cstdlib>
#include <cstdio>

#include "socket.h"
#include "selector.h"
#include "PacketBuffer.h"
#include "closeconnectionex.h"
#include "types.h"


using namespace std;

namespace tr 
{
	namespace net
	{
		class CConnection
		{
		protected:
			CSocket socket;
			CSelector selector;
			std::vector<CPacketBuffer*> msg_queue;
			CPacketBuffer m_out;
			CPacketBuffer m_in;
			bool close_requested;
			bool closed;
			char ip[20];
		public:
			CConnection( CSocket& socket, CSelector& selector );
			~CConnection()
			{
				//delete[] ip;
			}
			//Event Handler
			virtual void on_accept( uint32_t gs_ip, uint32_t gs_port ) = 0;
			virtual void on_read();
			virtual void on_write() = 0;
			virtual void on_disconnect() = 0;
			
			CSocket& get_socket();
			
			std::vector<CPacketBuffer*>& get_msg_queue();
			
			const std::string& get_ip() const { return this->ip; }
			
			bool is_close_requested() const;
			
			bool is_closed() const;
			
			void close();
			
			void on_close();
			
			void send( CPacketBuffer& buf );			
			void send( );
			
			void send( char* buf, int len );
			
			CPacketBuffer& get_write_buffer();
		};//CConnection
	}//net
}//tr

#endif//_CONNECTION_H_