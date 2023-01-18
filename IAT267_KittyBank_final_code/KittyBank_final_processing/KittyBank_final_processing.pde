import processing.serial.*;
import ddf.minim.*;
Serial port;
PFont font;  // Create a font object to display text

Minim minim;
AudioPlayer cateating;
AudioPlayer bgmusic;
AudioPlayer meow;

byte[] inBuffer = new byte[255];

//three readig from the sensor
int valP_position;// states for position, presence detection
int valP_light;// states for light, coin detection
int valP_weight; // states for weight, detect weight in box
float coinValue; 

int displaytimer;
boolean portwrite = false;

int state = 0; //for switching visuals

PImage bankclose;
PImage bankopen;
PImage coineating;
PImage calculating;
PImage valuedisplay;



void setup() {
  size(1180, 820); //size of window
  noStroke();
  frameRate(50);
  font = loadFont("Rockwell-48.vlw");

  bankclose = loadImage("bankclosed.png");
  bankopen = loadImage("bankopen.png");
  coineating = loadImage("coineating.png");
  calculating = loadImage("calculating.png");
  valuedisplay = loadImage("valuedisplay.png");

  minim = new Minim(this);
  cateating = minim.loadFile("cateating.mp3");
  bgmusic = minim.loadFile("backgroundmusic.mp3");
  meow = minim.loadFile("meow.mp3");

  port = new Serial(this, Serial.list()[0], 9600);
}


void draw() {
  if (0 < port.available()) { // If data is available to read,
    bankClosed();
    bgmusic.play();

    port.readBytesUntil('&', inBuffer);  //read in all data until '&' is encountered

    if (inBuffer != null) {
      String myString = new String(inBuffer);
      String[] p = splitTokens(myString, "&");
      if (p.length < 2) return;  //exit this function if packet is broken

      //get position sensor reading //////////////////////////////////////////////////
      String[] position_sensor = splitTokens(p[0], "a");
      if (position_sensor.length != 3) return;  //exit this function if packet is broken
      valP_position = int(position_sensor[1]);

      println("position sensor switch:" + valP_position);  // 0 bank closed, 1 bank open

      //get light sensor reading //////////////////////////////////////////////////
      String[] light_sensor = splitTokens(p[0], "b");
      if (light_sensor.length != 3) return;  //exit this function if packet is broken
      valP_light = int(light_sensor[1]);

      println("light sensor switch:" + valP_light);

      //get weight sensor reading //////////////////////////////////////////////////
      String[] weight_sensor = splitTokens(p[0], "c");  //get weight sensor reading
      if (weight_sensor.length != 3) return;  //exit this function if packet is broken
      float coinValue = float(weight_sensor[1]);

      println("CoinValue:" + coinValue);

      String[] coinIn = splitTokens(p[0], "d");
      if (coinIn.length != 3) return;  //exit this function if packet is broken
      valP_weight = int(coinIn[1]);

      println("Coin In State:" + valP_weight);

//////////////// STATES////////////////////////////////////////

      if (state == 0) { //bank close
        bankClosed();
      } else if (state == 1) { //bank open
        //bgmusic.loop();
        bankOpen();
      } else if (state == 2) { //coin received
        cateating.loop();
        fill(183, 203, 225);
        rect(0, 0, width, height);
        coineating.resize(1180, 820);
        image(coineating, 0, 0);
      } else if (state == 3) { //coin counting
        fill(183, 203, 225);
        rect(0, 0, width, height);
        coineating.resize(1180, 820);
        image(coineating, 0, 0);
      }else if (state == 4) {  //bank open with total value
        cateating.pause();
        meow.loop();
        fill(183, 203, 225);
        rect(0, 0, width, height);
        valuedisplay.resize(1180, 820);
        image(valuedisplay, 0, 0);
        if (coinValue > 0) {
          fill(0);
          strokeWeight(2); 
          String value = nf(coinValue, 0, 2);
          textSize(60);
          text("You put a $ " + value + " coin", 320, 175);
          displaytimer++;
          if (displaytimer >400){
          port.write('t');
          portwrite =true;
          displaytimer = 0;
          }
        }

      }

//////////////////////////STATE TRANSITION////////////////////////////////

      if (valP_position == 0) {
        state = 0;}
      if (valP_position == 1) {
        state = 1;}
      if (valP_position == 1 && valP_light == 1) {
        state = 2;
      } 
      if (valP_position == 1 && valP_light ==0 && valP_weight ==1) {
        state = 3;} 
        
      if(valP_position == 1 && valP_light ==0 && valP_weight ==1 && coinValue != 0.00){
        state = 4;}
      if(portwrite) {state=1;}
      }
    }
  }

void bankClosed() {
  fill(0);
  rect(0, 0, width, height);
  bankclose.resize(1180, 820);
  image(bankclose, 0, 0);
}

void bankOpen() {
  fill(183, 203, 225);
  rect(0, 0, width, height);
  bankopen.resize(1180, 820);
  image(bankopen, 0, 0);
  fill(255, 0, 0);
  strokeWeight(2);
  textFont(font);
  textSize(48);
}
