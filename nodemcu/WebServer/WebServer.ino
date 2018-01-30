#include <ESP8266WiFi.h>



#include <EEPROM.h>
#include <OneWire.h>



#include "./DallasTemperature.h"

#include "Configuration.h"
#include "Node.h"
#include "ImGrowthHTTPServer.h"






Configuration configuration;
Node node;


OneWire oneWire(configuration.oneWirePin);
DallasTemperature temperatureSensor(&oneWire);


ImGrowthHTTPServer HTTPServer;







void setup(void) {
	Configuration configuration;

	EEPROM.begin(512);

	Serial.begin(configuration.serialSpeed);
	Serial.printf("initialize");

	temperatureSensor.begin();
	node.setSensor(temperatureSensor);


	HTTPServer.setNode(node);
	HTTPServer.initialize();






}




unsigned long previousMillis = 0;

void loop(void) {


	// a chaque iteration, on appelle handleClient pour que les requetes soient traitees
	HTTPServer.listen();


	delay(configuration.globalDelay);


}



















