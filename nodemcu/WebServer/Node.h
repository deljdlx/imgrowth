#ifndef NODE_H
#define NODE_H

#include <ESP8266WebServer.h>
#include "Pompe.h"
#include "HumiditySensor.h"

class Node
{

	HumiditySensor humiditySensor;

	Pompe pompe;


	ESP8266WebServer server;





	unsigned long previousMillis = 0;
	int PinAnalogiqueHumidite = 0;
	int interval = 1000;



	public: Node(void);

	public: void initialize(void);


	public: void initializeServer(void);

	public: void listen(void);

	public: void start(void);



	public: void wifiConnection(char* ssid, char* password);

	public: int getHumidity(void);


};

#endif