#ifndef NODEMCU_H
#define NODEMCU_H


#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>


class NodeMCU
{





	public: NodeMCU(void);


	public: void setSerialPinsToGPIO(void);

	public: void GPIOUp();


};

#endif
