#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// WiFi parameters
#define WLAN_SSID       "Guaripolo"
#define WLAN_PASS       "Internet1"

// Adafruit IO
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "ClaudioRay"
#define AIO_KEY         "57b0c505622e76376f141c0f1e0e6447dca7d4cf"


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
//char datam[6]; //correspondiente a EC
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
// Functions
void connect();

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

// Store the MQTT server, client ID, username, and password in flash memory.
const char MQTT_SERVER[] PROGMEM    = AIO_SERVER;

// Set a unique MQTT client ID using the AIO key + the date and time the sketch
// was compiled (so this should be unique across multiple devices for a user,
// alternatively you can manually set this to a GUID or other random value).
const char MQTT_CLIENTID[] PROGMEM  = AIO_KEY __DATE__ __TIME__;
const char MQTT_USERNAME[] PROGMEM  = AIO_USERNAME;
const char MQTT_PASSWORD[] PROGMEM  = AIO_KEY;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, AIO_SERVERPORT, MQTT_CLIENTID, MQTT_USERNAME, MQTT_PASSWORD);/****************************** Feeds ***************************************/

// Setup feeds

const char ADAHS_FEED[] PROGMEM = AIO_USERNAME "/feeds/adahs";
Adafruit_MQTT_Publish adahs = Adafruit_MQTT_Publish(&mqtt, ADAHS_FEED);

const char ADATS_FEED[] PROGMEM = AIO_USERNAME "/feeds/adats";
Adafruit_MQTT_Publish adats = Adafruit_MQTT_Publish(&mqtt, ADATS_FEED);

const char ADAHI_FEED[] PROGMEM = AIO_USERNAME "/feeds/adahi";
Adafruit_MQTT_Publish adahi = Adafruit_MQTT_Publish(&mqtt, ADAHI_FEED);

const char ADATI_FEED[] PROGMEM = AIO_USERNAME "/feeds/adati";
Adafruit_MQTT_Publish adati = Adafruit_MQTT_Publish(&mqtt, ADATI_FEED);

const char ADACO2S_FEED[] PROGMEM = AIO_USERNAME "/feeds/adaco2s";
Adafruit_MQTT_Publish adaco2s = Adafruit_MQTT_Publish(&mqtt, ADACO2S_FEED);

const char ADACO2I_FEED[] PROGMEM = AIO_USERNAME "/feeds/adaco2i";
Adafruit_MQTT_Publish adaco2i = Adafruit_MQTT_Publish(&mqtt, ADACO2I_FEED);

const char ADALDRS_FEED[] PROGMEM = AIO_USERNAME "/feeds/adaldrs";
Adafruit_MQTT_Publish adaldrs = Adafruit_MQTT_Publish(&mqtt, ADALDRS_FEED);

const char ADALDRI_FEED[] PROGMEM = AIO_USERNAME "/feeds/adaldri";
Adafruit_MQTT_Publish adaldri = Adafruit_MQTT_Publish(&mqtt, ADALDRI_FEED);

const char ADAT1_FEED[] PROGMEM = AIO_USERNAME "/feeds/adat1";
Adafruit_MQTT_Publish adat1 = Adafruit_MQTT_Publish(&mqtt, ADAT1_FEED);

const char ADAT2_FEED[] PROGMEM = AIO_USERNAME "/feeds/adat2";
Adafruit_MQTT_Publish adat2 = Adafruit_MQTT_Publish(&mqtt, ADAT2_FEED);

const char ADAT3_FEED[] PROGMEM = AIO_USERNAME "/feeds/adat3";
Adafruit_MQTT_Publish adat3 = Adafruit_MQTT_Publish(&mqtt, ADAT3_FEED);

const char ADAPH_FEED[] PROGMEM = AIO_USERNAME "/feeds/adaph";
Adafruit_MQTT_Publish adaph = Adafruit_MQTT_Publish(&mqtt, ADAPH_FEED);

const char ADAEC_FEED[] PROGMEM = AIO_USERNAME "/feeds/adaec";
Adafruit_MQTT_Publish adaec = Adafruit_MQTT_Publish(&mqtt, ADAEC_FEED);

const char ADAFS_FEED[] PROGMEM = AIO_USERNAME "/feeds/adafs";
Adafruit_MQTT_Publish adafs = Adafruit_MQTT_Publish(&mqtt, ADAFS_FEED);

const char ADAFI_FEED[] PROGMEM = AIO_USERNAME "/feeds/adafi";
Adafruit_MQTT_Publish adafi = Adafruit_MQTT_Publish(&mqtt, ADAFI_FEED);

const char ADAFB_FEED[] PROGMEM = AIO_USERNAME "/feeds/adafb";
Adafruit_MQTT_Publish adafb = Adafruit_MQTT_Publish(&mqtt, ADAFB_FEED);


/*************************** Sketch Code ************************************/

