#include <string>
#include <OneWire.h>

//#include <TimeLib.h>
//#include <NtpClientLib.h>



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

	Serial.println("\nInitializing Node");


	this->setSerialPinsToGPIO();



	//NTP.begin("pool.ntp.org", 1, true);
	//NTP.setInterval(60);
	//Serial.println(NTP.getTimeDateString ());





	Serial.println("\nInitializing Pins");
	pinMode(this->configuration.analogInputPIN, INPUT);
	pinMode(this->configuration.lightPIN, OUTPUT);



	this->wifiAutoConnection(
		this->configuration.wifiSSID.c_str(),
		this->configuration.wifiPassword.c_str()
	);




	Serial.println("\nStarting hotspot");

	/*
	this->startHotspot(
		this->configuration.hotspotSSID.c_str()
		,this->configuration.hotspotPassword.c_str()
	);
	*/

	Serial.println("\nHotspot started");
}


void Node::reset(void) {
	this->enableOutput(this->configuration.resetOutput);
}





void Node::listen(void) {

	unsigned long current =  millis();

	this->dnsServer.processNextRequest();

	if(current - this->postDatatLastTime > this->postDataDelay) {
		this->postData();
		this->postDatatLastTime = millis();
	}

	this->irrigate();

}


String Node::postData(void) {

	String response;

	response = this->post(
		this->configuration.sendDataURL,
		this->formatResponse(this->getData())
	);

	return response;
}




String Node::post(String url, String data) {
	HTTPClient http;
	http.begin(url);
	http.addHeader("Content-Type", "application/x-www-form-urlencoded");
	http.POST("data="+data);
	String response = http.getString();
	http.writeToStream(&Serial);
	http.end();
	return response;
}






String Node::getData(void) {


	String data = "{";
	String response = "";



	String temperature = String(this->getTemperature());

	String light = String(this->getLight());


	data +="\"humidity\": [";
	for(int i = 0; i < 4; i++) {
		data += ""+String(this->getHumidity(i))+",";
	}

	data = data.substring(0, data.length()-1);
	data +="],";


	data += "\"humidities\":[";




	for(int i = 0 ; i<4 ; i++) {
		data += "[";

		for(int j = 0 ; j<this->configuration.humidityMesureCount; j++) {
			data += String(this->getHumidity(i))+",";
		}
		data = data.substring(0, data.length()-1);
		data +="],";
	}

	data = data.substring(0, data.length()-1);
	data +="]";


	data += ",\"temperature\":";
	data += temperature;

	data += ",\"light\":";
	data += light;

	data += "}";

	return data;

}



String Node::formatResponse(String data)
{
	String response="{";

	response += "\"meta\": {";
		response += "\"id\":";
		response += "\""+this->configuration.node_id+"\"";
		response += ",";

		response += "\"version\":";
		response += "\""+this->configuration.node_version+"\"";
		response += ",";

		response += "\"mac\":";
		response += "\""+WiFi.macAddress()+"\"";
		response += ",";

		response += "\"ip\":";
		response += "\""+WiFi.localIP().toString()+"\"";
		response += ",";

		response += "\"firmware\":";
		response += "\""+this->configuration.firmware+"\"";
	response += "},";

	response += "\"data\": ";
		response += data;
	response += "}";

	return response;

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


