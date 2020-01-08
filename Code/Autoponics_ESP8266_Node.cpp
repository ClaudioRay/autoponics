// Adafruit IO REST API access with ESP8266
//
// For use with ESP8266 Arduino from:
//   https://github.com/esp8266/Arduino
//
// Works great with ESP8266 modules like the Adafruit Huzzah ESP:
//  ----> https://www.adafruit.com/product/2471
//
// Written by Tony DiCola for Adafruit Industries.  
// MIT license, all text above must be included in any redistribution.
#include <ESP8266WiFi.h>
#include "Adafruit_IO_Client.h"


// Configure WiFi access point details.
#define WLAN_SSID       "Guaripolo"
#define WLAN_PASS       "Internet1"

// Configure Adafruit IO access.
#define AIO_KEY         "57b0c505622e76376f141c0f1e0e6447dca7d4cf"
WiFiClient client; // Create an ESP8266 WiFiClient class to connect to the AIO server.
Adafruit_IO_Client aio = Adafruit_IO_Client(client, AIO_KEY);

//variables codigo obtencion
char data[9];
char dato;
int i;
char dataa[6];
char datab[6];
char datac[6];
char datad[6];
char datae[6];
char dataf[6];
char datag[6];
char datah[6];
char datai[6];
char dataj[6];
char datak[6];
char datal[6];
char datam[6]; //correspondiente a EC
//char datan[6]; //correspondiente a flujo superior
//char datao[6]; //correspondiente a flujo inferior
//char datap[6]; //correspondiente a flujo base
float dhs;
float dts;
float dhi;
float dti;
float dco2s;
float dco2i;
int dldrs;
int dldri;
float dt1;
float dt2;
float dt3;
float dph;
float dec; // dato EC
int dfs; // dato flujo superior
int dfi; // dato flujo inferior
int dfb; // dato flujo base
unsigned long currentMillis;
unsigned long toggle=0;

Adafruit_IO_Feed reinicio = aio.getFeed("reinicio");
Adafruit_IO_Feed adahs = aio.getFeed("adahs");
Adafruit_IO_Feed adats = aio.getFeed("adats");
Adafruit_IO_Feed adahi = aio.getFeed("adahi");
Adafruit_IO_Feed adati = aio.getFeed("adati");
Adafruit_IO_Feed adaco2s = aio.getFeed("adaco2s");
Adafruit_IO_Feed adaco2i = aio.getFeed("adaco2i");
Adafruit_IO_Feed adaldrs = aio.getFeed("adaldrs");
Adafruit_IO_Feed adaldri = aio.getFeed("adaldri");
Adafruit_IO_Feed adat1 = aio.getFeed("adat1");
Adafruit_IO_Feed adat2 = aio.getFeed("adat2");
Adafruit_IO_Feed adat3 = aio.getFeed("adat3");
Adafruit_IO_Feed adaph = aio.getFeed("adaph");
Adafruit_IO_Feed adaec = aio.getFeed("adaec");
Adafruit_IO_Feed adafs = aio.getFeed("adafs");
Adafruit_IO_Feed adafi = aio.getFeed("adafi");
Adafruit_IO_Feed adafb = aio.getFeed("adafb");

unsigned int count = 0;

void setup() {
  Serial.begin(115200);
  delay(5);
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");  
  Serial.println("IP address: "); Serial.println(WiFi.localIP());  
  aio.begin();
}

