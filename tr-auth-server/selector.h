/*
 *  selector.h
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 30.05.11.
 *  Copyright 2011 HTL. All rights reserved.
 *
 */


#ifndef _SELECTOR_H_
#define _SELECTOR_H_

#include <sys/select.h>

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <vector>

#include "log.h"

namespace tr
{
	namespace net
	{
		//Selector implementation
		//Takes care of the selection stuff, to multiplex.
		//Able to register and unregister for read and write notifications.
		//-SolidLeon
		class CSelector
		{
		private:
			static tr::util::CLog log;
			int max_fds;//nfds - always highest filedescriptor of 3 descriptor sets
			fd_set readfds;
			fd_set writefds;
			int acceptfds;
			
		public:
			enum 
			{ 
				OP_ACCEPT	= 0x0001, 
				OP_READ		= 0x0010, 
				OP_WRITE	= 0x0100
			};
			CSelector();
			int select();
			void fill_fds(int*, int, int*, int);
			
			void register_accept( int );
			void register_read( int );
			void register_write( int );
			void clear_read( int );
			void clear_write( int );
			
			bool is_acceptable( int );
			bool is_readable( int );
			bool is_writeable( int );
		};//CSelector
	}//net
}//tr
#endif//_SELECTOR_H_