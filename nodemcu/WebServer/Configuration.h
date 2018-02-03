#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include <string>

class Configuration
{




	public: String node_version="0.0.1";
	public: String node_id = "000-000-000";


	public: int serialSpeed = 115200;


	public: int saveMemoryAddress = 32;
	public: char* configurationVersion="abc";

	public: String wifiSSID="biniou";
	public: String wifiPassword="16641664";


	public: int humidyInterval = 60000;

	public: int oneWirePin = D4;

	public: int writePIN_0 = 14;
	public: int writePIN_1 = 12;
	public: int writePIN_2 = 13;

	public:  int resetOutput = 7;

	public: int readPIN_0 = 15;
	public: int readPIN_1 = 3;
	public: int readPIN_2 = 1;

	public: int analogInputPIN = 0;

	public:  int lightPIN = 16;


	public: int startWriteOutput = 0;
	public: int startListenInput = 0;


	public: int wateringDelay = 10000;


	String declareURL="http://192.168.0.10/project/imgrowth-web/www/index.php/node/register";
	String configurationURL = "http://192.168.0.10/project/imgrowth-web/www/index.php/server/getConfiguration";

	String node_dataURI="/node/getData";


	public: String hotspotSSID ="imGrowthConfig";
	public: String hotspotPassword ="imGrowthConfig";


	public: int pingInterval=60000;
	public: int previousPingTime;

	public: int globalDelay=10;

};

#endif