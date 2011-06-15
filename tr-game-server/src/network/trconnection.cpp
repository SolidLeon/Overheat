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
        
        /*m_out.clear();
        uint8_t smsg_gl_serverkey[] = {
            0x8D, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
            0x13, 0xE1, 0xB1, 0x6D, 0x1C, 0x85, 0xF2, 0x5F, 0x4D, 0xBB, 0x8D, 0x25, 0x95, 0x93, 0x84, 0x15,
            0x05, 0x21, 0x74, 0xAA, 0xBC, 0x03, 0x60, 0x03, 0xC8, 0xDF, 0xFD, 0x82, 0xE8, 0x45, 0x87, 0x86,
            0x94, 0xD1, 0xE7, 0xFA, 0xAC, 0x30, 0xBC, 0xA4, 0xAF, 0x75, 0x54, 0xAF, 0xB0, 0xF0, 0x4B, 0xDA,
            0x04, 0x87, 0xF7, 0xB9, 0x80, 0x18, 0x40, 0xDE, 0x4E, 0x09, 0x8E, 0xF8, 0xB5, 0x16, 0x98, 0x3E,
            0x98, 0x0F, 0x91, 0xEA, 0xAD, 0xAD, 0x8E, 0x7D, 0xF9, 0xEC, 0x43, 0x1D, 0xD4, 0x1C, 0xEF, 0x3F,
            0xBE, 0xCF, 0xD1, 0xAE, 0xD2, 0x77, 0x1C, 0xCF, 0xF8, 0x5E, 0xF8, 0x85, 0x3E, 0x2F, 0x9B, 0xC8,
            0x30, 0x2E, 0xD3, 0xC4, 0x7F, 0xE6, 0x29, 0x72, 0xE0, 0x08, 0xE9, 0x32, 0x53, 0x97, 0xDB, 0x41,
            0x37, 0x98, 0xB3, 0x8A, 0xDC, 0xB8, 0xAF, 0xD3, 0x6A, 0x69, 0xD5, 0x12, 0xEC, 0x32, 0x61, 0xAF,
            0x05
        };
        m_out.putArray(smsg_gl_serverkey, 0x8D+4);
        m_out.flip();*/
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
        crypt.encrypt((uint32_t*)(m_out.array()+4), 0x08);
        
        CConnection::send(m_out);//CLIENT DISCONNECTS RIGHT HERE
        
        //???: Encryption wrong?
        printf("Sent SMSG_ENCOK to %s\n", ip);
        //DROP CLIENT
        //return;
        //CMSG_AUTH_VERSION_CHECK
        /*printf("READ CMSG_AUTH_VERSION_CHECK\n");
        m_in.clear();
        try {CConnection::on_read();}catch(CConnectionClosedEx& ex) {close();return;}
        m_in.rewind();
        m_in.debug_out();
        
        
        close();*/
        return;
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
    
    //decrypt
    crypt.decrypt((uint32_t*)(m_in.array()+4), m_in.limit());
    m_in.debug_out();
    
    int pLen = m_in.getUInt();
    int alignBytes = m_in.get();
    m_in.position(m_in.position() + alignBytes-1);
    
    //decode PACKET
    uint32_t subSize = m_in.getUShort();
    uint32_t majorOpcode = m_in.getUShort();
    if( majorOpcode )
    {
        printf("ERROR: MAJOR OPCODE\n");
        //ignore these packets
        return;
    }
    uint8_t unk1 = m_in.get();
    if( unk1 != 2 )
    {
        sleep(5);
    }
    uint8_t opcode = m_in.get();
    uint8_t unk2 = m_in.get();
    if( unk2 != 0 ) 
    {
        printf("UNK2 != 0\n");
        sleep(5);
    }
    uint8_t xorCheckA = m_in.get();
    if( xorCheckA != 3 )
        sleep(5);
    uint32_t hdrB_start = m_in.position();
    uint8_t unk3 = m_in.get();
    if( unk3 != 3 ) 
    {
        printf("UNK3 != 3\n");
        sleep(5);
    }
    
	switch ( opcode )
	{
        case 0x02:
        {
            if( m_in.get() != 0x29 )
                sleep(5);
            uint8_t unk02_1 = m_in.get();
            if( unk02_1 != 3 )
                sleep(5);
            uint8_t unk02_2 = m_in.get();
            if( unk02_2 != 1 )
                sleep(5);
            uint8_t preffix02_1 = m_in.get();
            if( preffix02_1 != 7 )
                sleep(5);
            uint32_t sessionId2 = m_in.getUInt();
            uint8_t preffix02_2 = m_in.get();
            if( preffix02_2 != 7 )
                sleep(5);
            uint32_t sessionId1 = m_in.getUInt();
            uint8_t unk02_3 = m_in.get();
            if( unk02_3 != 0xD )
                sleep(5);
            //part 2
            if(m_in.get() != 0xCB)
                sleep(5);
            
            uint32_t version_len = m_in.get();
            bool wrongVersion = false;
            if( version_len != 8 )
                wrongVersion = true;
            if( !wrongVersion )
            {
                if( memcmp(m_in.array() + m_in.position(), "1.11.6.0", version_len))
                    wrongVersion = true;
            }
            m_in.position(m_in.position() + version_len);
            if( wrongVersion ) 
            {
                close();
                return;
            }
            uint8_t unk02_4 = m_in.get();
            if( unk02_4 != 0x2A )
            {
                close();
                return;
            }
            //check session
            //Query session
            std::stringstream sql;
            sql << "SELECT session_id1, session_id2, uid, account FROM sessions WHERE session_id1 = ";
            sql << sessionId1;
            sql << " AND session_id2 = ";
            sql << sessionId2;
            printf("QUERY:\n");
            printf("%s\n", sql.str().c_str());
            tr::util::DBMgr::get_instance().query(sql.str().c_str());
            MYSQL_RES* res = tr::util::DBMgr::get_instance().store_result();
            uint64_t num_rows = tr::util::DBMgr::get_instance().num_rows(res);
            MYSQL_ROW row = tr::util::DBMgr::get_instance().fetch_row(res);
            if( num_rows == 1 )
            {
                this->session.update(sessionId1, sessionId2);
                UID = atoi(row[2]);
                printf("[GameServer] Client Successfully connected to GS, with valid SESSION: %s\n", ip);
                
                //Begin charselection
                pym.init();
                pym.tuple_begin();
                pym.addUnicode("FamilyName");
                pym.addInt(1);//hasCharacters
                pym.addInt(UID); //userId
                pym.tuple_begin();
                pym.addInt(1); //human
                pym.addInt(0); //forean_hybrid
                pym.addInt(0); //brann_hybrid
                pym.addInt(0); //thrax_hybrid
                pym.tuple_end();
                pym.addInt(1); //bCanSkipBootcamp
                pym.tuple_end();
                
                methodCallRaw(5, 413);
                
                //m_out.flip();
                //CConnection::send(m_out);
                return;
            }
            //invalid session
            printf("[GameServer] Client sent invalid SESSION: %s\n", ip);
            printf("id1: '%u'\n", sessionId1);
            printf("id2: '%u'\n", sessionId2);
            close();
            return;
        }
            break;
        default:
            printf("[GameServer] received unknown opcode from %s: %02X\n", ip, opcode);
            close();
            break;
	}
}

