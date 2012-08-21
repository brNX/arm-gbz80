#include <inc/lm3s1776.h>
#include <inc/hw_types.h>
#include <inc/hw_memmap.h>
#include <inc/hw_gpio.h>
#include <driverlib/debug.h>
#include <driverlib/sysctl.h>
#include <driverlib/systick.h>
#include <driverlib/gpio.h>
#include <driverlib/rom.h>
#include <stdlib.h>
#include "s6d1121_lld.h"

static unsigned long ulClockMS=0;

enum {WHITE,LIGHT_GRAY,DARK_GRAY,BLACK};

uint16_t fcolor;
uint16_t bcolor;
long disp_x_size, disp_y_size;

#define LCD_RST_LOW		HWREG(GPIO_PORTE_BASE + GPIO_O_DATA + (LCD_RST_PIN << 2)) = 0;
#define LCD_RST_HIGH	HWREG(GPIO_PORTE_BASE + GPIO_O_DATA + (LCD_RST_PIN << 2)) = LCD_RST_PIN;


#define s6d1121_delay(n) 	SysCtlDelay(n * ulClockMS)

#define LCD_CS_LOW		HWREG(GPIO_PORTE_BASE + GPIO_O_DATA + (LCD_CS_PIN << 2)) = 0;
#define LCD_CS_HIGH		HWREG(GPIO_PORTE_BASE + GPIO_O_DATA + (LCD_CS_PIN << 2)) = LCD_CS_PIN;


#define LCD_RS_LOW		HWREG(GPIO_PORTE_BASE + GPIO_O_DATA + (LCD_RS_PIN << 2)) = 0;
#define LCD_RS_HIGH		HWREG(GPIO_PORTE_BASE + GPIO_O_DATA + (LCD_RS_PIN << 2)) = LCD_RS_PIN;


#define LCD_RD_LOW		HWREG(GPIO_PORTE_BASE + GPIO_O_DATA + (LCD_RD_PIN << 2)) = 0;
#define LCD_RD_HIGH		HWREG(GPIO_PORTE_BASE + GPIO_O_DATA + (LCD_RD_PIN << 2)) = LCD_RD_PIN;


#define LCD_WR_LOW  	HWREG(GPIO_PORTE_BASE + GPIO_O_DATA + (LCD_WR_PIN << 2)) = 0;
#define LCD_WR_HIGH 	HWREG(GPIO_PORTE_BASE + GPIO_O_DATA + (LCD_WR_PIN << 2)) = LCD_WR_PIN;



static inline void lld_lcdwrite(uint16_t db)
{

	//GPIO_PORTB_DATA_R = (db>>8)&0xFF;
	HWREG(GPIO_PORTB_BASE + GPIO_O_DATA + (0xFF << 2)) = (db>>8);

	LCD_WR_LOW;
	LCD_WR_HIGH;


	//GPIO_PORTB_DATA_R = db&0xFF;
	HWREG(GPIO_PORTB_BASE + GPIO_O_DATA + (0xFF << 2)) = db;

	LCD_WR_LOW;
	LCD_WR_HIGH;


}

static inline void lld_lcdWriteIndex(uint16_t lcdReg) {
	LCD_RS_LOW;

	lld_lcdwrite(lcdReg);

	LCD_RS_HIGH;
}

static inline void lld_lcdWriteData(uint16_t lcdData) {
	lld_lcdwrite(lcdData);
}

static inline void lld_lcdWriteReg(uint16_t lcdReg, uint16_t lcdRegValue) {
	LCD_CS_LOW;

	lld_lcdWriteIndex(lcdReg);
	lld_lcdWriteData(lcdRegValue);

	LCD_CS_HIGH;
}

static __inline void lld_lcdWriteStreamStart(void) {
	LCD_CS_LOW;
	lld_lcdWriteIndex(0x0022);
}

static __inline void lld_lcdWriteStreamStop(void) {
	LCD_CS_HIGH;
}


