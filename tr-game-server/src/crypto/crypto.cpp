/*
 *  crypto.cpp
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 31.05.11.
 *  Copyright 2011 Overheat. All rights reserved.
 *
 */

#include "crypto.h"

using namespace tr::crypto;

CCryptMgr* CCryptMgr::_instance = 0;

CCryptMgr& CCryptMgr::instance()
{
	if( _instance == 0 )
	{
		printf("Create Crypto Instance...\n");
		CCryptMgr::_instance = new CCryptMgr();
	}
	return *_instance;
}

CCryptMgr::CCryptMgr()
{
	printf("Copy Structure... %d - %d\n", sizeof(trcrypt.DecStruct2_CEA1B0), sizeof(CTRCrypt::CEA1B0_DATA));
	trcrypt.DecStruct2_CEA1B0 = (CTRCrypt::_DecStruct2T*)CTRCrypt::CEA1B0_DATA;
	this->blowfish.BFInit();
	this->trcrypt.TRInit();
	this->_rsa.RSAInit();
    DH_new();
}

CCryptMgr::~CCryptMgr()
{
	printf("CCryptMgr destroy\n");
}

std::string CCryptMgr::gen_md5( char* data, int length )
{
	MD5 md5 = MD5();
	md5.update( data, (unsigned) length );
	md5.finalize();
	return md5.hexdigest();
}

CBlowfish& CCryptMgr::get_blowfish()
{
	return blowfish;
}

CTRCrypt& CCryptMgr::get_tr_crypt()
{
	return trcrypt;
}


