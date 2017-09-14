#include <ESP8266WiFi.h>

#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>


#include <ESP8266mDNS.h>


#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>


#define ONE_WIRE_BUS            D4
#include <OneWire.h>
#include "DallasTemperature.h"





#include "Configuration.h"
#include "Node.h"





const int pompePin =  5;
int pompeState = LOW;

const int PinAnalogiqueHumidite = 0;




const int ledPin = 12;
int ledState = LOW;





Node node;


OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature temperatureSensor(&oneWire);




void initialize(void) {

  Configuration configuration;

  Serial.begin(115200);
  Serial.printf("initialize");


	node.initialize();



  //pinMode(D9, OUTPUT);
  //pinMode(D10, OUTPUT);


  pinMode(pompePin, OUTPUT);


  pinMode(PinAnalogiqueHumidite, INPUT);
  pinMode(pompePin, OUTPUT);


    pinMode(5, OUTPUT);


    //pinMode(13, OUTPUT);
    //pinMode(15, OUTPUT);
    //pinMode(3, OUTPUT);



  node.wifiConnection(
    configuration.wifiSSID,
    configuration.wifiPassword
  );




  temperatureSensor.begin();
  node.setSensor(temperatureSensor);


  node.initializeServer();
}






void setup(void) {
  initialize();

  pinMode(ledPin, OUTPUT);

	node.enableInput(0);

	//digitalWrite(15, HIGH);
	//digitalWrite(3, HIGH);




	//
	//digitalWrite(8, HIGH);


}




unsigned long previousMillis = 0;

void loop(void) {



	Configuration configuration;



	if (ledState == LOW) {
		ledState = HIGH;
    } else {
		ledState = LOW;
    }

    //ledState = HIGH;

	//digitalWrite(ledPin, ledState);


	//int input = random(0,7);
	//Serial.println(input);



	// a chaque iteration, on appelle handleClient pour que les requetes soient traitees
	node.listen();
	node.getLight();
	node.getTemperature();
	node.getHumidity();







	delay(configuration.globalDelay);


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



















