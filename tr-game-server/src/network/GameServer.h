//
//  GameServer.h
//  tr-game-server
//
//  Created by Markus Mannel on 04.06.11.
//  Copyright 2011 Overheat. All rights reserved.
//

#ifndef _TR_GAMESERVER_H_
#define _TR_GAMESERVER_H_

#include "types.h"
#include "DBMgr.h"
#include <string>
#include <sstream>

namespace tr
{
    namespace net
    {
        class CGameServer
        {
        private:
            uint8_t server_id;
            uint32_t host;
            uint32_t port;
            uint8_t age_limit;
            uint8_t pk_flag;
            uint16_t current_user_count;
            uint16_t max_user_count;
            uint8_t server_status;
            uint32_t auth_gg_port;
            uint32_t auth_port1;
        public:
            uint32_t get_ip() const { return host; }
            uint32_t get_port() const { return port; }
            uint32_t get_auth_gg_port() const { return auth_gg_port; }
            uint32_t get_auth_port1() const { return auth_port1; }
            uint32_t get_auth_gaming_port() const { return port; }
            
            static CGameServer& load_by_id( uint8_t server_id )
            {
                CGameServer *gs = new CGameServer();
                char sql[0xFF];
                
                
                sprintf(sql, "SELECT server_id, host, port, age_limit, pk_flag, current_users, max_users, status, auth_gg_port, auth_port1 FROM game_servers WHERE server_id = %d", server_id);
                
                tr::util::DBMgr::get_instance().query(sql);
                MYSQL_RES* res;
                res = tr::util::DBMgr::get_instance().store_result();
                uint64_t num_rows = tr::util::DBMgr::get_instance().num_rows(res);
                if( num_rows > 0 )
                {
                    MYSQL_ROW row;
                    row = tr::util::DBMgr::get_instance().fetch_row(res);
                    
                    gs->server_id           = (uint8_t)atoi(row[0]);      //server_id
                    gs->host                = (uint32_t)(ip2ul(row[1]))&0xFFFFFFFF;   //host 
                    gs->port                = atoi(row[2]);               //port
                    gs->age_limit           = (uint8_t)atoi(row[3]);      //age_limit
                    gs->pk_flag             = (uint8_t)atoi(row[4]);      //pk_flag
                    gs->current_user_count  = atoi(row[5]);               //current_user_count
                    gs->max_user_count      = atoi(row[6]);               //max_user_count
                    gs->server_status       = (uint8_t)atoi(row[7]);      //server_status
                    gs->auth_gg_port        = (uint32_t) atoi(row[8]);
                    gs->auth_port1        = (uint32_t) atoi(row[9]);
                    printf(">> GameServer: Loaded GS from Database: ID %d\n", gs->server_id);
                }
                else
                {
                    printf("Could not find server with id %d\n", server_id);
                }
                return *gs;
            }
            void store()
            {
                std::stringstream converter;
                std::string sql;
                std::string sServerID, sHost, sPort, sAgeLimit, sPkFlag, sCurrentUsers, sMaxUsers, sStatus, sAuthGGPort, sAuthPort1;
                
                converter << server_id; converter >> sServerID; converter.clear();
                converter << host; converter >> sHost; converter.clear();
                converter << port; converter >> sPort; converter.clear();
                converter << age_limit; converter >> sAgeLimit; converter.clear();
                converter << pk_flag; converter >> sPkFlag; converter.clear();
                converter << current_user_count; converter >> sCurrentUsers; converter.clear();
                converter << max_user_count; converter >> sMaxUsers; converter.clear();
                converter << server_status; converter >> sStatus; converter.clear();
                converter << auth_gg_port; converter >> sAuthGGPort; converter.clear();
                converter << auth_port1; converter >> sAuthPort1; converter.clear();
                
                
                sql = "INSERT INTO game_servers(server_id, host, port, age_limit, pk_flag, current_users, max_users, status, static, auth_gg_port, auth_port1) VALUES(";
                sql += sServerID;
                sql += ", ";
                sql += sHost;
                sql += ", ";
                sql += sPort;
                sql += ", ";
                sql += sAgeLimit;
                sql += ", ";
                sql += sPkFlag;
                sql += ", ";
                sql += sCurrentUsers;
                sql += ", ";
                sql += sMaxUsers;
                sql += ", ";
                sql += sStatus;
                sql += ", ";
                sql += "0, ";
                sql += sAuthGGPort;
                sql += ", ";
                sql += sAuthPort1;
                sql += ")";
                tr::util::DBMgr::get_instance().query(sql.c_str());
            }
            
            static uint64_t ip2ul(const char* ip)
            {
                unsigned long localhost = 16777343;
                if(strcmp(ip, "localhost") == 0)
                {
                    return localhost;
                }
                
                char sip[strlen(ip)];
                strcpy(sip, ip); 
                uint8_t bip[4];
                char* token;
                int i = 0;
                for( token = strtok(sip, "."); token; token = strtok(NULL, "."))
                {
                    bip[i] = strtoul(token, NULL, 10);
                    i++;
                }
                uint32_t uip = 0;
                uip |= bip[0];
                uip |= bip[1]<<8;
                uip |= bip[2]<<16;
                uip |= bip[3]<<24;
                return uip;
            }
        };
    }
}

#endif
