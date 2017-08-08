#include <ESP8266WiFi.h>

#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>


#include <ESP8266mDNS.h>


#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>



#include "Configuration.h"
#include "Node.h"




#define ONE_WIRE_BUS            D4
#include <OneWire.h>
#include "DallasTemperature.h"


#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>




OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensor(&oneWire);


Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);






const int pompePin =  5;
int pompeState = LOW;

const int PinAnalogiqueHumidite = 0;


Node node;





void initialize(void) {

  Configuration configuration;

  Serial.begin(115200);
  Serial.printf("initialize");






  /* You can also manually set the gain or enable auto-gain support */
  // tsl.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
  // tsl.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
  tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */

  /* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */

  /* Update these values depending on what you've set above! */
  Serial.println("------------------------------------");
  Serial.print  ("Gain:         "); Serial.println("Auto");
  Serial.print  ("Timing:       "); Serial.println("13 ms");
  Serial.println("------------------------------------");







  pinMode(PinAnalogiqueHumidite, INPUT);
  pinMode(pompePin, OUTPUT);

  node.wifiConnection(
    configuration.wifiSSID,
    configuration.wifiPassword
  );

  sensor.begin();
  node.setSensor(sensor);


  node.initializeServer();
}






void setup(void) {
  initialize();
}




unsigned long previousMillis = 0;

void loop(void) {



  // a chaque iteration, on appelle handleClient pour que les requetes soient traitees
  node.listen();




  delay(1000);


/*
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    hsol = getHumidity();
    Serial.println(hsol);
    if (hsol > 600) {
      digitalWrite(pompePin, HIGH);
    }
    else {
      digitalWrite(pompePin, LOW);
    }
  }
  */
}



















