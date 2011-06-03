/*
 *  chatclient.h
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 30.05.11.
 *  Copyright 2011 HTL. All rights reserved.
 *
 */


#ifndef _C_CLIENT_H_
#define _C_CLIENT_H_

#include "log.h"
#include "socket.h"
#include "PacketBuffer.h"
#include "selector.h"
#include "connection.h"

using namespace tr::net;
using namespace tr::util;


//Test Class
//Implemenatation of an simple Chat protocol
//1)Connected
//2)Ask for nickname
//3)Put into lobby
//4)chat...
//5)on_disconnect
//5.1)broadcast "leave"
//5.2)send player "good bye"
//5.3)close connection
class ChatClient : public CConnection
{
public:
	virtual void on_accept()
	{
		m_out.clear();
		m_out << 'h' << 'i';
		m_out.flip();
		socket.send(m_out);//BAD, todo: selector register write interest, passing the m_out, as sliced copy to a queue
	}
	virtual void on_read()
	{
	}
	virtual void on_write()
	{
	}
};
#endif//_C_CLIENT_H_
