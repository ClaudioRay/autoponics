///////////////////////////////////////////////////////////////////////////////
///                         Autor: Claudio Ray Ullrich                      ///
///                 Invernadero Hidroponico Automatizado V5                 /// 
///                     Ultima Actualizacion: 07-06-2016                    ///
///////////////////////////////////////////////////////////////////////////////
//LDR        #OK#   // DHT22   #OK#   // PH      #OK# // EC            #OK#  //
//DS18B20    #OK#   // Flujo   #OK#   // CO2     #OK# // ON/OFF Bomba  #OK#  //
//PID        #OK#   // Llenado/Vaciado           #OK# // Control Nivel #OK#  //
//RTC        #OK#   // SD      #OK#   // DataLog #OK# // Luz           #OK#  //
///////////////////////////////////////////////////////////////////////////////
///                           NOTAS DE LA VERSION                           ///
///                   FALTA AGREGAR LECTURA DE EC, CRITICO                  ///
///  - Se corrige el tiempo de funcionamiento del sensor DHT22 agregando    ///
///  500 milisegundos entre cada lectura                                    ///
///  - Se cambia el sistema de respaldo en la memoria SD para hacerlo       ///
///  compatible con excel                                                   ///
///  - Se cambia lo que imprime PI mid y top de input temperatura           ///
///  a setpoint. hacer lo mismo con funcion subir  #OK#                     /// 
///  - Se invierte el tiempo de apagado/prendido de luces para              ///
///     testear correcion. Verificar si funciona                            ///
///////////////////////////////////////////////////////////////////////////////

#include <DHT.h> 
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PID_v1.h>
#include <Wire.h> 
#include "RTClib.h"
#include <SPI.h>
#include <SD.h>

#define pin_dht_s 41                     // D3 pin DHT22
#define pin_dht_i 40                     // D2 pin DHT22
#define DHTTYPE DHT22
#define ds18b20 5                       // D5 DATA DS18B20
#define pin_co2_inferior 11             // A13 CO2 inferior
#define pin_co2_superior 10             // A10 CO2 superior
#define DC_GAIN (8.5)  
#define READ_SAMPLE_INTERVAL (50)   
#define READ_SAMPLE_TIMES (5)      
#define ZERO_POINT_VOLTAGE (0.220) 
#define REACTION_VOLTGAE (0.020)
#define pin_ph A2                        // A2 PH
#define Offset 0.16                      // OFFSET pH CALIBRACION
#define ArrayLenth  40    


//VARIABLES LOOP
unsigned long Millisloop = 0;
// VARIABLES DHT22
unsigned long MillisDHT = 0;
// DHT Superior
DHT dht_s(pin_dht_s, DHTTYPE);
float hs;     
float ts;
// DHT Inferior 
DHT dht_i(pin_dht_i, DHTTYPE);
float hi;     
float ti; 

// VARIABLES LDR
//LDR Superior
int ldr_s=15;
int valor_ldr_s=0;
//LDR Inferior
int ldr_i=14;
int valor_ldr_i=0;

// VARIABLES CO2
int porcentaje_i;
int porcentaje_s;
float volts; 
float CO2Curve[3]  =  {2.602,ZERO_POINT_VOLTAGE,(REACTION_VOLTGAE/(2.602-3))};

// VARIABLES DS18B20
OneWire oneWire(ds18b20);
DallasTemperature sensors(&oneWire);

// VARIABLES PH
int pHArray[ArrayLenth];   //Store the average value of the sensor feedback
int pHArrayIndex=0;   
float valor_pH,voltage;

//VARIABLES EC
byte ECsensorPin = A4;  //EC Meter analog output,pin on analog 1
unsigned int AnalogSampleInterval=25,printInterval=700;  //analog sample interval;serial print interval;temperature sample interval
unsigned long AnalogValueTotal = 0;                  // the running total
unsigned int Voltage = 0, valorEC = 0; 
unsigned long printTime,tempSampleTime;
float temperature,ECcurrent;

