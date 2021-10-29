#include "Arduino.h"
#include <GyverTimers.h>
#include <DynamixelSerial.h>
#include <math.h>

#define ID        (1u)
//#define FQ        150.15// Хотим 3Гц. Чтобы синус был гладкий - предположим, что нужно знать значение синуса в 50 точках
//3 Гц = 333 мс; 333/50 = 6,66 мс на одну точку( каждые 6.66 мс знаем значение синуса)
// 1/6.66мс = 150,15 Гц - требуемая частота вызова прерывания
#define FQ        3.0 //Частота в ГЦ

int analogPin = A0; // potentiometer wiper (middle terminal) connected to analog pin 0
                    // outside leads to ground and +5V
int val = 0;        // variable to store the value read
int pos = 512;
int Position = 0;
int flag = 0;
int read_val[2] = {0, 0};
int t = 0;

char T_sig_buff[10];
char T_sin_buff[10];
char FQ_sin_buff[10];

float f_val[3] = {0.0, 0.0, 0.0};
float ref = 0.0;
float myTimer = 0.0;
float T_sig   = 0.0;   //333 мс
float T_sin   = 0.0;   //6.66 мс
float FQ_sin  = 0.0;   //150.15 Гц
float k = 0.5;
float newVal = 0.0;


void setup(){
  Serial.begin(115200);          //  setup serial
  FQ_sin = *(freq_comput()+2); 
  Dynamixel.setSerial(&Serial1); // &Serial - Arduino UNO/NANO/MICRO, &Serial1, &Serial2, &Serial3 - Arduino Mega
  Dynamixel.begin(1000000,4);    // Inicialize the servo at 1 Mbps and Pin Control 4
  Timer3.setFrequencyFloat(FQ_sin); 
  Timer3.enableISR(); // На каждый вызов прерывания вычисляется синус
}

ISR(TIMER3_A){
  flag = 1;
  T_sin = *(freq_comput()+1);
  myTimer+=T_sin;
}

float* freq_comput(){
  float *p = f_val;
  *p       = 1.0 / FQ;
  *(p+1)   = *p/50.0; 
  *(p+2)   = 1.0/ *(p+1);
  return p;
  }


void myFunc(float myTimer){
  ref = 30*sin(2*M_PI*myTimer*FQ)+float(pos);
  
  
  //debug_plots() //Для отладки частот
  //if (millis()<5000) {
    print_plots();
   // Dynamixel.move(ID, ref);
    flag = 0;
   // }
  }

int* read_func(){
  int *p = read_val;
  *p = analogRead(analogPin);
  *(p+1) = Dynamixel.readPosition(ID);
  return p;
  }

float expRunningAverage(float newVal) {
  static float filVal = 0;
  filVal += (newVal - filVal) * k;
  return filVal;
}

void print_plots(){
  val = *(read_func()); 
  Position = *(read_func()+1);
  newVal = expRunningAverage(val);
  Serial.print("*** Sensor position: ");
  Serial.print((newVal-255)/2);
  //Serial.print("; ");
  Serial.print(" Dynamixel position: "); 
  Serial.println((Position-280-230)/2);
  //String DATA = "$" + String((val-320)/2.5) + " " + String((Position-190-320)/2.5) + " " + String(millis())+";";
  //Serial.println(DATA); 
  }

void debug_plots(){
  dtostrf(*(freq_comput()),   4, 6, T_sig_buff);
  Serial.print("*** T_sig: ");
  Serial.print(T_sig_buff);
  dtostrf(*(freq_comput()+1), 4, 6, T_sin_buff);
  Serial.print("; T_sin: ");
  Serial.print(T_sin_buff);
  dtostrf(*(freq_comput()+2), 4, 6, FQ_sin_buff);
  Serial.print("; FQ_sin: ");
  Serial.println(FQ_sin_buff);
  }

void loop(){
 
  if(flag == 1){
    myFunc(myTimer);
  }
}