void TRConnection::methodCallRaw(uint32_t entityId, uint32_t MethodID/*, uint8_t* pyObjString, int pyObjLen*/)
{
    m_out.clear();
    
    //AlignBytes
    m_out.put(0x01);
    int LenBegin = m_out.position();
    
    //SubSize
    m_out.putUShort(0x41);
    
    //Opcode
    m_out.put(0x00);
    m_out.put(1);
    
    int XORCheck = m_out.position();
    //datablock 1 (header)
    m_out.put(0); //2:6 mask
    m_out.put((7<<1)|(0)); //Opcode and flag --> 7 means the main packet handler
    m_out.put(GEN_XOR_BYTE(m_out.position() - XORCheck));
    XORCheck = m_out.position();
    //datablock 2 (header 2)
    m_out.put(0x00); //compression
    //OPD1:
    m_out.put(0x01);
    //EntityID
    if( entityId == 0 )
    {
        m_out.put(0);
    }
    else
    {
        while( entityId ) 
        {
            if( entityId&~0x7F )
                m_out.put( (entityId&0x7F)|0x80 );
            else
                m_out.put( entityId&0x7F );
            entityId >>= 7;
        }
    }
    
    //MethodID
    if( MethodID <= 0x7F )
    {
        m_out.put(MethodID);
    }
    else if( MethodID <= 0x3FFF )
    {
        m_out.put( 0x80 | (MethodID&0x7F) );
        m_out.put( MethodID>>7 );
    } 
    else if( MethodID <= 0x1FFFFF )
    {
        m_out.put( 0x80 | (MethodID&0x7F) );
        m_out.put( 0x80 | ((MethodID>>7)&0x7F) );
        m_out.put( MethodID>>14 );
    } 
    else if( MethodID <= 0xFFFFFFF )
    {
        m_out.put( 0x80 | (MethodID&0x7F) );
        m_out.put( 0x80 | ((MethodID>>7)&0x7F) );
        m_out.put( 0x80 | ((MethodID>>14)&0x7F) );
        m_out.put( MethodID>>21 );
    } 
    else
    {
        printf("ERROR: MethodID???\n");
    }
    
    //Parameter Block
    //add block size placeholder
    m_out.put(0xFF).put(0xFF);
    int ParamBlockBegin = m_out.position();
    m_out.put('M');
    m_out.putUInt( pym.getLen() );
    m_out.putArray( pym.getData(), pym.getLen() );
    m_out.put(0x66);
    
    int PBLen = m_out.position() - ParamBlockBegin;
    if( PBLen > 0xFFF )
    {
        printf("ERROR PBLen > 0xFFF\n");
        sleep(5);
    }
    
    m_out.array()[ParamBlockBegin-2] = (PBLen&0x3F) | 0x40;
    m_out.array()[ParamBlockBegin-1] = PBLen>>6;
    //Follower
    m_out.put(GEN_XOR_BYTE(m_out.position() - XORCheck));
    XORCheck = m_out.position();
    //Last Checksum
    m_out.put(0x06).put(0x06);
    
    //Align len to 8 (-1)
    int LenNow = m_out.position() - LenBegin;
    int PaddingNeeded = (8-((LenNow+1)%8))%8;
    for(int x=0;x<PaddingNeeded;x++)
        m_out.put('?');
    //uint8_t*arr = m_out.array();
    
    //*(uint16_t*)(arr[LenBegin]) = m_out.position() - LenBegin;
    m_out.putUShort(m_out.position() - LenBegin, LenBegin);
    crypt.encrypt((uint32_t*)(m_out.array()+4), m_out.position()-4);
    m_out.flip();
    CConnection::send( m_out );
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
