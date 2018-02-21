#ifndef SN74HC595N_H
#define SN74HC595N_H


class SN74HC595N
{
	int dataPin;
	int latchPin;
	int clockPin;

	int registre[8];



	public: SN74HC595N(int dataPin, int latchPin, int clockPin);

	public: void set(int registre[8]);

	public: void reset();
	public: void send();

};







#endif