#include <ESP8266WiFi.h>
#include "./SN74HC595N.h"

SN74HC595N::SN74HC595N(int dataPin, int latchPin, int clockPin) {

	this->dataPin = dataPin;
	this->latchPin = latchPin;
	this->clockPin = clockPin;

    pinMode(this->dataPin, OUTPUT);
    pinMode(this->latchPin, OUTPUT);
    pinMode(this->clockPin, OUTPUT);

    this->reset();

 }


void SN74HC595N::set(int registre[8])
{
	for (int i = 0; i<8; i++)
	{
		this->registre[i] = registre[i];
	}
}


void SN74HC595N::reset()
{
	for (int i = 0; i<8; i++)
	{
		this->registre[i] = 0;
	}
}



void SN74HC595N::send()
{
	int valueSent;
	valueSent = this->registre[0] +
	this->registre[1] * 2 +
	this->registre[2] * 4 +
	this->registre[3] * 8 +
	this->registre[4] * 16 +
	this->registre[5] * 32 +
	this->registre[6] * 64 +
	this->registre[7] * 128;

	digitalWrite(this->latchPin, LOW);                       // Set latch LOW to start sending data
	shiftOut(this->dataPin, this->clockPin, MSBFIRST, valueSent);  // Send the data
	digitalWrite(this->latchPin, HIGH);                      // Set latch HIGH to end.

	//Serial.print("Wrote ");
	Serial.println(valueSent);


	//Serial.println(this->dataPin);
	//Serial.println(this->latchPin);
	//Serial.println(this->clockPin);


}