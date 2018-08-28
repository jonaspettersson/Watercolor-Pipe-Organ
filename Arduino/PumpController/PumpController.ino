/*
 * Watercolor Pipe Organ Windpump
 * 
 * Jonas Pettersson, hello@jonaspettersson.se
 * 2018-08-28
 * 
 * Using:
 * - Two standard servos
 * - EK-XTOP Revo D5 PWM cooler pump
 * - BMP180 Barometric pressure sensor
 * 
 * 
 *  Use this code in the arduino that is  controlling the wind 
 *  delivered to the acrylic windchest tube
 *  
 */

#include <Servo.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>

// PRESSURE SENSOR 
Adafruit_BMP085 bmp;
int kPsiMin = 1.4 ; // ? had to be verified
int kPsiMax = 1.6 ; // ? had to be verified

// MAKE SHURE SERVO 1 AND TRIG/ECHO 1 IS ON THE SAME WATERPIPE

// SERVO SETUP =======================
Servo servo1;   // Pin 6
Servo servo2;   // Pin 7

int servo1Pin = 6;
int servo2Pin = 7;

// Set servo position range
int openPos = 65;
int closePos = 140;


// PING Setup ========================

int reverseDistance = 20; // 20 cm
long duration, cm;


// Pipe I
const int trigPin1 = 9;
const int echoPin1 = 10;

// Pipe II
const int trigPin2 = 11;
const int echoPin2 = 12;


// PUMP DIRECTION
int pumpdirection = -1;

void setup() {

  Serial.begin(115200);

  // Setup pins
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);

  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  
/*

  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }

  Serial.println("BPM Sensor Ok!");
  
*/
  
  servo1.attach(servo1Pin);
  servo2.attach(servo2Pin);
  Serial.println("Servos init!");
  
  initPump();
  Serial.println("Pump init!");
  

}

void loop() {

  //checkPump();

  moveServos(pumpdirection);
  delay(100);
  pumpdirection= -pumpdirection;
  delay(2500);

}

void checkPump() {
  
  int WaterLevel;
  int usepump;
  long actualkPsi = bmp.readPressure() / 1000;

  if (pumpdirection==1) {
      WaterLevel = getDistance1;
      usepump = 1;
    } else {
      WaterLevel=getDistance2;
      usepump=-1;
     }

   if (WaterLevel<=reverseDistance) {pumpdirection=usepump;}

   if (actualkPsi <= kPsiMin) {
      // Pump ON
      
    } else {
      if (actualkPsi > kPsiMax) { 
          // Pump OFF
          
        }
    }  
  
  }


int getDistance1() {
  
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);

  duration = pulseIn(echoPin1, HIGH);
  
  // Calculating the distance
  float distance = duration*0.034/2;  
  return  distance;
  
  }

int getDistance2() {
  
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);

  duration = pulseIn(echoPin2, HIGH);
  
  // Calculating the distance
  float distance = duration*0.034/2;  
  return  distance;
  
  }


void initPump() {

  moveServos(pumpdirection);
  
  }


void moveServos(int dir) {

  if (dir==1) {
    servo1.write(openPos);  
    servo2.write(openPos); 
    Serial.println(openPos);
    
  } else {

    servo1.write(closePos);  
    servo2.write(closePos);
    Serial.println(closePos);
    }

    
    
}



