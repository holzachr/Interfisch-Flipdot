#include <Arduino.h>

#include <Homie.h>

HomieNode displayNode("display", "Display", "display");
HomieNode settingsNode("settings", "Settings", "settings");
HomieNode resultNode("result", "Result", "result");

#include "flipdot.h"
#include "image.h"


Image flip;

unsigned long loopmillis=0;
unsigned long last_update=0;

bool clearFirst=true;
bool optimizeClear=false;
bool optimizeSet=false;

uint8_t stringToBool(String s);

bool presetHandler(const HomieRange& range, const String& value);
bool orderHandler(const HomieRange& range, const String& value);
bool dataHandler(const HomieRange& range, const String& value);
bool controlHandler(const HomieRange& range, const String& value);
bool textHandler(const HomieRange& range, const String& value) ;
bool textLargeHandler(const HomieRange& range, const String& value) ;

bool clearFirstHandler(const HomieRange& range, const String& value);
bool optimizeClearHandler(const HomieRange& range, const String& value);
bool optimizeSetHandler(const HomieRange& range, const String& value);
bool driveDotSetDelayUsHandler(const HomieRange& range, const String& value);
bool driveDotClearDelayUsHandler(const HomieRange& range, const String& value);
bool shiftDelayUsHandler(const HomieRange& range, const String& value);
bool shiftLatchTimeUsHandler(const HomieRange& range, const String& value);

String error; //used to buffer error messages to be send via mqtt

void setup() {
    flip.init();
    flip.setDisplayMirror(false, false); //true,true=mirror both axis (rotate 180deg)

    Serial.begin(115200);

    // Setup Homie
    Homie_setFirmware("flipdot", "0.1.2");
    displayNode.advertise("preset").settable(presetHandler);
    displayNode.advertise("data").settable(dataHandler);
    displayNode.advertise("order").settable(orderHandler);
    displayNode.advertise("control").settable(controlHandler);
    displayNode.advertise("text").settable(textHandler);
    displayNode.advertise("textlarge").settable(textLargeHandler);
    settingsNode.advertise("clearfirst").settable(clearFirstHandler);
    settingsNode.advertise("optimizeclear").settable(optimizeClearHandler);
    settingsNode.advertise("optimizeset").settable(optimizeSetHandler);
    settingsNode.advertise("driveDotSetDelayUs").settable(driveDotSetDelayUsHandler);
    settingsNode.advertise("driveDotClearDelayUs").settable(driveDotClearDelayUsHandler);
    settingsNode.advertise("shiftDelayUs").settable(shiftDelayUsHandler);
    settingsNode.advertise("shiftLatchTimeUs").settable(shiftLatchTimeUsHandler);
   
    Homie.setup();

    error="";
}


void loop() {
    Homie.loop();
    loopmillis = millis();
    
    static unsigned long last_change=0;

    UpdateReturn result = flip.updateByColumn(clearFirst, optimizeClear, optimizeSet); //0=not finished, 1=finished
    //UpdateReturn result=flip.updateByColumn(true,false,false); //0=not finished, 1=finished   <- most simple
    static UpdateReturn last_result = finished;
    if (last_result == nochange && result != last_result) { //was finished but has just started updating again
        last_change = loopmillis; //display started changing dots. set starting time.
    }
    last_result = result;
    if (result == finished) //just finished
    {
        unsigned long duration=millis()-last_change;
        Serial.print("Last Change took "); Serial.print(duration); Serial.println(" ms");
        Serial.print("Update max took "); Serial.print(flip.updateDuration); Serial.println(" ms");
        resultNode.setProperty("duration").send((String)duration);
        resultNode.setProperty("updateduration").send((String)flip.updateDuration);
        flip.updateDuration=0; //reset
    }

    if (error.length()>0) { //is there an error to send?
        resultNode.setProperty("error").send(error);
        error=""; //clear
    }    
}


bool orderHandler(const HomieRange& range, const String& value) 
{
    Serial.print("Order payload="); Serial.println(value);

    if (value=="ascending") 
        flip.resetOrder(true);
    else if (value=="descending") 
        flip.resetOrder(false);
    else if (value=="shuffle")
        flip.shuffleOrder(8);
    else if (value=="random")
        flip.shuffleOrder(flip.getW());
    else
        return false;

    displayNode.setProperty("order").send(value);
    return true;    
}

bool presetHandler(const HomieRange& range, const String& value) 
{
    Serial.print("Preset payload="); Serial.println(value);

    if (value == "white"){
        flip.setBuffer_solid(1);
        Homie.getLogger() << "Preset is White" << endl;
    }else if(value == "black"){
        flip.setBuffer_solid(0);
        Homie.getLogger() << "Preset is Black" << endl;
    }else if(value == "stripes"){
        flip.setBuffer_Preset_Stripes();
        Homie.getLogger() << "Preset is stripes" << endl;
    }else if(value == "checkers"){
        flip.setBuffer_Preset_Checkers();
        Homie.getLogger() << "Preset is checkers" << endl;
    }else if(value == "datamatrixctdo"){
        flip.setBuffer_Preset_Datamatrixctdo();
        Homie.getLogger() << "Preset is datamatrixctdo" << endl;
    }else if(value == "random"){
        flip.setBuffer_random(50);
    }else if(value == "uptime") {
        flip.setBuffer_solid(0);
        flip.addBuffer_text((String)(millis()/1000),0,0);
    }else{
        error="preset \""+value+"\" not found";
        return false;
    }

    displayNode.setProperty("preset").send(value);

    return true;
}

