//FIXME: If you get a compiler error that the type uintX_t is not defined, include the "types.h" in the corresoponding header file.


#include <iostream>
#include <string>
#include "server.h"
//#include "crypto.h"
#include "DBMgr.h"
#include "ThreadUtils.h"
#include <pthread.h>

using namespace std;

pthread_mutex_t gMutex;
bool server_running = true;

#ifndef _WIN32
#define Sleep(x) sleep(x/1000)
#endif

//#include "PacketBuffer.h"
void* game_auth_gg_main(void* param);
void* game_auth1_main(void* param);
void* game_auth2_main(void* param);

int main (int argc, char * const argv[]) 
{	
	printf(" ==============================================================================\r\n");
	printf("                                    Overheat                                   \r\n");
	printf("                                Auth Game Server                            \r\n\r\n");
	printf("  Version 0.4                                             http://overheat.com/ \r\n");
	printf(" ==============================================================================\r\n");

	//printf(">> Load Crypto...\n");
	//tr::crypto::CCryptMgr::instance();
	
	printf(">> Load Database Driver...\n");
	try { tr::util::DBMgr::create( "127.0.0.1", 8889, "root", "root", "tabuladb" ); } catch(const char* ex) { printf("Could not load Database: %s\n", ex); return 1; }
	printf(">> Database loaded OK\n");
    printf("\n");
    
    pthread_t pt1, pt2, pt3;
	if( Thread::New(&pt1, game_auth_gg_main, NULL)) printf(">> Game Auth GG Thread started\n");
	else printf("Could not start Game Auth GG Thread!\n");
    Sleep(1000);
	if( Thread::New(&pt2, game_auth1_main, NULL)) printf(">> Game Auth 1 Thread started\n");
	else printf("Could not start Game Auth 1 Thread!\n");
    Sleep(1000);

	if( Thread::New(&pt3, game_auth2_main, NULL)) printf(">> Game Auth 2 Thread started\n");
	else printf("Could not start Game Auth 2 Thread!\n");
    Sleep(1000);
    

    Thread::Join(pt1);
    Thread::Join(pt2);
    Thread::Join(pt3);
	return 0;
}

void* game_auth_gg_main(void* param)
{
    tr::net::CGameServer gs = tr::net::CGameServer::load_by_id(53);
	tr::net::CServer server(tr::net::CServer::AUTH_GG, gs);
	server.start();
    return 0;
}

void* game_auth1_main(void* param)
{
    tr::net::CGameServer gs = tr::net::CGameServer::load_by_id(53);
	tr::net::CServer server(tr::net::CServer::AUTH_GG_FIRST, gs);
	server.start();
    return 0;
}

void* game_auth2_main(void* param)
{
    tr::net::CGameServer gs = tr::net::CGameServer::load_by_id(53);
	tr::net::CServer server(tr::net::CServer::AUTH_GAMING, gs);
	server.start();
    return 0;
}