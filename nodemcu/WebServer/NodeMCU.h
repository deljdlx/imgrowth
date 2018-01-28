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



	public : IPAddress myIP;

	public : IPAddress hotspotIP;


	public: NodeMCU(void);


	public: void setSerialPinsToGPIO(void);

	public: void GPIOUp();

	public: bool wifiConnection(const char* ssid, const char* password);
	public: bool wifiAutoConnection(const char* ssid, const char* password);

	public: void saveWifiConfig(const char* ssid, const char* password);
	public: bool connectFromSavedConfiguration();

};

#endif
