/*
 *  types.h
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 31.05.11.
 *  Copyright 2011 InfiniteRasa. All rights reserved.
 *
 */

#ifndef _TR_TYPES_H_
#define _TR_TYPES_H_

#include <stdint.h>

#ifndef _WIN32
typedef int8_t __int8;
typedef uint8_t __uint8; 
typedef int16_t __int16;
typedef uint16_t __uint16; 
typedef int32_t __int32;
typedef uint32_t __uint32;
typedef int64_t __int64;
typedef uint64_t __uint64;
#endif

#ifndef _WIN32
#define __cdecl /**/

#define __debugbreak() /**/
#endif

#endif