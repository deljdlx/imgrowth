#include "Node.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

Node::Node() {

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

  unsigned long currentMillis = millis();
  int hsol;


	//Serial.println("Listening");

	this->server.handleClient();


  if (currentMillis - previousMillis >= this->interval) {
    this->previousMillis = currentMillis;
    hsol = this->getHumidity();
    Serial.println(hsol);

    if (hsol > 600) {
    	this->pompe.start();
    }
    else {
    	this->pompe.stop();
    }
  }

}


void Node::initializeServer(void) {


  this->server.on("/do", [this](){
    HTTPClient http;




    http.begin("http://192.168.0.10/hello.php"); //HTTP

    int httpCode = http.GET();

    if(httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if(httpCode == 200) {
        String response = http.getString();
        Serial.println(response);
        this->server.send(200, "text/html", response);
      }
    }
    else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }


  });



  this->server.on("/humidity", [this](){


    int hsol;
    hsol = this->getHumidity();
    Serial.println(hsol);
    server.send(200, "text/plain", String(hsol));

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

  	/*
    digitalWrite(pompePin, HIGH);
    server.send(200, "text/plain", "ON");
    Serial.println("on");
    */

  });


  this->server.on("/off", [this](){

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