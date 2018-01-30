#include <string>
#include <OneWire.h>


#include "./CD4051B.h"
#include "./DallasTemperature.h"
#include "./DNSServer.h"



#include "Node.h"


#include "Configuration.h"
#include "HumiditySensor.h"







Node::Node() {


	this->lightSensor= Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
	this->lightSensor.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */
	/* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
	//this->lightSensor.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
	this->lightSensor.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
	// tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */

}


void Node::initialize(void) {

	pinMode(this->configuration.analogInputPIN, INPUT);
	pinMode(this->configuration.lightPIN, OUTPUT);


	/*
	this->wifiAutoConnection(
		this->configuration.wifiSSID.c_str(),
		this->configuration.wifiPassword.c_str()
	);
	*/



	this->startHotspot(
		this->configuration.hotspotSSID.c_str()
		,this->configuration.hotspotPassword.c_str()
	);
}


void Node::wifiConnect(void) {
	this->wifiAutoConnection(
		this->configuration.wifiSSID.c_str(),
		this->configuration.wifiPassword.c_str()
	);
}



void Node::reset(void) {
	this->enableOutput(this->configuration.resetOutput);
}

void Node::startHotspot(const char * ssid, const char * password) {



	//WiFi.mode(WIFI_AP);
	this->hotspotIP = WiFi.softAPIP();


	Serial.print("\nAP IP address: "+this->configuration.hotspotSSID+"\n");
	Serial.println(this->hotspotIP);



	WiFi.softAPConfig(this->hotspotIP, this->hotspotIP, IPAddress(255, 255, 255, 0));

	WiFi.softAP(
		ssid
		//,password
	);





	const byte DNS_PORT = 53;
	this->dnsServer.setTTL(300);
	// set which return code will be used for all other domains (e.g. sending
	// ServerFailure instead of NonExistentDomain will reduce number of queries
	// sent by clients)
	// default is DNSReplyCode::NonExistentDomain
	this->dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);

	// start DNS server for a specific domain name
	this->dnsServer.start(DNS_PORT, "config.imgrowth", this->hotspotIP);


}





void Node::listen(void) {

	//Serial.println("dns listen");
	this->dnsServer.processNextRequest();


	//this->checkHumidity();
	//this->getLight();
	//this->ping();
}






float Node::getTemperature() {

	this->temperatureSensor.requestTemperatures();
	Serial.print(this->temperatureSensor.getTempCByIndex(0));
	Serial.println(" °");
	return this->temperatureSensor.getTempCByIndex(0);
}






float Node::getLight(void) {

	sensors_event_t event;
	this->lightSensor.getEvent(&event);

	/* Display the results (light is measured in lux) */
	if (event.light) {
		Serial.print(event.light);
		Serial.println(" lux");
		return event.light;
	}
	else {
		return 0;
	}
}


int Node::enableInput(int input) {
	this->inputMutiplexer.enable(input);
	return input;
}


int Node::enableOutput(int output) {
	this->outputMutiplexer.enable(output);
	return output;
}






void Node::setSensor(DallasTemperature sensor) {
	this->temperatureSensor=sensor;
}




void Node::checkHumidity(void) {


	int humidity;

	unsigned long humidityTimer = millis();

	if (humidityTimer - this->previousHumidyTime >= this->configuration.humidyInterval) {
		this->previousHumidyTime = humidityTimer;
		humidity = this->getHumidity();
		Serial.println(humidity);
	}
}







int Node::getHumidity(void) {
	int humidity = this->humiditySensor.get();
	Serial.println(humidity);
	return humidity;
}