void loop() 
{
   
  
  if (Serial.available() > 0) 
        {
            dato = Serial.read();                
            data[i]=dato;
            if(data[i]=='\n')
            {
              
                i=0;
                
                switch (data[0])
                {
                    case 'a':
                    
                      for (int i=1; i<=6; i++)
                      {
                        
                          dataa[i-1]=data[i];
                           dhs =atof(dataa);                
                      }
                      break;
                    case 'b':
                      for (int i=1; i<=6; i++)
                      {
                          datab[i-1]=data[i];
                           dts =atof(datab);                
                      }
                      break;
                    case 'c':
                      for (int i=1; i<=6; i++)
                      {
                          datac[i-1]=data[i];
                           dhi =atof(datac);                
                      }
                      break;
                    case 'd':
                      for (int i=1; i<=6; i++)
                      {
                          datad[i-1]=data[i];
                           dti =atof(datad);                
                      }
                      break;
                    case 'e':
                      for (int i=1; i<=6; i++)
                      {
                          datae[i-1]=data[i];
                           dco2s =atof(datae);                
                      }
                      break;
                    case 'f':
                      for (int i=1; i<=6; i++)
                      {
                          dataf[i-1]=data[i];
                           dco2i =atof(dataf);                
                      }
                      break;  
                    case 'g':
                      for (int i=1; i<=6; i++)
                      {
                          datag[i-1]=data[i];
                           dldrs =atof(datag);                
                      }
                      break;
                    case 'h':
                      for (int i=1; i<=6; i++)
                      {
                          datah[i-1]=data[i];
                           dldri =atoi(datah);                
                      }
                      break;
                    case 'i':
                      for (int i=1; i<=6; i++)
                      {
                          datai[i-1]=data[i];
                           dt1 =atoi(datai);                
                      }
                      break;
                      case 'j':
                      for (int i=1; i<=6; i++)
                      {
                          dataj[i-1]=data[i];
                           dt2 =atoi(dataj);                
                      }
                      break;
                      case 'k':
                      for (int i=1; i<=6; i++)
                      {
                          datak[i-1]=data[i];
                           dt3 =atoi(datak);                
                      }
                      break;
                      case 'l':
                      for (int i=1; i<=6; i++)
                      {
                          datal[i-1]=data[i];
                           dph =atof(datal);                
                      }
                      break;
                      case 'm':
                      for (int i=1; i<=6; i++)
                      {
                          datal[i-1]=data[i];
                           dec =atof(datal);                
                      }
                      break;                      
                }
                
           Serial.print("Humedad Superior: ");
           Serial.println(dhs);
           Serial.print("Temperatura Superior: ");
           Serial.println(dts);
           Serial.print("Humedad Inferior: ");
           Serial.println(dhi); 
           Serial.print("Temperatura Inferior: ");
           Serial.println(dti);
           Serial.print("Nivel de CO2 Superior: ");
           Serial.println(dco2s);
           Serial.print("Nivel de CO2 Inferior: ");
           Serial.println(dco2i);
           Serial.print("Luminosidad Superior: ");
           Serial.println(dldrs);
           Serial.print("Luminosidad Inferior: ");
           Serial.println(dldri);
           Serial.print("Tempertura 1: ");
           Serial.println(dt1);
           Serial.print("Tempertura 2: ");
           Serial.println(dt2);
           Serial.print("Tempertura 3: ");
           Serial.println(dt3);
           Serial.print("Nivel de pH: ");
           Serial.println(dph);
           Serial.print("Nivel de EC: ");
           Serial.println(dec);
           Serial.print("Flujo Superior: ");
           Serial.println(dfs);
           Serial.print("Flujo Inferior: ");
           Serial.println(dfi);
           Serial.print("Flujo Base: ");
           Serial.println(dfb);
            
        }
        else 
        {
            i++;
        }
     }

FeedData latest = reinicio.receive();
  if (latest.isValid()) 
  {
    Serial.print(F("Leer: ")); Serial.println(latest);
    // By default the received feed data item has a string value, however you
    // can use the following functions to attempt to convert it to a numeric
    // value like an int or float.  Each function returns a boolean that indicates
    // if the conversion succeeded, and takes as a parameter by reference the
    // output value.
    int i;
    if (latest.intValue(&i)) 
    {
      Serial.print(F("Valor: ")); Serial.println(i, DEC);
    }
    // Other functions that you can use include:
    //  latest.uintValue() (unsigned int)
    //  latest.longValue() (long)
    //  latest.ulongValue() (unsigned long)
    //  latest.floatValue() (float)
    //  latest.doubleValue() (double)
  }
  else 
  {
    Serial.print(F("Failed to receive the latest feed value!"));
  }
  
  currentMillis = millis();     
 if(currentMillis - toggle >= 120000)
    { 
      toggle = currentMillis;  
      
      // se realiza ping a adafruit para coonfirmar la conexion

      
    // con lo anterior, se suben los datos
      //DHS
       if (! adahs.send(dhs))
        Serial.println(F("Error en publicar la Humedad Superior"));
      else
        Serial.println(F("Humedad Superior Publicada!"));
      //DTS 
      if (! adats.send(dts))
        Serial.println(F("Error en publicar la Temperatura Superior"));
      else
        Serial.println(F("Temperatura Superior Publicada!"));
       // DHI
       if (! adahi.send(dhi))
        Serial.println(F("Error en publicar la Humedad Inferior"));
      else
        Serial.println(F("Humedad Inferior Publicada!")); 
        //DTI
      if (! adati.send(dti))
        Serial.println(F("Error en publicar la Temperatura Inferior"));
      else
        Serial.println(F("Temperatura Inferior Publicada!"));
        //DCO2S
      if (! adaco2s.send(dco2s))
        Serial.println(F("Error en la publicacion de CO2 Superior"));
      else
        Serial.println(F("CO2 Superior Publicado!"));
        //DCO2I
      if (! adaco2i.send(dco2i))
        Serial.println(F("Error en la publicacion de CO2 Inferior"));
      else
        Serial.println(F("CO2 Inferior Publicado!"));
        //DLDRS
          if (! adaldrs.send(dldrs))
        Serial.println(F("Error en la Publicacion del LDR Superior"));
      else
        Serial.println(F("LDR Superior Publicado!"));
        //DLDRI
          if (! adaldri.send(dldri))
        Serial.println(F("Error en la Publicacion del LDR Inferior"));
      else
        Serial.println(F("LDR Inferior Publicado!"));
        //DT1
      if (! adat1.send(dt1))
        Serial.println(F("Error en la Publicacion de T1"));
      else
        Serial.println(F("T1 Publicado!"));
        //DT2
        if (! adat2.send(dt2))
        Serial.println(F("Error en la Publicacion de T2"));
      else
        Serial.println(F("T2 Publicado!"));
        //DT2
        if (! adat3.send(dt3))
        Serial.println(F("Error en la Publicacion de T3"));
      else
        Serial.println(F("T3 Publicado!"));
        //DPH
        if (! adaph.send(dph))
        Serial.println(F("Error en Publicar pH"));
      else
        Serial.println(F("pH Publicado!"));
        //DEC
        if (! adaec.send(dec))
        Serial.println(F("Error en Publicar EC"));
      else
        Serial.println(F("EC Publicado!"));
        //DFS
        if (! adafs.send(dfs))
        Serial.println(F("Error en Publicar Flujo Superior"));
      else
        Serial.println(F("Flujo Superior Publicado!"));
        //DFI
        if (! adafi.send(dfi))
        Serial.println(F("Error en Publicar Flujo Inferior"));
      else
        Serial.println(F("Flujo Inferior Publicado!"));
        //DFB
        if (! adafb.send(dfb))
        Serial.println(F("Error en Publicar Flujo Base"));
      else
        Serial.println(F("Flujo Base Publicado!"));          
    }
}