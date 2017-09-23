#include "Node.h"


#include <string>


#define ONE_WIRE_BUS            D4
#include <OneWire.h>
#include "DallasTemperature.h"


#include "Configuration.h"
#include "Pompe.h"
#include "HumiditySensor.h"

#include "CD4051B.h"




//OneWire oneWire(ONE_WIRE_BUS);


#define CONFIG_VERSION "ar1"
#define CONFIG_START 0






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

	//this->enableInput(2);
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




/*
void Node::wifiConnection(char* ssid, char* password) {



	const char *localSSID = "imgrowth";
	const char *localPassword = "16641664";


	WiFi.softAP(localSSID, localPassword);
	IPAddress myIP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(myIP);




	if(this->connectFromSavedConfiguration()) {
		return;
	}

  Serial.println("\nPress WPS button on your router ...");
  delay(5000);

  Serial.println("WPS config start");
  // WPS works in STA (Station mode) only -> not working in WIFI_AP_STA !!!
  WiFi.mode(WIFI_STA);
  delay(1000);
  WiFi.begin("foobar",""); // make a failed connection
  while (WiFi.status() == WL_DISCONNECTED) {
    delay(500);
    Serial.print(".");
  }
  bool wpsSuccess = WiFi.beginWPSConfig();
  if(wpsSuccess) {
      // Well this means not always success :-/ in case of a timeout we have an empty ssid
      String newSSID = WiFi.SSID();
      if(newSSID.length() > 0) {
        // WPSConfig has already connected in STA mode successfully to the new station.
        Serial.printf("WPS finished. Connected successfull to SSID '%s'", newSSID.c_str());


		Serial.print("IP address: ");
		Serial.println(WiFi.localIP());
		this->saveWifiConfig(newSSID.c_str(),  WiFi.psk().c_str());

      } else {
        wpsSuccess = false;
      }
  }
}
*/


void Node::listen(void) {
	this->server.handleClient();
	//this->checkHumidity();
	//this->getLight();
	//this->ping();
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





	this->server.on("", [this](){
		server.send(200, "text/plain", "Node server is up ");

			//node.getLight();
        	//node.getTemperature();
        	//node.getHumidity();
	});


	this->server.on("/listen0", [this](){
		this->enableInput(0);
		server.send(200, "text/plain", "Listening PIN 0");
	});
	this->server.on("/listen1", [this](){
		this->enableInput(1);
		server.send(200, "text/plain", "Listening PIN 1");
	});
	this->server.on("/listen2", [this](){
		this->enableInput(2);
		server.send(200, "text/plain", "Listening PIN 2");
	});
	this->server.on("/listen3", [this](){
		this->enableInput(3);
		server.send(200, "text/plain", "Listening PIN 3");
	});
	this->server.on("/listen4", [this](){
		this->enableInput(4);
		server.send(200, "text/plain", "Listening PIN 4");
	});
	this->server.on("/listen5", [this](){
		this->enableInput(5);
		server.send(200, "text/plain", "Listening PIN 5");
	});
	this->server.on("/listen6", [this](){
		this->enableInput(6);
		server.send(200, "text/plain", "Listening PIN 6");
	});
	this->server.on("/listen7", [this](){
		this->enableInput(7);
		server.send(200, "text/plain", "Listening PIN 7");
	});




	this->server.on("/write0", [this](){
		this->enableOutput(0);
		server.send(200, "text/plain", "writing PIN 0");
	});
	this->server.on("/write1", [this](){
		this->enableOutput(1);
		server.send(200, "text/plain", "writing PIN 1");
	});
	this->server.on("/write2", [this](){
		this->enableOutput(2);
		server.send(200, "text/plain", "writing PIN 2");
	});
	this->server.on("/write3", [this](){
		this->enableOutput(3);
		server.send(200, "text/plain", "writing PIN 3");
	});
	this->server.on("/write4", [this](){
		this->enableOutput(4);
		server.send(200, "text/plain", "writing PIN 4");
	});
	this->server.on("/write5", [this](){
		this->enableOutput(5);
		server.send(200, "text/plain", "writing PIN 5");
	});
	this->server.on("/write6", [this](){
		this->enableOutput(6);
		server.send(200, "text/plain", "writing PIN 6");
	});
	this->server.on("/write7", [this](){
		this->enableOutput(7);
		server.send(200, "text/plain", "writing PIN 7");
	});











	this->server.on("/", [this](){


	String humidity = String(this->getHumidity());
	String temperature = String(this->getTemperature());
	String light = String(this->getLight());


	String response="Server is  up <br/><pre>{\n";

	response=response+"\"humidity\":";
	response=response+humidity+"\n";

	response=response+",\"temperature\":";
	response=response+temperature+"\n";

	response=response+",\"light\":";
	response=response+light+"\n";


	response=response+"}</pre>";


		server.send(200, "text/html", response);
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
	int humidity = this->humiditySensor.get();
	Serial.println(humidity);
	return humidity;
}