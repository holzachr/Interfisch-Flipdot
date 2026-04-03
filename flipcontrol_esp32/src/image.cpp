#include "image.h"

#include "fonts.h"

Image::Image()
{    
}

void Image::serialPrintInt(uint16_t source)
{
    for(uint8_t i = 0; i < 16; i++)
        Serial.print((source & (1 << i)) > 0);
}

void Image::init()
{
    flipdot.init(); //sets pin modes etc.

    flag_updating=false;
    update_counter=0;
    updateDelay=10;
    
    resetOrder(true);
}

uint8_t Image::getW() 
{
    return COLUMNS;
}

uint8_t Image::getH() 
{
    return ROWS;
}

void Image::setBuffer_solid(bool set)
{
    for (uint8_t x = 0; x < getW(); x++) 
    {
        if (set) 
            backBuffer[x]=0xffff; //all white
        else
            backBuffer[x]=0x0; //all black
    }

    flag_updating = true; //make update run
}

void Image::addBuffer_text(String text, uint8_t xoffset, uint8_t yoffset, uint8_t pfont)
{
    uint8_t font_width;
    uint8_t font_spacing;
    switch (pfont){
        case 0:
            font_width=font_normal_width;
            if (xoffset=255) { //centering?
                xoffset=(COLUMNS-text.length()*(font_normal_mean_width+font_spacing))/2;
            }
            break;
        case 1:
            font_width=font_large_width;
            if (xoffset=255) { //centering?
                xoffset=(COLUMNS-text.length()*(font_large_mean_width+font_spacing))/2;
            }
            break;
    }

    uint8_t bufferxpos=xoffset;
    for (uint8_t textpos=0;textpos<text.length();textpos++){
        
        char currentchar=text.charAt(textpos);
        Serial.print("Current Char "); Serial.print((uint8_t)currentchar); Serial.print(":"); Serial.println(currentchar);
        
        uint8_t emptycolumncounter=0; //count how many empty trailing columns for a character
        for (uint8_t x=0;x<font_width;x++) {
            uint16_t addBuffer;
            if (currentchar==' ') { //space
                addBuffer=0; //empty
            }else{
                switch (pfont) {
                    case 0:
                        addBuffer=( (uint16_t)( font_normal[(uint8_t)currentchar-font_offset][x] ) << yoffset);
                        break;
                    case 1:
                        addBuffer=( (uint16_t)( font_large[(uint8_t)currentchar-font_offset][x] ) << yoffset);
                        break;
                }

                if (addBuffer==0) {
                    emptycolumncounter++;
                }else{
                    emptycolumncounter=0; //reset
                }
            }
            backBuffer[bufferxpos]|= addBuffer;
            Serial.print("Buffer at "); Serial.print(bufferxpos); Serial.print(" add ");Serial.print(addBuffer,2); Serial.print(" result="); Serial.println(backBuffer[bufferxpos],2);

            bufferxpos++;
        }
        bufferxpos-= emptycolumncounter-font_spacing; //remove trailing empty columns except one
    }
    Serial.println("Finished Text");
}


void Image::setBuffer_Preset_Stripes()
{
    for (int i = 0; i < getW(); i++)
    {
        backBuffer[i] = (1 << (i % 16));
    }    

    flag_updating = true; //make update run
}

void Image::setBuffer_Preset_Checkers()
{
    for (int i = 0; i < getW(); i++)
    {
        if (i % 2)
            backBuffer[i] = 0xAAAA;
        else
            backBuffer[i] = 0x5555;
    }    

    flag_updating = true; //make update run
}


