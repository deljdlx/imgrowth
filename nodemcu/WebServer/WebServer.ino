#include <ESP8266WiFi.h>

#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>


#include <ESP8266mDNS.h>


#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>


#include "Node.h"


const int pompePin =  5;
int pompeState = LOW;

const int PinAnalogiqueHumidite = 0;



Node node;


//ESP8266WebServer server(80); // on instancie un serveur ecoutant sur le port 80





void initialize(void) {

  char* ssid = "biniou"; // remplacer par le SSID de votre WiFi
  char* password = "16641664"; // remplacer par le mot de passe de votre WiFi

  Serial.begin(115200);
  Serial.printf("initialize");

  pinMode(PinAnalogiqueHumidite, INPUT);
  pinMode(pompePin, OUTPUT);

  node.wifiConnection(ssid, password);
  node.initializeServer();

}






void setup(void) {
	initialize();
}




unsigned long previousMillis = 0;

void loop(void) {

  long interval = 1000;           // interval at which to blink (milliseconds

  unsigned long currentMillis = millis();


  int hsol;


  // a chaque iteration, on appelle handleClient pour que les requetes soient traitees
	node.listen();


/*
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    hsol = getHumidity();
    Serial.println(hsol);
    if (hsol > 600) {
      digitalWrite(pompePin, HIGH);
    }
    else {
      digitalWrite(pompePin, LOW);
    }
  }
  */
}


















