/*
 *  authcodes.h
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 31.05.11.
 *  Copyright 2011 Overheat. All rights reserved.
 *
 */

#ifndef _TR_AUTHCODES_H_
#define _TR_AUTHCODES_H_

namespace tr
{
	namespace net 
	{
		namespace AuthError {
			enum AuthError {
				SYSTEM_ERROR		= 0x01,
				INVALID_PASSWORD	= 0x02,
				ALREADY_LOGGED_IN	= 0x07
			};
		}
		
		namespace AuthCodes {
			
			enum AuthCode
			{
					AuthHello				= 0x00,
					AuthError				= 0x01,
					AuthErrorAccountBlocked	= 0x02,
					AuthLogin				= 0x00,
					AuthOk					= 0x03,
					AuthRequestServerList	= 0x05,
					AuthServerList			= 0x04,
					AuthSelectServer		= 0x02,
					AuthGSHandoff			= 0x0C
			};
		}
				
	}
}

#endif