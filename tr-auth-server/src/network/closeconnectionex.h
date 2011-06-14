/*
 *  closeconnectionex.h
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 30.05.11.
 *  Copyright 2011 Overheat. All rights reserved.
 *
 */

#ifndef _CLOSECONNECTIONEX_H_
#define _CLOSECONNECTIONEX_H_

#include <string>
#include <stdexcept>

namespace tr
{
	namespace net
	{
		class CConnectionClosedEx : public std::runtime_error
		{
		private:
		public:
			CConnectionClosedEx() : std::runtime_error("Connection is Closed"){}
		}; //CConnectionClosedEx
	}//net
}//tr
#endif//_CLOSECONNECTIONEX_H_