#include "Node.h"


#include <string>
#include <OneWire.h>
#include "DallasTemperature.h"


#include "Configuration.h"
#include "Pompe.h"
#include "HumiditySensor.h"

#include "CD4051B.h"





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



void Node::startHotspot(const char * ssid, const char * password) {



	//WiFi.mode(WIFI_AP);
	this->hotspotIP = WiFi.softAPIP();


	Serial.print("AP IP address: "+this->configuration.hotspotSSID+"\n");
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
	this->server.handleClient();
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


	this->enableInput(0);
	String humidity0 = String(this->getHumidity());

	this->enableInput(1);
	String humidity1 = String(this->getHumidity());

	this->enableInput(2);
	String humidity2 = String(this->getHumidity());

	this->enableInput(3);
	String humidity3 = String(this->getHumidity());



	String temperature = String(this->getTemperature());
	String light = String(this->getLight());


	String response="{";

	response=response+"\"humidity\": ["+
		humidity0+","+
		humidity1+","+
		humidity2+","+
		humidity3+
	"]";

	//response=response+humidity;

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

		Serial.println("get data");

		return this->http_getData();
	});





	this->server.on("", [this](){
		server.send(200, "text/plain", "Node server is up ");
			//node.getLight();
        	//node.getTemperature();
        	//node.getHumidity();
	});


	this->server.on("/lightOn", [this](){
		server.send(200, "text/plain", "Light ON");
		digitalWrite(this->configuration.lightPIN, HIGH);
	});

	this->server.on("/lightOff", [this](){
		server.send(200, "text/plain", "Light Off");
		digitalWrite(this->configuration.lightPIN, LOW);
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
	});






	this->server.on("/wifi/configuration", [this](){
		String response = "<!doctype html><html><head>";
		response += "<style>";
		response += "html {font-size:2em; font-family:sans-serif}";
		response += "input {font-size:1.5em;}";
		response += "</style>";
		response += "</head>";

		response += "<body><form method='post' action='/wifi/configuration/save'>";

		response += "<fieldset><label>SSID <br/><input name='ssid'/></fieldset>";
		response += "<fieldset><label>Password <br/><input name='password'/></fieldset>";
		response += "<fieldset><input type='submit' value='Save'/></fieldset>";

		response += "</form><body>";
		server.send(200, "text/html", response);
  });




  this->server.on("/wifi/configuration/save", [this](){

  	String ssid = this->server.arg("ssid");
  	String password =  this->server.arg("password");


	if(WiFi.status() == WL_CONNECTED) {
		Serial.println("Wifi disconnection");
		WiFi.disconnect(true);
	}



	if(this->wifiConnection(ssid.c_str(), password.c_str())) {
		server.send(200, "text/html", "Wifi successfully save : \n SSID : "+ssid+"\nPassword : "+password);
	}
	else {
		server.send(200, "text/html", "Wifi configuration failed : \n SSID : "+ssid+"\nPassword : "+password);
	}






  });

  // on commence a ecouter les requetes venant de l'exterieur
  this->server.begin();
}





int Node::getHumidity(void) {
	int humidity = this->humiditySensor.get();
	Serial.println(humidity);
	return humidity;
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
