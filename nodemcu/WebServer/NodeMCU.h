#ifndef NODEMCU_H
#define NODEMCU_H

#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>


class NodeMCU
{





	public: NodeMCU(void);


	public: void setSerialPinsToGPIO(void);

	public: void GPIOUp();

	public:  void wifiConnection(char* ssid, char* password);
	public: void saveWifiConfig(const char* ssid, const char* password);
	public: int connectFromSavedConfiguration();

};

#endif
