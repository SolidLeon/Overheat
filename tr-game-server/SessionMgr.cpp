/*
 *  SessionMgr.cpp
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 02.06.11.
 *  Copyright 2011 InfiniteRasa. All rights reserved.
 *
 */

#include "SessionMgr.h"

using namespace tr::net;

CSessionMgr* CSessionMgr::_instance = 0;

CSessionMgr& CSessionMgr::instance()
{
	if( _instance == 0 )
	{ 
		_instance = new CSessionMgr();
	}
	return *_instance;
}

uint64_t CSessionMgr::generate_unique_key()
{
	uint64_t key;
	uint32_t genTime = clock();
	key = rand() + genTime;
	for( int i = 0; i < 30; i++ )
	{
		key = (key<<2) | (key>>62);
		key += rand();
		key += genTime / (i+1);
	}
	return key;
}

uint64_t CSessionMgr::generate_session( const char* acc, int64_t UID )
{
	uint64_t sessionID;
	uint32_t sessionID1;
	uint32_t sessionID2;
	
	sessionID = generate_unique_key();
	sessionID1 = (uint32_t) (sessionID & 0xFFFFFFFF);
	sessionID2 = (uint32_t) (sessionID >> 32);
	
	std::stringstream converter;
	std::stringstream sql;
	std::string sSessionID1;
	std::string sSessionID2;
	std::string sUID;
	converter << sessionID1; converter >> sSessionID1; converter.clear();
	converter << sessionID2; converter >> sSessionID2; converter.clear();
	converter << UID; converter >> sUID; converter.clear();
	sql << "INSERT INTO sessions (session_id1, session_id2, uid, account, checktime) VALUES(";
	sql << sSessionID1 << ", ";
	sql << sSessionID2 << ", ";
	sql << sUID << ", ";
	sql << "'" << std::string(acc) << "',";
	sql << "NULL" << ")";
	tr::util::DBMgr::get_instance().query(sql.str().c_str());
	return sessionID;
}