#ifndef NODE_H
#define NODE_H

#include <ESP8266WebServer.h>

#include "Pompe.h"
#include "HumiditySensor.h"
#include "Configuration.h"

#include "NodeMCU.h"






#include <OneWire.h>
#include "DallasTemperature.h"



#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>


#include "CD4051B.h"


class Node : public NodeMCU
{

	HumiditySensor humiditySensor;

	Pompe pompe;


	ESP8266WebServer server;

	Configuration configuration;


	Adafruit_TSL2561_Unified lightSensor = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);



	CD4051B inputMutiplexer = CD4051B(14, 12, 13);
	CD4051B outputMutiplexer = CD4051B(15, 3, 1);



	int PinAnalogiqueHumidite = 0;


	unsigned long previousHumidyTime = 0;
	unsigned long previousPingTime = 0;



	DallasTemperature temperatureSensor;


	public: Node(void);


	public: int enableInput(int input);
	public: int enableOutput(int output);


	//public: void saveWifiConfig(const char* ssid, const char* password);
	//public: int connectFromSavedConfiguration();


	public: void initialize(void);

	public: void declareServer(void);
	public: void http_getData(void);


	public: void initializeServer(void);

	public: void listen(void);

	public: void start(void);

	public: void checkHumidity(void);


	public: void setSensor(DallasTemperature sensor);


	//public: void wifiConnection(char* ssid, char* password);


	public: int getHumidity(void);
	public: float getTemperature(void);
	public: float getLight(void);


	public: void ping(void);




};

#endif
