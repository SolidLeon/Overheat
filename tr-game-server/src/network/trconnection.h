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
				AUTHED
			};
		private:
			static tr::util::CLog log;
			AuthState state;
			CServerPacket* send_packet;
			
			std::string account_name;
			int64_t UID;
            
            CSession session;
		public:
			TRConnection( CSocket& socket, CSelector& selector)
			: CConnection( socket, selector )
			{
				state = CONNECTED;
			}
			~TRConnection()
			{
				delete send_packet;
			}
			virtual void on_accept();
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