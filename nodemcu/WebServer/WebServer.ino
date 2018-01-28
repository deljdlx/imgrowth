#include <ESP8266WiFi.h>

#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>


#include <ESP8266mDNS.h>


#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>

#include <OneWire.h>
#include "DallasTemperature.h"





#include "Configuration.h"
#include "Node.h"

#include "NodeMCU.h"

#include <EEPROM.h>



const int ledPin = 12;
int ledState = LOW;




Configuration configuration;
Node node;


OneWire oneWire(configuration.oneWirePin);
DallasTemperature temperatureSensor(&oneWire);





void initialize(void) {

	Configuration configuration;

	EEPROM.begin(512);

	Serial.begin(configuration.serialSpeed);
	Serial.printf("initialize");


	node.initialize();





  temperatureSensor.begin();
  node.setSensor(temperatureSensor);


  node.initializeServer();
}






void setup(void) {
	initialize();
	node.enableInput(configuration.startListenInput);
	node.enableOutput(configuration.startWriteOutput);


}




unsigned long previousMillis = 0;

void loop(void) {


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


	//node.getLight();
	//node.getTemperature();
	//node.getHumidity();




  //analogWrite(LEDpin, dutycycle);
  //delay(100);







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



















