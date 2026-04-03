#include "flipdot.h"


Flipdot::Flipdot()
{  
}

void Flipdot::init()
{
  pinMode(PIN_SR_DATA, OUTPUT);
  pinMode(PIN_SR_CLK, OUTPUT);
  pinMode(PIN_SR_OE, OUTPUT);
  
  pinMode(PIN_SR_LATCH, OUTPUT);
  pinMode(PIN_CLEAR, OUTPUT);
  pinMode(PIN_DRIVE, OUTPUT);  

  pinMode(PIN_DATA_DRVBRD, OUTPUT);
  pinMode(PIN_CLK_DRVBRD, OUTPUT);
  pinMode(PIN_RESET_DRVBRD, OUTPUT);  
    
  digitalWrite(PIN_SR_OE, HIGH); //Active Low  
  digitalWrite(PIN_SR_LATCH, LOW);  
  digitalWrite(PIN_DRIVE, LOW);
  digitalWrite(PIN_RESET_DRVBRD,HIGH); //high = reset. set low to enable data flow to driverboard shift registers  

  driveDotSetDelayUs = MICROS_DRIVEDOTSET;
  driveDotClearDelayUs = MICROS_DRIVEDOTCLEAR;
  shiftDelayUs = MICROS_SHIFTDELAY;
  shiftLatchTimeUs = MICROS_SHIFT_LATCH;
}

void Flipdot::shiftOutSlow(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val)
{
	uint8_t i;

	for (i = 0; i < 8; i++)  
    {
		if (bitOrder == LSBFIRST)
			digitalWrite(dataPin, !!(val & (1 << i)));
		else	
			digitalWrite(dataPin, !!(val & (1 << (7 - i))));
			
        delayMicroseconds(shiftDelayUs);
		digitalWrite(clockPin, HIGH);
        delayMicroseconds(shiftDelayUs);
		digitalWrite(clockPin, LOW);
	}
}

void Flipdot::selectColumnClear(uint8_t selcolumn) 
{
    selectColumn(selcolumn, true); 
}

void Flipdot::selectColumnSet(uint8_t selcolumn) 
{
    selectColumn(selcolumn, false);
}

void Flipdot::selectColumn(uint8_t selcolumn, bool clear) 
{
    if (column_reversed) 
    {
        selcolumn=COLUMNBYTES*4-1-1 - selcolumn; //COLUMNBYTES*4-1 = COLUMNS
    }
    //set shift registers for columns to select one column to positive voltage
    uint8_t sc_bit = 3 - (selcolumn % 4); //each two shift registers control four columns
    uint8_t sc_byte = selcolumn / 4;

    resetColumns();

    col[sc_byte] = pow(2, (sc_bit*2 + clear)); // possible numbers for clear=false: 1,4,16,64
}

bool Flipdot::clearSelectedColumn() 
{
    shiftDataColumn();

    //Clear Columns

    for (uint8_t cc = 0; cc < COLUMNBYTES; cc++) 
    {
        //Serial.print("checking cc="); Serial.println(cc);
        for (uint8_t i = 0; i < 8; i += 2) 
        {
            if (CHECK_BIT(col[cc],i)) 
            {
                Serial.print("Error on bit ");
                Serial.print(i); Serial.print(" col="); Serial.println(cc);
                return 0; //a column is set to ground (should not be set for clear column)
            }
        }
    }

    digitalWrite(PIN_DRIVE, HIGH);
    digitalWrite(PIN_CLEAR, HIGH);
    delayMicroseconds(driveDotClearDelayUs);
    digitalWrite(PIN_CLEAR, LOW);
    digitalWrite(PIN_DRIVE, LOW);
    digitalWrite(PIN_RESET_DRVBRD,HIGH); //reset driverboard shift registers

    return 1;
}

