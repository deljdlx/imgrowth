#include <string>
#include <OneWire.h>


#include "./CD4051B.h"
#include "./DallasTemperature.h"




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


void Node::reset(void) {
	this->enableOutput(this->configuration.resetOutput);
}





void Node::listen(void) {
	this->dnsServer.processNextRequest();
	this->irrigate();

}


void Node::irrigate(void) {

	unsigned long current = millis();

	bool watered = false;

	if(current - this->humidityLastTime0 > this->humidityCheckDelay) {
		watered = this->doWatering(0);
		this->humidityLastTime0 = millis();
		if(watered) {
			return;
		}
	}

	if(current - this->humidityLastTime1 > this->humidityCheckDelay) {
		watered = this->doWatering(1);
		this->humidityLastTime1 = millis();
		if(watered) {
			return;
		}
	}

	if(current - this->humidityLastTime2 > this->humidityCheckDelay) {
		watered = this->doWatering(2);
		this->humidityLastTime2 = millis();
		if(watered) {
			return;
		}
	}

	if(current - this->humidityLastTime3 > this->humidityCheckDelay) {
		watered = this->doWatering(3);
		this->humidityLastTime3 = millis();
		if(watered) {
			return;
		}
	}

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





bool Node::doWatering(int input) {

	int humidity = this->getHumidity(input);

	Serial.println(humidity);
	Serial.println(this->humidityTresholds[input]);


	if(humidity > this->humidityTresholds[input]) {
		Serial.println("Watering " + input);
		this->watering(input);
		return true;
	}
	else {
		Serial.println("Skipping " + input);
		return false;
	}


}


void Node::watering(int pompeIndex) {
	this->enableOutput(pompeIndex+1);
	delay(this->configuration.wateringDelay);
	this->enableOutput(0);
}


