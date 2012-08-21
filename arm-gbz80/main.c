#include <inc/lm3s1776.h>
#include <inc/hw_types.h>
#include <driverlib/debug.h>
#include <driverlib/sysctl.h>
#include <driverlib/rom.h>
#include "Z80.h"
#include "memory.h"
#include "lookuptables.h"
#include "lcd.h"
#include "drivers/s6d1121_lld.h"



inline static void draw(){


	//lcd enabled ?
	if (LCDC & 0x80) {
		drawGBLCD(0,0,gbcpu.lcd->display);
	} else {
		lld_fillRect(0, 0, 159, 143) ;
	}

}



int
main(void)
{
	//ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_12MHZ);
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |SYSCTL_XTAL_10MHZ);


	lld_lcdInit();

	lld_setBackColor(128,128,128);
	lld_lcdClear();


    Memory mem;
    LCD lcd;

    resetZ80(&mem,&lcd);


    //
    // Loop forever.
    //
    while(1){

        execute(70221,1);

        draw();

        execute(70221*2,0);

    }

    return 0;
}
