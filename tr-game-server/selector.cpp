/*
 *  selector.cpp
 *  tr-auth-server
 *
 *  Created by Markus Mannel on 30.05.11.
 *  Copyright 2011 HTL. All rights reserved.
 *
 */

#include "selector.h"

using tr::net::CSelector;

static tr::util::CLog log = tr::util::CLog::get_logger("Selector");

//Creates a new Selector object
//-SolidLeon
CSelector::CSelector()
{
	max_fds = 0;
	acceptfds = 0;
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
}

//Resets the FDs
//Sets the LISTENING fd
//Sets all read and write fds given.
//-SolidLeon
void CSelector::fill_fds(int* rfds, int nr, int* wfds, int nw)
{
	FD_ZERO( &readfds );
	FD_ZERO( &writefds );
	
	FD_SET( acceptfds, &readfds );
	
	for (int i = 0; i < nr; i++)
	{
		if( rfds[i] != 0 )		
			FD_SET( rfds[i], &readfds );
	}
	
	for (int i = 0; i < nw; i++)
	{
		if( wfds[i] != 0 )
			FD_SET( wfds[i], &writefds );
	}
}

//Calls select()
//Returns the max number of changes in any fd
//-SolidLeon
int CSelector::select()
{		
	//::log.info("select()...");
	int rc = ::select(max_fds+1, &readfds, &writefds, NULL, NULL);
	if( rc == -1 )
	{
		::log.error( "Select return -1" );
	}
	return rc;
}

//Registrates the given FD as read descriptor, 
//but stores it also as acceptfds.
//But only if there was not already another register_accept call!
//-SolidLeon
void CSelector::register_accept( int fd )
{
	if( acceptfds != 0 )
		return;
	register_read( fd );
	acceptfds = fd;
}

//Registers the fd with the readfds,
//and updates highes fd counter
//-SolidLeon
void CSelector::register_read( int fd )
{
	FD_SET( fd, &readfds );
	if( fd > max_fds )
		max_fds = fd;
}

//Registers the fd with the writefds,
//and updates highes fd counter
//-SolidLeon
void CSelector::register_write( int fd )
{
	FD_SET( fd, &writefds );
	if( fd > max_fds )
		max_fds = fd;	
}

//Removes the given fd from readfds
//-SolidLeon
void CSelector::clear_read( int fd )
{
	FD_CLR( fd, &readfds );
}

//Removes the given fd from writefds
//-SolidLeon
void CSelector::clear_write( int fd )
{
	FD_CLR( fd, &writefds );
}

//Checks if the listening fds is ready
//Param: just leave it at 0
//-SolidLeon
bool CSelector::is_acceptable( int fd )
{
	return FD_ISSET( acceptfds, &readfds );
}

//Checks if the given descriptor is marked readable
//Param: fd - Descriptor to check
//Return: true if given fd is not the accept fds and it is marked as readable
//-SolidLeon
bool CSelector::is_readable( int fd )
{
	return fd != acceptfds && FD_ISSET( fd, &readfds );
}

//Checks if the given descriptor is marked writeable
//Param: fd - Descriptor to check
//Return: true if given fd is not the accept fds and it is marked as writeable
//-SolidLeon
bool CSelector::is_writeable( int fd )
{
	return fd != acceptfds && FD_ISSET( fd, &writefds );
}