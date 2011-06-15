//
//  PyMarshal.h
//  tr-game-server
//
//  Created by Markus Mannel on 15.06.11.
//  Copyright 2011 Overheat. All rights reserved.
//

#ifndef _TR_PYMARSHAL_H_
#define _TR_PYMARSHAL_H_

#include "PacketBuffer.h"

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
namespace tr 
{
    namespace net
    {
        namespace packet 
        {
            class CPyMarshal
            {
            private:
                typedef struct  
                {
                    char type; //'t' tuple, 'd' dict
                    // general
                    int startOffset;
                    int subelements;
                    
                    // dict info
                    
                    // tuple info
                    //int tupleSize;
                }containerStackEntry_t;
                
                typedef struct  
                {
                    unsigned char buffer[1024];
                    int idx;
                    containerStackEntry_t containerStack[16];
                    int stackIdx;
                }pyMarshalString_t;
                
                typedef struct  
                {
                    char type; // 'd' - dict, 't' - tuple
                    unsigned int size;
                    unsigned int subelementsLeft;
                }pyUnmarshalContainer_t;
                
                typedef struct  
                {
                    unsigned char *buffer;
                    int idx;
                    int len;
                    bool unpackErrorEncountered;
                    pyUnmarshalContainer_t containerStack[16];
                    int stackIndex;
                }pyUnmarshalString_t;
                
                pyMarshalString_t pms;
                pyUnmarshalString_t pums;
                
                void pym_init(pyMarshalString_t *pms);
                // new marshal packing lib
                
                
                void pym_tuple_begin(pyMarshalString_t *pms);
                void pym_tuple_end(pyMarshalString_t *pms);
                
                void pym_list_begin(pyMarshalString_t *pms);
                void pym_list_end(pyMarshalString_t *pms);
                
                void pym_dict_begin(pyMarshalString_t *pms);
                void pym_dict_end(pyMarshalString_t *pms);
                void pym_dict_addKey(pyMarshalString_t *pms, char *s);
                
                void pym_addInt(pyMarshalString_t *pms, int value);
                void pym_addString(pyMarshalString_t *pms, char *s);
                void pym_addUnicode(pyMarshalString_t *pms, char *s);
                void pym_addNoneStruct(pyMarshalString_t *pms);
                void pym_addBool(pyMarshalString_t *pms, bool b);
                void pym_addFloat(pyMarshalString_t *pms, float value);
                
                unsigned char *pym_getData(pyMarshalString_t *pms);
                unsigned int pym_getLen(pyMarshalString_t *pms);
                
                // new marshal unpacking lib
                
                void pym_init(pyUnmarshalString_t *pms, unsigned char *rawData, int len);
                bool pym_unpackTuple_begin(pyUnmarshalString_t *pms);
                bool pym_unpackDict_begin(pyUnmarshalString_t *pms);
                int pym_getContainerSize(pyUnmarshalString_t *pms);
                int pym_unpackInt(pyUnmarshalString_t *pms);
                long long pym_unpackLongLong(pyUnmarshalString_t *pms);
                int pym_unpackUnicode(pyUnmarshalString_t *pms, char *dst, int limit);
                float pym_unpackFloat(pyUnmarshalString_t *pms);
                bool pym_unpackNoneStruct(pyUnmarshalString_t *pms);
                
                bool pym_unpack_isNoneStruct(pyUnmarshalString_t *pms);
            public:
                CPyMarshal( )
                {
                }
                
                void init()
                {
                    pym_init(&pms);
                }
                // new marshal packing lib
                
                
                void tuple_begin(){ pym_tuple_begin(&pms); }
                void tuple_end(){ pym_tuple_end(&pms); }
                
                void list_begin() { pym_list_begin(&pms); }
                void list_end() { pym_list_end(&pms); }
                
                void dict_begin() { pym_dict_begin(&pms); }
                void dict_end() { pym_dict_end(&pms); }
                void dict_addKey(char *s) { pym_dict_addKey(&pms, s); }
                
                void addInt(int value) { pym_addInt(&pms, value); }
                void addString(char *s) { pym_addString(&pms, s); }
                void addUnicode(char *s) { pym_addUnicode(&pms, s); }
                void addNoneStruct() { pym_addNoneStruct(&pms); } 
                void addBool(bool b) { pym_addBool(&pms, b); }
                void addFloat(float value) { pym_addFloat(&pms, value); }
                
                unsigned char *getData() { return pym_getData(&pms); }
                unsigned int getLen() { return pym_getLen(&pms); }
                
                // new marshal unpacking lib
                
                void init(unsigned char *rawData, int len) { pym_init(&pums, rawData, len); }
                bool unpackTuple_begin() { return pym_unpackTuple_begin(&pums); }
                bool unpackDict_begin() { return pym_unpackDict_begin(&pums); }
                int getContainerSize() { return pym_getContainerSize(&pums); }
                int unpackInt() { return pym_unpackInt(&pums); }
                long long unpackLongLong() { return pym_unpackLongLong(&pums); }
                int unpackUnicode(char* dst, int limit) { return pym_unpackUnicode(&pums, dst, limit);}
                float unpackFloat() { return pym_unpackFloat(&pums); }
                bool unpackNoneStruct() { return pym_unpackNoneStruct(&pums); }
                
                bool unpack_isNoneStruct() { return pym_unpack_isNoneStruct(&pums); }
                
            };
        }
    }
}

#endif