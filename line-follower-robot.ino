#include <QTRSensors.h>

unsigned int sensorValues[8];
QTRSensorsRC qtrrc((unsigned char[]) {4, 6, 7, 8, 9, 10, 11, 12}, 8, 2000, QTR_NO_EMITTER_PIN); 

unsigned int referenceValue = 45;
unsigned int currentValue = 0;

int error = 0;
int lastError = 0;
int diffError = 0;
float const coefP = 3.5;
float const coefD = 1.3;
float const coefI = 0;
float P = 0;
float D = 0;
float I = 0;
float feedBack = 0;
int normalSpeed = 150; 
float lastSpeed1 = 0;
float lastSpeed2 = 0;
int i; //SAYAÇ DEĞİŞKENİ

void setup()
{
  Serial.begin(9600);
  DDRB = B111111;
  DDRD = B00000000;
  DDRC = B111111;
  calibrateSensors();  
}

void loop()
{
  
  currentValue = readSensors();
  error = referenceValue - currentValue;
  diffError = error - lastError;
  lastError = error;
  
  P = error * coefP;
  I = I + error * coefI;
  D = diffError * coefD;
  
  feedBack = P + D;
  lastSpeed1 = normalSpeed + feedBack;
  lastSpeed2 = normalSpeed - feedBack;
  
  if(lastSpeed1 > 255 || lastSpeed2 > 255)
    {
      lastSpeed1 = 255;
      lastSpeed2 = 255;
    }
  
  if(currentValue > 45)
  {
     PORTC = B011000;   // ileri hareket için 
    analogWrite(3,lastSpeed2);      // sağ motor  hızı arttı
    analogWrite(5,lastSpeed1);          // sol motor hızı azaldı
   
    
    if(currentValue == 80)  
    {
      PORTC = B010100;
      analogWrite(5,-2.5*feedBack);
      analogWrite(3,lastSpeed2); 
    }
  }
    
  if(currentValue < 45)
  {
  PORTC = B011000;   // ileri hareket için 
  analogWrite(3,lastSpeed2);      // sağ motor hızı azaldı
  analogWrite(5,lastSpeed1);        // sol motor hızı ARTTI
         
  
   if(currentValue == 10)  
    {
      PORTC = B101000;
      analogWrite(3,2.5*feedBack);
      analogWrite(5,lastSpeed1);
    }  
  }
    
  if(currentValue == 45)
  {
    PORTC = B011000;   // ileri hareket için
    analogWrite(5,normalSpeed);
    analogWrite(3,normalSpeed);         

  }     
}

void calibrateSensors()
{
  delay(500);
  digitalWrite(13, HIGH);    
  for (int i = 0; i < 400; i++) 
  { 
    qtrrc.calibrate();       
  }
  digitalWrite(13, LOW);     
}

unsigned int readSensors()
{
   unsigned int position = qtrrc.readLine(sensorValues);
   unsigned int total = 0;
   int i=0;
   int count = 0;
   
   for(i=0;i<8;i++)
   {
     if(sensorValues[i] < 500)
     {
       total += (i+1)*10;
       count++;
     }
   }
   
   if (total != 0)
   {
     return total/count;
   }
   else
   {
     return 45;
   }   
}

