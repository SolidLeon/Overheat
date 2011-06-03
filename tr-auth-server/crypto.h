/*
 *  crypto.h
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 31.05.11.
 *  Copyright 2011 InfiniteRasa. All rights reserved.
 *
 */

#ifndef _TR_CRYPTO_H_
#define _TR_CRYPTO_H_

#include <string>
#include "blowfish.h"
#include "trcrypt.h"

#include "md5.h"

namespace tr
{
	namespace crypto {
		
		class CCryptMgr
		{
			
		private:
			static CCryptMgr* _instance;
			CBlowfish blowfish;
			CTRCrypt trcrypt;
			
			CCryptMgr();
		public:
			//singleton
			static CCryptMgr& instance();
			
			~CCryptMgr();
			
			CBlowfish& get_blowfish();
			CTRCrypt& get_tr_crypt();
			
			//MD5
			std::string gen_md5( char*, int );
			
		};
		
		
	}
}

#endif