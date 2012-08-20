/*
 * memory.c
 *
 *  Created on: 2010/06/18
 *      Author: brNX
 */
#include "Z80.h"
#include "memory.h"
#include "lookuptables.h"


//0000-3FFF   16KB ROM Bank 00     (in cartridge, fixed at bank 00)
//4000-7FFF   16KB ROM Bank 01..NN (in cartridge, switchable bank number)
//8000-9FFF   8KB Video RAM (VRAM) (switchable bank 0-1 in CGB Mode)
//A000-BFFF   8KB External RAM     (in cartridge, switchable bank, if any)
//C000-CFFF   4KB Work RAM Bank 0 (WRAM)
//D000-DFFF   4KB Work RAM Bank 1 (WRAM)  (switchable bank 1-7 in CGB Mode)
//E000-FDFF   Same as C000-DDFF (ECHO)    (typically not used)
//FE00-FE9F   Sprite Attribute Table (OAM)
//FEA0-FEFF   Not Usable
//FF00-FF7F   I/O Ports
//FF80-FFFE   High RAM (HRAM)
//FFFF        Interrupt Enable Register

inline uint8 readMem(uint16 address,Memory * mem)
{
    uint16 addr;

    switch (address >> 12){

    /************ROM**************/
    case 0x0:
    case 0x1:
    case 0x2:
    case 0x3:
    case 0x4:
    case 0x5:
    case 0x6:
    case 0x7:
        /*index=address >> 14;
        newaddress = address + index*((mem->cart->rombank-1)*0x4000);*/
        return cart[address];
        break;

    /*********VRAM**************/
    case 0x8:
    case 0x9:
        return mem->vram[address-0x8000];
        break;

    /*********external ram*********/
    case 0xA:
    case 0xB:
        return 0xFF;
        break;

    /***********Work RAM************/
    case 0xC:
    case 0xD:
        return mem->wram[address-0xC000];
        break;

    /********part of echo ram*******/
    case 0xE:
         return mem->wram[address-0xE000];
         break;

    //E000-FDFF   Same as C000-DDFF (ECHO)    (typically not used)
    //FE00-FE9F   Sprite Attribute Table (OAM)
    //FEA0-FEFF   Not Usable
    //FF00-FF7F   I/O Ports
    //FF80-FFFE   High RAM (HRAM)
    //FFFF        Interrupt Enable Register
    case 0xF:

         //FF00  joypad
         if (address == 0xFF00){
             return getJoypadState(mem);
             break;
         }


         //FE00-FE9F   Sprite Attribute Table (OAM)
         /*FE00 <= addr <= 0xFE9F*/
         addr= address - 0xFE00;
         if (addr <= 0x9F){
             return mem->OAM[addr];
             break;
         }

         //FF00-FF7F   I/O Ports
         addr= address - 0xFF00;
         /*FF00 <= addr <= 0xFF7F*/
         if (addr <= 0x7F){
             if (addr==0x0F)
                 return mem->rif;
             else
                 return mem->IO[addr];
              break;
         }

         //FF80-FFFE   High RAM (HRAM)
         /*FF80 <= addr <= 0xFFFE*/
         addr= address - 0xFF80;
         if (addr <= 0x7E){
             return mem->hram[addr];
             break;
         }


         //FFFF  Interrupt Enable Register
         if (address == 0xFFFF){
             return mem->rie;
             break;
         }

         //echo ram
         /*F000 <= addr <= 0xFDFF*/
         addr= address - 0xF000;
         if (addr <= 0xDFF){
             return mem->wram[0X1000|addr];
             break;
         }

        break;
    default:
        break;

    }

    return 0;
}


inline void writeMem(uint16 address, uint8 value,Memory * mem)
{
    uint16 addr;
    switch (address >> 12){

    case 0x0:
    case 0x1:
    case 0x2:
    case 0x3:
    case 0x4:
    case 0x5:
    case 0x6:
    case 0x7:
        break;

    /*******Video Ram*********/
    case 0x8:
    case 0x9:
        mem->vram[address-0x8000]=value;
        break;

    /*******external ram********/
    case 0xA:
    case 0xB:
        break;

    /*C000-CFFF   4KB Work RAM Bank 0 (WRAM)*/
    /*D000-DFFF   4KB Work RAM Bank 1 (WRAM)  (switchable bank 1-7 in CGB Mode)*/
    case 0xC:
        addr=address & 0xFFF;
        mem->wram[addr] = value;
        break;
    case 0xD:
        addr= address - 0xC000;
        mem->wram[addr] = value;
        break;

    /*E000-FDFF   Same as C000-DDFF (ECHO)    (typically not used)*/
    case 0xE:
        addr=address & 0xFFF;
        mem->wram[addr] = value;
        break;

    //I/O Zone
    case 0xF:
        writeToIOZone(address,value,mem);
        break;
    }


}

inline uint8 getJoypadState(Memory * mem){

    uint8 state = ~JOYP;

    //directional keys
    if (!(state & (1<<4))){

        state &= ((gbcpu.joypad  >> 4) | 0xF0);

    }
    //button keys
    else if (!(state & (1<<5))){
        state &= ((gbcpu.joypad & 0xF) | 0xF0);
    }

    return state;
}

inline void writeToIOZone(uint16 address, uint8 value,Memory * mem){

    uint16 addr;

    //FE00-FE9F   Sprite Attribute Table (OAM)
    /*FE00 <= addr <= 0xFE9F*/
    addr = address - 0xFE00;

    if (addr <= 0x9F){
        mem->OAM[addr]=value;
        return;
    }

    //FF80-FFFE   High RAM (HRAM)
    /*FF80 <= addr <= 0xFFFE*/
    addr = address - 0xFF80;
    if (addr <= 0x7E){
        mem->hram[addr]=value;
        return;
    }

    //FF00-FF7F   I/O Ports
    addr= address - 0xFF00;
    /*FF00 <= addr <= 0xFF7F*/

    if (addr <= 0x7F)
    {

        //FF04 - DIV - Divider Register (R/W)
        if (addr == 0x04)
        {
            mem->IO[addr]=0;
            mem->timer1=0;
            return;
        }

        //FF07 - TAC
        if (addr == 0x07)
        {
            mem->IO[addr]=value|0xF8;
            mem->timer2=0;
            return;
        }

        //FF0F - IF
        if (addr == 0x0F)
        {
             mem->rif=value|0xE0;
             return;
        }

        //FF44 - LY - LCDC Y-Coordinate (R)
        if (addr == 0x44)
        {
            mem->IO[addr]=0;
            return;
        }

        //FF46 - DMA - DMA Transfer and Start Address (W)
        if (addr == 0x46)
        {
            lcdDMA(value,mem);
            return;
        }

        mem->IO[addr]=value;
        return;
    }

    //FFFF  Interrupt Enable Register
    if (address == 0xFFFF)
    {
        mem->rie=value;
        return;
    }

    //echo ram
    /*F000 <= addr <= 0xFDFF*/
    addr= address - 0xF000;
    if (addr <= 0xDFF)
    {
        mem->wram[0x1000|addr] = value;
        return;
    }


}

//DMA transfer
inline void lcdDMA(uint8 value,Memory * mem)
{
    int i=0;
    uint16 addr = value << 8;
    for (; i < 0xA0; i++)
        writeMem(0xFE00+i, readMem(addr+i,mem),mem);
}
