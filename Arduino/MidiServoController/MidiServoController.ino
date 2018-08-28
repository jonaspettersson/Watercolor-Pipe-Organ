/*
 * Watercolor Pipe Organ Servo Controller
 * 
 * Use this code for the unit next to the pipe organ
 * Use rtpMidi, PC-version https://www.tobias-erichsen.de/software/rtpmidi.html
 * or on Mac OS Apple MIDI to communicate directly with the arduino
 * 
 * Written for Adafruit feather 32u4 esp8266
 * 
 * 
 */
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiUdp.h>
#include <SocketIoClient.h>

/*
 * Connections
 */


const char* socketserver = "organ.jonaspettersson.se";
const char* ssid     = "your-ssid";
const char* pass = "your-password";



// Json
#include <ArduinoJson.h>
const size_t bufferSize = JSON_OBJECT_SIZE(4) + 30;
DynamicJsonBuffer jsonBuffer(bufferSize);


// MIDI
#include "AppleMidi.h"
APPLEMIDI_CREATE_INSTANCE(WiFiUDP, AppleMIDI); // see definition in AppleMidi_Defs.h



// Forward declaration
void OnAppleMidiConnected(uint32_t ssrc, char* name);
void OnAppleMidiDisconnected(uint32_t ssrc);
void OnAppleMidiNoteOn(byte channel, byte note, byte velocity);
void OnAppleMidiNoteOff(byte channel, byte note, byte velocity);


// Servo PWM

int FirstPipeSectorNote = 48 ; // MIDI C2

Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver();
Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(0x41);
Adafruit_PWMServoDriver pwm3 = Adafruit_PWMServoDriver(0x42);
Adafruit_PWMServoDriver pwm4 = Adafruit_PWMServoDriver(0x43);

// For SG-90 servo and letting it rest at "close" position
#define SERVOMIN  200 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  400 // this is the 'maximum' pulse length count (out of 4096)


unsigned long t0 = millis();
bool isConnected = false;

#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;
SocketIoClient webSocket;


void welcome(const char * payload, size_t length) {
  USE_SERIAL.printf("connected to server: %s\n", payload);

}

void news(const char * payload, size_t length) {
  USE_SERIAL.printf("connected: %s\n", payload);
  blinkred(3);
}

void midijson(const char * payload, size_t length) {

  JsonObject& root = jsonBuffer.parseObject(payload);

  
  byte a = root["a"]; // "0"    On/Off
  byte c = root["c"]; // "0"    Channel
  byte n = root["n"]; // "122"  Note
  byte v = root["v"]; // "127"  Velocity

  int PipeSector = (int)(((int)n - FirstPipeSectorNote) / 12) + 1;
  int servoNo = (((int)n - ((PipeSector - 1) * 12)) - FirstPipeSectorNote) + 1;

  //USE_SERIAL.println("On PipeSector: " + String(PipeSector));
  //USE_SERIAL.println("On servoNo: " + String(servoNo));

  if (a==1) {
    
      AppleMIDI.noteOn(n, v, c);
      digitalWrite(LED_BUILTIN, LOW);
 
      moveServo(PipeSector,servoNo,60);
      
      
    } else {
      AppleMIDI.noteOff(n, v, c);
      digitalWrite(LED_BUILTIN, HIGH);

      moveServo(PipeSector,servoNo,0);
      
    }

 
}

void midi(const char * payload, size_t length) {
  
  USE_SERIAL.printf("did get midi: %s\n", payload);
}

void onmidi(const char * payload, size_t length) {


  //USE_SERIAL.printf("did get midi: %s\n", payload);
  //USE_SERIAL.println("midi:" + String(payload));
    
}



void disconnected(const char * payload, size_t length) {
  USE_SERIAL.printf("disconnected: %s\n", payload);
}


