/*
 *  log.h
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 29.05.11.
 *  Copyright 2011 HTL. All rights reserved.
 *
 */

#ifndef _LOG_H_
#define _LOG_H_

#include <iostream>
#include <string>

namespace tr 
{
	namespace util
	{

		//Class CLog
		//Handles logging messages.
		//-SolidLeon
		class CLog
		{
		public:
			enum LogLevel 
			{
				LOG_DEBUG, //Only show if debug = on
				LOG_INFO, //Print to stdout
				LOG_WARN, //Print to stdout
				LOG_ERROR //Print to stderr and if given to a file
			};

		private:
			/* Name of the class the logger is used for */
			char* clasz;
			/* Default constructor */
			CLog( const char* );

		public:
		
			~CLog( )
			{
				delete[] clasz;
			}
			
			/* Returns a new Logger for the given class */
			static CLog& get_logger(const char*);

			//void log(LogLevel level, const char* msg);
			void info(const char*);
			void error(const char*);

		}; //CLog

	} //util
} //tr

#endif //_LOG_H_

