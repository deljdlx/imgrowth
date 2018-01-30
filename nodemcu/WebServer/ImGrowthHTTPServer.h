#ifndef IMGROWTHHTTP_SERVER_H
#define IMGROWTHHTTP_SERVER_H



#include <ESP8266WebServer.h>
#include "Configuration.h"
#include "NodeMCU.h"
#include "Node.h"





#include <ESP8266WebServer.h>


class ImGrowthHTTPServer
{

	public: bool configurationLoaded =false;


	public: unsigned long previousPingTime = 0;
	public: ESP8266WebServer server;

	public: Configuration configuration;

	public: Node node;






	public: ImGrowthHTTPServer(void);


	public: String declareServer(void);


	public: void initialize(void);
	public: void initializeIO(void);
	public: void initializeWater(void);


	public: bool loadConfiguration(void);


	public: void setNode(Node node);

	public: void listen(void);

	public: String getData(void);

	public: void ping(void);






}

;
#endif
