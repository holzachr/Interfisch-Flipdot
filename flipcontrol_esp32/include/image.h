#ifndef IMAGE_H
#define IMAGE_H

#include <Arduino.h>

#include "flipdot.h"
#include <string.h>


#define COLUMNS 115
#define ROWS 16


enum UpdateReturn {
    wait,
    finished,
    nochange,
    updating
};

class Image
{
    
private: 

    //buffer is 16 bit because of 16 Rows
    uint16_t frontBuffer[COLUMNS];  //1 is bright dot / set dot. 0 is black / cleared
    uint16_t backBuffer[COLUMNS]; //least significant bit is bottom dot.

    bool flag_updating; //when true, display flip is in progress. frontBuffer does not match backBuffer
    bool flag_redraw; //when set true, settings for updating are overwritten such that all dots are cleared and set

    uint8_t update_counter; //used for keeping track of progress for updating

    int countz=0;

    unsigned long lastUpdateMillis; //time when last dots where started flipping

    unsigned long updateDelay;

    uint8_t orderArray[COLUMNS];

    void serialPrintInt(uint16_t source);

    bool isBuffersEqual();
    
public:
    Image();
    void init();

    Flipdot flipdot;

    UpdateReturn updateByColumn(bool clearFirst, bool optimizeClear, bool optimizeSet);

    uint8_t getW(); //returns Columns
    uint8_t getH(); //returns Rows

    void resetOrder(bool ascending);
    void shuffleOrder(uint8_t iterations);

    void setBuffer_solid(bool set);
    void setBufferColumn(uint8_t _colnum, uint16_t _rowdata);
    void setBuffer_random(uint8_t brightness);
    void setBuffer_byString(String data,String& error);
    void setBuffer_byInt(String data,String& error);

    void addBuffer_text(String text,uint8_t xoffset, uint8_t yoffset, uint8_t pfont=0);

    void setBuffer_Preset_Stripes();
    void setBuffer_Preset_Checkers();
    void setBuffer_Preset_CheckersInverse();

    void redraw();

    void setDisplayMirror(bool x, bool y);    

    unsigned long updateDuration; //for statistics and debugging. time it took for one update (max)
};


#endif

