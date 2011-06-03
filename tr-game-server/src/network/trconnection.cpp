/*
 *  trconnection.cpp
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 30.05.11.
 *  Copyright 2011 Overheat. All rights reserved.
 *
 */
 // TODO: on_accept
 // TODO: on_read
 // TODO: on_write
 // TODO: on_disconnect

#include "trconnection.h"

using namespace tr::net;

static tr::util::CLog log = tr::util::CLog::get_logger( "TRConnection" );

void TRConnection::on_accept()
{
	//SMSG_ITD_FAKE1
	CPacketBuffer packet(0x1B);
	packet.byte_order(CPacketBuffer::BO_LITTLE_ENDIAN);
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
	try {CConnection::on_read();}catch(CConnectionClosedEx ex) {close();return;}
	m_in.rewind();
	uint32_t packet_len;
	uint32_t blen;
	uint8_t client_pubkey[6];
	
	packet_len = m_in.getUInt();
	if( packet_len != 0x0C )
	{
		::log.info("CMSG_ITD_FAKE1RESPONSE: Wront packet len!");
		close();
		return;
	}
	blen = m_in.getUInt();
	memcpy(m_in.array() + 8,client_pubkey, 6);
	
	//SMSG_ITD_FAKE2
	packet.clear();
	packet.putUInt(0x06);
	packet.put(0x45).put(0x4e).put(0x43).put(0x20).put(0x4f).put(0x4b); //ENC_OK
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
	case AuthCodes::AuthLogin:
		
		unsigned char data[30];
		memcpy((void*)data, (void*)(m_in.array()+3), 30);
		//Special decryption just for user data
		CCryptMgr::instance().get_tr_crypt().TRDecrypt((uint8_t*) data, 30);
		
		char acc[14];
		char pw[16];
		memcpy((void*)acc, (void*)data, 14);
		memcpy((void*)pw, (void*)(data+14), 16);
		
		{
			string s = "LOGON CHALLENGE: Username = <";
			s += acc;
			s += "> IP = <";
			s += ip;
			s += ">";
			::log.info( s.c_str());
		}
		
		//TODO: Check Login against Database
		{
			int rc = tr::util::DBMgr::get_instance().validate_player( acc, 
																	 CCryptMgr::instance().gen_md5(pw, strlen(pw)).c_str(), 
			&UID );
			
			if( rc == 0 )
			{
				
				string s = "LOGON CHALLENGE succeed for ";
				s += acc;
				s += ", ";
				s += ip;
				::log.info( s.c_str());
				
				send_packet = new SMSG_AuthOk();
				send_packet->write(*this);
				send( );
				account_name = std::string( acc );
			}
			else if( rc == -1 ) // ACC Information Wrong
			{
				string s = "LOGON CHALLENGE failed for ";
				s += acc;
				s += ", ";
				s += ip;
				//s += ", ";
				//s += no_tries;
				//s += " times";
				::log.info( s.c_str());
				
				send_packet = new SMSG_AuthError(AuthError::INVALID_PASSWORD);
				send_packet->write(*this);
				send( );
				close();
			}
			else if( rc == 1 ) //ACC BLOCKED
			{
				string s = "LOGON CHALLENGE failed for ";
				s += acc;
				s += ", ";
				s += ip;
				//s += ", ";
				//s += no_tries;
				//s += " times";
				::log.info( s.c_str());
				
				send_packet = new SMSG_AuthErrorAccountBlocked(0x02);
				send_packet->write(*this);
				send( );
				close();
				
			} 
			else if( rc == 2 ) //ACC Already Logged in
			{
				string s = "LOGON CHALLENGE failed for ";
				s += acc;
				s += ", ";
				s += ip;
				//s += ", ";
				//s += no_tries;
				//s += " times";
				::log.info( s.c_str());
				
				send_packet = new SMSG_AuthError(AuthError::INVALID_PASSWORD);
				send_packet->write(*this);
				send( );
				close();
				
			}
		}
		break;
		
	case AuthCodes::AuthRequestServerList:
		send_packet = new SMSG_AuthServerList();
		send_packet->write(*this);
		send();
		break;
		
	case AuthCodes::AuthSelectServer:
		{
			CMSG_AuthSelectServer p;
			p.read( m_in ); 
			uint8_t serverID = p.get_server_id();
			//Handoff to GameServer
			if( serverID > 0 )
			{
				uint64_t sessionID = CSessionMgr::instance().generate_session( account_name.c_str(), UID );
				send_packet = new SMSG_AuthGSHandoff( sessionID, serverID );
				send_packet->write( *this );
				send();
				std::string s;
				s = "Handoff client <";
				s += ip;
				s += ">";
				::log.info(s.c_str());
			}
		}
		break;
	default:
		send_packet = new SMSG_AuthError(AuthError::SYSTEM_ERROR);
		send_packet->write(*this);
		send();
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
	}
	
}