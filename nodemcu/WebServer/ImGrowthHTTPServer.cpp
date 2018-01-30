#include "ImGrowthHTTPServer.h"

#include <ESP8266WebServer.h>
#include "Configuration.h"
#include "NodeMCU.h"
#include "Node.h"




ImGrowthHTTPServer::ImGrowthHTTPServer(void)
{

}




void ImGrowthHTTPServer::listen(void)
{
	this->node.listen();
	this->server.handleClient();
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

	this->node.enableInput(0);
	String humidity0 = String(this->node.getHumidity());

	this->node.enableInput(1);
	String humidity1 = String(this->node.getHumidity());



	this->node.enableInput(2);
	String humidity2 = String(this->node.getHumidity());

	this->node.enableInput(3);
	String humidity3 = String(this->node.getHumidity());




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



	this->server.on("/lightOn", [this](){
		server.send(200, "text/plain", "Light ON");
		digitalWrite(this->configuration.lightPIN, HIGH);
	});

	this->server.on("/lightOff", [this](){
		server.send(200, "text/plain", "Light Off");
		digitalWrite(this->configuration.lightPIN, LOW);
	});





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






	this->server.on("/sensor/light", [this](){
		String light = String(this->node.getLight());
		server.send(200, "application/json", "{\"light\" :" + light+"}");
	});

	this->server.on("/sensor/temperature", [this](){
		String temperature = String(this->node.getTemperature());
		server.send(200, "application/json", "{\"temperature\" :" + temperature+"}");
	});




	this->server.on("", [this](){
		server.send(200, "text/plain", "Node server is up ");
	});



	this->server.on("/", [this](){


	String humidity = String(this->node.getHumidity());

	//String temperature = String(this->node.getTemperature());


	//String light = String(this->node.getLight());


	String response="Server is  up <br/><pre>{\n";

	//response=response+"\"humidity\":";
	//response=response+humidity+"\n";

	//response=response+",\"temperature\":";
	//response=response+temperature+"\n";

	//response=response+",\"light\":";
	//response=response+light+"\n";


	response=response+"}</pre>";


		server.send(200, "text/html", response);
	});



	this->server.on("/humidity", [this](){

		int humidity;
		humidity = this->node.getHumidity();
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


  this->server.on("/node/declare", [this](){
	this->declareServer();
	server.send(200, "text/html", "Server registered");
  });



  // on commence a ecouter les requetes venant de l'exterieur
  this->server.begin();
}




void ImGrowthHTTPServer::ping(void) {
	unsigned long pingTimer = millis();

	if (pingTimer - this->previousPingTime >= this->configuration.pingInterval) {
		this->previousPingTime = pingTimer;
		this->declareServer();
	}
}




