//
//  Session.h
//  tr-game-server
//
//  Created by Markus Mannel on 04.06.11.
//  Copyright 2011 HTL. All rights reserved.
//

#ifndef _TR_SESSION_H_
#define _TR_SESSION_H_

#include "types.h"

namespace tr
{
    namespace net
    {
        class CSession
        {
        private:
            union {
                uint64_t sessionID;
                struct {
                    uint32_t sessionID1;
                    uint32_t sessionID2; 
                }s;
            }sess;
        public:
            CSession()
            {
                sess.sessionID = 0;
            }
            CSession( uint64_t sessionID ) 
            {
                this->sess.sessionID = sessionID;
            }
            
            void update( uint64_t sessionID )
            {
                this->sess.sessionID = sessionID;
            }
            
            void update( uint32_t session1, uint32_t session2 )
            {
                this->sess.s.sessionID1 = session1;
                this->sess.s.sessionID2 = session2;
            }
            
            uint64_t session() const
            {
                return this->sess.sessionID;
            }
            
            uint32_t session1() const
            {
                return this->sess.s.sessionID1;
            }
            uint32_t session2() const
            {
                return this->sess.s.sessionID2;
            }
        };
    }
}

#endif