void Image::setBuffer_Preset_Datamatrixctdo() //Datamatrix ctdo.de
{    
    backBuffer[0]=0b1111111111111111;
    backBuffer[1]=0b1111111111111111;
    backBuffer[2]=0b1111111111111111;
    backBuffer[3]=0b1111111111111111;
    backBuffer[4]=0b1111111111111111;
    backBuffer[5]=0b1111111111111111;
    backBuffer[6]=0b1111111111111111;
    backBuffer[7]=0b1111111111111111;
    backBuffer[8]=0b1111111111111111;
    backBuffer[9]=0b1111111111111111;
    backBuffer[10]=0b1111111111111111;
    backBuffer[11]=0b1111111111111111;
    backBuffer[12]=0b1111111111111111;
    backBuffer[13]=0b1111111111111111;
    backBuffer[14]=0b1111111111111111;
    backBuffer[15]=0b1111111111111111;
    backBuffer[16]=0b1111111111111111;
    backBuffer[17]=0b1111111111111111;
    backBuffer[18]=0b1111111111111111;
    backBuffer[19]=0b1111111111111111;
    backBuffer[20]=0b1111111111111111;
    backBuffer[21]=0b1111111111111111;
    backBuffer[22]=0b1111111111111111;
    backBuffer[23]=0b1111111111111111;
    backBuffer[24]=0b1111111111111111;
    backBuffer[25]=0b1111111111111111;
    backBuffer[26]=0b1111111111111111;
    backBuffer[27]=0b1111111111111111;
    backBuffer[28]=0b1111111111111111;
    backBuffer[29]=0b1111111111111111;
    backBuffer[30]=0b1111111111111111;
    backBuffer[31]=0b1000000000000001;
    backBuffer[32]=0b1000100101100111;
    backBuffer[33]=0b1011111100110001;
    backBuffer[34]=0b1010110011001011;
    backBuffer[35]=0b1011010001001101;
    backBuffer[36]=0b1000000001011011;
    backBuffer[37]=0b1001110110001001;
    backBuffer[38]=0b1001101101011011;
    backBuffer[39]=0b1010011100011101;
    backBuffer[40]=0b1010111111001111;
    backBuffer[41]=0b1010110101110001;
    backBuffer[42]=0b1001100110110111;
    backBuffer[43]=0b1000111010011101;
    backBuffer[44]=0b1010101010101011;
    backBuffer[45]=0b1111111111111111;
    backBuffer[46]=0b1111111111111111;
    backBuffer[47]=0b1111111111111111;
    backBuffer[48]=0b1111111111111111;
    backBuffer[49]=0b1111111111111111;
    backBuffer[50]=0b1111111111111111;
    backBuffer[51]=0b1111111111111111;
    backBuffer[52]=0b1111111111111111;
    backBuffer[53]=0b1111111111111111;
    backBuffer[54]=0b1111111111111111;
    backBuffer[55]=0b1111111111111111;
    backBuffer[56]=0b1111111111111111;
    backBuffer[57]=0b1111111111111111;
    backBuffer[58]=0b1111111111111111;
    backBuffer[59]=0b1111111111111111;
    backBuffer[60]=0b1111111111111111;
    backBuffer[61]=0b1111111111111111;
    backBuffer[62]=0b1111111111111111;
    backBuffer[63]=0b1111111111111111;
    backBuffer[64]=0b1111111111111111;
    backBuffer[65]=0b1111111111111111;
    backBuffer[66]=0b1111111111111111;
    backBuffer[67]=0b1111111111111111;
    backBuffer[68]=0b1111111111111111;
    backBuffer[69]=0b1111111111111111;
    backBuffer[70]=0b1111111111111111;
    backBuffer[71]=0b1111111111111111;
    backBuffer[72]=0b1111111111111111;
    backBuffer[73]=0b1111111111111111;
    backBuffer[74]=0b1111111111111111;

    flag_updating=true; //make update run
}

void Image::setBufferColumn(uint8_t _colnum, uint16_t _rowdata)
{
    if (_colnum >= COLUMNS) 
    {
        Serial.println("colnum too high");
    }
    else
    {
        backBuffer[_colnum]=_rowdata; //all white

        flag_updating = true; //make update run
    }
}

void Image::setBuffer_random(uint8_t brightness)
{
    for (uint8_t x = 0; x < getW(); x++) 
    {
        uint16_t randomnumber = 0;
        
        for (uint8_t y = 0; y < getH(); y++) 
        {
            if (random(256) < brightness)  //the higher the parameter the more white
                randomnumber += pow(2, y);
        }

        backBuffer[x] = randomnumber;
    }

    flag_updating=true; //make update run
}


void Image::setBuffer_byString(String data, String& error)
{
    //first check size
    if (data.length() != getW() * getH())  //if characters matches pixels then its likely binary
        error="setBuffer_byString: size mismatch";

    for (uint8_t x = 0; x < getW(); x++) 
    {
        uint16_t columnnumber = 0;
        for (uint8_t y = 0; y < getH(); y++) 
        {
            char currentChar = data.charAt(x * getH() + y);
            if (currentChar != '0' && currentChar != ' ' && currentChar != '_') { //0, Space or "_" are black pixels.
                columnnumber += pow(2, y); //if not one of the above, then make white
            }            
        }
        backBuffer[x]=columnnumber;  
    }

    flag_updating=true; //make update run    
}

