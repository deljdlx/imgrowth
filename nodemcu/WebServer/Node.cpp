#include "Node.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <string>


#define ONE_WIRE_BUS            D4
#include <OneWire.h>
#include "DallasTemperature.h"


#include "Pompe.h"
#include "HumiditySensor.h"
#include "Configuration.h"



//OneWire oneWire(ONE_WIRE_BUS);


Node::Node() {
	this->lightSensor= Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
	this->lightSensor.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */
	/* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
	//this->lightSensor.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
	this->lightSensor.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
	// tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */




 }


void Node::initialize(void) {
	//OneWire oneWire(ONE_WIRE_BUS);
	//DallasTemperature sensor(&oneWire);
	//sensor.begin();
	//this->setSensor(sensor);

	//this->temperatureSensor = DallasTemperature(&oneWire);
	//this->temperatureSensor.begin();
  	//node.setSensor(sensor);
}






float Node::getTemperature() {

	this->temperatureSensor.requestTemperatures();
	Serial.println(this->temperatureSensor.getTempCByIndex(0));
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







void Node::setSensor(DallasTemperature sensor) {
	this->temperatureSensor=sensor;
}





void Node::wifiConnection(char* ssid, char* password) {
	// on demande la connexion au WiFi
	WiFi.begin(ssid, password);
	Serial.println("");
	// on attend d'etre connecte au WiFi avant de continuer
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}

	// on affiche l'adresse IP qui nous a ete attribuee
	Serial.println("");
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());
}


void Node::listen(void) {
	this->server.handleClient();
	this->checkHumidity();
	this->getLight();
	this->ping();
}




void Node::checkHumidity(void) {


	int humidity;

	unsigned long humidityTimer = millis();

	if (humidityTimer - this->previousHumidyTime >= this->configuration.humidyInterval) {
		this->previousHumidyTime = humidityTimer;
		humidity = this->getHumidity();
		Serial.println(humidity);

		if (humidity > 600) {
			this->pompe.start();
		}
		else {
			this->pompe.stop();
		}
	}
}


void Node::ping(void) {
	unsigned long pingTimer = millis();

	if (pingTimer - this->previousPingTime >= this->configuration.pingInterval) {
		this->previousPingTime = pingTimer;
		this->declareServer();
	}
}


void Node::declareServer(void) {
	HTTPClient http;

    String declareURL =
    	this->configuration.declareURL +
    	"?dataURI="+this->configuration.node_dataURI +
    	"&nodeVersion="+this->configuration.node_version
    ;

	http.begin(declareURL);

	int httpCode = http.GET();

	if(httpCode > 0) {
	// HTTP header has been send and Server response header has been handled
		Serial.printf("[HTTP] GET... code: %d\n", httpCode);

		// file found at server
		if(httpCode == 200) {
			String response = http.getString();
			Serial.println(response);
			//this->server.send(200, "text/html", response);
		}
	}
	else {
		Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
	}
}


void Node::http_getData() {

	String humidity = String(this->getHumidity());
	String temperature = String(this->getTemperature());
	String light = String(this->getLight());


	String response="{";

	response=response+"\"humidity\":";
	response=response+humidity;

	response=response+",\"temperature\":";
	response=response+temperature;

	response=response+",\"light\":";
	response=response+light;


	response=response+"}";


	this->server.send(200, "application/json", response);

}






void Node::initializeServer(void) {


	this->declareServer();


	this->server.on(this->configuration.node_dataURI.c_str(), [this](){
		return this->http_getData();
	});


	this->server.on("/do", [this](){

	});



	this->server.on("/humidity", [this](){

		int humidity;
		humidity = this->getHumidity();
		Serial.println(humidity);
		server.send(200, "text/plain", String(humidity));

		/*
		if(hsol>600) {
		digitalWrite(pompePin, HIGH);
		}
		else {
		digitalWrite(pompePin, LOW);
		}
		*/
	});


	this->server.on("/on", [this](){
		Serial.println("on");
		server.send(200, "text/plain", "ON");
		/*
		digitalWrite(pompePin, HIGH);
		server.send(200, "text/plain", "ON");
		*/
  });


  this->server.on("/off", [this](){

	Serial.println("off");

  /*
    int hsol;
    hsol = getHumidity();
    Serial.println(hsol);

    digitalWrite(pompePin, LOW);
    server.send(200, "text/plain", String(hsol));
    Serial.println("off");
    */
  });

  // on commence a ecouter les requetes venant de l'exterieur
  this->server.begin();
}





int Node::getHumidity(void) {
  return this->humiditySensor.get();
}