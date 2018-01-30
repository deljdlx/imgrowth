#ifndef IMGROWTHHTTP_SERVER_H
#define IMGROWTHHTTP_SERVER_H



#include <ESP8266WebServer.h>
#include "Configuration.h"
#include "NodeMCU.h"
#include "Node.h"





#include <ESP8266WebServer.h>


class ImGrowthHTTPServer
{

	public: ESP8266WebServer server;

	public: Configuration configuration;

	public: Node node;

	public: unsigned long previousPingTime = 0;


	public: ImGrowthHTTPServer(void);


	public: String declareServer(void);
	public: void initialize(void);
	public: void listen(void);

	public: void getData(void);

	public: void ping(void);

}

;
#endif
