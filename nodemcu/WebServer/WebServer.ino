#include <ESP8266WiFi.h>



#include <EEPROM.h>
#include <OneWire.h>


#include "ArduinoJson.h"


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


	/*
	char json[] = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";
	StaticJsonBuffer<200> jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(json);
	const char* sensor = root["sensor"];
	long time          = root["time"];
	double latitude    = root["data"][0];
	double longitude   = root["data"][1];
	Serial.println(time);
	*/



}




unsigned long previousMillis = 0;

void loop(void) {


	// a chaque iteration, on appelle handleClient pour que les requetes soient traitees
	HTTPServer.listen();


	delay(configuration.globalDelay);


}



















