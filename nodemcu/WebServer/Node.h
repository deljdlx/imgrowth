#ifndef NODE_H
#define NODE_H

#include <ESP8266WebServer.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>



#include <OneWire.h>
#include "./DallasTemperature.h"


#include "./CD4051B.h"



#include "Configuration.h"

#include "HumiditySensor.h"


#include "NodeMCU.h"





class Node : public NodeMCU
{



	public: unsigned long humidityLastTime0;
	public: unsigned long humidityLastTime1;
	public: unsigned long humidityLastTime2;
	public: unsigned long humidityLastTime3;

	public: unsigned long humidityCheckDelay = 10000;
	public: int humidityTresholds[4];



	HumiditySensor humiditySensor;
	DallasTemperature temperatureSensor;



	ESP8266WebServer server;





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


	public: void initialize(void);


	public: void listen(void);

	public: void start(void);


	public: void setSensor(DallasTemperature sensor);



	public: int getHumidity(int input);



	public: float getTemperature(void);
	public: float getLight(void);


	public: void reset(void);


	public: void watering(int pompeIndex);
	public: bool doWatering(int input);
	public: void irrigate(void);



};

#endif
