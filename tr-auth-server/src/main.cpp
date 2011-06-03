//FIXME: If you get a compiler error that the type uintX_t is not defined, include the "types.h" in the corresponding header file.


#include <iostream>
#include <string>
#include "server.h"
#include "crypto.h"
#include "DBMgr.h"
#include "SessionMgr.h"
#include "ThreadUtils.h"
#include <pthread.h>

using namespace std;

pthread_mutex_t gMutex;
bool server_running = true;

#ifndef _WIN32
#define Sleep(x) sleep(x/1000)
#endif

void* session_maintenance_thread( void* );

int main (int argc, char * const argv[]) 
{		
	
	printf(" ==============================================================================\r\n");
	printf("                                    Overheat                                   \r\n");
	printf("                              Authentication Server                        \r\n\r\n");
	printf("  Version 0.4                                             http://overheat.com/ \r\n");
	printf(" ==============================================================================\r\n");

	printf("Load Crypto...\n");
	tr::crypto::CCryptMgr::instance();
	
	printf("Load Database Driver...\n");
	tr::util::DBMgr::create( "127.0.0.1", 8889, "root", "root", "tabuladb" );
	printf("Database loaded OK\n");
	tr::net::CServer server(2106);
    
	//Delete sessions and server
	printf("Wipe sessions...\n");
	tr::net::CSessionMgr::instance().wipe_sessions();
	printf("Wipe server...\n");
	tr::net::CSessionMgr::instance().wipe_servers();
	//Start maintenance thread
	pthread_t maintenance_thread_id;
	if( Thread::New(&maintenance_thread_id, session_maintenance_thread, NULL))
	{
		printf("Maintenance Thread started\n");
	}
	else 
	{
		printf("Could not start Maintenance Thread!\n");
	}

	server.start();
	
	return 0;
}

void* session_maintenance_thread(void* parameter)
{
	while (server_running)
	{
		CSessionMgr::instance().remove_expired_sessions();
		CSessionMgr::instance().remove_expired_servers();
		Sleep(CSessionMgr::instance().session_timeout*500);
	}
	Thread::Exit();
	return 0;
}