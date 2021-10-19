#include "Arduino.h"
#include <GyverTimers.h>
#include <DynamixelSerial.h>
#include <math.h>

#define ID        (1u)
//#define FQ        150.15// Хотим 3Гц. Чтобы синус был гладкий - предположим, что нужно знать значение синуса в 50 точках
//3 Гц = 333 мс; 333/50 = 6,66 мс на одну точку( каждые 6.66 мс знаем значение синуса)
// 1/6.66мс = 150,15 Гц - требуемая частота вызова прерывания
//#define FQ        3

int analogPin = A0; // potentiometer wiper (middle terminal) connected to analog pin 3
                    // outside leads to ground and +5V
int val = 0;        // variable to store the value read
int initial = 512;
int pos = 512;
int Position = 0;
float ref = 0.0;
int delta = 5;      // 1.45 degree
int max = initial + 100;
int min = initial - 100;
int flag = 0;

float FQ = 3.0;
float T_sig  = 1.0 / FQ;      //333 мс
float T_sin  = T_sig / 50.0;  //6.66 мс
float FQ_sin = (float)1.0 / T_sin;   //150.15 Гц
float myTimer = 0.0;

char buff[10];

void setup(){
  Serial.begin(115200);          //  setup serial
  /*------Вывод начальных значений------*/
  dtostrf(FQ_sin, 4, 6, buff);
  Serial.print("FQ_sin: ");
  Serial.println(buff);
  dtostrf(T_sin, 4, 6, buff);
  Serial.print("FQ_sin: ");
  Serial.println(buff);
  delay(2500);
  /*------------------------------------*/
  
  Dynamixel.setSerial(&Serial1); // &Serial - Arduino UNO/NANO/MICRO, &Serial1, &Serial2, &Serial3 - Arduino Mega
  Dynamixel.begin(1000000,4);    // Inicialize the servo at 1 Mbps and Pin Control 4
  Timer3.setFrequencyFloat(FQ_sin); 
  Timer3.enableISR(); // На каждый вызов прерывания вычисляется синус
}

ISR(TIMER3_A){
  flag = 1;
  myTimer+=T_sin;
  //myTimer+=0.0067;
  
}

void loop(){
 
  if(flag == 1){
    myFunc(myTimer);
  }
}


void myFunc(float myTimer){
  ref = 50*sin(2*M_PI*myTimer*FQ)+float(pos);
  val = analogRead(analogPin); 
  Position = Dynamixel.readPosition(ID);

  //String DATA = String("$%d %d;") + "," + String(millis()) + "," + String(ref);
  //String buff = "$" + myTimer + " " +ref + ";";
  
  //String DATA = "$" + String(ref) + " " + String(millis())+";";
  //Serial.println(ref); 
  
//  Serial.println(T_sin); 
  //Нельзя использовать просто аргумент(синусоида не получается - не генерируются уникальные значения)
  //Чтобы получился красивый график, нужно изменить массив аргументов, чтобы он шел от 0 до 2PI
  //Для получения синусоиды необходимо массив данных умножить на 2PI и поделить на требуемую частоту сигнала
  /*Serial.print("*** Sensor position: ");
  Serial.println(val);
  Serial.print(" Dynamixel position: "); 
  Serial.println(Position-350);
  Dynamixel.move(ID, ref);*/
  flag = 0;
  }
