#include "Arduino.h"
#include <GyverTimers.h>
#include <DynamixelSerial.h>
#define ID        (1u)


int analogPin = A0; // potentiometer wiper (middle terminal) connected to analog pin 3
                    // outside leads to ground and +5V
int val = 0;        // variable to store the value read
int initial = 512;
int pos =512;
int Position = 0;
int delta = 5;      // 1.45 degree
int max = initial + 100;
int min = initial - 100;
int i = 0;


void setup(){
Dynamixel.setSerial(&Serial1); // &Serial - Arduino UNO/NANO/MICRO, &Serial1, &Serial2, &Serial3 - Arduino Mega
Dynamixel.begin(1000000,4);    // Inicialize the servo at 1 Mbps and Pin Control 4
Serial.begin(115200);          //  setup serial
}

void loop(){
  
    pos = pos + delta;
    
    if (pos > max)
    {
      pos = max;
      delta *= -1;
    }

    if (pos < min)
    {
      pos = min;
      delta *= -1;
    }
  
    val = analogRead(analogPin); 
    Position = Dynamixel.readPosition(ID); // Request and Print the Position
    Serial.print("*** Sensor position: "); 
    Serial.print(val);
    Serial.print(" Dynamixel position: "); 
    Serial.println(Position-330);
    delay(250);
    Dynamixel.move(ID, pos);
    delay(250);
 
}
