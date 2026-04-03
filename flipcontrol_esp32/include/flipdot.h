#ifndef FLIPDOT_H
#define FLIPDOT_H

#include <Arduino.h>

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

/*
D7 - Ser (data)
D5 - clock
D1 - _OE
D2 - latch
D3 - _clear
*/

//Pins connected to Shift registers on own controller board
#define PIN_SR_DATA 13
#define PIN_SR_CLK 14
#define PIN_SR_OE 27 //active low
#define PIN_SR_LATCH 26

//Pins connected to stuff on annax driver board
#define PIN_DATA_DRVBRD 33
#define PIN_CLK_DRVBRD 32
#define PIN_RESET_DRVBRD 15
#define PIN_DRIVE 25 //enables 12v to panels via transistor
#define PIN_CLEAR 12 //connects CLEAR Pin from Annax board to GND (clears column)

#define COLUMNBYTES 29  //4 columns per byte. one panel has 25 columns.  (int)((numpanels*25)/4+1)


//### Timings ###
// Thight: //TODO optimize
#define MICROS_DRIVEDOTSET   5000 //time dot will be powered for flipping to bright side. in microseconds. 700 sometimes flips too late
#define MICROS_DRIVEDOTCLEAR 3300 //time dot will be powered for flipping to black. in microseconds. always flips at least a whole column. 10000 is too short. last value was 20000

#define MICROS_SHIFTDELAY  19 //shift register clock (100/2 = 1/(100/1000000) Hz). At 16 nothing is flipping!
#define MICROS_SHIFT_LATCH 50 //latch high time for 595 (row drivers)

/*
// These values worked before okay:
#define MICROS_DRIVEDOTSET 1500 //time dot will be powered for flipping to bright side. in microseconds. 700 sometimes flips too late
#define MICROS_DRIVEDOTCLEAR 30000 //time dot will be powered for flipping to black. in microseconds. always flips at least a whole column. 10000 is too short. last value was 20000

#define MICROS_SHIFTDELAY 50/2 //shift register clock (100/2 = 1/(100/1000000) Hz) .at 25/2 nothing is flipping!
#define MICROS_SHIFT_LATCH 100 //latch high time for 595 (row drivers)
*/

/*
//Slower values
#define MICROS_DRIVEDOTSET 5000 //time dot will be powered for flipping to bright side. in microseconds. 700 sometimes flips too late
#define MICROS_DRIVEDOTCLEAR 30000 //time dot will be powered for flipping to black. in microseconds. always flips at least a whole column. 10000 is too short. last value was 20000

#define MICROS_SHIFTDELAY 100/2 //shift register clock (100/2 = 1/(100/1000000) Hz) .at 25/2 nothing is flipping!
#define MICROS_SHIFT_LATCH 200 //latch high time for 595 (row drivers)
*/

class Flipdot
{
    
private:
    bool HBridgeOK();
    void resetColumns();
    void shiftOutSlow(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val);

    uint16_t row; //controls shift registers on own controller pcb

    uint8_t col[COLUMNBYTES]; //column drivers and shift registers on annax pcb

    bool rowdata_msbfirst;
    bool column_reversed;

    uint16_t driveDotSetDelayUs;
    uint16_t driveDotClearDelayUs;
    uint16_t shiftDelayUs;
    uint16_t shiftLatchTimeUs;

public:
    Flipdot();
    void init();

    bool clearSelectedColumn();
    bool setSelectedDot();

    void selectColumnClear(uint8_t selcolumn);
    void selectColumnSet(uint8_t selcolumn);
    void selectColumn(uint8_t selcolumn, bool clear);

    void setRow(uint16_t _row);
    uint16_t getRow();

    void shiftDataRow();
    void shiftDataColumn();

    void setDisplayMirror(bool x, bool y);

    void setDriveDotSetDelayUs(uint16_t delayUs);
    void setDriveDotClearDelayUs(uint16_t delayUs);
    void setShiftDelayUs(uint16_t delayUs);
    void setShiftLatchTimeUs(uint16_t timeUs);
};

#endif

