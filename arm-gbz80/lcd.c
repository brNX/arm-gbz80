#include "lcd.h"
#include "Z80.h"


////mode 0 - BGP FF47
////mode 1 - OBP0 FF48
////mode 2 - OBP1 FF49
inline static unsigned int getColor(int number,int mode){

    unsigned int colorindex=0;
    unsigned int palette=BGP;

    int hi=1,lo=0;



    switch(number)
    {
        case 0: hi = 1<<1 ; lo = 1<<0 ;break ;
        case 1: hi = 1<<3 ; lo = 1<<2 ;break ;
        case 2: hi = 1<<5 ; lo = 1<<4 ;break ;
        case 3: hi = 1<<7 ; lo = 1<<6 ;break ;
    }

    switch(mode)
    {
        case 0: palette=BGP ;  break;
        case 1: palette=OBP0;  break;
        case 2: palette=OBP1;  break;
    }

    if (palette & hi)
    	colorindex = 0x2;
    if (palette & lo)
    	colorindex |= 0x1;

    return colorindex;

}

void drawBG(){

    int i;
    int useWindow = 0;
    uint8 yPos,xPos;
    uint16 rowPos=0,colPos;
    uint16 tileAddress;
    uint16 backgroundAddress = 0;
    const unsigned int LCDC_n=LCDC,LY_n=LY;


    //window enabled and scanline within window ?
    if ((LCDC_n & (1<<5)) && ( WY <= LY_n ))
    {
        yPos=LY_n-WY;

        //Window Tile Map Display Select
        if(LCDC_n & (1<<6))
            //0x9c00
            backgroundAddress=0x1C00;
        else
            //0x9800
            backgroundAddress=0x1800;

        useWindow=1;
    }
    else //not using window
    {
        yPos=SCY+LY_n;

        //Window Tile Map Display Select
        if(LCDC_n & (1<<3))
            //0x9c00
            backgroundAddress=0x1C00;
        else
            //0x9800
            backgroundAddress=0x1800;
    }



    //rowPos o current scanline (of the 8 pixels)
    rowPos = ((yPos>>3)&0xFF)<<5;


    const unsigned int WX_n=WX;
    const unsigned int SCX_n=SCX;
    //draw de 160 pixels in current line  TODO: (4 by 4)
    for (i=0;i<160;i++){

        uint8 line;
        uint8 data1;
        uint8 data2;
        int colorBit;
        int colorNumber;
        unsigned int color;

        xPos = i+SCX_n;

        if(useWindow){
            uint8 rWX = WX_n -7;
            if (i >= rWX)
                 xPos = i - rWX;
        }

        //divide by 8
        colPos=(xPos>>3);


        // get the tile identity number
        // which tile data are we using?
        if (LCDC_n & (1<<4))
        {
            uint8 tilenumber;
            tileAddress = 0x0 ;
            tilenumber= gbcpu.mem->vram[backgroundAddress+rowPos+colPos];
            tileAddress+=(tilenumber*16);
        }
        else
        {
            int8 tilenumber;
            tileAddress = 0x800 ;
            tilenumber= gbcpu.mem->vram[backgroundAddress+rowPos+colPos];
            tileAddress+=((tilenumber+128)*16);
        }

        // each vertical line takes up two bytes of memory
        line = (yPos % 8) <<1;

        data1=gbcpu.mem->vram[tileAddress+line];
        data2=gbcpu.mem->vram[tileAddress+line+1];

        colorBit = ((xPos % 8)-7)*-1;

        // combine data 2 and data 1 to get the colour id for this pixel
        colorNumber = (data2 & (1<<colorBit))?0x2:0;
        colorNumber |= (data1 & (1<<colorBit))?1:0;


        //finaly get color from palette and draw
        color = getColor(colorNumber,0);

        //draw
        gbcpu.lcd->display[i][LY_n]=color;

    }

}



void drawSprites(){

    int i,sizeY;

    //printf("printsprites at %04x\n",PC);

    //loop throught the 40 sprites
    const unsigned int LCDC_n=LCDC,LY_n=LY;
    for(i=0;i<40;i++){

        //4 bytes in OAM (Sprite attribute table)
        unsigned int index = i*4;
        unsigned int posY = gbcpu.mem->OAM[index]-16;
        unsigned int posX = gbcpu.mem->OAM[index+1]-8;
        unsigned int tileLocation = gbcpu.mem->OAM[index+2];
        unsigned int attributes = gbcpu.mem->OAM[index+3];

        //check y - Size in LCDC
        if (LCDC_n & 0x4)
          sizeY = 16;
        else
          sizeY = 8;


        //check if sprite is in current Scanline
        if ((LY_n >= posY) && (LY_n < (posY+sizeY)))
        {
            int pixel;
            uint8 data1,data2;
            uint16 dataaddress;
            int line = LY_n - posY ;


            //flip y-axis ?
            if (attributes & 0x40)
                line = -(line - sizeY);

            // each vertical line takes up two bytes of memory
            line *=2;

            //vram (0x8000 +(tileLocation*16))+line
            dataaddress=(tileLocation*16) +line;
            data1=gbcpu.mem->vram[dataaddress];
            data2=gbcpu.mem->vram[dataaddress+1];

            for(pixel=7;pixel>=0;pixel--){

            	unsigned int color;
                int colorNumber;
                int colorBit=pixel;

                int pos= 7-pixel+posX;
                if(pos > 159 || pos < 0)
                	continue;

                 //flip x-axis ?
                if (attributes & 0x20)
                    colorBit = (colorBit -7)*-1;

                // combine data 2 and data 1 to get the colour id for this pixel
                colorNumber = (data2 & (1<<colorBit))?0x2:0;
                colorNumber |= (data1 & (1<<colorBit))?1:0;

                //get color from palette
                color = getColor(colorNumber, ((attributes & 0x10)>>4)+1);

                //white = transparent for sprites
                if (color==WHITE)
                    continue;

                //dont' draw if behind backgound
                if(!(attributes & (1<<7)) || (gbcpu.lcd->display[pos][LY_n] == WHITE)){
                	gbcpu.lcd->display[pos][LY_n]=color;
                }

            }
        }
    }
}



