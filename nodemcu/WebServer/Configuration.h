#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include <string>

class Configuration
{
	//public: char* wifiSSID="imgrowth";
	//public: char* wifiPassword="imgrowth";

	public: char* wifiSSID="biniou";
	public: char* wifiPassword="16641664";

	//public: int humidyInterval = 60000;
	public: int humidyInterval = 60000;


	public : int TEMPERATURE_PIN= D4;



	String node_version="0.0.1";
	String declareURL="http://192.168.0.10/project/imgrowth-web/www/index.php/declare";

	String node_dataURI="/getData";



	public: int pingInterval=60000;
	public: int previousPingTime;

};

#endif