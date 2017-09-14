#include <ESP8266WiFi.h>
#include "CD4051B.h"

CD4051B::CD4051B(int pinA, int pinB, int pinC) {

	this->pinA = pinA;
	this->pinB = pinB;
	this->pinC = pinC;

    pinMode(this->pinA, OUTPUT);
    pinMode(this->pinB, OUTPUT);
    pinMode(this->pinC, OUTPUT);

    this->enable(0);

 }


 int CD4051B::enable(int pin) {
	if(pin == 0) {
		digitalWrite(this->pinA, LOW);
		digitalWrite(this->pinB, LOW);
		digitalWrite(this->pinC, LOW);
	}

	else if(pin == 1) {
		digitalWrite(this->pinA, HIGH);
		digitalWrite(this->pinB, LOW);
		digitalWrite(this->pinC, LOW);
	}

	else if(pin == 2) {
		digitalWrite(this->pinA, LOW);
		digitalWrite(this->pinB, HIGH);
		digitalWrite(this->pinC, LOW);
	}

	else if(pin == 3) {
		digitalWrite(this->pinA, HIGH);
		digitalWrite(this->pinB, HIGH);
		digitalWrite(this->pinC, LOW);
	}

	else if(pin == 4) {
		digitalWrite(this->pinA, LOW);
		digitalWrite(this->pinB, LOW);
		digitalWrite(this->pinC, HIGH);
	}

	else if(pin == 5) {
		digitalWrite(this->pinA, HIGH);
		digitalWrite(this->pinB, LOW);
		digitalWrite(this->pinC, HIGH);
	}

	else if(pin == 6) {
		digitalWrite(this->pinA, LOW);
		digitalWrite(this->pinB, HIGH);
		digitalWrite(this->pinC, HIGH);
	}

	else if(pin == 7) {
		digitalWrite(this->pinA, HIGH);
		digitalWrite(this->pinB, HIGH);
		digitalWrite(this->pinC, HIGH);
	}


	return pin;
 }
