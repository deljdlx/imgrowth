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


	this->setSerialPinsToGPIO();


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
	this->dnsServer.processNextRequest();
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





int Node::getHumidity(int input) {

	this->enableInput(input);
	int humidity = this->humiditySensor.get();
	Serial.println("Humidity : "+humidity);

	return humidity;
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


void Node::watering(int output) {
	this->enableOutput(output);
	delay(10000);
	this->enableOutput(0);
}


int * Node::checkHumidity(int * tresholds) {

	int i = 0;
	int humidity;

	String response = "";

	int * enabledOutputs;
	enabledOutputs = (int *) malloc(sizeof(int)*4);

	for(i = 0 ; i<4 ; i++) {
		enabledOutputs[i] = 0;
	}

	for(i = 0 ; i<4 ; i++) {
		humidity = this->getHumidity(i);

		if(humidity > tresholds[i] && tresholds[i]) {

			Serial.println("==================");
			Serial.println("Water !");
			Serial.println(String(tresholds[i]));
			Serial.println(String(humidity));
			Serial.println("==================");

			enabledOutputs[i] = 1;
			response += "Water on ";
			response += String(i);
			response += "\n";
		}
		else {
			enabledOutputs[i] = 0;
		}

		//Serial.println(response);
	}

	return enabledOutputs;


}

