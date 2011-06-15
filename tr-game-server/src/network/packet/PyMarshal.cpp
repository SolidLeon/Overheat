//
//  PyMarshal.cpp
//  tr-game-server
//
//  Created by Markus Mannel on 15.06.11.
//  Copyright 2011 Overheat. All rights reserved.
//

#include "PyMarshal.h"

using tr::net::packet::CPyMarshal;

void CPyMarshal::pym_init(pyMarshalString_t *pms)
{
	pms->stackIdx = 0;
	pms->idx = 0;
}

void CPyMarshal::pym_tuple_begin(pyMarshalString_t *pms)
{
	if( pms->stackIdx )
		pms->containerStack[pms->stackIdx-1].subelements++;
	pms->containerStack[pms->stackIdx].type = 't';
	pms->containerStack[pms->stackIdx].startOffset = pms->idx;
	pms->containerStack[pms->stackIdx].subelements = 0;
	
	// tuple data
	pms->buffer[pms->idx] = 0x28; pms->idx++;
	pms->idx += 4;
    
	pms->stackIdx++;
}

void CPyMarshal::pym_tuple_end(pyMarshalString_t *pms)
{
	pms->stackIdx--;
	if( pms->containerStack[pms->stackIdx].type != 't' )
	{
		__debugbreak();
	}
	*(unsigned int*)(pms->buffer+pms->containerStack[pms->stackIdx].startOffset+1) = pms->containerStack[pms->stackIdx].subelements;
}

void CPyMarshal::pym_list_begin(pyMarshalString_t *pms)
{
	if( pms->stackIdx )
		pms->containerStack[pms->stackIdx-1].subelements++;
	pms->containerStack[pms->stackIdx].type = 'l';
	pms->containerStack[pms->stackIdx].startOffset = pms->idx;
	pms->containerStack[pms->stackIdx].subelements = 0;
	
	// list data
	pms->buffer[pms->idx] = 0x5B; pms->idx++;
	pms->idx += 4;
    
	//*(unsigned int*)&(pms->buffer+pms->idx) = 0x00000000; pys->CLen += 4; //Elementcount - Dont forget to update this
    
	pms->stackIdx++;
}

void CPyMarshal::pym_list_end(pyMarshalString_t *pms)
{
	pms->stackIdx--;
	if( pms->containerStack[pms->stackIdx].type != 'l' )
	{
		__debugbreak();
	}
	*(unsigned int*)(pms->buffer+pms->containerStack[pms->stackIdx].startOffset+1) = pms->containerStack[pms->stackIdx].subelements;
}

void CPyMarshal::pym_dict_begin(pyMarshalString_t *pms)
{
	if( pms->stackIdx )
		pms->containerStack[pms->stackIdx-1].subelements++;
	pms->containerStack[pms->stackIdx].type = 'd';
	pms->containerStack[pms->stackIdx].startOffset = pms->idx;
	pms->containerStack[pms->stackIdx].subelements = 0;
	// dict data
	pms->buffer[pms->idx] = 0x7B; pms->idx++;
	pms->stackIdx++;
}

void CPyMarshal::pym_dict_end(pyMarshalString_t *pms)
{
	pms->stackIdx--;
	if( pms->containerStack[pms->stackIdx].type != 'd' )
	{
		__debugbreak();
	}
	pms->buffer[pms->idx] = 0x30; pms->idx++;
}

void CPyMarshal::pym_dict_addKey(pyMarshalString_t *pms, char *s)
{
	unsigned int slen = strlen(s);
	pms->buffer[pms->idx] = 0x74; pms->idx++;
	*(int*)(pms->buffer+pms->idx) = slen; pms->idx += 4;
	// append string data
	for(int i=0; i<slen; i++)
	{
		*(char*)(pms->buffer+pms->idx) = s[i]; pms->idx++;
	}
	// increase subelement counter
	if( pms->stackIdx )
		pms->containerStack[pms->stackIdx-1].subelements++;
}

void CPyMarshal::pym_addInt(pyMarshalString_t *pms, int value)
{
	pms->buffer[pms->idx] = 0x69; pms->idx++;
	*(int*)(pms->buffer+pms->idx) = value; pms->idx += 4;
	if( pms->stackIdx )
	{
		pms->containerStack[pms->stackIdx-1].subelements++;
	}
}

void CPyMarshal::pym_addString(pyMarshalString_t *pms, char *s)
{
	unsigned int slen = strlen(s);
	pms->buffer[pms->idx] = 's'; pms->idx++;
	*(int*)(pms->buffer+pms->idx) = slen; pms->idx += 4;
	// append string data
	for(int i=0; i<slen; i++)
	{
		*(char*)(pms->buffer+pms->idx) = s[i]; pms->idx++;
	}
	// increase subelement counter
	if( pms->stackIdx )
		pms->containerStack[pms->stackIdx-1].subelements++;
}