void setup() {

    pinMode(LED_BUILTIN, OUTPUT);

    // 0x40
    pwm1.begin();
    pwm1.setPWMFreq(60);  
  
    // 0x41
    pwm2.begin();
    pwm2.setPWMFreq(60); 

    // 0x42
    pwm3.begin();
    pwm3.setPWMFreq(60); 

    // 0x43
    pwm4.begin();
    pwm4.setPWMFreq(60); 

    
  
    USE_SERIAL.begin(115200);

    USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

      for(uint8_t t = 4; t > 0; t--) {
          USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
          USE_SERIAL.flush();
          delay(1000);
      }

    WiFiMulti.addAP(ssid, pass );

    while(WiFiMulti.run() != WL_CONNECTED) {
        delay(100);
    }

    if(WiFi.status() == WL_CONNECTED) {
          blinkred(2);
    }
    
    webSocket.begin(socketserver);
   
    webSocket.on("midijson", midijson);
    webSocket.on("news", news);
    webSocket.on("midi", midi);
    webSocket.on("onmidi", onmidi);
    
        


  // Create a session and wait for a remote host to connect to us
  AppleMIDI.begin("RemoteMidi");

  AppleMIDI.OnConnected(OnAppleMidiConnected);
  AppleMIDI.OnDisconnected(OnAppleMidiDisconnected);

  AppleMIDI.OnReceiveNoteOn(OnAppleMidiNoteOn);
  AppleMIDI.OnReceiveNoteOff(OnAppleMidiNoteOff);



  
}

void loop() {
  
  if(WiFi.status() == WL_CONNECTED) {
    
    webSocket.loop();
    AppleMIDI.run();
   } else {
    //webSocket.disconnect();
  }
   
}


// ====================================================================================
// Event handlers for incoming MIDI messages
// ====================================================================================

// -----------------------------------------------------------------------------
// rtpMIDI session. Device connected
// -----------------------------------------------------------------------------
void OnAppleMidiConnected(uint32_t ssrc, char* name) {
  isConnected  = true;
  USE_SERIAL.print(F("Connected to session "));
  USE_SERIAL.println(name);
}

// -----------------------------------------------------------------------------
// rtpMIDI session. Device disconnected
// -----------------------------------------------------------------------------
void OnAppleMidiDisconnected(uint32_t ssrc) {
  isConnected  = false;
  USE_SERIAL.println(F("Disconnected"));
  digitalWrite(LED_BUILTIN, HIGH);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OnAppleMidiNoteOn(byte chnl, byte note, byte velocity) {

  digitalWrite(LED_BUILTIN, LOW);


  int PipeSector = (int)((note - FirstPipeSectorNote) / 12) + 1;
  int servoNo = ((note - ((PipeSector - 1) * 12)) - FirstPipeSectorNote) + 1;

  USE_SERIAL.println("On PipeSector " + PipeSector);
  USE_SERIAL.println("On servoNo " + servoNo);

  moveServo(PipeSector,servoNo,60);
 
  String a = String(1);
  String c = String(chnl);
  String n = String(note);
  String v = String(velocity);


  String o = "{\"a\": \""+ a + "\",\"c\": \"" + c + "\",\"n\": \"" + n + "\", \"v\": \"" + v + "\"}";
  webSocket.emit("midi",o.c_str());


}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void OnAppleMidiNoteOff(byte chnl, byte note, byte velocity) {

  digitalWrite(LED_BUILTIN, HIGH);

  int PipeSector = (int)((note - FirstPipeSectorNote) / 12) + 1;
  int servoNo = ((note - ((PipeSector - 1) * 12)) - FirstPipeSectorNote) + 1;

  USE_SERIAL.println("Off PipeSector " + PipeSector);
  USE_SERIAL.println("Off servoNo " + servoNo);

  moveServo(PipeSector,servoNo,0);

  String a = String(0);
  String c = String(chnl);
  String n = String(note);
  String v = String(velocity);

  String o = "{\"a\": \""+ a + "\",\"c\": \"" + c + "\",\"n\": \"" + n + "\", \"v\": \"" + v + "\"}";
  
  webSocket.emit("midi",o.c_str());

}

void blinkred(int rep) {

      for(uint8_t t = 0; t < rep; t++) {
          digitalWrite(LED_BUILTIN, HIGH);
          delay(300);
          digitalWrite(LED_BUILTIN, LOW);
          delay(200);
      }

  }



// Servo movement

void moveServo(int pipesector, int servo, int angle) {

  servo = servo - 1; //Zero based

  //USE_SERIAL.println("PipeSector: " + String(pipesector));
  //USE_SERIAL.println("servoNo: " + String(servo));
  //USE_SERIAL.println("angle: " + String(angle));
  
    int pulselength = map(angle, 0, 180, SERVOMIN, SERVOMAX);
    
    switch (pipesector) {
      case 1:
        // Bottom
        pwm1.setPWM(servo, 0, pulselength);
      break;
      case 2:
        pwm2.setPWM(servo, 0, pulselength);
      break;
      case 3:
        pwm3.setPWM(servo, 0, pulselength);
      break;
      case 4:
        // Top
        pwm4.setPWM(servo, 0, pulselength);
      break;
    }
       
}  