void setup() {


  Serial.begin(115200);
  //Serial.println(F("Adafruit IO Example"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  delay(10);
  Serial.print(F("Conectando con "));
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();

  Serial.println(F("Conectado mediante WiFi"));
  Serial.println(F("Direccion IP: "));
  Serial.println(WiFi.localIP());

  // connect to adafruit io
  connect();

}




void loop() {

          
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


     // se realiza un comparador millis ya que tener todo este segmento de codigo con un delay retrasa mucho la 
     // recopilacion de datos. por lo que se define que los datos sean subidos cada 2 minutos 
       
 currentMillis = millis();     
 if(currentMillis - toggle >= 120000)
    { 
      toggle = currentMillis;  
      
      // se realiza ping a adafruit para coonfirmar la conexion
      if(! mqtt.ping(3)) 
      {
        // en el caso de no estar conectados, se solicita la conexion
        if(! mqtt.connected())
          connect();
      }
      
    // con lo anterior, se suben los datos
      //DHS
       if (! adahs.publish(dhs))
        Serial.println(F("Error en publicar la Humedad Superior"));
      else
        Serial.println(F("Humedad Superior Publicada!"));
      //DTS 
      if (! adats.publish(dts))
        Serial.println(F("Error en publicar la Temperatura Superior"));
      else
        Serial.println(F("Temperatura Superior Publicada!"));
       // DHI
       if (! adahi.publish(dhi))
        Serial.println(F("Error en publicar la Humedad Inferior"));
      else
        Serial.println(F("Humedad Inferior Publicada!")); 
        //DTI
      if (! adati.publish(dti))
        Serial.println(F("Error en publicar la Temperatura Inferior"));
      else
        Serial.println(F("Temperatura Inferior Publicada!"));
        //DCO2S
      if (! adaco2s.publish(dco2s))
        Serial.println(F("Error en la publicacion de CO2 Superior"));
      else
        Serial.println(F("CO2 Superior Publicado!"));
        //DCO2I
      if (! adaco2i.publish(dco2i))
        Serial.println(F("Error en la publicacion de CO2 Inferior"));
      else
        Serial.println(F("CO2 Inferior Publicado!"));
        //DLDRS
          if (! adaldrs.publish(dldrs))
        Serial.println(F("Error en la Publicacion del LDR Superior"));
      else
        Serial.println(F("LDR Superior Publicado!"));
        //DLDRI
          if (! adaldri.publish(dldri))
        Serial.println(F("Error en la Publicacion del LDR Inferior"));
      else
        Serial.println(F("LDR Inferior Publicado!"));
        //DT1
      if (! adat1.publish(dt1))
        Serial.println(F("Error en la Publicacion de T1"));
      else
        Serial.println(F("T1 Publicado!"));
        //DT2
        if (! adat2.publish(dt2))
        Serial.println(F("Error en la Publicacion de T2"));
      else
        Serial.println(F("T2 Publicado!"));
        //DT2
        if (! adat3.publish(dt3))
        Serial.println(F("Error en la Publicacion de T3"));
      else
        Serial.println(F("T3 Publicado!"));
        //DPH
        if (! adaph.publish(dph))
        Serial.println(F("Error en Publicar pH"));
      else
        Serial.println(F("pH Publicado!"));
        //DEC
        if (! adaec.publish(dec))
        Serial.println(F("Error en Publicar EC"));
      else
        Serial.println(F("EC Publicado!"));
        //DFS
        if (! adafs.publish(dfs))
        Serial.println(F("Error en Publicar Flujo Superior"));
      else
        Serial.println(F("Flujo Superior Publicado!"));
        //DFI
        if (! adafi.publish(dfi))
        Serial.println(F("Error en Publicar Flujo Inferior"));
      else
        Serial.println(F("Flujo Inferior Publicado!"));
        //DFB
        if (! adafb.publish(dfb))
        Serial.println(F("Error en Publicar Flujo Base"));
      else
        Serial.println(F("Flujo Base Publicado!"));          
    }
}

// connect to adafruit io via MQTT
void connect() {

  Serial.print(F("Conectando con Adafruit.io... "));

  int8_t ret;

  while ((ret = mqtt.connect()) != 0) {

    switch (ret) {
      case 1: Serial.println(F("Protocolo Incorrecto")); break;
      case 2: Serial.println(F("ID Rechazada")); break;
      case 3: Serial.println(F("Server No Disponible")); break;
      case 4: Serial.println(F("Usuario/Clave Incorrectas")); break;
      case 5: Serial.println(F("No Autorizado")); break;
      case 6: Serial.println(F("Error en la Subscripcion")); break;
      default: Serial.println(F("Error en la Conexion")); break;
    }

    if(ret >= 0)
      mqtt.disconnect();

    Serial.println(F("Reconectando..."));
    delay(5000);

  }

  Serial.println(F("Conectado con Adafruit.io!"));

}