void CPyMarshal::pym_addUnicode(pyMarshalString_t *pms, char *s)
{
	unsigned int slen = strlen(s);
	pms->buffer[pms->idx] = 'u'; pms->idx++;
	*(int*)(pms->buffer+pms->idx) = slen; pms->idx += 4;
	// append string data
	for(int i=0; i<slen; i++)
	{
		*(char*)(pms->buffer+pms->idx) = s[i]; pms->idx++;
	}
	// increase subelement counter
	if( pms->stackIdx )
		pms->containerStack[pms->stackIdx-1].subelements++;
}

void CPyMarshal::pym_addNoneStruct(pyMarshalString_t *pms)
{
	pms->buffer[pms->idx] = 0x4E; pms->idx++;
	if( pms->stackIdx )
	{
		pms->containerStack[pms->stackIdx-1].subelements++;
	}
}

void CPyMarshal::pym_addBool(pyMarshalString_t *pms, bool b)
{
	pms->buffer[pms->idx] = b?'T':'F'; pms->idx++;
	if( pms->stackIdx )
	{
		pms->containerStack[pms->stackIdx-1].subelements++;
	}
}

void CPyMarshal::pym_addFloat(pyMarshalString_t *pms, float value)
{
	pms->buffer[pms->idx] = 0x66; pms->idx++;
	// convert to string
	int len = 0;
	char floatData[128];
	sprintf(floatData, "%f", value);
	// write len
	len = strlen(floatData);
	pms->buffer[pms->idx] = len; pms->idx++;
	// write float as string
	for(int i=0; i<len; i++)
	{
		*(char*)(pms->buffer+pms->idx) = floatData[i]; pms->idx++;
	}
	if( pms->stackIdx )
	{
		pms->containerStack[pms->stackIdx-1].subelements++;
	}
}

unsigned char *CPyMarshal::pym_getData(pyMarshalString_t *pms)
{
	return pms->buffer;
}

unsigned int CPyMarshal::pym_getLen(pyMarshalString_t *pms)
{
	return pms->idx;
}



//unpacking
#pragma mark Unpacking

void CPyMarshal::pym_init(pyUnmarshalString_t *pms, unsigned char *rawData, int len)
{
	pms->buffer = rawData;
	pms->len = len;
	pms->idx = 0;
	pms->unpackErrorEncountered = false;
	pms->stackIndex = 0;
}

bool CPyMarshal::pym_unpackTuple_begin(pyUnmarshalString_t *pms)
{
	unsigned char p = pms->buffer[pms->idx];
	if( (p&0xF0) == 0x80 )
	{
		int c = p & 0x0F;
		int tupleLen;
		if( c <= 0xC )
		{
			tupleLen = c;
			pms->idx++;
		}
		else if( c == 0xD )
		{
			tupleLen = *(unsigned char*)(pms->buffer+pms->idx+1);
			pms->idx += 2;
		}
		else if( c == 0xE )
		{
			tupleLen = *(unsigned short*)(pms->buffer+pms->idx+1);
			pms->idx += 3;
		}
		else
		{
			tupleLen = *(unsigned int*)(pms->buffer+pms->idx+1);
			pms->idx += 5;
		}
		// add tuple container
		pms->containerStack[pms->stackIndex].type = 't';
		pms->containerStack[pms->stackIndex].size = tupleLen;
		pms->containerStack[pms->stackIndex].subelementsLeft = tupleLen;
		// dont reduce parent yet
        
		pms->stackIndex++;
		return true;
	}
	else
	{
		pms->unpackErrorEncountered = true;
		return false;
	}
}

bool CPyMarshal::pym_unpackDict_begin(pyUnmarshalString_t *pms)
{
	unsigned char p = pms->buffer[pms->idx];
	if( (p&0xF0) == 0x60 )
	{
		int c = p & 0x0F;
		int dictSize;
		if( c <= 0xC )
		{
			dictSize = c;
			pms->idx++;
		}
		else if( c == 0xD )
		{
			dictSize = *(unsigned char*)(pms->buffer+pms->idx+1);
			pms->idx += 2;
		}
		else if( c == 0xE )
		{
			dictSize = *(unsigned short*)(pms->buffer+pms->idx+1);
			pms->idx += 3;
		}
		else
		{
			dictSize = *(unsigned int*)(pms->buffer+pms->idx+1);
			pms->idx += 5;
		}
		// add dict container
		pms->containerStack[pms->stackIndex].type = 'd';
		pms->containerStack[pms->stackIndex].size = dictSize;
		pms->containerStack[pms->stackIndex].subelementsLeft = dictSize*2;
		pms->stackIndex++;
		return true;
	}
	else
	{
		pms->unpackErrorEncountered = true;
		return false;
	}
}

int CPyMarshal::pym_getContainerSize(pyUnmarshalString_t *pms)
{
	if( pms->stackIndex == 0 )
		return 0;
	return pms->containerStack[pms->stackIndex-1].size;
}

