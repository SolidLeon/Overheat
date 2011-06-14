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

//static tr::util::CLog log = tr::util::CLog::get_logger( "TRConnection" );

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
            //::log.info("CMSG_ITD_FAKE1RESPONSE: Wrong packet len!");
            printf("CMSG_ITD_FAKE1RESPONSE: Wrong packet len!");
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
            //::log.info("CMSG_ITD_FAKE1RESPONSE: Wrong packet len!");
            printf("CMSG_ITD_FAKE1RESPONSE: Wrong packet len!");
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
        
        struct CMSG_AUTH_B {
            uint32_t packet_len;
            uint32_t blen;
            
        } p1;
        //Client passed first handshake
        //Now do the DiffieHellman Key exchange
        printf(">> Accepted a new Client for First DH KeyExchange\n");
        //Create DH values
        crypt.setup_dh();
        //SMSG_AUTH_HELLO
        //send g,p,A to client
        m_out.clear();
        m_out.putUInt(0x8D); //packet length
        m_out.putUInt(0x40); //LenA
        m_out.putUInt(0x40); //LenP
        m_out.putUInt(0x01); //LenG  
        uint8_t A[0x40];
        uint8_t P[0x40];
        BN_bn2bin(crypt.A(), A);
        BN_bn2bin(crypt.Prime(), P);
        m_out.putArray(A, 0x40);
        m_out.putArray(P, 0x40);
        m_out.put(0x05);
        m_out.flip();
        CConnection::send( m_out );
        
        //CMSG_AUTH_B
        m_in.clear();
        try {CConnection::on_read();}catch(CConnectionClosedEx ex) {close();return;}
        m_in.rewind();
        //printf("===== CMSG_AUTH_B =====\n");
        //m_in.debug_out();
        p1.packet_len = m_in.getUInt();
        //printf("PacketLen: %d\n", p1.packet_len);
        if( m_in.array()[0] != 0x44 )
        {
            //::log.info("CMSG_AUTH_B: Wrong packet len!");
            printf("CMSG_AUTH_B: Wrong packet len!\n");
            close();
            return;
        }
        uint8_t B[0x40];
        memcpy((void*)B, (void*)(m_in.array() + 8), 0x40); //+8: skip header, and BLen
        BIGNUM *b = BN_new();
        BN_bin2bn(B, 0x40, b);
        crypt.DH_UpdateB(b);
        //DH Key Exchange completed
        
        /*//Check encryption
        printf("\n\n== ENCRYPTION CHECK==\n");
        unsigned char test_data[] = {
            0x08, 0x00, 0x00, 0x00,
            0x02, 0x00,
            'E', 'N', 'C', ' ', 'O', 'K'
        };
        unsigned char encrypted[256];
        unsigned char decrypted[256];
        memcpy((void*)encrypted, (void*)(test_data),8+4);
        crypt.encrypt((uint32_t*) (encrypted+4), 8);
        memcpy((void*)decrypted, (void*)(encrypted), 8+4);
        crypt.decrypt((uint32_t*) (decrypted+4), 8);
        
        printf("__ PACKET __\n");
        for(int i = 0; i < 8+4; i++)
        {
            printf("%02X(%c)  ", test_data[i], (test_data[i] >= 'A' && test_data[i]<='Z'?test_data[i]: ' '));
        }
        printf("\n");
        printf("__ ENC __\n");
        for(int i = 0; i < 8+4; i++)
        {
            printf("%02X(%c)  ", encrypted[i], (encrypted[i] >= 'A' && encrypted[i]<='Z'?encrypted[i]: ' '));
        }
        printf("\n");
        printf("__ DEC __\n");
        for(int i = 0; i < 8+4; i++)
        {
            printf("%02X(%c)  ", decrypted[i], (decrypted[i] >= 'A' && decrypted[i]<='Z'?decrypted[i]: ' '));
        }
        printf("\n\n");*/
        
        //SMSG_ENCOK
        m_out.clear();
        
        m_out.putUInt(0x08);
        m_out.putUShort(0x02);
        m_out.put('E');
        m_out.put('N');
        m_out.put('C');
        m_out.put(' ');
        m_out.put('O');
        m_out.put('K');
        
        m_out.flip();
        //m_out.debug_out();
        crypt.encrypt((uint32_t*)(m_out.array()+4), 8);
        CConnection::send(m_out);//CLIENT DISCONNECTS RIGHT HERE
        //???: Encryption wrong?
        printf("Sent SMSG_ENCOK to %s\n", ip);
        //DROP CLIENT
        return;
        //CMSG_AUTH_VERSION_CHECK
        printf("READ CMSG_AUTH_VERSION_CHECK\n");
        m_in.clear();
        try {CConnection::on_read();}catch(CConnectionClosedEx& ex) {close();return;}
        m_in.rewind();
        
        crypt.decrypt((uint32_t*)(m_in.array()+4), *(unsigned int*)m_in.array()[0] + 4);
        
        m_in.debug_out();
        
        
        
        printf("Pass Client to GameServer...\n");
        //state = AUTHED_GG_FIRST;
        //close();
    }
    else if( state == AUTHED_GG_FIRST )
    {
        //Client passed the second handshake,
        //Do again the handshake but instead of forwarding, send character data
        printf(">->->->->->->->->->->->->->->->->->->->->->->->->->->->->->\n");
        printf(">> GAME_SERVER: New Player\n");
        printf(">->->->->->->->->->->->->->->->->->->->->->->->->->->->->->\n");
        close();
    }
}

void TRConnection::on_read()
{
    if( state != AUTHED_GG_FIRST )
        return;
    //state = AUTHED_GG_FIRST, reads appear here
	if( is_closed() || is_close_requested() )
		return;
    printf("TRConnection: on_read() from %s\n", ip);
	try
	{
		CConnection::on_read();
	}
	catch( CConnectionClosedEx& ex )
	{
        printf("CConnection::on_read() failed!\n");
		close();
		return;
	}
	//handle packet
	m_in.rewind();	
    printf("TRConnection: read data from %s\n", ip);
    printf("DATA:\n");
    m_in.debug_out();
    sleep(5);
    
//    crypt.decrypt((uint32_t*)(m_in.array()+4), packet_size);
	
	int packet_size = *(unsigned short*) m_in.array();
	if( packet_size != m_in.limit() )
	{
		//::log.info("ERROR: Wrong packet size");
		printf("ERROR: Wrong packet size");
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
	if( state == AUTHED_GG_FIRST )
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
	}
}
