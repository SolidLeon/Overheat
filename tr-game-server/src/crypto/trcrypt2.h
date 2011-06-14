/*
 *  trcrypt2.h
 *  tr-game-server
 *
 *  Created by Markus Mannel on 02.06.11.
 *  Copyright 2011 Overheat. All rights reserved.
 *
 */

#ifndef _TR_TRCRYPT2_H_
#define _TR_TRCRYPT2_H_

#include "types.h"
#include <string>
//#include "md5.h"
#include <openssl/dh.h>
#include <openssl/bn.h>
#include <openssl/md5.h>

#define BYTE1(x) ((x&0xFF00)>>8)
#define _BYTE	uint8_t
#define _DWORD	uint32_t

namespace tr
{
	namespace crypto
	{
		class CTRCrypt2
		{
		private:
			typedef struct
			{
				unsigned char CryptBlock[0x1018 + 0x10]; //Unknown + MD5
				unsigned char K[0x40]; //Backup of K
			}TABULACRYPT2;
			
            static const uint8_t DH_Constant_Prime[0x40];
			static uint8_t UnknownCryptTable1_BAE6D0[];
			static uint8_t InputK[0x40];
			static uint8_t CompareD[0x1018 + 0x10];
			//int __usercall sub_709330<eax>(uint32_t *result<eax>, uint32_t *a2<esi>)
			void sub_709330(uint32_t *result, uint32_t *a2);			
			//int __usercall sub_708C60<eax>(uint8_t *input<eax>, uint8_t *output)
			void Copy_708C60(uint8_t *input, uint8_t *output);		
			void BuildDecryptorTableByK(uint8_t *in, uint64_t out);
			int SwitchEndianInt(int Value);
			void __Tabula_Decrypt2(uint32_t *result, uint32_t *a2, uint32_t *X, uint32_t *Y);		
			void __Tabula_Encrypt2(uint32_t *result, uint32_t *a2, uint32_t *X, uint32_t *Y);
			//K must have a size of 0x40(64) bytes!
			void Tabula_CryptInit2(TABULACRYPT2 *tbc2, uint8_t *InputK); //Call after key exchange
			
            DH *dh;
            BIGNUM* bnB;
            BIGNUM* bnK;
            BN_CTX *bn_ctx;
            TABULACRYPT2 tbc2;
            
            
		public:
            
			CTRCrypt2();
            ~CTRCrypt2();
			void Tabula_Encrypt2(TABULACRYPT2 *tbc2, uint32_t *PacketData, uint32_t Len);			
			void Tabula_Decrypt2(TABULACRYPT2 *tbc2, uint32_t *PacketData, uint32_t Len);
            
            //Diffie Hellman
            void setup_dh();
            
            //Public Key, generator ^ priv_key
            const BIGNUM* A() const;
            //Primenumber
            const BIGNUM* Prime() const;
            //Generator
            const BIGNUM* G() const ;
            void DH_UpdateB(BIGNUM* newB);
            const BIGNUM* K() const;
            
            void encrypt(uint32_t* packet_data, uint32_t len);
            void decrypt(uint32_t* packet_data, uint32_t len);
		private:	
		};
	}
}

#endif