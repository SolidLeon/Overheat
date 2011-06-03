/*
 *  rsa.h
 *  tr-game-server
 *
 *  Created by Markus Mannel on 03.06.11.
 *  Copyright 2011 HTL. All rights reserved.
 *
 */

#ifndef _TR_RSA_H_
#define _TR_RSA_H_

#include "types.h"

#include <openssl/rsa.h>
#include <openssl/engine.h>

namespace tr {
	namespace crypto {
		
		class CRSA
		{
		private:
			RSA* key;
		public:
			CRSA();
			~CRSA();
			
			void RSAInit();
			BIGNUM* public_key() const
			{
				return key->e;
			}
			BIGNUM* private_key() const
			{
				return key->d;
			}
			
			void RSAEncrypt( unsigned char* data, uint32_t len, unsigned char* outBuffer );
			void RSADecrypt( unsigned char* data, uint32_t len, unsigned char* outBuffer );
		};
		
	}
}

#endif