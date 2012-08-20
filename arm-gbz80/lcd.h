#ifndef LCD_H
#define LCD_H

#include "types.h"

typedef struct _LCD{
    int scanlinecyclecounter;
    uint8 display[160][144];
}LCD;

enum {WHITE,LIGHT_GRAY,DARK_GRAY,BLACK};

//inline void drawScanline();
void drawBG();
void drawSprites();

#endif // LCD_H
