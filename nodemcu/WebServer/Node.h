#ifndef NODE_H
#define NODE_H

#include <ESP8266WebServer.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>



#include <OneWire.h>
#include "./DallasTemperature.h"


#include "./CD4051B.h"
#include "./DNSServer.h"



#include "Configuration.h"

#include "HumiditySensor.h"


#include "NodeMCU.h"





class Node : public NodeMCU
{

	HumiditySensor humiditySensor;
	DallasTemperature temperatureSensor;



	ESP8266WebServer server;

	DNSServer dnsServer;




	Configuration configuration;


	Adafruit_TSL2561_Unified lightSensor = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);



	CD4051B outputMutiplexer = CD4051B(
		configuration.writePIN_0,
		configuration.writePIN_1,
		configuration.writePIN_2
	);
	CD4051B  inputMutiplexer = CD4051B(
		configuration.readPIN_0,
		configuration.readPIN_1,
		configuration.readPIN_2
	);


	unsigned long previousHumidyTime = 0;




	public: Node(void);


	public: int enableInput(int input);
	public: int enableOutput(int output);


	//public: void saveWifiConfig(const char* ssid, const char* password);
	//public: int connectFromSavedConfiguration();


	public: void initialize(void);


	public: void listen(void);

	public: void start(void);

	public: void checkHumidity(void);


	public: void setSensor(DallasTemperature sensor);


	//public: void wifiConnection(char* ssid, char* password);



	public: void wifiConnect(void);

	public: int getHumidity(void);
	public: float getTemperature(void);
	public: float getLight(void);

	public: void startHotspot(const char * ssid, const char * password);


	public:  void reset(void);



};

#endif
