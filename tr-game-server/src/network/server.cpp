/*
 *  server.cpp
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 29.05.11.
 *  Copyright 2011 Overheat. All rights reserved.
 *
 */
 
 // FIXME: Each Client IO is handled in sequence (the 2nd client only knows the first but not the 3rd)
 // TODO: IO should be done per enabling read/write interest

#include "server.h"

using namespace tr::net;

using tr::util::CLog;

static CLog log = CLog::get_logger( "Server" );

//Creates server with port depending on type 
//Type: 0 - auth_gg_port ; this is the port that the auth_server sends (AUTH_GG)
//      1 - auth_port1 ; this is the first port the game_server sends (AUTH_GG_FIRST)
//      2 - port ; this is the second port the game_server  sends, and the actual gaming port (AUTH_GAMING)
CServer::CServer( int type, CGameServer &gs )
: game_server(gs), server(0)
{
    this->type = type;
    if( type == AUTH_GG )
        server = CServerSocket(gs.get_auth_gg_port());
    else if( type == AUTH_GG_FIRST )
        server = CServerSocket(gs.get_auth_port1());
    else if( type == AUTH_GAMING )
        server = CServerSocket(gs.get_auth_gaming_port());
    server.init();
}

//Creates Server with auth_gg_port as port
CServer::CServer( CGameServer &gs )
: game_server(gs), server(gs.get_auth_gg_port())
{
    this->type = AUTH_GG;
    server.init();
}

void CServer::start()
{
    std::string s;
    int p = 0;
    switch( type )
    {
        case AUTH_GG:
            s = "Auth GG";
            p = game_server.get_auth_gg_port();
            break;
        case AUTH_GG_FIRST:
            s = "Auth GG First";
            p = game_server.get_auth_port1();
            break;
        case AUTH_GAMING:
            s = "Auth Gaming";
            p = game_server.get_auth_gaming_port();
            break;
        default:
            s = "INVALID SERVER TYPE";
            break;
    }
    printf(">>--------------------------------------------------<<\n");
    printf(">> Start Game Server on port %d (%s) <<\n", 
           p,s.c_str());
    printf(">>--------------------------------------------------<<\n");
    
	server._register( selector );
	main_loop();
}

void CServer::main_loop()
{
	::log.info( "Server started" );
	for(;;)
	{
		for (int i = 0; i <  clients.size(); i++)
		{
			if( clients[i]->is_close_requested() )
			{
				::log.info( "Process Close" );
				clients[i]->on_close();
			}
			if( clients[i]->is_closed() ) 
			{
				::log.info( "Remove Client " );
				clients.erase( clients.begin() + i );
			}
		}
		int c[clients.size()];
		for (int i = 0; i < clients.size(); i++)
		{
			c[i] = clients[i]->get_socket().get_socket();
		}
		selector.fill_fds(c, clients.size(), NULL, 0);
		selector.select();
				
		if( selector.is_acceptable(0) )
		{
			//onaccept
			CSocket *sck = server.accept();
			sck->_register( selector, CSelector::OP_READ );
			CConnection *con = new TRConnection( *sck, selector );
			clients.push_back( con );
            //Switch TRConnection state depending on server type
            switch( type )
            {
                case AUTH_GG: ((TRConnection*)con)->connected(tr::net::TRConnection::CONNECTED);
                    break;
                case AUTH_GG_FIRST: ((TRConnection*)con)->connected(tr::net::TRConnection::AUTHED_GG);
                    break;
                case AUTH_GAMING: ((TRConnection*)con)->connected(tr::net::TRConnection::AUTHED_GG_FIRST);
                    break;
            }
			con->on_accept( game_server.get_ip(), game_server.get_auth_port1() );
		}
        
            for( int i = 0; i < clients.size(); i++ )
            {			
                if( !clients[i]->get_socket().get_socket() )
                    continue;
                if( selector.is_readable( clients[i]->get_socket().get_socket() ) )
                {
                    //onread				
                    //packet::CPacketBuffer pbRead(8192);
                    //clients[i]->get_socket().read(pbRead);
                    //TODO:
                    // call the on_read method of the appropiate, Connection class				
                    //on_read(clients[i]->get_socket(), pbRead);
                    char sbuf[30];
                    sprintf(sbuf, "Read from client %s\n", clients[i]->get_ip().c_str());
                    ::log.info(sbuf);
                    
                    clients[i]->on_read();
                    
                }
                if( selector.is_writeable( clients[i]->get_socket().get_socket() ))
                {
                    //onwrite
                    
                }
        }
		
	}

}

void CServer::on_accept( CSocket& cli)
{
	char b[256];
	packet::CPacketBuffer pb(512);
	pb.wrap((unsigned char*)b, strlen(b));
	pb.flip();
	cli.send( pb );
}

void CServer::on_read( CSocket& cli, packet::CPacketBuffer& pb)
{
	for( int i = 0; i < clients.size(); i++)
	{
		if( !clients[i]->get_socket().is_ready() )
			continue;
		if( clients[i]->get_socket().equals(cli) )
			continue;
		clients[i]->get_socket().send( pb );
	}
}

void CServer::send( CConnection& conn, packet::CPacketBuffer& pb )
{
	::log.info("Send msg");
	conn.get_socket().send( pb );
}

// TODO: Dont send the data directly, use queue and write interest instead
void CServer::send_broadcast( packet::CPacketBuffer& pb )
{
	::log.info("Send Broadcast");
	for( int i = 0; i < clients.size(); i++ )
	{
		if( !clients[i]->get_socket().is_ready() )
			continue;
		pb.rewind();
		clients[i]->get_socket().send( pb );
	}
}