void Image::setBuffer_byInt(String data,String& error)
{
    int str_len = data.length() + 1; 
    char char_array[str_len];
    data.toCharArray(char_array, str_len);

    char *token = strtok(char_array, ",");

    uint8_t x=0;
    while (token != NULL && x<getW()) {
        String tokenString(token);
        
        uint16_t tokenInt=tokenString.toInt();
        
        backBuffer[x]=tokenInt;

        token = strtok(NULL, ","); //get next token
        x++;
    }
    if (x!=getW()) { //x reached exaclty the end
        Serial.println("Warning: Not Enough Data!");
        error="setBuffer_byInt: not enough data. x="+x;
    }
    if (token!= NULL) { //exited loop but there was another valid token
        Serial.println("Warning: too much data");
        error="setBuffer_byInt: too much data";
    }

    flag_updating=true; //make update run    
}


void Image::resetOrder(bool ascending) {
    for (uint8_t i=0;i<getW();i++) { //fill with ascending numbers
        if (ascending) {
            orderArray[i]=i;
        }else{
            orderArray[i]=(getW()-1) - i;
        }
    }
}

void Image::shuffleOrder(uint8_t iterations) 
{
    for (uint8_t s=0;s<iterations;s++) { //shuffle iterations
        uint8_t _r1=random(getW());
        uint8_t _r2=random(getW());
        uint8_t _backup=orderArray[_r1];
        //swap numbers
        orderArray[_r1]=orderArray[_r2];
        orderArray[_r2]=_backup; 
    }

    for (uint8_t i=0;i<getW();i++) { //fill with ascending numbers
        Serial.print(orderArray[i]); Serial.print(" ");
    }
    Serial.println();
}

UpdateReturn Image::updateByColumn(bool clearFirst, bool optimizeClear, bool optimizeSet)
{
    /*
    clearFirst : if True, go through all columns and clear them (by set order). Then go through same order and set dots. If False, clear one column then set it immediately afterwards.
    optimizeClear : if True, do not clear columns where dots need to be set or no change is happening. if False, clear every column
    optimizeSet : if True, do not set already set dots again.
    */

    if (flag_redraw) {
        clearFirst=false;
        optimizeClear=false;
        optimizeSet=false;
    }

    if (!flag_updating) {
        return nochange; //finished
    }

    if (millis()-lastUpdateMillis<updateDelay){ //too early
        return wait; //not finished
    }

    unsigned long functionStartMillis=millis();
    

    uint8_t x=update_counter/2;
    bool setDot = (update_counter%2==1);

    if (clearFirst) {
        x=update_counter%getW();
        setDot = !(update_counter/getW() == 0);
    }

    x = orderArray[x]; //use custom order

    if (!setDot) { //even counter numbers are for clearing
        uint16_t _colClear=frontBuffer[x]& ~backBuffer[x]; //Front & ~Back = Which bits have to be cleared

        if (!optimizeClear || _colClear>0) { //at least on dot has to be cleared in this column
            flipdot.setRow(0);
            flipdot.selectColumnClear(x);

            if (!flipdot.clearSelectedColumn()) {
                Serial.println("Error clearing column!");
            }
            lastUpdateMillis=millis();
            frontBuffer[x]=0;
        }

    }else{ //odd counter numbers are for setting
        uint16_t backBuffer_x=backBuffer[x]; //store buffer, because it may change when different data arrives during setting dots
        uint16_t _colSet=backBuffer_x& ~frontBuffer[x];
        if (!optimizeSet || _colSet>0) { //at least on dot has to be set in this column (if optimize is enabled)
            flipdot.selectColumnSet(x); //lower column number is on the left
            if (!optimizeSet) {
                flipdot.setRow(backBuffer_x); //flip all set dots in this column
            }else{           
                flipdot.setRow(_colSet); //flip only currently not set dots in this column that have to be set
            }

            if (!flipdot.setSelectedDot()) {
                Serial.println("Error setting dots!");
            }
            lastUpdateMillis=millis();

            frontBuffer[x]=backBuffer_x; //flip current column in buffer
        }
    }

    update_counter++; //next column

    unsigned long _duration=millis()-functionStartMillis; //how long it took to shift data and flip dots in this update
    updateDuration=max(updateDuration,_duration); //store maximum value
    

    if (update_counter/2>=getW()) { //reached last column
        flag_updating=false;
        update_counter=0;
        flag_redraw=false;
        if (!isBuffersEqual()) {
            flag_updating=true; //update again if buffers not equal
            return updating;
        }
        
        return finished; //finished
    }
    return updating; //not finished
}

