//FIXME: If you get a compiler error that the type uintX_t is not defined, include the "types.h" in the corresoponding header file.


#include <iostream>
#include <string>
#include "server.h"
#include "crypto.h"
#include "DBMgr.h"
#include "ThreadUtils.h"
#include <pthread.h>

using namespace std;

pthread_mutex_t gMutex;
bool server_running = true;

#ifndef _WIN32
#define Sleep(x) sleep(x/1000)
#endif


int main (int argc, char * const argv[]) 
{		
	
	printf(" ==============================================================================\r\n");
	printf("                                    Overheat                                   \r\n");
	printf("                                  Game Server                              \r\n\r\n");
	printf("  Version 0.4                                             http://overheat.com/ \r\n");
	printf(" ==============================================================================\r\n");

	printf("Load Crypto...\n");
	tr::crypto::CCryptMgr::instance();
	
	printf("Load Database Driver...\n");
	tr::util::DBMgr::create( "127.0.0.1", 8889, "root", "root", "tabuladb" );
	printf("Database loaded OK\n");
    
    tr::net::CGameServer gs = tr::net::CGameServer::load_by_id(53);
	tr::net::CServer server(gs);
	
	server.start();
	
	return 0;
}