bool dataHandler(const HomieRange& range, const String& value) 
{
    Serial.print("Data Received: "); Serial.println(value);
    Serial.print("Length: "); Serial.println(value.length());
    /*payload can be either be Binary/String:
     "10101101011010.." 
     "1  111 11 11 1 1..."
     "a_aa_aa_aa_aa__a_a_a.."

     Or Int (unsigned 16 bit. comma separated for each column)
     "837491,12347,592,920,114,0,12,45125,..."
    */
    if (value.length() == flip.getW() * flip.getH()) { //if characters matches pixels then its likely binary
        Serial.println("Using byString");
        flip.setBuffer_byString(value,error);
    }else{
        Serial.println("Using byInt");
        flip.setBuffer_byInt(value,error);
    }    
    
    return true;   
}

bool clearFirstHandler(const HomieRange& range, const String& value) 
{
    Serial.print("clearFirstHandler payload="); Serial.println(value);

    uint8_t res=stringToBool(value);
    if (res==0)
        clearFirst=false;
    else if(res==1)
        clearFirst=true;
    else
    {
        error="no valid input";
        return false;
    }

    settingsNode.setProperty("clearfirst").send((String)res);

    return true;
}

bool optimizeClearHandler(const HomieRange& range, const String& value) {
    Serial.print("OptimizeClearHandler payload="); Serial.println(value);
    uint8_t res=stringToBool(value);
    if (res==0){
        optimizeClear=false;
    }else if(res==1){
        optimizeClear=true;
    }else{
        error="no valid input";
        return false;
    }
    settingsNode.setProperty("optimizeclear").send((String)res);
    return true;
}

bool optimizeSetHandler(const HomieRange& range, const String& value) {
    Serial.print("OptimizeSetHandler payload="); Serial.println(value);
    uint8_t res=stringToBool(value);
    if (res==0){
        optimizeSet=false;
    }else if(res==1){
        optimizeSet=true;
    }else{
        error="no valid input";
        return false;
    }
    settingsNode.setProperty("optimizeset").send((String)res);
    return true;
}

bool driveDotSetDelayUsHandler(const HomieRange& range, const String& value) {
    Serial.print("driveDotSetDelayUsHandler payload="); Serial.println(value);
    uint16_t res = value.toInt();
    flip.flipdot.setDriveDotSetDelayUs(res);
    settingsNode.setProperty("driveDotSetDelayUs").send((String)res);
    return true;
}

bool driveDotClearDelayUsHandler(const HomieRange& range, const String& value) {
    Serial.print("driveDotClearDelayUsHandler payload="); Serial.println(value);
    uint16_t res = value.toInt();
    flip.flipdot.setDriveDotClearDelayUs(res);
    settingsNode.setProperty("driveDotClearDelayUs").send((String)res);
    return true;
}

bool shiftDelayUsHandler(const HomieRange& range, const String& value) {
    Serial.print("driveDotClearDelayUsHandler payload="); Serial.println(value);
    uint16_t res = value.toInt();
    flip.flipdot.setShiftDelayUs(res);
    settingsNode.setProperty("shiftDelayUs").send((String)res);
    return true;
}

bool shiftLatchTimeUsHandler(const HomieRange& range, const String& value) {
    Serial.print("shiftDelayUs payload="); Serial.println(value);
    uint16_t res = value.toInt();
    flip.flipdot.setShiftLatchTimeUs(res);
    settingsNode.setProperty("shiftLatchTimeUs").send((String)res);
    return true;
}


uint8_t stringToBool(String s) {
    String param=s;
    param.toLowerCase();
    if (param == "true" ||  param == "1" || param == "on" || param == "yes" || param == "y"){
        return 1;
    }else if(param == "false" || param == "0" || param == "off" || param== "no" || param == "n"){
        return 0;
    }else{
        return 2;
    }
}
 



bool controlHandler(const HomieRange& range, const String& value) {
    Serial.print("Control payload="); Serial.println(value);
    if (value == "redraw"){
        flip.redraw();
    }else if(value == "black"){
        flip.setBuffer_solid(0);
        flip.redraw();
    }else{
        error="preset \""+value+"\" not found";
        return false;
    }

  displayNode.setProperty("control").send(value);

  return true;
}

bool textHandler(const HomieRange& range, const String& value) 
{
    Serial.print("Text payload="); Serial.println(value);

    String row1;
    String row2;

    uint8_t linebreakindex = value.indexOf("\n"); //search for "\n" in string. 255 if not found
    Serial.print("Linebreakindex="); Serial.println(linebreakindex);
    if (linebreakindex!=255)  //found linebreak
    {
        row1=value.substring(0,linebreakindex);
        row2=value.substring(linebreakindex+1);
    }
    else
    {
        row1=value.substring(0, flip.getW()/6);
        row2=value.substring(flip.getW()/6);
        row2.trim(); //remove leading and trailing whitespaces
        String overflow = row2.substring(flip.getW()/6); //save text that did not fit
        row2 = row2.substring(0, flip.getW()/6);

        if (overflow.length() > 0) 
        {
            error="Text overflow:"+overflow;
            Serial.println("Text overflow");
        }
    }

    Serial.print("Row1="); Serial.println(row1);
    Serial.print("Row2="); Serial.println(row2);
    
    flip.setBuffer_solid(0); //clear

    if (row2.length() <= 0)
    {
        Serial.println("Only one row");
        flip.addBuffer_text(row1,0,4);
    }
    else
    {
        Serial.println("Two rows");
        flip.addBuffer_text(row1,0,0);
        flip.addBuffer_text(row2,0,8);
    }

    return true;
}


bool textLargeHandler(const HomieRange& range, const String& value) 
{
    Serial.print("Text Large payload="); Serial.println(value);

    String row1=value;

    Serial.print("Row="); Serial.println(row1);
    
    flip.setBuffer_solid(0); //clear

    flip.addBuffer_text(row1,255,0,1); //large font, centered

    return true;
}