/*
 *  trconnection.cpp
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 30.05.11.
 *  Copyright 2011 Overheat. All rights reserved.
 *
 */
//TODO: on_accept
//TODO: on_read
//TODO: on_write
//TODO: on_disconnect
//TODO: on_accept(): 2nd handshake
//TODO: on_accept(): DH KeyExchange
//TODO: on_accept(): 3rd handshake
//FIXME: on_accept(): remove close() in the on_accept() handler
// for the gaming port, must be removed, since the Game Server does not remove the player after handshake

#include "trconnection.h"

using namespace tr::net;

static tr::util::CLog log = tr::util::CLog::get_logger( "TRConnection" );

void TRConnection::on_accept( uint32_t gs_ip, uint32_t gs_port )
{
    //Switch action depending on STATE
    if( state == CONNECTED )
    {
        //Client just got forwarded from the AUTH_SERVER to the GAME_SERVER
        //Do the first handshake
        struct CMSG_ITD_FAKE1RESPONSE {
            uint32_t packet_len;
            uint32_t blen;
            uint8_t client_pubkey[6];
        } p1;
        struct CMSG_ITD_FAKE2RESPONSE {
            uint32_t packet_len;
            uint8_t op;
            uint32_t login_id1;
            uint32_t login_id2;
        }p2;
        CPacketBuffer packet(8192);
        packet.byte_order(CPacketBuffer::BO_LITTLE_ENDIAN);
        //SMSG_ITD_FAKE1
        //SMSG_SERVERKEY
        packet.clear();
        packet.putUInt(0x1b);
        packet.putUInt(0x08);
        packet.putUInt(0x06);
        packet.putUInt(0x01);
        packet.put(0x50).put(0x55).put(0x42).put(0x4b).put(0x45).put(0x59).put(0x31).put(0x32); //PUBKEY12
        packet.put(0x46).put(0x4f).put(0x4f).put(0x42).put(0x41).put(0x52); //FOOBAR
        packet.put(0x41);
        packet.flip();
        CConnection::send(packet);
        //CMSG_ITD_FAKE1RESPONSE
        //CMSG
        try {CConnection::on_read();}catch(CConnectionClosedEx ex) {close();return;}
        m_in.rewind();
        p1.packet_len = m_in.getUInt();
        if( m_in.array()[0] != 0x0C )
        {
            ::log.info("CMSG_ITD_FAKE1RESPONSE: Wrong packet len!");
            printf("Data: %X\n", *(uint32_t*)&p1);
            close();
            return;
        }
        p1.blen = m_in.getUInt();
        memcpy(m_in.array() + 8,p1.client_pubkey, 6);
        
        //SMSG_ITD_FAKE2
        packet.clear();
        packet.putUInt(0x06);
        packet.put(0x45).put(0x4e).put(0x43).put(0x20).put(0x4f).put(0x4b); //ENC_OK
        packet.flip();
        CConnection::send(packet);
        
        //CMSG_ITD_FAKE2RESPONSE
        //CMSG_SESSION
        m_in.clear();
        try {CConnection::on_read();}catch(CConnectionClosedEx ex) {close();return;}
        m_in.rewind();
        
        p2.packet_len = m_in.getUInt();
        printf("PacketLen: %d\n", p2.packet_len);
        if( m_in.array()[0] != 0x09 )
        {
            ::log.info("CMSG_ITD_FAKE1RESPONSE: Wrong packet len!");
            close();
            return;
        }
        m_in.debug_out();
        p2.op = m_in.get();
        p2.login_id1 = m_in.getUInt();
        p2.login_id2 = m_in.getUInt();
        session.update(p2.login_id1, p2.login_id2);
        printf("SESSION UPDATE: sessionID1=%X, sessionID2=%X\n", session.session1(), session.session2());
        
        //SMSG_ITD_FAKE3
        //SMSG_QUEUE_POSITION
        packet.clear();
        packet.putUInt(0x09);
        packet.put(0x0d);
        packet.putUInt(0x00); //Position
        packet.putUInt(0x00); //Estimated Time
        packet.flip();
        CConnection::send(packet);
        
        //SMSG_ITD_FORWARD
        printf("Forward to GS IP: %X, on port %X\n", gs_ip, gs_port);
        
        packet.clear();
        packet.putUInt(0x11);
        packet.put(0x0e);
        packet.putUInt(gs_ip); //(0x89f86bd8);
        packet.putUInt(gs_port); //(0x00001f41);
        packet.putUInt(session.session1());
        packet.putUInt(session.session2());
        packet.flip();
        CConnection::send(packet);
        
        //Client disconnectes and connects to SMSG_ITD_FORWARD Gip and Port
        close();
    }
    else if( state == AUTHED_GG )
    {
        //Client passed first handshake
        //Now do the DiffieHellman Key exchange
        close();
    }
    else if( state == AUTHED_GG_FIRST )
    {
        //Client passed the second handshake,
        //Do again the handshake but instead of forwarding, send character data
        close();
    }
}

void TRConnection::on_read()
{
	if( is_closed() || is_close_requested() )
		return;
	
	try
	{
		CConnection::on_read();
	}
	catch( CConnectionClosedEx ex )
	{
		close();
		return;
	}
	//handle packet
	m_in.rewind();	
	
	int packet_size = *(unsigned short*) m_in.array();
	if( packet_size != m_in.limit() )
	{
		::log.info("ERROR: Wrong packet size");
	}
		
	//Each Packet is BF encrypted
	for(int i=0; i<(packet_size-2)/8; i++)
	{
		CCryptMgr::instance().get_blowfish().BFDecrypt((uint32_t*)&(m_in.array()[2+i*8]), 
											(uint32_t*)&(m_in.array()[2+i*8+4]));
	}
	//m_in.debug_out();
	int opcode = m_in.array()[2];
	
	switch ( opcode )
	{
	default:
		close();
		break;
	}
}

void TRConnection::on_write()
{
}
void TRConnection::on_disconnect()
{
}

void TRConnection::send()
{
    /*
	if( state == AUTHED )
	{
		char buffer[128];
		memset((void*)buffer, 0, 128);
		//printf("M_OUT: %d\n", m_out.limit());
		int length = m_out.limit();
		//printf("PRE-ENCRYPT PACKET: Size: %X\n",length); 
		memcpy((void*)&buffer, (void*)m_out.array(), length);
		//Align to 8 bytes
		length = length + ((8-((length-2)%8))%8);
		//Calculate checksum
		unsigned long CS = 0;
		for(int p=0; p<(length-2)/4; p++)
			CS ^= *(unsigned long*)&buffer[p*4+2];
		//Add checksum and unknown value
		//PacketOut_AddDword(pb, CS);
		//PacketOut_AddDword(pb, 0x00);
		*(unsigned int*)&buffer[length] = CS;
		*(unsigned int*)&buffer[length+4] = 0;
		length += 8;
		//Update base len
		*(unsigned short*)buffer = length;
		//Encrypt it
		for(int p=0; p<(length-2)/8; p++)
		{
			CCryptMgr::instance().get_blowfish().BFEncrypt(
														 (uint32_t*)&buffer[p*8+2], 
														 (uint32_t*)&buffer[6 + p*8]);
		}
		//printf("POST-ENCRYPT PACKET: Size: %X\n", length); 
		m_out.clear();
		m_out.wrap((unsigned char*)buffer, length);
		m_out.flip();
		//m_out.debug_out();
		CConnection::send( m_out );
	}
	else {
		//m_out.debug_out();
     CConnection::send( );
	}*/
	
}