bool Flipdot::setSelectedDot() 
{
    shiftDataRow();
    shiftDataColumn();
    
    for (uint8_t cc = 0; cc < COLUMNBYTES; cc++) 
    {
        //Serial.print("checking cc="); Serial.println(cc);
        for (uint8_t i = 1; i < 8; i += 2)
        {
            if (CHECK_BIT(col[cc], i)) 
            {
                Serial.print("Error on bit ");
                Serial.print(i); Serial.print(" col="); Serial.println(cc);
                return 0; //a column is set to ground (should not be set for clear column)
            }
        }
    }

    if (!HBridgeOK()) 
    {
        return 0;
    }

    digitalWrite(PIN_SR_OE, LOW); //Active Low. Enable Row
    digitalWrite(PIN_DRIVE, HIGH);
    delayMicroseconds(driveDotSetDelayUs); //Drive Dot
    digitalWrite(PIN_SR_OE, HIGH); //Active Low
    digitalWrite(PIN_DRIVE, LOW);
    digitalWrite(PIN_RESET_DRVBRD,HIGH); //reset driverboard shift registers

    return 1;
}

bool Flipdot::HBridgeOK() 
{
    for (uint8_t cc = 0; cc < COLUMNBYTES; cc++) 
    {
        //Serial.print("checking cc="); Serial.println(cc);
        for (uint8_t i = 0; i < 8; i += 2) 
        {
            if (CHECK_BIT(col[cc], i) && CHECK_BIT(col[cc], i+1)) 
            {
                Serial.print("Short circuit on bit ");
                Serial.print(i); Serial.print(" col="); Serial.println(cc);
                return 0; //a column is set to ground (should not be set for clear column)
            }
        }
    }

    return 1;
}


void Flipdot::shiftDataRow()   //send out all data to shift registers
{
    //select Rows via shift registers on own controller board
    //LSBFIRST= LSB is QH, bit 8 is QA. //upper byte 
    if (rowdata_msbfirst) 
    {
        shiftOutSlow(PIN_SR_DATA, PIN_SR_CLK, LSBFIRST, row & 0xff);  //lower byte
        shiftOutSlow(PIN_SR_DATA, PIN_SR_CLK, LSBFIRST, row >> 8);    //MSBFIRST= LSB is QH, bit 8 is QA
    }
    else
    {
        shiftOutSlow(PIN_SR_DATA, PIN_SR_CLK, MSBFIRST, row >> 8);    //MSBFIRST= LSB is QH, bit 8 is QA
        shiftOutSlow(PIN_SR_DATA, PIN_SR_CLK, MSBFIRST, row & 0xff);  //lower byte
    }

    digitalWrite(PIN_SR_LATCH, HIGH);
    delayMicroseconds(shiftLatchTimeUs);
    digitalWrite(PIN_SR_LATCH, LOW);
}

void Flipdot::shiftDataColumn()   //send out all data to shift registers
{ 
    digitalWrite(PIN_RESET_DRVBRD,LOW); //get driverboard shift registers out of reset

    //Select Columns via Shift registers
    for (uint8_t i = 0; i < COLUMNBYTES; i++) 
    {
        shiftOutSlow(PIN_DATA_DRVBRD, PIN_CLK_DRVBRD, LSBFIRST, col[COLUMNBYTES-1-i]); //asdf previously col·[6-i]
    }
}

void Flipdot::resetColumns() 
{
    for (uint8_t i = 0; i < COLUMNBYTES; i++) 
    {
        col[i]=0;
    }
}

void Flipdot::setRow(uint16_t _row)
{
    row = _row; //data for one column
}

uint16_t Flipdot::getRow() 
{
    return row;
}

void Flipdot::setDisplayMirror(bool x, bool y)
{
    rowdata_msbfirst = y; //false=not mirrored (connector at top)
    column_reversed = x;
}

void Flipdot::setDriveDotSetDelayUs(uint16_t delayUs)
{
    driveDotSetDelayUs = delayUs;
}

void Flipdot::setDriveDotClearDelayUs(uint16_t delayUs)
{
    driveDotClearDelayUs = delayUs;
}

void Flipdot::setShiftDelayUs(uint16_t delayUs)
{
    shiftDelayUs = delayUs;
}

void Flipdot::setShiftLatchTimeUs(uint16_t timeUs)
{
    shiftLatchTimeUs = timeUs;
}