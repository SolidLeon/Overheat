/*
 *  trconnection.h
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 30.05.11.
 *  Copyright 2011 Overheat. All rights reserved.
 *
 */

#ifndef _TRCONNECTION_H_
#define _TRCONNECTION_H_

#include "connection.h"  //CConnection
//#include "socket.h" //CSocket
//#include "selector.h" //CSelector
#include "log.h"

#include "crypto.h"
#include "authcodes.h"
#include "DBMgr.h"
#include "Session.h"
#include "serverpacket.h"

#include <string>


using tr::crypto::CCryptMgr;
using tr::net::packet::CServerPacket;

namespace tr
{
	namespace net
	{
		
		class TRConnection : public CConnection
		{
		public:
			enum AuthState {
				CONNECTED,
				AUTHED_GG,
                AUTHED_GG_FIRST,
                CONNECTED_TO_GAME
			};
		private:
			//static tr::util::CLog log;
			AuthState state;
			CServerPacket* send_packet;
			
			std::string account_name;
			int64_t UID;
            
            CSession session;
            crypto::CTRCrypt2 crypt; 
		public:
			TRConnection( CSocket& socket, CSelector& selector)
			: CConnection( socket, selector ), crypt()
			{
				state = CONNECTED;
			}
			~TRConnection()
			{
				delete send_packet;
			}
            void connected(AuthState state)
            {
                this->state = state;
                printf("Created new TR Connection State: ");
                switch( state )
                {
                    case CONNECTED: printf( "Connected\n");
                        break;
                    case AUTHED_GG: printf( "AUTHED_GG\n");
                        break;
                    case AUTHED_GG_FIRST: printf( "AUTHED_GG_FIRST\n");
                        break;
                    case CONNECTED_TO_GAME: printf( "Connected to Game\n");
                        break;
                }
            }
			virtual void on_accept( uint32_t gs_ip, uint32_t gs_port );
			virtual void on_read();
			virtual void on_write();
			virtual void on_disconnect();
			
			void send();
			
			AuthState get_state() const
			{
				return state;
			}
		};//TRConnection
	}//net
}//tr

#endif