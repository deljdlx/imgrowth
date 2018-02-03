#include "NodeMCU.h"


#include "./DNSServer.h"

NodeMCU::NodeMCU() {


}

void NodeMCU::setSerialPinsToGPIO(void) {
    pinMode(1, OUTPUT);
    pinMode(3, OUTPUT);
}

void NodeMCU::setSerialPinsToGPIO(void) {
    pinMode(1, OUTPUT);
    pinMode(3, OUTPUT);

}




void NodeMCU::GPIOUp(void) {

}





void NodeMCU::startHotspot(const char * ssid, const char * password) {



	//WiFi.mode(WIFI_AP);
	this->hotspotIP = WiFi.softAPIP();


	Serial.print("\nAP IP address: "+this->configuration.hotspotSSID+"\n");
	Serial.println(this->hotspotIP);



	WiFi.softAPConfig(this->hotspotIP, this->hotspotIP, IPAddress(255, 255, 255, 0));

	WiFi.softAP(
		ssid
		//,password
	);





	const byte DNS_PORT = 53;
	this->dnsServer.setTTL(300);
	// set which return code will be used for all other domains (e.g. sending
	// ServerFailure instead of NonExistentDomain will reduce number of queries
	// sent by clients)
	// default is DNSReplyCode::NonExistentDomain
	this->dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);

	// start DNS server for a specific domain name
	this->dnsServer.start(DNS_PORT, "config.imgrowth", this->hotspotIP);


}










void NodeMCU::wifiConnect(void) {
	this->wifiAutoConnection(
		this->configuration.wifiSSID.c_str(),
		this->configuration.wifiPassword.c_str()
	);
}








bool NodeMCU::connectFromSavedConfiguration() {

/* Don't set this wifi credentials. They are configurated at runtime and stored on EEPROM */
	char ssid[32] = "";
	char password[32] = "";


  EEPROM.begin(512);
  EEPROM.get(0, ssid);
  EEPROM.get(0+sizeof(ssid), password);
  char ok[2+1];
  EEPROM.get(0+sizeof(ssid)+sizeof(password), ok);
  EEPROM.end();

  if (String(ok) != String("OK")) {
    ssid[0] = 0;
    password[0] = 0;
  }
  Serial.println("Recovered credentials:");
  Serial.println(ssid);
  Serial.println(strlen(password)>0? password :"<no password>");

	return this->wifiConnection(ssid, password);

}


void NodeMCU::saveWifiConfig(const char* ssid, const char* password) {

	char ssid2[32] = "";
	char password2[32] = "";

	strcpy(ssid2, ssid);
	strcpy(password2, password);

	EEPROM.begin(512);
	EEPROM.put(0, ssid2);
	EEPROM.put(0+sizeof(ssid2), password2);
	char ok[2+1] = "OK";
	EEPROM.put(0+sizeof(ssid2)+sizeof(password2), ok);
	EEPROM.commit();
	EEPROM.end();
}












bool NodeMCU::wifiConnection(const char* ssid, const char* password) {

	Serial.println("Trying connection");

	Serial.print("SSID : ");
	Serial.println(ssid);

	Serial.print("Password : ");
	Serial.println(password);

	Serial.println("");


	// on demande la connexion au WiFi
	WiFi.begin(ssid, password);
	// on attend d'etre connecte au WiFi avant de continuer
	int loop = 0;
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
		loop++;
		if(loop>20) {
			Serial.println("");
			Serial.println("Connection failed");
			return false;
		}
	}
	// on affiche l'adresse IP qui nous a ete attribuee
	Serial.println("");
	Serial.println("Connection successfull");
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());

	this->myIP = WiFi.localIP();

	return true;

}





bool NodeMCU::wifiAutoConnection(const char* ssid, const char* password) {


	if(WiFi.status() != WL_DISCONNECTED) {
		Serial.println("Wifi disconnection");
		WiFi.disconnect();
	}

	if(this->wifiConnection(ssid, password)) {
		WiFi.mode(WIFI_AP_STA);
		return true;
	}



	if(this->connectFromSavedConfiguration()) {
		WiFi.mode(WIFI_AP_STA);
		return true;
	}

  Serial.println("\nPress WPS button on your router ...");
  delay(5000);

  Serial.println("WPS config start");
  // WPS works in STA (Station mode) only -> not working in WIFI_AP_STA !!!

  WiFi.mode(WIFI_STA);


  delay(1000);
  WiFi.begin("foobar",""); // make a failed connection
  while (WiFi.status() == WL_DISCONNECTED) {
    delay(500);
    Serial.print(".");
  }
  bool wpsSuccess = WiFi.beginWPSConfig();
  if(wpsSuccess) {
      // Well this means not always success :-/ in case of a timeout we have an empty ssid
      String newSSID = WiFi.SSID();
      if(newSSID.length() > 0) {
        // WPSConfig has already connected in STA mode successfully to the new station.
        Serial.printf("WPS finished. Connected successfull to SSID '%s'", newSSID.c_str());


		Serial.print("IP address: ");
		Serial.println(WiFi.localIP());

		this->myIP = WiFi.localIP();

		this->saveWifiConfig(newSSID.c_str(),  WiFi.psk().c_str());
		return true;
      } else {
        return false;
      }
  }

  return false;
}

