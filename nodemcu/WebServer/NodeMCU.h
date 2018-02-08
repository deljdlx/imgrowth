#ifndef NODEMCU_H
#define NODEMCU_H

#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>




#include "./DNSServer.h"

#include "Configuration.h"

class NodeMCU
{


	public: Configuration configuration;
	public : IPAddress myIP;

	public : IPAddress hotspotIP;

	public: DNSServer dnsServer;



	public: NodeMCU(void);
	public: void setSerialPinsToGPIO(void);
	public: void GPIOUp();
	public: bool wifiConnection(const char* ssid, const char* password);
	public: bool wifiAutoConnection(const char* ssid, const char* password);
	public: void saveWifiConfig(const char* ssid, const char* password);
	public: bool connectFromSavedConfiguration();
	public: void wifiConnect(void);


	public: bool wifiWPSConnection();


	public: void startHotspot(const char * ssid, const char * password);

};

#endif
