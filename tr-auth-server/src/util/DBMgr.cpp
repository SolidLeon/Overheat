/*
 *  DBMgr.cpp
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 01.06.11.
 *  Copyright 2011 Overheat. All rights reserved.
 *
 */

#include "DBMgr.h"

using namespace tr::util;

DBMgr *DBMgr::instance = 0;

DBMgr::DBMgr(const char *dbHost, unsigned int dbPort, const char *dbUser, const char *dbPass, const char *dbName)
{
	this->dbHost = std::string(dbHost);
	this->dbPort = dbPort;
	this->dbUser = std::string(dbUser);
	this->dbPass = std::string(dbPass);
	this->dbName = std::string(dbName);
	
	this->dbHandle = mysql_init(0);
	printf("MySQL init OK\n");
	MYSQL *dbHandleErr = dbHandle;
	//printf("Connect to %s on port %d\n", this->dbHost, this->dbPort);
	printf("Connect to %s on port %d\n", this->dbHost.c_str(), this->dbPort);
	this->dbHandle = mysql_real_connect(dbHandle, this->dbHost.c_str(), this->dbUser.c_str(), this->dbPass.c_str(), this->dbName.c_str(), this->dbPort, NULL, 0);
	printf("MySQL real_connect OK\n");
	if( this->dbHandle == NULL )
	{
		printf("MySQL: %s\n", mysql_error(dbHandleErr));
		Sleep(10*1000);
	}
}

DBMgr::~DBMgr()
{
	if( this->dbHandle != NULL )
	{
		mysql_close(this->dbHandle);
	}
}

DBMgr& DBMgr::get_instance()
{
	return *instance;
}

DBMgr& DBMgr::create(const char *dbHost, unsigned int dbPort, const char *dbUser, const char *dbPass, const char *dbName)
{
	instance = new DBMgr(dbHost, dbPort, dbUser, dbPass, dbName);
	return *instance;
}

bool DBMgr::query(const char *sql)
{
	if( mysql_query(this->dbHandle, sql) )
	{
		printf("MySQL: Error with query: %s\n", mysql_error(this->dbHandle));
		return false;
	}
	return true;
}

MYSQL_RES* DBMgr::store_result()
{
	return mysql_store_result(this->dbHandle);
}

unsigned long long DBMgr::num_rows(MYSQL_RES* result)
{
	return mysql_num_rows(result);
}

MYSQL_ROW DBMgr::fetch_row(MYSQL_RES *result)
{
	MYSQL_ROW row = mysql_fetch_row(result);
	return row;
}

void DBMgr::free_result(MYSQL_RES* result)
{
	mysql_free_result(result);
}


int DBMgr::validate_player( const char *name, const char *pw, int64_t *UID )
{
	std::string sql;
	sql = "SELECT * FROM `users` WHERE username='";
	sql += name;
	sql += "' AND password='";
	sql += pw;
	sql += "'";
	
	if(!query(sql.c_str()))
	{
		return -1;
	}
	
	MYSQL_RES* res = store_result();
	uint64_t rows = num_rows(res);
	if( rows > 0 )
	{
		MYSQL_ROW row = fetch_row(res);
		int acc_status = atoi(row[4]);
		switch( acc_status )
		{
			case 1: //ACC BLOCKED
				free_result(res);
				return 1;
			case 2: //ACC Already Logged In
				free_result(res);
				return 2;
		}
		*UID = atoi(row[0]);
		free_result(res);
		return 0;
	}
	free_result(res);
	return -1;
}