// VARIABLES FLUJOMETRO
volatile int  flow_frequency_1;   // Measures flow meter pulses
volatile int  flow_frequency_2;
volatile int  flow_frequency_3; 
unsigned int  l_min_1;          // Calculated litres/hour   
unsigned int  l_min_2;
unsigned int  l_min_3;  
unsigned char flowmeter_1 = 21;  // Flow Meter Pin number
unsigned char flowmeter_2 = 19;  
unsigned char flowmeter_3 = 2; 
unsigned long currentMillis;  
unsigned long cloopTime;

//VARIABLES CONTROL RELES Y SENSORES DE NIVEL
const int  niveltop = 34;
int estadonivelTOP;
const int nivelmid = 35;
int estadonivelMID;
const int nivelbot = 36;
int estadonivelBOT = 0;  
const int rele1 = 26;
const int rele2 = 27;
const int rele3 = 24;
const int rele4 = 25;   
const int rele5 = 28;
const int rele6 = 29;
const int rele7 = 30;
const int rele8 = 31;
int caso = 0; 

//VARIABLES LLENADO/VACIADO
bool llenado = 1;
bool estadoTOP= 1; 
bool estadoMID = 0; 
bool vaciadoTOP = 1;
bool vaciadoMID = 0;
bool toggleLL = 1;
bool toggleV = 0;
const long intervalo_llenado = 1500000;  //2700000  
const long intervalo_vaciado = 1500000;  //1000000
unsigned long toggleMillis = 0; 
int nflujometro;
unsigned long intervalo = 0;

//VARIABLES PID
double Setpoint_top, Input_top, Output_top, Setpoint_mid, Input_mid, Output_mid ;
PID PID_top(&Input_top, &Output_top, &Setpoint_top,58.94,0.17,0, DIRECT);
PID PID_mid(&Input_mid, &Output_mid, &Setpoint_mid,58.94,0.17,0, DIRECT);

//VARIABLES RTC
RTC_DS1307 RTC;

//VARIABLES DATALOGGER
File myFile;
unsigned long Millisprevios = 0; 

//VARIABLES ADAFRUIT
unsigned long MillisADA = 0;

//VARIABLES LUZ
bool toggleON = 0;
bool toggleOFF = 1;
bool  toggle2ON = 0;
bool  toggle2OFF = 1;
unsigned long previousMillis = 0; 
 

