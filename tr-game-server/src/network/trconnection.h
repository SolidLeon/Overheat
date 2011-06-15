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

#define GEN_XOR_BYTE(x) ((x&0xFF)^((x>>8)&0xFF)^((x>>16)&0xFF)^((x>>24)&0xFF))

#include "connection.h"  //CConnection
//#include "socket.h" //CSocket
//#include "selector.h" //CSelector
#include "log.h"

#include "crypto.h"
#include "authcodes.h"
#include "DBMgr.h"
#include "Session.h"
#include "serverpacket.h"
#include "PyMarshal.h"

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
				CONNECTED, //Needs first AUTH
				AUTHED_GG, //Needs DH KeyExchange
                AUTHED_GG_FIRST, //Connected to GameServer PORT
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
            packet::CPyMarshal pym;
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
            void methodCallRaw(uint32_t entityId, uint32_t MethodID/*, uint8_t* pyObjString, int pyObjLen*/);
			
			void send();
			
			AuthState get_state() const
			{
				return state;
			}
		};//TRConnection
	}//net
}//tr

#endif