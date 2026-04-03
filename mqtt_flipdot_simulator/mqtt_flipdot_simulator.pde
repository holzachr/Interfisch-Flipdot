import mqtt.*;

MQTTClient client;

static int pixeldistance=16;
static int pixelsize=14;
static int displaywidth=75;
static int displayheight=16;
static float pixelcornerradius=3;
static float pixelborderweight=1;

color colorWhite=color(240,255,150);
color colorBlack=color(50,50,50);
color colorBG=color(127,127,127);

String lastDataReceived=""; 
int lastDataReceivedTime=0;

PGraphics pg;

PFont smallfont;
// The font must be located in the sketch's 
// "data" directory to load successfully


void setup() {
  size(1300,300);
  smooth(0);
  pg = createGraphics(int(displaywidth*pixeldistance+pixelborderweight/2),int(displayheight*pixeldistance+pixelborderweight/2));
  background(colorBG);
  
  //printArray(PFont.list());
  smallfont = createFont("Monospaced.plain", 12);
  


  client = new MQTTClient(this);
  client.connect("mqtt://192.168.1.6", "processing-flipdot");
}

void draw() {
  fill(colorBG);
  rect(0,0,width,height);
  pg.beginDraw();
  pg.clear();
  for (int y=0;y<displayheight;y++){
    for (int x=0;x<displaywidth;x++){
      pg.strokeWeight(pixelborderweight);
      if (getPixelFromString(x,y)) {
        pg.stroke(0,0,0);
        pg.fill(colorWhite);
      }else{
        pg.stroke(0,0,0);
        pg.fill(colorBlack);
      }
      pg.rect(x*pixeldistance+(pixeldistance-pixelsize)/2,y*pixeldistance+(pixeldistance-pixelsize)/2, pixelsize,pixelsize, pixelcornerradius) ;
    }
  }
  pg.endDraw();
  image(pg,5,5);
  
  textFont(smallfont);
  fill(255,255,255);
  int showFirstData=100;
  int showLastData=32;
  text(lastDataReceived.substring(0,min(showFirstData,lastDataReceived.length()))+"..",5,pg.height+16);
  text(".."+lastDataReceived.substring(max(lastDataReceived.length()-showLastData,0),lastDataReceived.length()), (showFirstData+2)*7.3 ,pg.height+16);
  text("length="+lastDataReceived.length(), (showFirstData+2)*7.3+(showLastData+2)*7.3 ,pg.height+16);
  
  text((millis()-lastDataReceivedTime)/1000.0+"s", 5,pg.height+16+16);
  
}

boolean getPixelFromString(int x,int y){
  if (lastDataReceived.length()>x*displayheight+y){
    return lastDataReceived.charAt(x*displayheight+y)=='1';
  }else{
    return false;
  }
}

void clientConnected() {
  println("client connected");

  client.subscribe("testflipdot/display/data/set");
}

void messageReceived(String topic, byte[] payload) {
  //println("new message: " + topic + " - " + new String(payload));
  println("new message: " + topic + " len= " + (new String(payload)).length());
  lastDataReceived=new String(payload);
  lastDataReceivedTime=millis();
}

void connectionLost() {
  println("connection lost");
}
