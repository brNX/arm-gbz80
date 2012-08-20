/*
 * memory.h
 *
 *  Created on: 2010/06/18
 *      Author: brNX
 */

#ifndef MEMORY_H_
#define MEMORY_H_

#include "types.h"



    typedef struct _Memory
    {
        uint8 wram[8192];
        uint8 hram[127];
        uint8 vram[8192];
        uint8 OAM[160];
        uint8 IO[128];
        uint8 rie;
        uint8 rif;

        int timer1;
        int timer2;
    }Memory;

     uint8 readMem(uint16 address,Memory * mem);
     void writeMem(uint16 address, uint8 value,Memory * mem);
     void lcdDMA(uint8 value,Memory * mem);
     void writeToIOZone(uint16 address, uint8 value,Memory * mem);
     uint8 getJoypadState(Memory * mem);
    //void printMEMStatus(Memory * mem);
    //void initMemory(Memory* mem,uint8_t * cart);


#endif /* MEMORY_H_ */
