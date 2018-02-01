#include "ImGrowthHTTPServer.h"

#include <ESP8266WebServer.h>
#include "Configuration.h"
#include "NodeMCU.h"
#include "Node.h"




#include "ArduinoJson.h"





ImGrowthHTTPServer::ImGrowthHTTPServer(void)
{

}




bool ImGrowthHTTPServer::listen(void)
{
	if(!this->configurationLoaded) {
		this->loadConfiguration();
	}

	if(this->configurationLoaded) {
		this->node.listen();
		this->server.handleClient();
		return true;
	}

	return false;
}

void ImGrowthHTTPServer::setNode(Node node)
{
	this->node = node;
}





String ImGrowthHTTPServer::declareServer(void) {
	HTTPClient http;

    String declareURL =
    	this->configuration.declareURL +
    	"?dataURI="+this->configuration.node_dataURI +
    	"&nodeVersion="+this->configuration.node_version
    ;


	Serial.println("Sending data : "+declareURL);
	http.begin(declareURL);

	int httpCode = http.GET();

	if(httpCode > 0) {
		Serial.printf("[HTTP] GET... code: %d\n", httpCode);
		// file found at server
		if(httpCode == 200) {
			String response = http.getString();
			Serial.println(response);

			return response;
		}
	}
	else {
		Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
	}
}




String ImGrowthHTTPServer::getData(void) {


	String response="{";


	String humidity0 = String(this->node.getHumidity(0));
	String humidity1 = String(this->node.getHumidity(1));
	String humidity2 = String(this->node.getHumidity(2));
	String humidity3 = String(this->node.getHumidity(3));

	String temperature = String(this->node.getTemperature());




	String light = String(this->node.getLight());




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


	return response;

}



void ImGrowthHTTPServer::initialize(void)
{

	this->node.initialize();





	this->server.on(this->configuration.node_dataURI.c_str(), [this](){

		String response = this->getData();

		this->server.send(200, "application/json", response);
	});



	this->server.on("/node/lightOn", [this](){
		server.send(200, "text/plain", "Light ON");
		digitalWrite(this->configuration.lightPIN, HIGH);
	});

	this->server.on("/node/lightOff", [this](){
		server.send(200, "text/plain", "Light Off");
		digitalWrite(this->configuration.lightPIN, LOW);
	});







	this->server.on("/node/sensor/light", [this](){
		String light = String(this->node.getLight());
		server.send(200, "application/json", "{\"light\" :" + light+"}");
	});

	this->server.on("/node/sensor/temperature", [this](){
		String temperature = String(this->node.getTemperature());
		server.send(200, "application/json", "{\"temperature\" :" + temperature+"}");
	});





	this->server.on("/", [this](){


		String response = "{\n";
			response += "\"LocalIP\": ";

			response +="\""+this->node.myIP.toString();
			response +=+"\",";

			response += "\"HotspotIP\": ";
			response +="\""+this->node.hotspotIP.toString();
			response +="\"";
		response += " }";



		server.send(200, "text/html", response);
	});



	this->server.on("/humidity", [this](){

		int humidity;
		humidity = this->node.getHumidity(0);
		Serial.println(humidity);
		server.send(200, "text/plain", String(humidity));
	});




	this->server.on("/wifi/connect", [this](){

		server.send(200, "application/json", "{\"message\" : \"Trying wifi connection\"}");
		this->node.wifiConnect();
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



	if(this->node.wifiConnection(ssid.c_str(), password.c_str())) {
		server.send(200, "text/html", "Wifi successfully save : \n SSID : "+ssid+"\nPassword : "+password);
	}
	else {
		server.send(200, "text/html", "Wifi configuration failed : \n SSID : "+ssid+"\nPassword : "+password);
	}
  });

  this->server.on("/node/gotoURL", [this](){
		String url = this->server.arg("url");

		Serial.println("Goto URL");
		Serial.println(url);

		String response = this->gotoURL(url);
		server.send(200, "text/html", response);
  });


  this->server.on("/node/declare", [this](){
	this->declareServer();
	server.send(200, "text/html", "Server registered");
  });



  this->server.on("/node/loadConfiguration", [this](){
		this->loadConfiguration();
  });


  this->server.on("/node/getConfiguration", [this](){
		server.send(200, "application/json", this->userConfiguration);
  });



	this->initializeIO();
	this->initializeWater();
	// on commence a ecouter les requetes venant de l'exterieur
	this->server.begin();
}


