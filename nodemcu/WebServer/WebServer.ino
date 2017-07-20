#include <ESP8266WiFi.h>

#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>


#include <ESP8266mDNS.h>


#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>


#include "Pompe.cpp"



const int pompePin =  5;
int pompeState = LOW;

const int PinAnalogiqueHumidite = 0;



ESP8266WebServer server(80); // on instancie un serveur ecoutant sur le port 80






void wifiConnect(void) {
  char* ssid = "ssid"; // remplacer par le SSID de votre WiFi
  char* password = "password"; // remplacer par le mot de passe de votre WiFi

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


void initialize(void) {
  Serial.begin(115200);
  Serial.printf("initialize");

  pinMode(PinAnalogiqueHumidite, INPUT);
  pinMode(pompePin, OUTPUT);

  wifiConnect();
}


void initializeServer(void) {

  server.on("/do", [](){
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
        server.send(200, "text/html", response);
      }
    }
    else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }


  });



  server.on("/humidity", [](){
    int hsol;
    hsol = getHumidity();
    Serial.println(hsol);
    server.send(200, "text/plain", String(hsol));
    if(hsol>600) {
      digitalWrite(pompePin, HIGH);
    }
    else {
      digitalWrite(pompePin, LOW);
    }
  });


  server.on("/on", [](){
    digitalWrite(pompePin, HIGH);
    server.send(200, "text/plain", "ON");
    Serial.println("on");

  });


  server.on("/off", [](){
    int hsol;

    hsol = getHumidity();
    Serial.println(hsol);

    digitalWrite(pompePin, LOW);
    server.send(200, "text/plain", String(hsol));
    Serial.println("off");
  });

  // on commence a ecouter les requetes venant de l'exterieur
  server.begin();
}



int getHumidity(void)
{
    int hsol;
    hsol = analogRead(PinAnalogiqueHumidite); // Lit la tension analogique
    //Serial.println(hsol);
    return hsol;
}



void setup(void){

  Pompe pompe;

  initialize();
  initializeServer();

}




unsigned long previousMillis = 0;

void loop(void){

    long interval = 1000;           // interval at which to blink (milliseconds

    unsigned long currentMillis = millis();


    int hsol;


  // a chaque iteration, on appelle handleClient pour que les requetes soient traitees
    server.handleClient();


    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      hsol = getHumidity();
      Serial.println(hsol);
      if(hsol>600) {
        digitalWrite(pompePin, HIGH);
      }
      else {
        digitalWrite(pompePin, LOW);
      }
    }
}