void setup()
{  
  Serial.begin(9600);
  Serial1.begin(115200);

  Wire.begin(); 
  RTC.begin();
  SD.begin(53);
  dht_s.begin();
  dht_i.begin(); 
  sensors.begin();
  Setpoint_top = 21;
  Setpoint_mid = 23;
  PID_top.SetMode(AUTOMATIC);
  PID_mid.SetMode(AUTOMATIC); 
  pinMode(flowmeter_1, INPUT);
  pinMode(flowmeter_2, INPUT);
  pinMode(flowmeter_3, INPUT);
  attachInterrupt(4, flow_1, RISING); // Setup Interrupt 
  attachInterrupt(0, flow_2, RISING); 
  attachInterrupt(1, flow_3, RISING); 
  pinMode(niveltop, INPUT);
  pinMode(nivelmid, INPUT);
  pinMode(nivelbot, INPUT);
  pinMode(rele1, OUTPUT);
  pinMode(rele2, OUTPUT);
  pinMode(rele3, OUTPUT);
  pinMode(rele4, OUTPUT);
  pinMode(rele5, OUTPUT);  
  pinMode(rele6, OUTPUT); 
  pinMode(rele7, OUTPUT);  
  pinMode(rele8, OUTPUT);                                 
  currentMillis = millis();
  cloopTime = currentMillis;   
  ///FIN VOID SETUP
}
void loop()
{
  currentMillis = millis();
  leerFLUJO();
  control_llenado();
  leerDHT();
  leerDS18B20();
    if (currentMillis - Millisloop >= 1000)
   {
     leerCO2();
     leerPH();
     leerEC();
   }    

  control_PID();
  leerLDR();
  control_luz();
  subir();
  respaldo();

   ///FIN VOID LOOP
}
// INICIO FUNCIONES
float MGRead(int mg_pin)
{
    int i;
    float v=0;
 
    for (i=0;i<READ_SAMPLE_TIMES;i++) {
        v += analogRead(mg_pin);
        delay(READ_SAMPLE_INTERVAL);
    }
    v = (v/READ_SAMPLE_TIMES) *5/1024 ;
    return v;  
}
int  MGGetPercentage(float volts, float *pcurve)
{
   if ((volts/DC_GAIN )>=ZERO_POINT_VOLTAGE) {
      return -1;
   } else { 
      return pow(10, ((volts/DC_GAIN)-pcurve[1])/pcurve[2]+pcurve[0]);
   }
}
double avergearray(int* arr, int number)
{
  int i;
  int max,min;
  double avg;
  long amount=0;
  if(number<=0){
    Serial.println("Error number for the array to avraging!/n");
    return 0;
  }
  if(number<5){   //less than 5, calculated directly statistics
    for(i=0;i<number;i++){
      amount+=arr[i];
    }
    avg = amount/number;
    return avg;
  }else{
    if(arr[0]<arr[1]){
      min = arr[0];max=arr[1];
    }
    else{
      min=arr[1];max=arr[0];
    }
    for(i=2;i<number;i++){
      if(arr[i]<min){
        amount+=min;        //arr<min
        min=arr[i];
      }else {
        if(arr[i]>max){
          amount+=max;    //arr>max
          max=arr[i];
        }else{
          amount+=arr[i]; //min<=arr<=max
        }
      }//if
    }//for
    avg = (double)amount/(number-2);
  }//if
  return avg;
}
void leerDHT()
{
  currentMillis = millis();
  if (currentMillis - MillisDHT >= 500)
   {
    //DHT Superior
    hs = dht_s.readHumidity();     
    ts = dht_s.readTemperature();
    //DHT Inferior
    hi = dht_i.readHumidity();     
    ti = dht_i.readTemperature();
   }    
}
void leerLDR()
{
  //LDR Superior
  valor_ldr_s=analogRead(ldr_s);
  valor_ldr_s=valor_ldr_s/10;
  //LDR Inferior
  valor_ldr_i=analogRead(ldr_i);
  valor_ldr_i=valor_ldr_i/10;
}
void leerDS18B20()
{
  sensors.requestTemperatures();
}
void leerCO2()
{
  //Bandeja inferior
  // a la funcion MGRead le va entrar el pin que se debe leer
  volts = MGRead(pin_co2_inferior); 
  porcentaje_i = MGGetPercentage(volts,CO2Curve);
  // Bandeja superior
  volts = MGRead(pin_co2_superior); 
  porcentaje_s = MGGetPercentage(volts,CO2Curve);
}
void leerPH()
{
  pHArray[pHArrayIndex++]=analogRead(pin_ph);      
  if(pHArrayIndex==ArrayLenth)pHArrayIndex=0;
    voltage = avergearray(pHArray, ArrayLenth)*5.0/1024;
    valor_pH = 3.5*voltage+Offset;  
}
void leerEC()
{  
  valorEC = analogRead(ECsensorPin);
  if(millis()-tempSampleTime>=500) 
  {
    tempSampleTime=millis();
    temperature = sensors.getTempCByIndex(1);  // read the current temperature from the  DS18B20
  }
    Voltage=valorEC*(float)5000/1024;
    float TempCoefficient=1.0+0.0185*(temperature-25.0);    //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.0185*(fTP-25.0));
    float CoefficientVolatge=(float)Voltage/TempCoefficient;   
    if(CoefficientVolatge<=448)ECcurrent=6.84*CoefficientVolatge-64.32;   //1ms/cm<EC<=3ms/cm
    else if(CoefficientVolatge<=1457)ECcurrent=6.98*CoefficientVolatge-127;  //3ms/cm<EC<=10ms/cm
    else ECcurrent=5.3*CoefficientVolatge+2278;                           //10ms/cm<EC<20ms/cm
    ECcurrent/=1000;    //convert us/cm to ms/cm   
}
void flow_1 ()                  
{ 
   flow_frequency_1++;
} 
void flow_2 ()                  
{ 
   flow_frequency_2++;
} 
void flow_3 ()                  
{ 
   flow_frequency_3++;
} 
int leerFLUJO()
{
  currentMillis = millis();
     if(currentMillis >= (cloopTime+ 1000))
    { 
      detachInterrupt(4);
      detachInterrupt(0);
      detachInterrupt(1);
      cloopTime = currentMillis;              
      l_min_1 = (flow_frequency_1  / 7.5);  
      l_min_2 = (flow_frequency_2  / 7.5); 
      l_min_3 = (flow_frequency_3  / 7.5); 
      flow_frequency_1 = 0;  
      flow_frequency_2 = 0;
      flow_frequency_3 = 0;                     
      attachInterrupt(4, flow_1, RISING); // Setup Interrupt 
      attachInterrupt(0, flow_2, RISING);  
      attachInterrupt(1, flow_3, RISING);   
    }
}
void control_PID()
{
  sensors.requestTemperatures();
  Input_top = sensors.getTempCByIndex(1);
  Input_mid = sensors.getTempCByIndex(0);
  PID_top.Compute();
  PID_mid.Compute();
  estadonivelTOP = digitalRead(niveltop);
  estadonivelMID = digitalRead(nivelmid);
  if (estadonivelTOP == HIGH)
  {
    analogWrite(10,Output_top);
  }
    if (estadonivelTOP == LOW)
  {
    analogWrite(10,LOW);
  }
  if (estadonivelMID == HIGH)
  {
    analogWrite(11,Output_mid);
  }
    if (estadonivelMID == LOW)
  {
    analogWrite(11,LOW);
  }
}
void subir()
{
  currentMillis = millis();
  if (currentMillis - MillisADA >= 120000)
   {
    //DHT
      //DHT Superior
      Serial1.print("a");     
      Serial1.println(hs);   
      Serial1.print("b");     
      Serial1.println(ts);    
      //DHT Inferior
      Serial1.print("c");     
      Serial1.println(hi);
      Serial1.print("d");     
      Serial1.println(ti);    
    //CO2
      //CO2 Superior
      Serial1.print("e");
      Serial1.println(porcentaje_s);    
      //CO2 Inferior
      Serial1.print("f");
      Serial1.println(porcentaje_i);    
    //LDR
      //LDR Superior
      Serial1.print("g");
      Serial1.println(valor_ldr_s);    
      //LDR Inferior
      Serial1.print("h");
      Serial1.println(valor_ldr_i);    
    //DS18B20
      Serial1.print("i");
      Serial1.println(sensors.getTempCByIndex(1));     
      Serial1.print("j");
      Serial1.println(sensors.getTempCByIndex(0));         
      Serial1.print("k");
      Serial1.println(sensors.getTempCByIndex(2));   
    //PH
      Serial1.print("l");
      Serial1.println(valor_pH,2);  
    //EC
      Serial1.print("m");
      Serial1.println(ECcurrent,2);      
    //FLUJO
      //Flujo TOP   
      Serial1.print("n");
      Serial1.print(l_min_1, DEC);     
      //Flujo MID
      Serial1.print("o");
      Serial1.print(l_min_2, DEC);     
      //Flujo BOT
      Serial1.print("p");
      Serial1.print(l_min_3, DEC);   
   }
    DateTime now = RTC.now();
    Serial.print("Fecha: ");
    Serial.print(now.day(), DEC); // Dia
    Serial.print('/');
    Serial.print(now.month(), DEC); // Mes
    Serial.print('/');    
    Serial.print(now.year(), DEC); // Año
    Serial.println(' ');
    Serial.print("Hora: ");
    Serial.print(now.hour(), DEC); // Horas
    Serial.print(':');
    Serial.print(now.minute(), DEC); // Minutos
    Serial.println(' ');
    Serial.print("Humedad Superior: ");
    Serial.println(hs);
    Serial.print("Temperatura Superior: ");
    Serial.println(ts);
    Serial.print("Humedad Inferior: ");
    Serial.println(hi); 
    Serial.print("Temperatura Inferior: ");
    Serial.println(ti);
    Serial.print("CO2 SUPERIOR: ");
    Serial.println(porcentaje_s);
    Serial.print("CO2 INFERIOR: ");
    Serial.println(porcentaje_i);
    Serial.print("LDR superior: ");
    Serial.println(valor_ldr_s);
    Serial.print("LDR Inferior: ");
    Serial.println(valor_ldr_i);
    Serial.print("Temperatura Agua TOP: ");
    Serial.println(sensors.getTempCByIndex(1));
    Serial.print("Temperatura Agua MID: ");
    Serial.println(sensors.getTempCByIndex(0));
    Serial.print("Temperatura Agua BOT: ");
    Serial.println(sensors.getTempCByIndex(2));
    Serial.print("pH: ");
    Serial.println(valor_pH,2);
    Serial.print("EC: ");
    Serial.print(ECcurrent,2);  //two decimal
    Serial.println(" ms/cm");
    Serial.print("FLujo TOP: ");
    Serial.print(l_min_1, DEC); 
    Serial.println(" L/min ");
    Serial.print("FLujo MID: ");
    Serial.print(l_min_2, DEC); 
    Serial.println(" L/min ");
    Serial.print("FLujo BOT: ");
    Serial.print(l_min_3, DEC); 
    Serial.println(" L/min ");
    Serial.print("PID TOP: Set Point: ");  
    Serial.print(Setpoint_top);
    Serial.print("\t");
    Serial.print("Temperatura: ");
    Serial.print(Input_top);
    Serial.print("\t");
    Serial.print("Salida:");  
    Serial.println(Output_top);
    Serial.print("PID MID: Set Point: ");  
    Serial.print(Setpoint_mid);
    Serial.print("\t");
    Serial.print("Temperatura: ");
    Serial.print(Input_mid);
    Serial.print("\t");
    Serial.print("Salida:");
    Serial.println(Output_mid);
    Serial.print("Estado Nivel Top: ");
    Serial.print(estadonivelTOP);
    Serial.print("  Mid: ");
    Serial.println(estadonivelMID);  
    Serial.print("Estado Bomba Bot: ");
    Serial.print(rele1);
    Serial.print("  Mid: ");
    Serial.print(rele2);
    Serial.print("  Top: ");
    Serial.println(rele3);  
    Serial.println("###########################");      
}
void control_luz() /*FALTA HACER INDEPENDIENTE LUZ TOP DE LUZ MID EN EL CASO DE REQUERIR DISTINTOS TIEMPOS AHORA FUNCIONAN AL MISMO TIEMPO*/
{
    if (currentMillis - previousMillis >= 480000 && toggleON)
   {
    toggleON = 0;
    toggleOFF = 1;
    digitalWrite(rele8, LOW); // LUZ TOP
    previousMillis = currentMillis; 
   }
     if (currentMillis - previousMillis >= 960000 && toggleOFF)
   {
    toggleON = 1;
    toggleOFF = 0;
    digitalWrite(rele8, HIGH); // LUZ TOP
    previousMillis = currentMillis; 
   }
   if (currentMillis - previousMillis >= 370000 && toggleON)
   {
    toggle2ON = 0;
    toggle2OFF = 1;
    digitalWrite(rele7, LOW); // LUZ MID    
    previousMillis = currentMillis; 
   }
     if (currentMillis - previousMillis >= 500000 && toggleOFF)
   {
    toggle2ON = 1;
    toggle2OFF = 0;
    digitalWrite(rele7, HIGH); // LUZ MID    
    previousMillis = currentMillis; 
   }
}
void control_llenado()
{
  estadonivelTOP = digitalRead(niveltop);
  estadonivelMID = digitalRead(nivelmid);
  estadonivelBOT = digitalRead(nivelbot);
  
unsigned long currentMillis = millis();

  if (currentMillis - toggleMillis >= intervalo_llenado && toggleLL)
   {
    toggleLL = 0;
    toggleV = 1;
    llenado = 0;
    toggleMillis = currentMillis; 
   }
     if (currentMillis - toggleMillis >= intervalo_vaciado && toggleV)
   {
    toggleLL = 1;
    toggleV = 0;
    llenado = 1;
    toggleMillis = currentMillis; 
   }
   
 if (llenado)
  {
    if (estadonivelTOP == LOW && estadoTOP)
    {
      estadoTOP = 1;
      estadoMID = 0;
      digitalWrite(rele1, LOW); //BOMBA BOT
      digitalWrite(rele2, HIGH); // BOMBA MID
      digitalWrite(rele3, HIGH); // BOMBA TOP
      digitalWrite(rele4, HIGH); // V2
      digitalWrite(rele5, LOW); // V1
      digitalWrite(rele6, HIGH); // V3 
    }
    if (estadonivelTOP == HIGH && estadoTOP)
    {
      estadoTOP = 0;
      estadoMID = 1;
      digitalWrite(rele1, HIGH); //BOMBA BOT
      digitalWrite(rele2, HIGH); // BOMBA MID
      digitalWrite(rele3, HIGH); // BOMBA TOP
      digitalWrite(rele4, HIGH); // V2
      digitalWrite(rele5, HIGH); // V1
      digitalWrite(rele6, HIGH); // V3
    }
    if (estadonivelMID == LOW && estadoMID)
    {
      estadoTOP = 0;
      estadoMID = 1;  
      digitalWrite(rele1, LOW); //BOMBA BOT
      digitalWrite(rele2, HIGH); // BOMBA MID
      digitalWrite(rele3, HIGH); // BOMBA TOP
      digitalWrite(rele4, LOW); // V2
      digitalWrite(rele5, HIGH); // V1
      digitalWrite(rele6, HIGH); // V3
    }
    if (estadonivelMID == HIGH && estadoMID)
    {
      estadoTOP = 1;
      estadoMID = 0;
      digitalWrite(rele1, HIGH); //BOMBA BOT
      digitalWrite(rele2, HIGH); // BOMBA MID
      digitalWrite(rele3, HIGH); // BOMBA TOP
      digitalWrite(rele4, HIGH); // V2
      digitalWrite(rele5, HIGH); // V1
      digitalWrite(rele6, HIGH); // V3
    }
  }
   
  if (!llenado)
   { 
    if (estadonivelTOP == HIGH)
    {
      vaciadoTOP = 1;
      vaciadoMID = 0 ;
      digitalWrite(rele1, HIGH); //BOMBA BOT
      digitalWrite(rele2, HIGH); // BOMBA MID
      digitalWrite(rele3, LOW); // BOMBA TOP
      digitalWrite(rele4, HIGH); // V2
      digitalWrite(rele5, HIGH); // V1
      digitalWrite(rele6, LOW); // V3 
    }
   if (currentMillis - intervalo >= 6000)
   { 
    if (estadonivelTOP == LOW && vaciadoTOP && l_min_1<=1)
    {
      vaciadoTOP = 0;
      vaciadoMID = 1;
      digitalWrite(rele1, HIGH); //BOMBA BOT
      digitalWrite(rele2, HIGH); // BOMBA MID
      digitalWrite(rele3, HIGH); // BOMBA TOP
      digitalWrite(rele4, HIGH); // V2
      digitalWrite(rele5, HIGH); // V1
      digitalWrite(rele6, HIGH); // V3 
    }
    intervalo = currentMillis;
   }
   
    if (estadonivelMID == HIGH && vaciadoMID)
    {
      vaciadoTOP = 0;
      vaciadoMID = 1;
      digitalWrite(rele1, HIGH); //BOMBA BOT
      digitalWrite(rele2, LOW); // BOMBA MID
      digitalWrite(rele3, HIGH); // BOMBA TOP
      digitalWrite(rele4, HIGH); // V2
      digitalWrite(rele5, HIGH); // V1
      digitalWrite(rele6, LOW); // V3 
    }
    
    
    if (estadonivelMID == LOW && vaciadoMID && l_min_2<=1)
    {
      vaciadoTOP = 1;
      vaciadoMID = 0;
      digitalWrite(rele1, HIGH); //BOMBA BOT
      digitalWrite(rele2, HIGH); // BOMBA MID
      digitalWrite(rele3, HIGH); // BOMBA TOP
      digitalWrite(rele4, HIGH); // V2
      digitalWrite(rele5, HIGH); // V1
      digitalWrite(rele6, HIGH); // V3 
    }
       
     
   } 
}
void respaldo()
{
  
  DateTime now = RTC.now();
  currentMillis = millis();
        if (currentMillis - Millisprevios >= 25000)
  {
    Millisprevios = currentMillis;
    if (SD.exists("data.csv")==1) 
    {
    myFile = SD.open("data.csv", FILE_WRITE); 
    myFile.print(now.day(), DEC); // Dia
    myFile.print('/');
    myFile.print(now.month(), DEC); // Mes
    myFile.print('/');    
    myFile.print(now.year(), DEC); // Año
    myFile.print(';');
    myFile.print(now.hour(), DEC); // Horas
    myFile.print(':');
    myFile.print(now.minute(), DEC); // Minutos
    myFile.print(";");    
	//DHT
    //DHT Superior
    myFile.print(hs);
    myFile.print(";");
    myFile.print(ts);
    myFile.print(";");
    //DHT Inferior
    myFile.print(hi);
    myFile.print(";");
    myFile.print(ti);
    myFile.print(";");
  //CO2
    //CO2 Superior
    myFile.print(porcentaje_s);
    myFile.print(";");
    //CO2 Inferior
    myFile.print(porcentaje_i);
    myFile.print(";");
  //LDR
    //LDR Superior
    myFile.print(valor_ldr_s);
    myFile.print(";");
    //LDR Inferior
    myFile.print(valor_ldr_i);
    myFile.print(";");
  //DS18B20
  myFile.print(sensors.getTempCByIndex(1));
  myFile.print(";");   
  myFile.print(sensors.getTempCByIndex(0));
  myFile.print(";");       
  myFile.print(sensors.getTempCByIndex(2));
  myFile.print(";"); 
  //PH
  myFile.print(valor_pH,2);
  myFile.print(";");
  //EC
  myFile.print(ECcurrent,2);
  myFile.print(";");
  //FLUJO
    //Flujo TOP   
    myFile.print(l_min_1, DEC);
    myFile.print(";"); 
    //Flujo MID
    myFile.print(l_min_2, DEC);
    myFile.print(";"); 
    //Flujo BOT
    myFile.print(l_min_3, DEC);
    myFile.print(";"); 
  //PID
    //TOP
  myFile.print(Setpoint_top);
  myFile.print(";");
  myFile.print(Output_top);
  myFile.print(";");
    //MID
  myFile.print(Setpoint_mid);
  myFile.print(";");
  myFile.print(Output_mid);
  myFile.print(";");
  //NIVEL
  myFile.print(estadonivelTOP);  
  myFile.print(";");
  myFile.print(estadonivelMID);  
  myFile.println();     
  myFile.close(); 
  Serial.println("Se Imprimen Datos...");  
  } 
  if (SD.exists("data.csv")==0) 
  {
  myFile = SD.open("data.csv", FILE_WRITE);
    myFile.print("Fecha");
	myFile.print(";");
    myFile.print("Hora");
	myFile.print(";");
    myFile.print("DHT.S.H");
	myFile.print(";");
    myFile.print("DHT.S.T");
	myFile.print(";");
    myFile.print("DHT.I.H");
	myFile.print(";");
    myFile.print("DHT.I.T");
	myFile.print(";");
    myFile.print("CO2.S");
	myFile.print(";");
    myFile.print("CO2.I");
	myFile.print(";");
    myFile.print("LDR.S");
	myFile.print(";");
    myFile.print("LDR.I");
	myFile.print(";");
    myFile.print("T.Top");
	myFile.print(";");
    myFile.print("T.Mid");
	myFile.print(";");
    myFile.print("T.Bot");
	myFile.print(";");
    myFile.print("pH");
	myFile.print(";");
    myFile.print("EC");
	myFile.print(";");
	  myFile.print("Flujo.Top");
	myFile.print(";");
	  myFile.print("Flujo.Mid");
	myFile.print(";");
	  myFile.print("Flujo.Bot");
	myFile.print(";");
	  myFile.print("PI.S/P.Top");
	myFile.print(";");
	  myFile.print("PI.Salida.Top");
	myFile.print(";");	
	  myFile.print("PI.S/P.Mid");
	myFile.print(";");
	  myFile.print("PI.Salida.Mid");
  myFile.print(";");
    myFile.print("Estado Nivel Top");
  myFile.print(";");
    myFile.print("Estado Nivel Mid");
  myFile.print(";"); 
    myFile.print("Estado Nivel Bot");
  myFile.print(";");
    myFile.print("Estado Bomba Top"); 
  myFile.print(";");
    myFile.print("Estado Bomba Mid");
  myFile.print(";"); 
    myFile.print("Estado Bomba Bot");      
	myFile.println();
  myFile.close();      
  Serial.println("Se Imprime Cabecera...");  
  }   
  Serial.println("Respaldo Guardado...");
  Serial.println("###########################"); 
  } 
}