void lld_lcdInit() {

	disp_x_size=239;
	disp_y_size=319;


	//
	// Get the current processor clock frequency.
	//
	ulClockMS = SysCtlClockGet() / (3 * 1000);


	ROM_SysCtlPeripheralEnable(LCD_DL_PERIPH);
	ROM_SysCtlPeripheralEnable(LCD_IO_PERIPH);


	ROM_GPIOPinTypeGPIOOutput(LCD_DL_GPIO, 0xff);
	ROM_GPIOPinTypeGPIOOutput(LCD_RST_GPIO, LCD_RST_PIN);

	//
	// Hardware Reset
	//
	LCD_RST_LOW;
	s6d1121_delay(15);
	LCD_RST_HIGH;
	s6d1121_delay(30);

	// IO Default Configurations
	ROM_GPIOPinTypeGPIOOutput(LCD_CS_GPIO, LCD_CS_PIN);
	ROM_GPIOPinTypeGPIOOutput(LCD_WR_GPIO, LCD_WR_PIN);
	ROM_GPIOPinTypeGPIOOutput(LCD_RD_GPIO, LCD_RD_PIN);
	ROM_GPIOPinTypeGPIOOutput(LCD_RS_GPIO, LCD_RS_PIN);


	LCD_CS_HIGH;
	LCD_RD_HIGH;
	LCD_WR_HIGH;

	s6d1121_delay(5);

	lld_lcdWriteReg(0x11,0x2004);
	lld_lcdWriteReg(0x13,0xCC00);
	lld_lcdWriteReg(0x15,0x2600);
	lld_lcdWriteReg(0x14,0x252A);
	lld_lcdWriteReg(0x12,0x0033);
	lld_lcdWriteReg(0x13,0xCC04);

	s6d1121_delay(5);

	lld_lcdWriteReg(0x13,0xCC06);

	s6d1121_delay(5);

	lld_lcdWriteReg(0x13,0xCC4F);

	s6d1121_delay(5);

	lld_lcdWriteReg(0x13,0x674F);
	lld_lcdWriteReg(0x11,0x2003);

	s6d1121_delay(5);

	// Gamma Setting
	lld_lcdWriteReg(0x30,0x2609);
	lld_lcdWriteReg(0x31,0x242C);
	lld_lcdWriteReg(0x32,0x1F23);
	lld_lcdWriteReg(0x33,0x2425);
	lld_lcdWriteReg(0x34,0x2226);
	lld_lcdWriteReg(0x35,0x2523);
	lld_lcdWriteReg(0x36,0x1C1A);
	lld_lcdWriteReg(0x37,0x131D);
	lld_lcdWriteReg(0x38,0x0B11);
	lld_lcdWriteReg(0x39,0x1210);
	lld_lcdWriteReg(0x3A,0x1315);
	lld_lcdWriteReg(0x3B,0x3619);
	lld_lcdWriteReg(0x3C,0x0D00);
	lld_lcdWriteReg(0x3D,0x000D);

	lld_lcdWriteReg(0x16,0x0007);
	lld_lcdWriteReg(0x02,0x0013);
	lld_lcdWriteReg(0x03,0x0003);
	lld_lcdWriteReg(0x01,0x0127);

	s6d1121_delay(5);

	lld_lcdWriteReg(0x08,0x0303);

	lld_lcdWriteReg(0x0A,0x000B);

	lld_lcdWriteReg(0x0B,0x0003);
	lld_lcdWriteReg(0x0C,0x0000);
	lld_lcdWriteReg(0x41,0x0000);
	lld_lcdWriteReg(0x50,0x0000);
	lld_lcdWriteReg(0x60,0x0005);
	lld_lcdWriteReg(0x70,0x000B);
	lld_lcdWriteReg(0x71,0x0000);
	lld_lcdWriteReg(0x78,0x0000);
	lld_lcdWriteReg(0x7A,0x0000);
	lld_lcdWriteReg(0x79,0x0007);
	lld_lcdWriteReg(0x07,0x0051);

	s6d1121_delay(5);

	lld_lcdWriteReg(0x07,0x0053);
	lld_lcdWriteReg(0x79,0x0000);

	lld_setColor(255, 255, 255);
	lld_setBackColor(0, 0, 0);

}

static inline void setXY(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{

	lld_lcdWriteReg(0x46,(x2 << 8) | x1);
	lld_lcdWriteReg(0x47,y2);
	lld_lcdWriteReg(0x48,y1);
	lld_lcdWriteReg(0x20,x1);
	lld_lcdWriteReg(0x21,y1);

}


inline void lld_lcdSetCursor(uint16_t x, uint16_t y) {

	lld_lcdWriteReg(0x20,x);
	lld_lcdWriteReg(0x21,y);

}

void lld_setColor(uint8_t r, uint8_t g, uint8_t b)
{
	uint8_t ch=((r&248)|g>>5);
	uint8_t cl=((g&28)<<3|b>>3);
	fcolor=(uint16_t)(ch<<8)|cl;
}


void lld_setBackColor(uint8_t r, uint8_t g, uint8_t b)
{
	uint8_t ch=((r&248)|g>>5);
	uint8_t cl=((g&28)<<3|b>>3);
	bcolor = (uint16_t)(ch<<8)|cl;
}


void drawGBLCD(int x , int y , uint8_t lcd[160][144]){
	int tx,ty;
	setXY(x, y, x+159, y+143);
	lld_lcdWriteStreamStart();
	for (ty=0; ty<144; ty++)
	{
		for (tx=0; tx<160; tx++){
			uint16_t color=0xFFFF;
			switch (lcd[tx][ty]){

				case WHITE:
					break;
				case LIGHT_GRAY:
					color=0xBDF7;
					break;
				case DARK_GRAY:
					color=0x5ACB;
					break;
				case BLACK:
					color = 0;
					break;
				default:
					break;
			}


			lld_lcdWriteData(color);
		}
	}
	lld_lcdWriteStreamStop();
}


void lld_fillRect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
	uint32_t index = 0, area;

	area = ((x1-x0)*(y1-y0));

    lld_lcdSetWindow(x0, y0, x1, y1);

    lld_lcdWriteStreamStart();

    for(index = 0; index < area; index++)
        lld_lcdWriteData(fcolor);

    lld_lcdWriteStreamStop();

    lld_lcdResetWindow();
}

void lld_lcdResetWindow() {
	setXY(0,0,disp_x_size,disp_y_size);
}

void lld_lcdSetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
	setXY(x0,y0,x1,y1);
}

void lld_lcdClear() {
    uint32_t index = 0;
    lld_lcdSetCursor(0, 0);
    lld_lcdWriteStreamStart();

    for(index = 0; index < (disp_x_size+1) * (disp_y_size+1); index++)
    	lld_lcdWriteData(bcolor);

    lld_lcdWriteStreamStop();
}
