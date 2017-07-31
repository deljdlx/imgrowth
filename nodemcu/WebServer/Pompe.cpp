#include "Pompe.h"
#include <ESP8266mDNS.h>


Pompe::Pompe(void) {

}

void Pompe::start(void) {
	digitalWrite(this->pin, HIGH);
}


void Pompe::stop(void) {
	digitalWrite(this->pin, LOW);
}


