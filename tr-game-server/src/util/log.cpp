/*
 *  log.cpp
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 29.05.11.
 *  Copyright 2011 Overheat. All rights reserved.
 *
 */

#include "log.h"

using tr::util::CLog;

CLog::CLog(const char* clasz)
{
	this->clasz = new char[strlen(clasz)];
	strncpy(this->clasz, clasz, strlen(clasz));
}

CLog& CLog::get_logger(const char* clasz)
{
	CLog *log = new CLog(clasz);
	
	return *log;
}

void CLog::info(const char* msg)
{
	std::cout << "[INFO - " << this->clasz << "]: " << msg << std::endl;
}

void CLog::error(const char* msg)
{
	std::cerr << "[ERROR - " << this->clasz << "]: " << msg << std::endl;
	exit(1);
}