bool Image::isBuffersEqual() {
    for (uint8_t x=0;x<COLUMNS;x++) {
        if (frontBuffer[x]!=backBuffer[x]) {
            return false;
        }
    }
    return true;
}

void Image::loop_testDots() {

    static bool init=false;
    if (!init) {
        flipdot.setRow(0);
        Serial.println("Clearing Display");
        for (int l=0;l<COLUMNS;l++) {
            flipdot.selectColumnClear(l%COLUMNS);


            if (!flipdot.clearSelectedColumn()) {
                Serial.println("Error clearing column!");
            }else{
                Serial.println("Cleared");
            }

            delay(50);
        }
        init=true;
        Serial.println("Finished Clearing");
        delay(1000);
    }
    

    

    Serial.println("Clearing");
    flipdot.setRow(0);
    flipdot.selectColumnClear(23);
    if (!flipdot.clearSelectedColumn()) {
        Serial.println("Error clearing column!");
    }
    delay(50);

    flipdot.setRow(0);
    flipdot.selectColumnClear(24);
    if (!flipdot.clearSelectedColumn()) {
        Serial.println("Error clearing column!");
    }
    delay(100);
    

    Serial.println("Setting");
    for (int i=23;i<25;i++) {
        flipdot.selectColumnSet(i); //lower column number is on the left
        
        flipdot.setRow(0);
        flipdot.setRow(flipdot.getRow()+pow(2, 4));//low significant bits are lower rows (when connector at top)
        flipdot.setRow(flipdot.getRow()+pow(2, 5));//low significant bits are lower rows (when connector at top)
        flipdot.setSelectedDot();
        delay(50);
    }

    delay(100);

    countz++;
    countz%=14;

    //init=false;
}


void Image::loop_drawClearTest() {

    static bool init=false;
    if (!init) {
        flipdot.setRow(0);
        Serial.println("Clearing Display");
        for (int l=0;l<COLUMNS;l++) {
            flipdot.selectColumnClear(l%COLUMNS);

            if (!flipdot.clearSelectedColumn()) {
                Serial.println("Error clearing column!");
            }else{
                Serial.println("Cleared");
            }

            delay(20);
        }
        init=true;
        delay(1000);
    }
    


    
    Serial.print("count=");
    Serial.print(countz);Serial.print(": ");

    //setting colX to 128, 32, 8,2 (or a combination of), then appling 12V to driver and GND to Clear, clears these colums
        // this applies +12v to selected columns
    //setting colX to 64,16,4,1 (or a combination of), then setting row shift registers to some setting sets the selected dots
        // this applies GND to selected columns
    //reset pin on annax board input should be used (not pulled to gnd for a short time) after dots have been flipped (to disable potentially activated transistors)


    //cycle testing set dots        
    flipdot.selectColumnSet(countz/ROWS); //lower column number is on the left
    flipdot.setRow(pow(2, (countz)%ROWS));//low significant bits are lower rows (when connector at top)

    
    
        
    /*Serial.print("Row="); Serial.print(row); Serial.print(" Col=");
    for (uint8_t i=0;i<7;i++) {
        Serial.print(","); Serial.print(col[i]);
    }
    Serial.println();*/
    //reset pin on ribbon cable high (12Vpullup/open), then low (via Transistor)

    unsigned long starttime=micros();

    flipdot.setSelectedDot();


    unsigned long shiftduration=micros()-starttime;
    Serial.println("");
    Serial.print("Duration="); Serial.println(shiftduration);


    
    countz++;

    if (countz>=ROWS*COLUMNS) {
        countz=0;
        init=false;
    }

}


void Image::redraw() {
    flag_redraw=true;
    flag_updating=true;
}


void Image::setDisplayMirror(bool x, bool y){
    flipdot.setDisplayMirror(x,y);
}