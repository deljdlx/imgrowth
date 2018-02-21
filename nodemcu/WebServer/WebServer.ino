#include <ESP8266WiFi.h>



#include <EEPROM.h>
#include <OneWire.h>


#include "ArduinoJson.h"


#include "./DallasTemperature.h"

#include "Configuration.h"
#include "Node.h"
#include "ImGrowthHTTPServer.h"











Configuration configuration;
Node node;


OneWire oneWire(configuration.oneWirePin);
DallasTemperature temperatureSensor(&oneWire);


ImGrowthHTTPServer HTTPServer;



/*
int dataPin = 13;        //Define which pins will be used for the Shift Register control
int latchPin = 12;
int clockPin = 14;
int Array[8];           // Declare an arrary of 8 integers. Each value will be either 0 or 1.



void resetArray()
{
for (int i = 0; i<8; i++)
  {
    Array[i] = 0;
  }
}


void sendArray()
{
  int valueSent;
  valueSent = Array[0] +
              Array[1] * 2 +
              Array[2] * 4 +
              Array[3] * 8 +
              Array[4] * 16 +
              Array[5] * 32 +
              Array[6] * 64 +
              Array[7] * 128;

  digitalWrite(latchPin, LOW);                       // Set latch LOW to start sending data
  shiftOut(dataPin, clockPin, MSBFIRST, valueSent);  // Send the data
  digitalWrite(latchPin, HIGH);                      // Set latch HIGH to end.

  Serial.print("Wrote ");
  Serial.println(valueSent);
}
*/





void setup(void) {


	Configuration configuration;


	Serial.begin(configuration.serialSpeed);



	//*
	temperatureSensor.begin();
	node.setSensor(temperatureSensor);





//*
	Serial.printf("initializing");
	node.initialize();

	HTTPServer.initialize();
	HTTPServer.setNode(node);
//*/



	//*/




/*
	WiFi.softAP(
		configuration.hotspotSSID.c_str()
		,configuration.hotspotPassword.c_str()
	);
	IPAddress myIP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(myIP);
*/


	/*
	HTTPServer.setNode(node);
	HTTPServer.initialize();
	*/



		//node.initialize();

		//HTTPServer.initialize();
		//HTTPServer.setNode(node);

	/*
	char json[] = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";
	StaticJsonBuffer<200> jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(json);
	const char* sensor = root["sensor"];
	long time          = root["time"];
	double latitude    = root["data"][0];
	double longitude   = root["data"][1];
	Serial.println(time);
	*/



}




unsigned long previousMillis = 0;

void loop(void) {


	//return;

	// a chaque iteration, on appelle handleClient pour que les requetes soient traitees
	HTTPServer.listen();

	//*
	int Array[8];
	int value;
	for(int i = 0 ; i< 8 ; i++) {
		if(random(0,3) > 1) {
			Array[i] = 1;
		}
		else {
			Array[i] = 0;
		}

	}

	//*/

	//Serial.println();
	node.writeToRegister(Array);

	delay(configuration.globalDelay);



  //resetArray();    // Ensure each value in the array is set to 0 at the start.

  // Here, your additional code will set an array value to 1 as you require
  // eg. :

  /*
	Array[0] = 1;
	Array[1] = 1;
	Array[2] = 1;
	Array[3] = 0;
	Array[4] = 0;
	Array[5] = 0;
	Array[6] = 1;
	Array[7] = 0;
	//*/


	//digitalWrite(14, HIGH);
	//digitalWrite(12, HIGH);
	//digitalWrite(13, HIGH);

  //sendArray();		 // Send the 8 digit array to the shift register




}



















