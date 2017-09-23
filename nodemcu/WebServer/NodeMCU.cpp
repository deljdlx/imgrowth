#include "NodeMCU.h"



NodeMCU::NodeMCU() {

}

void NodeMCU::setSerialPinsToGPIO(void) {
    pinMode(1, OUTPUT);
    pinMode(3, OUTPUT);

}


void NodeMCU::GPIOUp(void) {

}













int NodeMCU::connectFromSavedConfiguration() {

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
  Serial.println(strlen(password)>0?"********":"<no password>");


	// on demande la connexion au WiFi
	WiFi.begin(ssid, password);
	Serial.println("");
	// on attend d'etre connecte au WiFi avant de continuer
	int loop = 0;
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
		loop++;
		if(loop>20) {
			return 0;
		}
	}
	// on affiche l'adresse IP qui nous a ete attribuee
	Serial.println("");
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());
	return 1;
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

















void NodeMCU::wifiConnection(char* ssid, char* password) {



	const char *localSSID = "imgrowth";
	const char *localPassword = "16641664";


	WiFi.softAP(localSSID, localPassword);
	IPAddress myIP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(myIP);




	if(this->connectFromSavedConfiguration()) {
		return;
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
		this->saveWifiConfig(newSSID.c_str(),  WiFi.psk().c_str());

      } else {
        wpsSuccess = false;
      }
  }
}

