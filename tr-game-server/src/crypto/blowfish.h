/*
 *  blowfish.h
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 31.05.11.
 *  Copyright 2011 Overheat. All rights reserved.
 *
 */

#ifndef _TR_BLOWFISH_H_
#define _TR_BLOWFISH_H_

#include <string>

namespace tr 
{
	namespace crypto
	{
		class CBlowfish
		{
		private:
			static const int N = 16;
			
			struct BLOWFISH_CTX {
				uint32_t P[16 + 2];
				uint32_t S[4][256];
			};
			
			static uint8_t sdata[];
			static uint8_t BF_PTransformed[4*18];
			static const uint32_t ORIG_P[16+2];
			static const uint32_t ORIG_S[4][256];
			
			BLOWFISH_CTX BlowfishContext;
			uint32_t F( uint32_t );
			
		public:		
			void BFInit( );
			void BFEncrypt( uint32_t*, uint32_t* );
			void BFDecrypt( uint32_t*, uint32_t* );	
		};
	}
}

#endif