String ImGrowthHTTPServer::gotoURL(String url) {
	HTTPClient http;

    String response = "";

	Serial.println("Retrieving data : "+url);
	http.begin(url);

	int httpCode = http.GET();

	if(httpCode > 0) {
		if(httpCode == 200) {
			response = http.getString();
			return response;
		}
	}
	else {
		return response;
	}
}



bool ImGrowthHTTPServer::loadConfiguration(void) {


	HTTPClient http;

    String url = this->configuration.configurationURL;

	Serial.println("Retrieving data : "+url);
	http.begin(url);

	int httpCode = http.GET();

	if(httpCode > 0) {
		if(httpCode == 200) {

			String response = http.getString();
			this->userConfiguration = response;

			Serial.println(response);
			this->server.send(200, "application/json", response);


			const char * json = response.c_str();
			StaticJsonBuffer<800> jsonBuffer;

			this->configurationLoaded = true;

			JsonObject& root = jsonBuffer.parseObject(json);

			int i = 0;

			for(i = 0 ; i<4 ; i++) {
				this->node.humidityTresholds[i] = root["humidity"][i];
			}

			return true;
		}
	}
	else {
		Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
		return false;
	}
}



void ImGrowthHTTPServer::initializeWater(void) {
  this->server.on("/node/water0", [this](){
  	server.send(200, "text/html", "Watering 0");
	this->node.watering(0);
  });

  this->server.on("/node/water1", [this](){
  	server.send(200, "text/html", "Watering 1");
	this->node.watering(1);
  });

  this->server.on("/node/water2", [this](){
  	server.send(200, "text/html", "Watering 2");
	this->node.watering(2);
  });

  this->server.on("/node/water3", [this](){
  	server.send(200, "text/html", "Watering 3");
	this->node.watering(3);
  });
}





void ImGrowthHTTPServer::initializeIO(void) {



	this->server.on("/listen0", [this](){
		this->node.enableInput(0);
		server.send(200, "text/plain", "Listening PIN 0");
	});
	this->server.on("/listen1", [this](){
		this->node.enableInput(1);
		server.send(200, "text/plain", "Listening PIN 1");
	});
	this->server.on("/listen2", [this](){
		this->node.enableInput(2);
		server.send(200, "text/plain", "Listening PIN 2");
	});
	this->server.on("/listen3", [this](){
		this->node.enableInput(3);
		server.send(200, "text/plain", "Listening PIN 3");
	});
	this->server.on("/listen4", [this](){
		this->node.enableInput(4);
		server.send(200, "text/plain", "Listening PIN 4");
	});
	this->server.on("/listen5", [this](){
		this->node.enableInput(5);
		server.send(200, "text/plain", "Listening PIN 5");
	});
	this->server.on("/listen6", [this](){
		this->node.enableInput(6);
		server.send(200, "text/plain", "Listening PIN 6");
	});
	this->server.on("/listen7", [this](){
		this->node.enableInput(7);
		server.send(200, "text/plain", "Listening PIN 7");
	});




	this->server.on("/write0", [this](){
		this->node.enableOutput(0);
		server.send(200, "text/plain", "writing PIN 0");
	});
	this->server.on("/write1", [this](){
		this->node.enableOutput(1);
		server.send(200, "text/plain", "writing PIN 1");
	});
	this->server.on("/write2", [this](){
		this->node.enableOutput(2);
		server.send(200, "text/plain", "writing PIN 2");
	});
	this->server.on("/write3", [this](){
		this->node.enableOutput(3);
		server.send(200, "text/plain", "writing PIN 3");
	});
	this->server.on("/write4", [this](){
		this->node.enableOutput(4);
		server.send(200, "text/plain", "writing PIN 4");
	});
	this->server.on("/write5", [this](){
		this->node.enableOutput(5);
		server.send(200, "text/plain", "writing PIN 5");
	});
	this->server.on("/write6", [this](){
		this->node.enableOutput(6);
		server.send(200, "text/plain", "writing PIN 6");
	});
	this->server.on("/write7", [this](){
		this->node.enableOutput(7);
		server.send(200, "text/plain", "writing PIN 7");
	});



}






void ImGrowthHTTPServer::ping(void) {
	unsigned long pingTimer = millis();

	if (pingTimer - this->previousPingTime >= this->configuration.pingInterval) {
		this->previousPingTime = pingTimer;
		this->declareServer();
	}
}




