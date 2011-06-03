/*
 *  server.cpp
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 29.05.11.
 *  Copyright 2011 HTL. All rights reserved.
 *
 */
 
 // FIXME: Each Client IO is handled in sequence (the 2nd client only knows the first but not the 3rd)
 // TODO: IO should be done per enabling read/write interest

#include "server.h"

using namespace tr::net;

using tr::util::CLog;

static CLog log = CLog::get_logger( "Server" );

CServer::CServer( int port )
: server( port )
{
}

void CServer::start()
{
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
			con->on_accept();
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
				
				::log.info(("Read from client " + clients[i]->get_ip()).c_str());
				
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

