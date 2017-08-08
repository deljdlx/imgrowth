#ifndef NODE_H
#define NODE_H

#include <ESP8266WebServer.h>

#include "Pompe.h"
#include "HumiditySensor.h"
#include "Configuration.h"






#include <OneWire.h>
#include "DallasTemperature.h"






class Node
{

	HumiditySensor humiditySensor;

	Pompe pompe;


	ESP8266WebServer server;

	Configuration configuration;



	int PinAnalogiqueHumidite = 0;


	unsigned long previousHumidyTime = 0;
	unsigned long previousPingTime = 0;



	DallasTemperature sensor;


	public: Node(void);

	public: void initialize(void);

	public: void declareServer(void);
	public: void http_getData(void);


	public: void initializeServer(void);

	public: void listen(void);

	public: void start(void);

	public: void checkHumidity(void);


	public: void setSensor(DallasTemperature sensor);


	public: void wifiConnection(char* ssid, char* password);


	public: int getHumidity(void);
	public: float getTemperature(void);


	public: void ping(void);


};

#endif
