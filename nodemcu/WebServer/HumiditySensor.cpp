#include "HumiditySensor.h"
#include <ESP8266mDNS.h>

HumiditySensor::HumiditySensor(int pin) {
	this->pin = pin;
}

int HumiditySensor::get(void) {
	int hsol;
	hsol = analogRead(this->pin); // Lit la tension analogique
	//Serial.println(hsol);
	return hsol;
}
