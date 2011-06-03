/*
 *  rsa.cpp
 *  tr-game-server
 *
 *  Created by Markus Mannel on 03.06.11.
 *  Copyright 2011 HTL. All rights reserved.
 *
 */

#include "rsa.h"

using namespace tr::crypto;

CRSA::CRSA()
{
	
}

CRSA::~CRSA()
{
	RSA_free(key);
}

void CRSA::RSAInit()
{
	key = RSA_new();
}

//Encrypt with public key
void CRSA::RSAEncrypt( unsigned char* data, uint32_t len, unsigned char* outBuffer )
{
	RSA_public_encrypt(len, data, outBuffer, key, RSA_PKCS1_PADDING);
}

void CRSA::RSADecrypt( unsigned char* data, uint32_t len, unsigned char* outBuffer )
{
	RSA_private_decrypt(len, data, outBuffer, key, RSA_PKCS1_PADDING);
}