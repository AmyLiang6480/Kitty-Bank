#include <GyverHX711.h>
#include <GyverHX711.h>
#include <HX711.h>
#include <HX711.h>
#include <HX711.h>
#include <Servo.h>
#include <HX711_ADC.h>
#if defined(ESP8266)|| defined(ESP32) || defined(AVR)
#include <EEPROM.h>
#endif

//pins:
const int HX711_dout = 4; //mcu > HX711 dout pin
const int HX711_sck = 5; //mcu > HX711 sck pin

//HX711 constructor:
HX711_ADC LoadCell(HX711_dout, HX711_sck);

const int calVal_eepromAdress = 0;
unsigned long t = 0;

const unsigned int trigPin = 12;
const unsigned int echoPin = 13;
const unsigned int baudRate = 9600;
const int light_sensor = A0;
Servo myservo;
float weight;
int val;
int ledPin = 3;
int buttonPin = 7;

int incomingByte;
int buttonState = 0;
int bankState;
int coinState;
int coinInState;
int counter;
float weightList[100];
float finalWeight;
float coinValue;


void setup() {

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(light_sensor, INPUT);
  pinMode(buttonPin, INPUT);
  myservo.attach(8);
  Serial.begin(baudRate);

  pinMode(ledPin, OUTPUT);


  LoadCell.begin();
  float calibrationValue; // calibration value (see example file "Calibration.ino")
  calibrationValue = 696.0; // uncomment this if you want to set the calibration value in the sketch
  EEPROM.get(calVal_eepromAdress, calibrationValue); // uncomment this if you want to fetch the calibration value from eeprom

  unsigned long stabilizingtime = 2000; // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
  boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    while (1);
  }
  else {
    LoadCell.setCalFactor(calibrationValue); // set calibration value (float)
    Serial.println("Startup is complete");
  }
}

void loop() {
  buttonState = digitalRead(buttonPin);
  
  static boolean newDataReady = 0;
  const int serialPrintInterval = 0; //increase value to slow down serial print activity

  // check for new data/start next conversion:
  if (LoadCell.update()) newDataReady = true;

  // get smoothed value from the dataset:
  if (newDataReady) {
    if (millis() > t + serialPrintInterval) {
      float i = LoadCell.getData();
      newDataReady = 0;
      t = millis();
    }
  }

  // check if last tare operation is complete:
  if (LoadCell.getTareStatus() == true) {
    Serial.println("Tare complete");
  }

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  const unsigned long duration = pulseIn(echoPin, HIGH);
  int distance = duration*0.034/2;
  val = analogRead(light_sensor);
  weight = LoadCell.getData();
  Serial.println(weight);
  myservo.write(110);

  if (distance >60 ) {
    bankState=0;
    digitalWrite(ledPin, LOW);
  } else {
    bankState=1;
    digitalWrite(ledPin, HIGH);

    if (val <= 100){
      coinState = 1;
      myservo.write(50);
      delay(1000);

    } else {
      coinState = 0;
    }
   
    if (weight <= 0.5){
      coinInState = 0;
    } else {
      coinInState = 1;

      counter++;
      Serial.println(counter);
      Serial.println(weight);

      for(int i=0; i<100; i++){
        weightList[counter] = weight;
      }

      finalWeight = weightList[30];
      Serial.println(finalWeight);

      if (Serial.available() > 0) {
      int incomingByte = Serial.read();
      if (incomingByte== 't' && finalWeight>3) {
       LoadCell.tareNoDelay();
       memset(weightList, weight, sizeof(weightList));
       counter=10;
       Serial.println("TARE COMPLETE");
    }
  }      

      if (finalWeight <=14.4 && finalWeight >= 13){ //TOonie
            coinValue = 2.00;
            Serial.println(coinValue);
            delay(1000);

          }
      else if (finalWeight <=12.8 && finalWeight >= 12){ //Loonie
            coinValue = 1.00;
            Serial.println(coinValue);
            delay(1000);

          }
      else if (finalWeight <=9.7 && finalWeight >= 8.5){ //Quarter
            coinValue = 0.25;
            Serial.println(coinValue);
            delay(1000);

          }
      else if (finalWeight <=4 && finalWeight >= 3){ //10cents
            coinValue = 0.10;
            Serial.println(coinValue);
            delay(1000);

          }    
      else if (finalWeight <=8.1 && finalWeight >= 7){ //5cents
            coinValue = 0.05;
            Serial.println(coinValue);
            delay(1000);

          }

    }
   
    
  }

    Serial.print("a");
    Serial.print(bankState);
    Serial.print("a");
    Serial.println(" ");

    Serial.print("b");
    Serial.print(coinState);
    Serial.print("b");
    Serial.println(" ");

    Serial.print("c");
    Serial.print(coinValue);
    Serial.println("c ");

    Serial.print("d");
    Serial.print(coinInState);
    Serial.println("d ");

  Serial.println("&"); //denotes end of readings from both sensors
  Serial.println(" ");

  delay(300);
}