int CPyMarshal::pym_unpackInt(pyUnmarshalString_t *pms)
{
	unsigned char p = pms->buffer[pms->idx];
	if( (p&0xF0) == 0x10 )
	{
		int c = p & 0x0F;
		int value;
		if( c <= 0xC )
		{
			value = c;
			pms->idx++;
		}
		else if( c == 0xD )
		{
			value = *(signed char*)(pms->buffer+pms->idx+1);
			pms->idx += 2;
		}
		else if( c == 0xE )
		{
			value = *(signed short*)(pms->buffer+pms->idx+1);
			pms->idx += 3;
		}
		else
		{
			value = *(signed int*)(pms->buffer+pms->idx+1);
			pms->idx += 5;
		}
		// reduce parent
		if( pms->stackIndex )
		{
			while( pms->stackIndex && pms->containerStack[pms->stackIndex-1].subelementsLeft == 1 )
			{
				pms->containerStack[pms->stackIndex-1].subelementsLeft = 0;
				pms->stackIndex--;
			}
		}
		return value;
	}
	else if( (p&0xF0) == 0x20 )
	{
		long long value = pym_unpackLongLong(pms);
		return (int)value;
	}
	return 0;
}

long long CPyMarshal::pym_unpackLongLong(pyUnmarshalString_t *pms)
{
	unsigned char p = pms->buffer[pms->idx];
	if( (p&0xF0) == 0x20 )
	{
		int c = p & 0x0F;
		long long value;
		if( c <= 0xC )
		{
			__debugbreak();
		}
		else if( c == 0xD )
		{
			__debugbreak();
		}
		else if( c == 0xE )
		{
			__debugbreak();
		}
		else
		{
			value = *(signed long long*)(pms->buffer+pms->idx+1);
			pms->idx += 9;
		}
		// reduce parent
		if( pms->stackIndex )
		{
			while( pms->stackIndex && pms->containerStack[pms->stackIndex-1].subelementsLeft == 1 )
			{
				pms->containerStack[pms->stackIndex-1].subelementsLeft = 0;
				pms->stackIndex--;
			}
		}
		return value;
	}
	else if( (p&0xF0) == 0x10 )
	{
		long long value = pym_unpackInt(pms);
		return value;
	}
	return 0;
}

int CPyMarshal::pym_unpackUnicode(pyUnmarshalString_t *pms, char *dst, int limit)
{
	// pyUnmarshalString_t *pms
	unsigned char p = pms->buffer[pms->idx];
	if( (p&0xF0) == 0x50 )
	{
		int c = p & 0x0F;
		int slen;
		if( c <= 0xC )
		{
			slen = c;
			pms->idx++;
		}
		else if( c == 0xD )
		{
			slen = *(unsigned char*)(pms->buffer+pms->idx+1);
			pms->idx += 2;
		}
		else if( c == 0xE )
		{
			slen = *(unsigned short*)(pms->buffer+pms->idx+1);
			pms->idx += 3;
		}
		else
		{
			slen = *(unsigned int*)(pms->buffer+pms->idx+1);
			pms->idx += 5;
		}
		// read string data
		int rlen = min(slen,limit-1);
		for(int i=0; i<rlen; i++)
		{
			dst[i] = (char)pms->buffer[pms->idx+i];
		}
		dst[rlen] = '\0';
		pms->idx += slen;
		// reduce parent
		if( pms->stackIndex )
		{
			while( pms->stackIndex && pms->containerStack[pms->stackIndex-1].subelementsLeft == 1 )
			{
				pms->containerStack[pms->stackIndex-1].subelementsLeft = 0;
				pms->stackIndex--;
			}
		}
		return rlen;
	}
	return -1;
}

float CPyMarshal::pym_unpackFloat(pyUnmarshalString_t *pms)
{
	unsigned char p = pms->buffer[pms->idx];
	if( (p&0xF0) == 0x30 )
	{
		float value;
		if( p == 0x31 )
		{
			value = 1.0f;
			pms->idx++;
		}
		else if( p == 0x3F )
		{
			value = *(float*)(pms->buffer+pms->idx+1);
			pms->idx += 5;
		}
		else
			__debugbreak();
		
		// reduce parent
		if( pms->stackIndex )
		{
			while( pms->stackIndex && pms->containerStack[pms->stackIndex-1].subelementsLeft == 1 )
			{
				pms->containerStack[pms->stackIndex-1].subelementsLeft = 0;
				pms->stackIndex--;
			}
		}
		return value;
	}
	return 0;
}

bool CPyMarshal::pym_unpack_isNoneStruct(pyUnmarshalString_t *pms)
{
	unsigned char p = pms->buffer[pms->idx];
	return p == 0;
}

bool CPyMarshal::pym_unpackNoneStruct(pyUnmarshalString_t *pms)
{
	unsigned char p = pms->buffer[pms->idx];
	if( p )
		return false;
	pms->idx++;
	return true;
}






